// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include <random>

#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"
#include "normal_client_module/normal_client_module.h"

#include "yaml-cpp/yaml.h"  // IWYU pragma: keep

auto random_int(int min, int max) {
  static std::mt19937 gen(std::random_device{}());
  return std::uniform_int_distribution<int>(min, max)(gen);
}

bool NormalRpcClientModule::Initialize(aimrt::CoreRef core) {
  core_ = core;

  try {
    // Read cfg
    std::string file_path = std::string(core_.GetConfigurator().GetConfigFilePath());
    if (!file_path.empty()) {
      YAML::Node cfg_node = YAML::LoadFile(file_path);
      rpc_frq_ = cfg_node["rpc_frq"].as<double>();
    }

    // Get executor handle with name "work_thread_pool",(it must be registered in aimrt config file)
    executor_ = core_.GetExecutorManager().GetExecutor("work_thread_pool");
    AIMRT_CHECK_ERROR_THROW(executor_, "Get executor 'work_thread_pool' failed.");

    // Register client(it contains two steps)
    // Step1: register rpc_handel
    auto rpc_handle = core_.GetRpcHandle();
    AIMRT_CHECK_ERROR_THROW(rpc_handle, "Get rpc handle failed.");
    bool ret = my_ros2_srvs::srv::RegisterCalculateSumClientFunc(rpc_handle);
    AIMRT_CHECK_ERROR_THROW(ret, "Register client failed.");

    // Step2: register client proxy
    proxy_ = std::make_shared<my_ros2_srvs::srv::CalculateSumSyncProxy>(rpc_handle);

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Init succeeded.");

  return true;
}

bool NormalRpcClientModule::Start() {
  try {
    run_flag_ = true;
    // Start main loop in executor_
    executor_.Execute(std::bind(&NormalRpcClientModule::MainLoop, this));
  } catch (const std::exception& e) {
    AIMRT_ERROR("Start failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Start succeeded.");
  return true;
}

void NormalRpcClientModule::Shutdown() {
  try {
    if (run_flag_) {
      run_flag_ = false;
      stop_sig_.get_future().wait();
    }
  } catch (const std::exception& e) {
    AIMRT_ERROR("Shutdown failed, {}", e.what());
    return;
  }

  AIMRT_INFO("Shutdown succeeded.");
}

// Main loop
void NormalRpcClientModule::MainLoop() {
  try {
    AIMRT_INFO("Start MainLoop.");

    uint32_t count = 0;
    while (run_flag_) {
      // Sleep
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint32_t>(1000 / rpc_frq_)));

      count++;
      AIMRT_INFO("Loop count : {} -------------------------", count);

      // Create req and rsp
      my_ros2_srvs::srv::CalculateSum_Request req;
      my_ros2_srvs::srv::CalculateSum_Response rsp;

      req.set__num1(random_int(1, 10));
      req.set__num2(random_int(1, 10));

      // Create ctx
      auto ctx_ptr = proxy_->NewContextSharedPtr();
      ctx_ptr->SetTimeout(std::chrono::seconds(3));

      AIMRT_INFO("Client start new rpc call. req: {}", my_ros2_srvs::srv::to_yaml(req));

      // Call rpc
      auto status = proxy_->CalculateSum(ctx_ptr, req, rsp);

      // Check result
      if (status.OK()) {
        AIMRT_INFO("Client get rpc ret, status: {}, rsp: {}", status.ToString(),
                   my_ros2_srvs::srv::to_yaml(rsp));
      } else {
        AIMRT_WARN("Client get rpc error ret, status: {}", status.ToString());
      }
    }

    AIMRT_INFO("Exit MainLoop.");
  } catch (const std::exception& e) {
    AIMRT_ERROR("Exit MainLoop with exception, {}", e.what());
  }

  stop_sig_.set_value();
}
