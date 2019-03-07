// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "common/utils/common/defines.h"
#include "common/utils/common/proto_utils.h"
#include "common/utils/status/status.h"
#include "common/utils/strings/utils.h"
#include "glog/logging.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/io/tokenizer.h"
#include "google/protobuf/message.h"
#include "google/protobuf/text_format.h"

namespace file {

class FileInterface {
 public:
  explicit FileInterface(std::string name) : name_(std::move(name)) {}
  virtual ~FileInterface() = default;

  // Returns the file name.
  const std::string& name() const { return name_; }

  virtual int64_t GetSize() const = 0;
  virtual int64_t GetCurrentOffset() const = 0;

  // Reads bytes to buffer from file. Returns number of bytes actually read. When return value is
  // not equal to input argument size, then either some error occured, or end-of-file is reached.
  // Some implementation might return a negative value to indicate error occurs.
  MUST_USE_RESULT virtual int64_t Read(void* buf, int64_t size) = 0;

  // Writes bytes of buffer to file. Some implementation might return a negative value to indicate
  // error occurs.
  MUST_USE_RESULT virtual int64_t Write(const void* buf, int64_t size) = 0;

  // Seeks to ‘offset’ bytes from the beginning of the file.
  MUST_USE_RESULT virtual bool SeekTo(int64_t offset) = 0;

  MUST_USE_RESULT virtual bool Close() = 0;

  // Flushes buffer.
  MUST_USE_RESULT virtual bool Flush() = 0;

 protected:
  std::string name_;
};

class File : public FileInterface {
 public:
  File(std::string name, const char* flag);
  File(std::string name, int fd, const char* flag);
  ~File() override;

  int64_t GetSize() const override;
  int64_t GetCurrentOffset() const override;

  // Reads bytes to buffer from file.
  MUST_USE_RESULT int64_t Read(void* buf, int64_t size) override;

  // Writes bytes of buffer to file.
  MUST_USE_RESULT int64_t Write(const void* buf, int64_t size) override;

  MUST_USE_RESULT bool SeekTo(int64_t offset) override;
  MUST_USE_RESULT bool Close() override;

  // Flushes buffer.
  MUST_USE_RESULT bool Flush() override;

 private:
  FILE* f_ = nullptr;
};

enum class Permission {
  kNormal,  // Just don't specifically set permissions.
  kFull,    // 777
};

std::unique_ptr<FileInterface> OpenFile(const std::string& file_path, const char* flag);
void ReadContents(const std::string& file_path, std::string* contents);
MUST_USE_RESULT bool WriteContents(const std::string& file_path, const std::string& contents);

MUST_USE_RESULT bool AppendContents(const std::string& file_path, const std::string& contents);

MUST_USE_RESULT bool WriteProtoToBinaryFile(const google::protobuf::Message& proto,
                                            const std::string& bin_file);

MUST_USE_RESULT bool WriteProtoToTextFile(const google::protobuf::Message& proto,
                                          const std::string& text_file);

MUST_USE_RESULT bool WriteProtoToFile(const google::protobuf::Message& proto,
                                      const std::string& file);

MUST_USE_RESULT bool ReadBinaryFileToProto(const std::string& bin_file,
                                           google::protobuf::Message* proto);

MUST_USE_RESULT bool ReadTextFileToProto(const std::string& text_file,
                                         google::protobuf::Message* proto);

MUST_USE_RESULT bool ReadFileToProto(const std::string& file, google::protobuf::Message* proto);

// Creates a folder. If any ancestor of the folder does not exist, then they will also be created.
// It's equivalent to 'mkdir -p'.
// If the permission is not kNormal, it will change the permission of new-created sub-directories.
void CreateFolder(const std::string& input_folder, Permission permission = Permission::kNormal);

// Creates a new subfolder of specified parent_folder with the intended name. If a file or subfolder
// of parent_folder already has the same name as intended_subfolder_name, we will try to find a
// subfolder name that starts with intended_subfolder_name and is unique within the parent_folder.
// Returns chosen subfolder name, e.g. "${intended_subfolder_name}_2".
std::string CreateNewUniqueSubfolder(const std::string& parent_folder,
                                     const std::string& intended_subfolder_name);

// Recursively deletes a folder. Any file or folders inside this folder will be deleted. It's
// equivalent to 'rm -r'.
// If force=true, then it ignores nonexistent folder, like 'rm -rf'.
void DeleteFolder(const std::string& folder, bool force = false);

utils::Status DeleteFileIfExists(const std::string& file);

// Returns file size in bytes.
int64_t GetFileSize(const std::string& path);

// Overwrite if dst_filename exists.
utils::Status CopyFile(const std::string& src_filename, const std::string& dst_filename);

// Only support regular files. Overwrite if dst_filename exists as a file. Errors out if
// dst_filename exists as a directory.
utils::Status RenameFile(const std::string& src_filename, const std::string& dst_filename);

// Create and open a temp file. Will add XXXXXX template at end of filename.
// Use FileInterface->name() to get the filename.
std::unique_ptr<File> CreateTempFile(const std::string& path_prefix);

// Create and open a temp file in /tmp.
std::unique_ptr<File> CreateTempFile();

// Generate a path containing random contents that can be a temp file.
std::string GenerateTempPath(const std::string& path_prefix);

// Copy the directory, like 'cp -r src_folder dst_folder/'.
// Will put the src_folder to the dst_folder/src_folder.
// Only support src_folder is directory, and not support wildcard char '*'.
utils::Status CopyFolder(const std::string& src_folder, const std::string& dst_folder);

}  // namespace file
