licenses(["notice"])  # Boost Software 1.0

load("@ponyai_public_course//utils/bazel:boost.bzl", "boost_library")

package(default_visibility = ["//visibility:public"])

boost_library(
    name = "algorithm",
    deps = [
        ":array",
        ":assert",
        ":bind",
        ":concept_check",
        ":config",
        ":detail",
        ":exception",
        ":function",
        ":integer",
        ":iterator",
        ":mpl",
        ":range",
        ":ref",
        ":regex",
        ":static_assert",
        ":throw_exception",
        ":tuple",
        ":type_traits",
        ":utility",
    ],
)

boost_library(
    name = "align",
)

boost_library(
    name = "assert",
)

boost_library(
    name = "memory_order",
)

boost_library(
    name = "atomic",
    deps = [
        ":assert",
        ":config",
        ":cstdint",
        ":memory_order",
    ],
)

boost_library(
    name = "get_pointer",
)

boost_library(
    name = "is_placeholder",
)

boost_library(
    name = "visit_each",
)

boost_library(
    name = "bind",
    deps = [
        ":config",
        ":detail",
        ":get_pointer",
        ":is_placeholder",
        ":mem_fn",
        ":ref",
        ":static_assert",
        ":type",
        ":visit_each",
    ],
)

boost_library(
    name = "version",
)

boost_library(
    name = "operators",
    deps = [
        ":iterator",
    ],
)

boost_library(
    name = "chrono",
    deps = [
        ":config",
        ":mpl",
        ":operators",
        ":predef",
        ":ratio",
        ":system",
        ":throw_exception",
        ":version",
    ],
)

boost_library(
    name = "concept",
)

boost_library(
    name = "concept_check",
    deps = [
        ":concept",
    ],
)

boost_library(
    name = "config",
)

boost_library(
    name = "container",
    deps = [
        ":intrusive",
    ],
)

boost_library(
    name = "context",
    srcs = [
        "libs/context/src/asm/jump_x86_64_sysv_elf_gas.S",
        "libs/context/src/asm/make_x86_64_sysv_elf_gas.S",
    ],
    copts = [
        "-fsplit-stack",
        "-DBOOST_USE_SEGMENTED_STACKS",
    ],
    exclude_srcs = [
        "libs/context/src/untested.cpp",
        "libs/context/src/unsupported.cpp",
    ],
    deps = [
        ":assert",
        ":config",
        ":cstdint",
        ":detail",
        ":intrusive_ptr",
    ],
)

boost_library(
    name = "intrusive_ptr",
    deps = [
        ":smart_ptr",
    ],
)

boost_library(
    name = "conversion",
)

boost_library(
    name = "core",
    deps = [
        ":config",
    ],
)

boost_library(
    name = "token_functions",
    deps = [
        ":assert",
        ":config",
        ":detail",
        ":mpl",
        ":throw_exception",
    ],
)

boost_library(
    name = "token_iterator",
    deps = [
        ":assert",
        ":iterator",
        ":token_functions",
    ],
)

boost_library(
    name = "tokenizer",
    deps = [
        ":token_iterator",
    ],
)

boost_library(
    name = "date_time",
    deps = [
        ":algorithm",
        ":assert",
        ":config",
        ":detail",
        ":io",
        ":lexical_cast",
        ":limits",
        ":mpl",
        ":operators",
        ":range",
        ":serialization",
        ":shared_ptr",
        ":static_assert",
        ":throw_exception",
        ":tokenizer",
        ":type_traits",
    ],
)

boost_library(
    name = "detail",
    deps = [
        ":limits",
    ],
)

boost_library(
    name = "exception",
    deps = [
        ":config",
    ],
)

boost_library(
    name = "filesystem",
    copts = [
        "-Wno-unused-parameter",
    ],
    deps = [
        ":config",
        ":functional",
        ":io",
        ":iterator",
        ":range",
        ":scoped_array",
        ":shared_ptr",
        ":smart_ptr",
        ":system",
        ":type_traits",
    ],
)

boost_library(
    name = "foreach",
)

boost_library(
    name = "function_equal",
)

boost_library(
    name = "mem_fn",
)

boost_library(
    name = "typeof",
    deps = [
        ":config",
        ":mpl",
        ":preprocessor",
        ":type_traits",
        ":utility",
    ],
)

