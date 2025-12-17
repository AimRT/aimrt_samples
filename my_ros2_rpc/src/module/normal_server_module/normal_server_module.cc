// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "normal_server_module/normal_server_module.h"
#include "normal_server_module/global.h"  // IWYU pragma: keep
#include "yaml-cpp/yaml.h"                // IWYU pragma: keep

bool NormalRpcServerModule::Initialize(aimrt::CoreRef core) {
  core_ = core;

  SetLogger(core_.GetLogger());

  try {
    // Read cfg
    std::string file_path = std::string(core_.GetConfigurator().GetConfigFilePath());
    if (!file_path.empty()) {
      YAML::Node cfg_node = YAML::LoadFile(file_path);
    }

    // Register server
    service_ptr_ = std::make_shared<CalculateServiceImpl>();
    bool ret = core_.GetRpcHandle().RegisterService(service_ptr_.get());
    AIMRT_CHECK_ERROR_THROW(ret, "Register service failed.");

    AIMRT_INFO("Register service succeeded.");

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Init succeeded.");

  return true;
}

bool NormalRpcServerModule::Start() { return true; }

void NormalRpcServerModule::Shutdown() {}
