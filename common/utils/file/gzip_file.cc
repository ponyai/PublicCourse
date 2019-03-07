// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/file/gzip_file.h"

namespace file {

GzipFile::GzipFile(std::string name, const char* flag) : FileInterface(std::move(name)) {
  f_ = gzopen(name_.c_str(), flag);
  CHECK(f_ != nullptr) << "Failed to open file at '" << name_ << "'.";
}

GzipFile::~GzipFile() {
  if (f_) {
    gzclose(f_);
  }
}

int64_t GzipFile::GetSize() const { return GetFileSize(name()); }

int64_t GzipFile::GetCurrentOffset() const { return static_cast<int64_t>(gzoffset64(f_)); }

bool GzipFile::Flush() { return gzflush(f_, Z_FINISH) == Z_OK; }

bool GzipFile::Close() {
  if (gzclose(f_) == Z_OK) {
    f_ = nullptr;
    return true;
  }
  return false;
}

bool GzipFile::SeekTo(int64_t offset) { return gzseek64(f_, offset, SEEK_SET) == 0; }

int64_t GzipFile::Read(void* buf, int64_t size) {
  // In gzread(), size is uint32_t.
  CHECK(size >= 0 && size <= std::numeric_limits<uint32_t>::max());
  return gzread(f_, buf, size);
}

int64_t GzipFile::Write(const void* buf, int64_t size) {
  // In gzwrite(), size is uint32_t.
  CHECK(size >= 0 && size <= std::numeric_limits<uint32_t>::max());
  return gzwrite(f_, buf, size);
}

}  // namespace file
