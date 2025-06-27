// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "helloworld_module/helloworld_module.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep

bool HelloWorldModule::Initialize(aimrt::CoreRef core) {
  core_ = core;

  try {
    // Read custom module configuration
    auto file_path = core_.GetConfigurator().GetConfigFilePath();
    if (!file_path.empty()) {
      YAML::Node cfg_node = YAML::LoadFile(file_path.data());
      for (const auto& itr : cfg_node) {
        auto k = itr.first.as<std::string>();
        auto v = itr.second.as<std::string>();
        AIMRT_INFO("cfg [{} : {}]", k, v);
      }
    }
    // Write your other initialization logic here ...

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Init succeeded.");

  return true;
}

bool HelloWorldModule::Start() {
  // Write your runtime logic here
  AIMRT_INFO("Start succeeded.");
  return true;
}

void HelloWorldModule::Shutdown() {
  // Write your resource release logic here
  AIMRT_INFO("Shutdown succeeded.");
}
