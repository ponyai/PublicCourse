// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/map.pb.h"
#include "common/utils/file/file.h"
#include "common/utils/file/path.h"

#include "glog/logging.h"
#include "gflags/gflags.h"

DECLARE_string(map_dir);

namespace homework4 {
namespace map {

class MapLib {
 public:
  MapLib() {
    if (FLAGS_map_dir.empty()) {
      CHECK(file::ReadFileToProto("homework4/map/grid2/map_proto.txt", &map_data_));
    }  else {
      CHECK(file::ReadTextFileToProto(file::path::Join(FLAGS_map_dir, "map_proto.txt"), &map_data_));
    }
  }

  const interface::map::Map& map_proto() const { return map_data_; }

 private:
  interface::map::Map map_data_;
};

}  // namespace map
}  // namespace homework4
