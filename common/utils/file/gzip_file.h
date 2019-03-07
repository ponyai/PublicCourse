// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <zlib.h>

#include "common/utils/file/file.h"
#include "common/utils/status/status.h"
#include "common/utils/strings/utils.h"

namespace file {

class GzipFile : public FileInterface {
 public:
  GzipFile(std::string name, const char* flag);
  ~GzipFile() override;

  int64_t GetSize() const override;
  int64_t GetCurrentOffset() const override;

  // Reads bytes to buffer from file.
  MUST_USE_RESULT int64_t Read(void* buf, int64_t size) override;

  // Writes bytes of buffer to file.
  MUST_USE_RESULT int64_t Write(const void* buf, int64_t size) override;

  MUST_USE_RESULT bool SeekTo(int64_t offset) override;
  MUST_USE_RESULT bool Close() override;

  // Flushes buffer. This operation terminates a gzip compression stream. The next call to Write()
  // will start a new compression stream. Therefore, calling this function frequently would degrade
  // compression quality.
  MUST_USE_RESULT bool Flush() override;

 private:
  gzFile f_ = nullptr;
};

}  // namespace file