boost_library(
    name = "function",
    deps = [
        ":assert",
        ":bind",
        ":config",
        ":detail",
        ":function_equal",
        ":integer",
        ":mem_fn",
        ":mpl",
        ":ref",
        ":typeof",
    ],
)

boost_library(
    name = "function_types",
)

boost_library(
    name = "rational",
    deps = [
        ":operators",
    ],
)

boost_library(
    name = "functional",
    deps = [
        ":detail",
        ":integer",
    ],
)

boost_library(
    name = "geometry",
    deps = [
        ":call_traits",
        ":function_types",
        ":rational",
        ":tokenizer",
        ":variant",
        ":version",
    ],
)

boost_library(
    name = "heap",
    deps = [
        ":parameter",
    ],
)

boost_library(
    name = "integer",
    deps = [
        ":integer_traits",
        ":static_assert",
    ],
)

boost_library(
    name = "iterator",
    deps = [
        ":detail",
        ":static_assert",
    ],
)

boost_library(
    name = "intrusive",
    deps = [
        ":assert",
        ":static_assert",
    ],
)

boost_library(
    name = "io",
)

boost_library(
    name = "ref",
)

boost_library(
    name = "cstdint",
)

boost_library(
    name = "iostreams",
    exclude_srcs = [
        "libs/iostreams/src/bzip2.cpp",
    ],
    deps = [
        ":assert",
        ":call_traits",
        ":config",
        ":cstdint",
        ":detail",
        ":integer_traits",
        ":range",
        ":ref",
        ":shared_ptr",
        ":static_assert",
        ":throw_exception",
        ":type_traits",
        "@zlib//:zlib",
    ],
)

boost_library(
    name = "pending",
    deps = [
        ":config",
        ":integer",
        ":iterator",
        ":limits",
        ":mpl",
        ":next_prior",
        ":none",
        ":operators",
        ":optional",
        ":serialization",
        ":static_assert",
        ":type_traits",
        ":utility",
    ],
)

boost_library(
    name = "random",
    deps = [
        ":assert",
        ":config",
        ":cstdint",
        ":detail",
        ":integer",
        ":integer_traits",
        ":limits",
        ":mpl",
        ":noncopyable",
        ":operators",
        ":pending",
        ":range",
        ":static_assert",
        ":system",
        ":throw_exception",
        ":type_traits",
        ":utility",
    ],
)

boost_library(
    name = "format",
    deps = [
        ":assert",
        ":config",
        ":detail",
        ":limits",
        ":optional",
        ":shared_ptr",
        ":throw_exception",
        ":utility",
    ],
)

boost_library(
    name = "math",
    deps = [
        ":format",
    ],
)

boost_library(
    name = "move",
)

boost_library(
    name = "mpl",
    deps = [
        ":detail",
        ":move",
        ":preprocessor",
    ],
)

boost_library(
    name = "multi_index",
    deps = [
        ":foreach",
        ":serialization",
        ":static_assert",
        ":tuple",
    ],
)

boost_library(
    name = "type",
)

boost_library(
    name = "none",
    deps = [
        ":none_t",
    ],
)

boost_library(
    name = "none_t",
)

boost_library(
    name = "optional",
    deps = [
        ":assert",
        ":config",
        ":core",
        ":detail",
        ":move",
        ":mpl",
        ":none",
        ":static_assert",
        ":throw_exception",
        ":type",
        ":type_traits",
        ":utility",
    ],
)

boost_library(
    name = "parameter",
)

boost_library(
    name = "predef",
)

boost_library(
    name = "preprocessor",
)

boost_library(
    name = "range",
    deps = [
        ":concept_check",
        ":cstdint",
        ":iterator",
        ":optional",
    ],
)

boost_library(
    name = "integer_traits",
)

boost_library(
    name = "ratio",
    deps = [
        ":cstdint",
        ":integer_traits",
        ":type_traits",
    ],
)

boost_library(
    name = "limits",
)

boost_library(
    name = "scoped_array",
)

boost_library(
    name = "cregex",
)

boost_library(
    name = "regex",
    copts = [
        "-Wno-deprecated-register",
    ],
    defines = [
        "BOOST_FALLTHROUGH",
    ],
    deps = [
        ":assert",
        ":config",
        ":cregex",
        ":cstdint",
        ":functional",
        ":integer",
        ":limits",
        ":mpl",
        ":ref",
        ":scoped_array",
        ":scoped_ptr",
        ":shared_ptr",
        ":smart_ptr",
        ":throw_exception",
        ":type_traits",
    ],
)

