// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/file/path.h"

#include <glob.h>
#include <glog/logging.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include "common/utils/common/optional.h"
#include "common/utils/strings/utils.h"

namespace file {
namespace path {

using strings::StringView;
using boost::filesystem::path;

bool Exists(const std::string& file) { return boost::filesystem::exists(file); }

std::string Basename(const std::string& path) {
  auto pos = path.find_last_of('/');
  if (pos == std::string::npos) {
    return path;
  }
  return path.substr(pos + 1);
}

std::string Dirname(const std::string& path) {
  auto pos = path.find_last_of('/');
  if (pos == std::string::npos) {
    return "";
  }
  if (pos == 0) {
    return "/";
  }
  return path.substr(0, pos);
}

std::string FilenameStem(const std::string& path) {
  std::string basename = Basename(path);
  auto pos = basename.find_last_of('.');
  if (pos == std::string::npos) {
    return basename;
  }
  return basename.substr(0, pos);
}

std::vector<std::string> FindFilesWithPrefixSuffix(const std::string& path,
                                                   const std::string& filename_prefix,
                                                   const std::string& filename_suffix) {
  std::vector<std::string> files;
  if (path.empty()) {
    return files;
  }
  if (!boost::filesystem::exists(path)) {
    LOG(ERROR) << "Path doesn't exist: " << path;
    return files;
  }
  boost::filesystem::recursive_directory_iterator iter(path);
  while (iter != boost::filesystem::recursive_directory_iterator()) {
    if ((filename_suffix.empty() || strings::EndWith(iter->path().string(), filename_suffix)) &&
        (filename_prefix.empty() ||
         strings::StartWith(iter->path().stem().string(), filename_prefix))) {
      files.emplace_back(iter->path().string());
    }
    ++iter;
  }
  return files;
}

std::vector<std::string> ListFiles(const std::string& path, const std::string& filename_prefix,
                                   const std::string& filename_suffix) {
  std::vector<std::string> files;
  if (path.empty()) {
    return files;
  }
  if (!boost::filesystem::exists(path)) {
    LOG(ERROR) << "Path doesn't exist: " << path;
    return files;
  }
  boost::filesystem::directory_iterator iter(path);
  while (iter != boost::filesystem::directory_iterator()) {
    if ((filename_suffix.empty() || strings::EndWith(iter->path().string(), filename_suffix)) &&
        (filename_prefix.empty() ||
         strings::StartWith(iter->path().stem().string(), filename_prefix))) {
      files.emplace_back(iter->path().string());
    }
    ++iter;
  }
  return files;
}

bool Compare(const std::string& a, const std::string& b) {
  const int la = static_cast<int>(a.length());
  const int lb = static_cast<int>(b.length());
  int pa = 0;
  int pb = 0;
  while (pa < la && pb < lb) {
    char ca = a[pa];
    char cb = b[pb];
    if (isdigit(ca) != isdigit(cb)) {
      return isdigit(ca);
    }
    if (!isdigit(ca)) {
      ca = toupper(ca);
      cb = toupper(cb);
      if (ca != cb) {
        return ca < cb;
      }
      ++pa;
      ++pb;
      continue;
    }
    double va = 0.0;
    for (; pa < la && isdigit(a[pa]); ++pa) {
      va = va * 10.0 + (a[pa] - '0');
    }
    double vb = 0.0;
    for (; pb < lb && isdigit(b[pb]); ++pb) {
      vb = vb * 10.0 + (b[pb] - '0');
    }
    if (std::abs(va - vb) > 0.5) {
      return va < vb;
    }
  }
  return (pa >= la && (pb < lb || a < b));
}

boost::filesystem::path GetRelativePath(const boost::filesystem::path& directory,
                                        const boost::filesystem::path& absolute_path) {
  boost::filesystem::path relative_path("");
  boost::filesystem::path path = absolute_path;
  while (path.has_parent_path() && !boost::filesystem::equivalent(path, directory)) {
    relative_path = path.filename() / relative_path;
    path.remove_filename();
  }
  return relative_path;
}

std::vector<std::string> GetSubdirectoryNames(const std::string& path) {
  std::vector<std::string> directories;
  if (path.empty() || !boost::filesystem::exists(path)) {
    return directories;
  }
  boost::filesystem::directory_iterator iter(path);
  while (iter != boost::filesystem::directory_iterator()) {
    if (boost::filesystem::is_directory(iter->path())) {
      directories.emplace_back(iter->path().string());
    }
    ++iter;
  }
  return directories;
}

bool IsFolder(const std::string& path) {
  struct stat info;
  return stat(path.c_str(), &info) == 0 && info.st_mode & S_IFDIR;
}

std::vector<std::string> Glob(const std::string& path) {
  glob_t glob_result;
  glob(path.c_str(), GLOB_TILDE, NULL, &glob_result);
  std::vector<std::string> ret;
  for (int i = 0; i < glob_result.gl_pathc; ++i) {
    ret.push_back(std::string(glob_result.gl_pathv[i]));
  }
  globfree(&glob_result);
  return ret;
}

namespace {
const boost::filesystem::path kProjectRootMarkerFile(".pony_ai_root.md");

boost::filesystem::path GetProjectRootPath(boost::filesystem::path path) {
  for (; path.has_parent_path(); path = path.parent_path()) {
    if (boost::filesystem::exists(path / kProjectRootMarkerFile)) {
      return path;
    }
  }
  return boost::filesystem::path();
}
}  // namespace

std::string GetProjectRootPath() {
  boost::filesystem::path proj_root_path;
  // Try environment variable
  const char* proj_root_env = std::getenv(kProjectRootEnvironmentVariable);
  if (proj_root_env != nullptr) {
    return proj_root_env;
  }
  // Try executable path
  boost::filesystem::path path = boost::filesystem::read_symlink("/proc/self/exe");
  if (!path.empty() && path.has_parent_path()) {
    proj_root_path = GetProjectRootPath(path.parent_path());
    if (!proj_root_path.empty()) {
      return proj_root_path.string();
    }
  }
  // Try initial path
  proj_root_path = GetProjectRootPath(boost::filesystem::initial_path());
  if (!proj_root_path.empty()) {
    return proj_root_path.string();
  }
  // Try current path
  proj_root_path = GetProjectRootPath(boost::filesystem::current_path());
  if (!proj_root_path.empty()) {
    return proj_root_path.string();
  }
  // Try current path of the ancestor processes
  for (int pid = getpid(); pid;) {
    path = boost::filesystem::read_symlink("/proc/" + std::to_string(pid) + "/cwd");
    if (!path.empty()) {
      proj_root_path = GetProjectRootPath(path);
      if (!proj_root_path.empty()) {
        return proj_root_path.string();
      }
    }
    std::ifstream in("/proc/" + std::to_string(pid) + "/stat");
    int stat_pid = 0;
    std::string stat_comm;
    char stat_state = 0;
    int stat_ppid = 0;
    in >> stat_pid >> stat_comm >> stat_state >> stat_ppid;
    CHECK(in);
    CHECK_EQ(pid, stat_pid);
    pid = stat_ppid;
  }
  // Fail
  return std::string();
}

}  // namespace path
}  // namespace file

namespace utils {
namespace path {

std::string GetVehicleParamsPath() {
  return file::path::Join(file::path::GetProjectRootPath(), "common", "data", "vehicle_params",
                          "vehicle_params.txt");
}

std::string Get3dModelsDirectory() {
  return file::path::Join(file::path::GetProjectRootPath(), "common", "data", "3d_models");
}

}
}  // namespace utils
