// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/map.pb.h"
#include "common/utils/file/file.h"

#include "glog/logging.h"

namespace homework5 {
namespace map {

class MapLib {
 public:
  MapLib() { CHECK(file::ReadFileToProto("homework5/map/grid2/map_proto.txt", &map_data_)); }

  const interface::map::Map& map_proto() const { return map_data_; }

 private:
  interface::map::Map map_data_;
};

}  // namespace map
}  // namespace homework5