boost_library(
    name = "compressed_pair",
)

boost_library(
    name = "pointee",
)

boost_library(
    name = "spirit",
    deps = [
        ":compressed_pair",
        ":optional",
        ":ref",
    ],
)

boost_library(
    name = "archive",
    deps = [
        ":assert",
        ":cstdint",
        ":integer",
        ":integer_traits",
        ":io",
        ":noncopyable",
        ":pointee",
        ":preprocessor",
        ":scoped_ptr",
        ":spirit",
        ":static_assert",
    ],
)

boost_library(
    name = "call_traits",
)

boost_library(
    name = "swap",
)

boost_library(
    name = "array",
    deps = [
        ":functional",
        ":swap",
    ],
)

boost_library(
    name = "shared_ptr",
    deps = [
        ":assert",
        ":smart_ptr",
    ],
)

boost_library(
    name = "serialization",
    exclude_srcs = [
        "libs/serialization/src/shared_ptr_helper.cpp",
    ],
    deps = [
        ":archive",
        ":array",
        ":call_traits",
        ":config",
        ":function",
        ":integer_traits",
        ":mpl",
        ":operators",
        ":shared_ptr",
        ":type_traits",
    ],
)

boost_library(
    name = "scoped_ptr",
    deps = [
        ":smart_ptr",
    ],
)

boost_library(
    name = "checked_delete",
)

boost_library(
    name = "smart_ptr",
    deps = [
        ":align",
        ":checked_delete",
        ":core",
        ":cstdint",
        ":predef",
        ":throw_exception",
        ":utility",
    ],
)

boost_library(
    name = "static_assert",
    deps = [
        ":detail",
    ],
)

boost_library(
    name = "noncopyable",
    deps = [
        ":core",
    ],
)

boost_library(
    name = "cerrno",
)

boost_library(
    name = "system",
    deps = [
        ":assert",
        ":cerrno",
        ":config",
        ":core",
        ":cstdint",
        ":noncopyable",
        ":predef",
        ":utility",
    ],
)

boost_library(
    name = "enable_shared_from_this",
    deps = [
        ":smart_ptr",
    ],
)

boost_library(
    name = "exception_ptr",
    deps = [
        ":exception",
    ],
)

boost_library(
    name = "thread",
    srcs = [
        "libs/thread/src/pthread/once_atomic.cpp",
        "libs/thread/src/pthread/thread.cpp",
    ],
    deps = [
        ":atomic",
        ":chrono",
        ":core",
        ":date_time",
        ":detail",
        ":enable_shared_from_this",
        ":exception",
        ":exception_ptr",
        ":io",
        ":system",
    ],
)

boost_library(
    name = "current_function",
)

boost_library(
    name = "throw_exception",
    deps = [
        ":current_function",
        ":exception",
    ],
)

boost_library(
    name = "type_index",
    deps = [
        ":core",
    ],
)

boost_library(
    name = "aligned_storage",
)

boost_library(
    name = "type_traits",
    deps = [
        ":aligned_storage",
        ":core",
        ":mpl",
        ":static_assert",
        ":utility",
    ],
)

boost_library(
    name = "tuple",
)

boost_library(
    name = "tr1",
    defines = [
        "BOOST_FALLTHROUGH",
    ],
    deps = [
        ":config",
    ],
)

boost_library(
    name = "next_prior",
)

boost_library(
    name = "utility",
    deps = [
        ":next_prior",
        ":noncopyable",
    ],
)

boost_library(
    name = "blank",
)

boost_library(
    name = "variant",
    deps = [
        ":blank",
        ":math",
        ":type_index",
    ],
)

boost_library(
    name = "asio",
    deps = [
        ":bind",
        ":config",
        ":date_time",
        ":filesystem",
        ":mpl",
        ":regex",
        ":smart_ptr",
        ":static_assert",
        ":throw_exception",
        ":type_traits",
        "@openssl//:crypto",
        "@openssl//:ssl",
    ],
)

boost_library(
    name = "numeric",
    deps = [
        ":serialization",
    ],
)

boost_library(
    name = "lexical_cast",
    deps = [
        ":array",
        ":container",
        ":integer",
        ":math",
        ":numeric",
        ":range",
    ],
)

boost_library(
    name = "uuid",
    deps = [
        ":assert",
        ":config",
        ":cstdint",
        ":iterator",
        ":random",
        ":throw_exception",
        ":type_traits",
    ],
)
