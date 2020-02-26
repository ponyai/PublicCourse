// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/agent_metric.pb.h"
#include "common/proto/agent_status.pb.h"
#include "pnc/utils/vehicle_status_helper.h"

namespace {

constexpr double kDeltaT = 0.1;
}

namespace simulation {

class MetricCollector {
 public:
  explicit MetricCollector(const std::string& name) : name_(name) {}
  virtual ~MetricCollector() = default;

  void AddMetricFrame(const interface::agent::AgentStatus& agent_status);

  interface::agent::AgentMetric DumpMetric();

 private:
  std::string name_;

  double last_frame_time_ = 0.0;
  int frame_count_ = 0;

  double big_acc_sqr_sum_ = 0.0;
  double curvature_sqr_sum_ = 0.0;
  int num_finished_trips_ = 0;

  interface::agent::VehicleStatus last_vehicle_status_;
  interface::agent::VehicleStatus last_last_vehicle_status_;
  double time_cost_ = 0.0;
};

};  // namespace simulation
