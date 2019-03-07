// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <boost/filesystem.hpp>
#include <string>

namespace file {
namespace path {

bool Exists(const std::string& file);
bool Compare(const std::string& a, const std::string& b);

// Gets the part of a path after the last '/'.
// For instance:
//   * "/usr/bin" --> "bin"
//   * "/usr/bin/" --> ""
std::string Basename(const std::string& path);

// Gets the part of a path before the last '/'.
// For instance:
//   * "/usr" --> "/"
//   * "/usr/bin" --> "/usr"
//   * "/usr/bin/" --> "/usr/bin"
// Special cases:
//   * "abc" --> ""
//   * "/" --> "/"
std::string Dirname(const std::string& path);

template <typename... Args>
std::string Join(const std::string& path1, const Args&... paths) {
  boost::filesystem::path joined(path1);
  int joins[sizeof...(Args)] = {(joined = joined / paths, 0)...};
  (void)joins;
  return joined.string();
}

// Gets file name without suffix: "test.txt" -> test.
std::string FilenameStem(const std::string& path);

// In directory 'path', *RECURSIVELY*, find all files with names with specified
// prefix and suffix. Returns the full paths of the found files.
std::vector<std::string> FindFilesWithPrefixSuffix(const std::string& path,
                                                   const std::string& filename_prefix,
                                                   const std::string& filename_suffix);

// In directory 'path', *NOT RECURSIVELY*, find all files with names with specified
// prefix and suffix. Returns the full paths of the found files.
std::vector<std::string> ListFiles(const std::string& path, const std::string& filename_prefix,
                                   const std::string& filename_suffix);

// Gets all subdirectory names under path, non-recursive.
std::vector<std::string> GetSubdirectoryNames(const std::string& path);

boost::filesystem::path GetRelativePath(const boost::filesystem::path& directory,
                                        const boost::filesystem::path& absolute_path);

// Returns true if the specified path is an existing directory.
bool IsFolder(const std::string& path);

// Extract all files matching the given path with glob pattern.
std::vector<std::string> Glob(const std::string& path);

constexpr char kProjectRootEnvironmentVariable[] = "PONY_ROOT";
// Returns the absolute path to the ponyai directory, or empty if failed.
std::string GetProjectRootPath();

}  // namespace path
}  // namespace file

namespace utils {
namespace path {

std::string GetVehicleParamsPath();

std::string Get3dModelsDirectory();

}
}  // namespace utils
