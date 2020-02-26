// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/map/map_lib.h"

#include "glog/logging.h"
#include "common/utils/file/file.h"

#include <iostream>

namespace pnc {
namespace map {

MapLib::MapLib() { CHECK(file::ReadFileToProto("pnc/map/grid3/map_proto.txt", &map_data_)); }

}  // namespace map
}  // namespace pnc
