// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/simulation_system.h"
#include "common/proto/agent_status.pb.h"
#include "common/proto/simulation.pb.h"

DEFINE_bool(record_simulation_history, true, "True if record simulation");
DEFINE_string(simulation_log_file_path, "/tmp/simulation_log.bin", "simulation log path");
DEFINE_string(simulation_metrics_file_path, "/tmp/simulation_metrics.txt",
              "simulation metrics path");
DEFINE_bool(multi_process_mode, true, "True if running simulation in multi process mode");
DEFINE_bool(srand_by_time, false, "True if truly randomize all stuffs");
DEFINE_bool(enable_debug_variables, true, "True if enabling debug variables ");

namespace simulation {

using interface::agent::AgentStatus;
using interface::simulation::AgentConfig;

SimulationSystem::SimulationSystem(
    const interface::simulation::SimulationConfig& simulation_config) {
  simulation_config_.CopyFrom(simulation_config);
}

void SimulationSystem::Initialize() {
  if (FLAGS_srand_by_time) {
    std::srand(time(0));
  }
  map_lib_ = std::make_unique<pnc::map::MapLib>();
  CHECK(file::ReadTextFileToProto(utils::path::GetVehicleParamsPath(), &vehicle_params_));
  simulation_world_ =
      std::make_unique<SimulationWorld>(map_lib_->map_proto(), vehicle_params_, simulation_config_);
  simulation_world_->Initialize();
  if (!FLAGS_multi_process_mode) {
    simulation_engine_ = std::make_unique<SingleThreadSimulationEngine>(simulation_config_);
  } else {
    simulation_engine_ = std::make_unique<MultiProcessSimulationEngine>(simulation_config_);
  }

  for (const AgentConfig& agent_config : simulation_config_.agent_config()) {
    interface::agent::AgentStatus agent_status =
        simulation_world_->AddVehicleAgent(agent_config.name());
  }

  simulation_engine_->SetAgentStatusMap(simulation_world_->GetAgentStatusMap());
  simulation_engine_->Initialize();

  LOG(INFO) << "Simulation system has been initialized. ";

  if (simulation_config_.add_keyboard_controlled_agent()) {
    VehicleAgent* manual_test_agent = AddManualTestAgent();
    interface::agent::AgentStatus manual_test_agent_status =
        simulation_world_->AddVehicleAgent(std::string(kKeyboardControlledAgentName));
    manual_test_agent->Initialize(manual_test_agent_status);
  }

  if (FLAGS_record_simulation_history) {
    CHECK_OK(file::DeleteFileIfExists(FLAGS_simulation_log_file_path));
    LOG(INFO) << "Simulation history is recorded to " << FLAGS_simulation_log_file_path;
    LOG(INFO) << "Simulation history index is written to "
              << FLAGS_simulation_log_file_path + ".index";

    simulation_log_file_ = std::make_unique<file::File>(FLAGS_simulation_log_file_path, "w");
    simulation_index_file_ =
        std::make_unique<file::File>(FLAGS_simulation_log_file_path + ".index", "w");
  }
}

void SimulationSystem::LogSimulationHistory() {
  if (FLAGS_record_simulation_history) {
    interface::simulation::SimulationSystemData frame_data = FetchData();
    std::string content = frame_data.SerializeAsString();

    double timestamp = frame_data.simulation_time();
    const int64_t offset = simulation_log_file_->GetCurrentOffset();

    const uint64_t size = content.size();
    CHECK(simulation_log_file_->Write(&size, sizeof(size)) == sizeof(size));
    CHECK(simulation_log_file_->Write(content.data(), size) == size);
    CHECK(simulation_log_file_->Flush());

    CHECK(simulation_index_file_->Write(&timestamp, sizeof(timestamp)) == sizeof(timestamp));
    CHECK(simulation_index_file_->Write(&offset, sizeof(offset)) == sizeof(offset));
    CHECK(simulation_index_file_->Flush());
  }
}

void SimulationSystem::Start() {
  LOG(INFO) << "Simulation system has been started";
  auto system_time = std::chrono::steady_clock::now();
  simulation_world_->Start();
  while (true) {
    FlushPlaybackCommands();
    if (!playback_status().playback_paused || MaybePlaybackNextOneIteration()) {
      {
        utils::MutexLocker lock(&mutex_);
        agent_status_map_ = simulation_world_->GetAgentStatusMap();
        simulation_time_ = simulation_world_->GetSimulationTime();
        traffic_light_status_.CopyFrom(simulation_world_->GetTrafficLightStatus());
        pedestrian_obstacles_ = simulation_world_->GetPedestrianObstacles();
        for (auto& iter : agent_status_map_) {
          debug_variables_map_map_[iter.first] = agent_response_map_[iter.first].debug_variables();
        }
      }
      LogSimulationHistory();
      simulation_engine_->SetAgentStatusMap(simulation_world_->GetAgentStatusMap());
      simulation_engine_->RunOneIteration();
      agent_response_map_ = simulation_engine_->GetAgentResponseMap();
      // If there is a keyboard controlled agent, update its control command
      if (simulation_config_.add_keyboard_controlled_agent()) {
        AgentStatus agent_status;
        {
          utils::MutexLocker lock(&mutex_);
          agent_status.CopyFrom(agent_status_map_[kKeyboardControlledAgentName]);
        }
        agent_response_map_[std::string(kKeyboardControlledAgentName)]
            .mutable_control_command()
            ->CopyFrom(manual_test_agent_->RunOneIteration(agent_status));
        agent_response_map_[std::string(kKeyboardControlledAgentName)].set_iteration_time_cost(0.0);
      }
      // Update all agents' status in simulation world
      simulation_world_->UpdateAgentResponse(agent_response_map_);
      simulation_world_->RunOneIteration();
      // If all agents are not alive, terminate simulation
      if (simulation_world_->DeterminSimulationFinished()) {
        break;
      }
    }
    if (FLAGS_enable_visualization) {
      // If visualization is enabled, keep simulation time the same speed as system time if possible
      system_time += std::chrono::microseconds(
          static_cast<int64_t>(10000.0 / playback_status().playback_speed_x));
      std::this_thread::sleep_until(system_time);
    }
  }
  // Update final agent status for all agents
  {
    utils::MutexLocker lock(&mutex_);
    agent_status_map_ = simulation_world_->GetAgentStatusMap();
    traffic_light_status_.CopyFrom(simulation_world_->GetTrafficLightStatus());
    simulation_time_ = simulation_world_->GetSimulationTime();
    pedestrian_obstacles_ = simulation_world_->GetPedestrianObstacles();
  }
  LogSimulationHistory();
  LogSimulationMetric();
}

interface::simulation::SimulationSystemData SimulationSystem::FetchData() {
  interface::simulation::SimulationSystemData simulation_world_data;
  {
    utils::MutexLocker lock(&mutex_);
    for (auto iter = agent_status_map_.begin(); iter != agent_status_map_.end(); iter++) {
      interface::simulation::VehicleAgentData* vehicle_agent_data =
          simulation_world_data.add_vehicle_agent();
      vehicle_agent_data->set_name(iter->first);
      vehicle_agent_data->mutable_agent_status()->CopyFrom(iter->second);
      vehicle_agent_data->mutable_debug_variables()->CopyFrom(
          debug_variables_map_map_[iter->first]);
    }
    for (int i = 0; i < pedestrian_obstacles_.size(); i++) {
      simulation_world_data.add_pedestrian()->CopyFrom(pedestrian_obstacles_[i]);
    }
    simulation_world_data.set_simulation_time(simulation_time_);
    simulation_world_data.mutable_traffic_light_status()->CopyFrom(traffic_light_status_);
  }
  return simulation_world_data;
}

VehicleAgent* SimulationSystem::AddManualTestAgent() {
  manual_test_agent_ = std::make_unique<ManualTestAgent>();
  return manual_test_agent_.get();
}

void SimulationSystem::LogSimulationMetric() {
  const interface::simulation::SimulationMetrics& simulation_metrics =
      simulation_world_->GetSimulationMetric();
  CHECK(file::WriteProtoToTextFile(simulation_metrics, FLAGS_simulation_metrics_file_path));
  LOG(INFO) << "Simulation metric has been written to " << FLAGS_simulation_metrics_file_path;
}

}  // namespace simulation
