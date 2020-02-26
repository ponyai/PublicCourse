# Copyright 2017 The TensorFlow Authors. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Utilities for defining Pony Bazel dependencies."""

def warn(msg):
    print("{red}{msg}{nc}".format(red = "\033[0;31m", msg = msg, nc = "\033[0m"))

def _is_windows(ctx):
    return ctx.os.name.lower().find("windows") != -1

def _get_env_var(ctx, name):
    if name in ctx.os.environ:
        return ctx.os.environ[name]
    else:
        return None

# Executes specified command with arguments and calls 'fail' if it exited with
# non-zero code
def _execute_and_check_ret_code(repo_ctx, cmd_and_args):
    result = repo_ctx.execute(cmd_and_args, timeout = 10)
    if result.return_code != 0:
        fail(("Non-zero return code({1}) when executing '{0}':\n" + "Stdout: {2}\n" +
              "Stderr: {3}").format(
            " ".join(cmd_and_args),
            result.return_code,
            result.stdout,
            result.stderr,
        ))

def _to_label(path):
    return Label("//" + path[::-1].replace("/", ":", 1)[::-1])

# Apply a patch_file to the repository root directory
# Runs 'patch -p1'
def _apply_patch(ctx, patch_file):
    # Don't check patch on Windows, because patch is only available under bash.
    if not _is_windows(ctx) and not ctx.which("patch"):
        fail("patch command is not found, please install it")
    cmd = ["patch", "-p1", "-d", ctx.path("."), "-i", ctx.path(patch_file)]
    if _is_windows(ctx):
        bazel_sh = _get_env_var(ctx, "BAZEL_SH")
        if not bazel_sh:
            fail("BAZEL_SH environment variable is not set")
        cmd = [bazel_sh, "-c", " ".join(cmd)]
    _execute_and_check_ret_code(ctx, cmd)

def _check_path_inside_archive(path, operation):
    if path.startswith("/"):
        fail("refusing to do operation '{0}' on path starting with '/': {1}".format(operation, path))
    if ".." in path:
        fail("refusing to do operation '{0}' on path containing '..': {1}".format(operation, path))

def _apply_delete(ctx, paths):
    for path in paths:
        _check_path_inside_archive(path, "rm -rf")
    _execute_and_check_ret_code(
        ctx,
        ["rm", "-rf"] + [ctx.path(path) for path in paths],
    )

# For why we need symbolic-link support, see the comment on the declaration of the
# pony_http_archive.symbolic_links attribute blow (inside the definition of the pony_http_archive
# repository_rule).
def _create_symbolic_links(ctx, path_mappings):
    for src_path, dest_path in path_mappings.items():
        _check_path_inside_archive(src_path, "ln -s")
        _check_path_inside_archive(dest_path, "ln -s")
        dest_path_parts = dest_path.rstrip("/").rsplit("/", 1)
        if len(dest_path_parts) > 1 and len(dest_path_parts[0]) > 0:
            _execute_and_check_ret_code(
                ctx,
                ["mkdir", "--parents", ctx.path(dest_path_parts[0])],
            )
        _execute_and_check_ret_code(
            ctx,
            ["ln", "--symbolic", ctx.path(src_path), ctx.path(dest_path)],
        )

def _download_and_extract(ctx):
    for url in ctx.attr.urls:
        ret = ctx.download_and_extract(
            url,
            "",
            ctx.attr.sha256,
            ctx.attr.type,
            ctx.attr.strip_prefix,
            allow_fail = True,
        )
        if ret.success:
            return ret
        else:
            warn("failed to download package from {}".format(url))

    fail("all the URLs in pony_http_archive(urls) are not valid")

def _pony_http_archive(ctx):
    _download_and_extract(ctx)
    if ctx.attr.fix_read_permission:
        ctx.execute(["/bin/chmod", "-R", "a+r", ctx.path(".").dirname])
    if ctx.attr.delete:
        _apply_delete(ctx, ctx.attr.delete)
    if ctx.attr.symbolic_links:
        _create_symbolic_links(ctx, ctx.attr.symbolic_links)

    if ctx.attr.patch_file != "" and len(ctx.attr.patch_files) > 0:
        fail("do not use both patch_file and patch_files")
    if ctx.attr.patch_file != "":
        _apply_patch(ctx, _to_label(ctx.attr.patch_file))
    for patch_file in ctx.attr.patch_files:
        _apply_patch(ctx, _to_label(patch_file))

    if ctx.attr.build_file != "":
        ctx.symlink(_to_label(ctx.attr.build_file), "BUILD.bazel")

pony_http_archive = repository_rule(
    implementation = _pony_http_archive,
    attrs = {
        "sha256": attr.string(mandatory = True),
        "urls": attr.string_list(mandatory = True, allow_empty = False),
        "strip_prefix": attr.string(),
        "type": attr.string(),
        "delete": attr.string_list(),
        # Certain PyPI source code packages have directory structures which make it impossible
        # to set PYTHONPATH properly with the "import" attribute of the "py_library" rule. So
        # we need to augment the directory structure with symbolic links.
        # One example: in psycopg2-binary-2.8.3, the "psycopg2" Python package maps to the "lib"
        # sub-directory inside the source code archive file.
        # https://files.pythonhosted.org/packages/80/91/91911be01869fa877135946f928ed0004e62044bdd876c1e0f12e1b5fb90/psycopg2-binary-2.8.3.tar.gz
        "symbolic_links": attr.string_dict(),
        "patch_file": attr.string(),
        # can use multiple patch files using patch_files
        # patches is applied in order
        "patch_files": attr.string_list(),
        "build_file": attr.string(),
        "fix_read_permission": attr.bool(),
    },
)
"""Downloads and creates Bazel repos for dependencies.
This is a swappable replacement for both http_archive() and
new_http_archive() that offers some additional features. It also helps
ensure best practices are followed.
"""
