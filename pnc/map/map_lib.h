// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/map.pb.h"

#include "glog/logging.h"

namespace pnc {
namespace map {

class MapLib {
 public:
  MapLib();

  const interface::map::Map& map_proto() const { return map_data_; }

 private:
  interface::map::Map map_data_;
};

}  // namespace map
}  // namespace pnc
