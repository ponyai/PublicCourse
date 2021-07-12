// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/map/map_lib.h"

#include "glog/logging.h"
#include "common/utils/file/file.h"

#include <iostream>

DEFINE_string(map_data_path, "pnc/map/grid3/map_proto.txt", "map data path");

namespace pnc {
namespace map {

MapLib::MapLib() { CHECK(file::ReadFileToProto(FLAGS_map_data_path, &map_data_)); }

}  // namespace map
}  // namespace pnc
