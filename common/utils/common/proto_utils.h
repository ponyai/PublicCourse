// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <glog/logging.h>
#include "google/protobuf/io/tokenizer.h"

template <class ProtoType>
ProtoType ProtoMove(ProtoType& proto) {  // NOLINT(runtime/references)
  ProtoType new_proto;
  new_proto.Swap(&proto);
  return new_proto;
}

class DummyErrorCollector : public ::google::protobuf::io::ErrorCollector {
 public:
  void AddError(int /*line*/, int /*column*/, const std::string& /*message*/) override {}

  void AddWarning(int /*line*/, int /*column*/, const std::string& /*message*/) override {}
};

class LogErrorCollector : public ::google::protobuf::io::ErrorCollector {
 public:
  void AddError(int line, int column, const std::string& message) override {
    LOG(ERROR) << "Protobuf parser error, "
               << "line:" << line << " column:" << column << " message:" << message;
  }

  void AddWarning(int line, int column, const std::string& message) override {
    LOG(WARNING) << "Protobuf parser warning, "
                 << "line:" << line << " column:" << column << " message:" << message;
  }
};
