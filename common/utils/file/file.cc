// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/file/file.h"

#include "common/utils/file/gzip_file.h"
#include "common/utils/file/path.h"
#include "common/utils/strings/format.h"

namespace file {

using utils::Status;

std::unique_ptr<FileInterface> OpenFile(const std::string& file_path, const char* flag) {
  if (strings::EndWith(file_path, ".gz")) {
    return std::make_unique<GzipFile>(file_path, flag);
  }
  return std::make_unique<File>(file_path, flag);
}

void ReadContents(const std::string& file_path, std::string* contents) {
  constexpr int kBufferSize = 16384;

  std::unique_ptr<FileInterface> f = OpenFile(file_path, "r");
  contents->clear();

  char buffer[kBufferSize];
  while (true) {
    int num_read_bytes = f->Read(buffer, kBufferSize);
    if (num_read_bytes <= 0) {
      return;
    }
    contents->append(buffer, num_read_bytes);
  }
}

bool WriteContents(const std::string& file_path, const std::string& contents) {
  std::unique_ptr<FileInterface> f = OpenFile(file_path, "w");
  int len = contents.size();
  int written = f->Write(contents.data(), len);
  return len == written;
}

bool AppendContents(const std::string& file_path, const std::string& contents) {
  std::unique_ptr<FileInterface> f = OpenFile(file_path, "a");
  int len = contents.size();
  int written = f->Write(contents.data(), len);
  return len == written;
}

bool WriteProtoToBinaryFile(const google::protobuf::Message& proto, const std::string& bin_file) {
  std::fstream output(bin_file, std::ios::out);
  if (!output.is_open()) {
    return false;
  }
  return proto.SerializeToOstream(&output);
}

bool WriteProtoToTextFile(const google::protobuf::Message& proto, const std::string& text_file) {
  std::fstream output(text_file, std::ios::out);
  if (!output.is_open()) {
    return false;
  }
  output << proto.DebugString();
  return true;
}

bool WriteProtoToFile(const google::protobuf::Message& proto, const std::string& file) {
  if (strings::EndWith(file, ".bin")) {
    return WriteProtoToBinaryFile(proto, file);
  } else {
    return WriteProtoToTextFile(proto, file);
  }
}

bool ReadBinaryFileToProto(const std::string& bin_file, google::protobuf::Message* proto) {
  CHECK(proto != nullptr);
  if (!boost::filesystem::exists(bin_file) || boost::filesystem::is_directory(bin_file)) {
    LOG(ERROR) << "Path doesn't exist or is not a file: " << bin_file;
    return false;
  }
  std::fstream input(bin_file, std::ios::in | std::ios::binary);
  if (!input.is_open()) {
    return false;
  }
  return proto->ParseFromIstream(&input);
}

bool ReadTextFileToProto(const std::string& text_file, google::protobuf::Message* proto) {
  CHECK(proto != nullptr);
  if (!boost::filesystem::exists(text_file) || boost::filesystem::is_directory(text_file)) {
    LOG(ERROR) << "Path doesn't exist or is not a file: " << text_file;
    return false;
  }
  std::fstream input(text_file, std::ios::in);
  if (!input.is_open()) {
    return false;
  }
  std::string data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  ::google::protobuf::TextFormat::Parser parser;
  LogErrorCollector error_collector;
  parser.RecordErrorsTo(&error_collector);
  return parser.ParseFromString(data, proto);
}

bool ReadFileToProto(const std::string& file, google::protobuf::Message* proto) {
  CHECK(proto != nullptr);
  if (strings::EndWith(file, ".bin")) {
    return ReadBinaryFileToProto(file, proto) || ReadTextFileToProto(file, proto);
  } else {
    return ReadTextFileToProto(file, proto) || ReadBinaryFileToProto(file, proto);
  }
}

File::File(std::string name, const char* flag) : FileInterface(std::move(name)) {
  f_ = fopen(name_.c_str(), flag);
  CHECK(f_ != nullptr) << "Failed to open file at '" << name_ << "'.";
}

File::File(std::string name, int fd, const char* flag) : FileInterface(std::move(name)) {
  f_ = fdopen(fd, flag);
  CHECK(f_ != nullptr) << "Failed to open file at '" << fd << "'.";
}

File::~File() {
  if (f_) {
    fclose(f_);
  }
}

int64_t File::GetSize() const { return GetFileSize(name()); }

int64_t File::GetCurrentOffset() const { return ftell(f_); }

bool File::Flush() { return fflush(f_) == 0; }

bool File::Close() {
  if (f_ != nullptr && fclose(f_) == 0) {
    f_ = nullptr;
    return true;
  }
  return false;
}

int64_t File::Read(void* buf, int64_t size) {
  CHECK_GE(size, 0);
  return fread(buf, 1, size, f_);
}

int64_t File::Write(const void* buf, int64_t size) {
  CHECK_GE(size, 0);
  return fwrite(buf, 1, size, f_);
}

bool File::SeekTo(int64_t offset) { return fseek(f_, offset, SEEK_SET) == 0; }

void CreateFolder(const std::string& input_folder, Permission permission) {
  const auto pos = input_folder.find_last_not_of('/');
  std::string folder = (pos == std::string::npos ? "" : input_folder.substr(0, pos + 1));
  std::string existing_folder = folder;
  while (!existing_folder.empty() && existing_folder != "/" && !path::Exists(existing_folder)) {
    existing_folder = path::Dirname(existing_folder);
  }
  if (!path::Exists(folder)) {
    boost::filesystem::create_directories(folder);
    LOG(INFO) << "Create folder: " << folder;
  }
  CHECK(boost::filesystem::is_directory(folder));
  if (permission == Permission::kFull) {
    while (folder != existing_folder) {
      LOG(INFO) << "Set folder to 777: " << folder;
      boost::filesystem::permissions(folder, boost::filesystem::perms::all_all);
      folder = path::Dirname(folder);
    }
  }
}

std::string CreateNewUniqueSubfolder(const std::string& parent_folder,
                                     const std::string& intended_subfolder_name) {
  std::string subfolder_name = intended_subfolder_name;
  std::string subfolder_path = file::path::Join(parent_folder, subfolder_name);
  int64_t index = 1;
  while (file::path::Exists(subfolder_path)) {
    ++index;
    subfolder_name = strings::Format("{}_{}", intended_subfolder_name, index);
    subfolder_path = file::path::Join(parent_folder, subfolder_name);
  }

  CreateFolder(subfolder_path);

  return subfolder_name;
}

void DeleteFolder(const std::string& folder, bool force) {
  if (!path::Exists(folder)) {
    LOG_IF(WARNING, !force) << "Folder: " << folder << " does not exist.";
    return;
  }
  boost::filesystem::remove_all(folder);
  LOG(INFO) << "Folder '" << folder << "' and everything inside it has been removed.";
}

utils::Status DeleteFileIfExists(const std::string& file) {
  if (path::Exists(file)) {
    boost::system::error_code error_code;
    boost::filesystem::remove(file, error_code);

    if (error_code) {
      return utils::Status::Error(error_code.message());
    }
  }

  return Status::OK;
}

int64_t GetFileSize(const std::string& path) {
  boost::system::error_code error_code;
  const auto size = boost::filesystem::file_size(path, error_code);
  CHECK(!error_code) << "Failed to get size of file \"" << path
                     << "\", details: " << error_code.message();
  if (size > std::numeric_limits<int64_t>::max()) {
    LOG(FATAL) << "Size of file \"" << path
               << "\" is too big to be represented in int64_t: " << size;
  }
  return static_cast<int64_t>(size);
}

utils::Status CopyFile(const std::string& src_filename, const std::string& dst_filename) {
  boost::system::error_code error_code;
  boost::filesystem::copy_file(src_filename, dst_filename,
                               boost::filesystem::copy_option::overwrite_if_exists, error_code);
  if (error_code) {
    return utils::Status::Error(error_code.message());
  }
  return Status::OK;
}

utils::Status RenameFile(const std::string& src_filename, const std::string& dst_filename) {
  if (!boost::filesystem::exists(src_filename)) {
    return utils::Status::Error(strings::Format("Path {} does not exist.", src_filename));
  }
  if (!boost::filesystem::is_regular_file(src_filename)) {
    return utils::Status::Error(
        strings::Format("RenameFile only supports regular files. Path {} is not.", src_filename));
  }
  if (src_filename == dst_filename) {
    return Status::OK;
  }
  if (std::rename(src_filename.c_str(), dst_filename.c_str()) != 0) {
    // When src and dst are in different devices, the std::rename may fail. Here
    // we try to do copy
    // followed by delete.
    RETURN_IF_ERROR(CopyFile(src_filename, dst_filename));
    RETURN_IF_ERROR(DeleteFileIfExists(src_filename));
  }
  return Status::OK;
}

std::unique_ptr<File> CreateTempFile(const std::string& path_prefix) {
  return std::make_unique<File>(GenerateTempPath(path_prefix), "w");
}

std::unique_ptr<File> CreateTempFile() { return CreateTempFile("/tmp/pony"); }

std::string GenerateTempPath(const std::string& path_prefix) {
  return path_prefix + boost::filesystem::unique_path().generic_string();
}

utils::Status CopyFolder(const std::string& src_folder, const std::string& dst_folder) {
  if (!file::path::IsFolder(src_folder)) {
    return utils::Status::Error("src_folder should be a existed folder.");
  }
  if (!file::path::IsFolder(dst_folder)) {
    return utils::Status::Error("dst_folder should be a existed folder.");
  }
  const std::string basename = file::path::Basename(src_folder);
  const std::string new_folder = file::path::Join(dst_folder, basename);
  CreateFolder(new_folder);
  const std::vector<std::string> files = file::path::ListFiles(src_folder, "", "");
  for (const std::string& file : files) {
    if (file::path::IsFolder(file)) {
      RETURN_IF_ERROR(CopyFolder(file, new_folder));
    } else {
      RETURN_IF_ERROR(CopyFile(file, file::path::Join(new_folder, file::path::Basename(file))));
    }
  }
  return Status::OK;
}

}  // namespace file
