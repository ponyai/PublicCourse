// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <unordered_map>
#include "common/utils/containers/utils.h"
#include "homework6/simulation/vehicle_agent.h"

namespace simulation {

class VehicleAgentFactory {
 public:
  void RegisterFactoryFunction(const std::string& class_name,
                               std::function<VehicleAgent*(std::string)> class_factory_function) {
    CHECK(factory_function_registry_.find(class_name) == factory_function_registry_.end())
        << "Class:" << class_name << " has been registered";
    factory_function_registry_[class_name] = class_factory_function;
  }

  VehicleAgent* Create(const std::string& class_name, const std::string& agent_name) {
    return utils::FindOrDie(factory_function_registry_, class_name)(agent_name);
  }

  static VehicleAgentFactory* Instance() {
    static VehicleAgentFactory factory;
    return &factory;
  }

 private:
  std::unordered_map<std::string, std::function<VehicleAgent*(std::string)>>
      factory_function_registry_;
};

template <class T>
class Registrar {
 public:
  Registrar(const std::string& class_name) {
    // register the class factory function
    VehicleAgentFactory::Instance()->RegisterFactoryFunction(
        class_name,
        [](const std::string& agent_name) -> VehicleAgent* { return new T(agent_name); });
  }
};

};  // namespace simulation
