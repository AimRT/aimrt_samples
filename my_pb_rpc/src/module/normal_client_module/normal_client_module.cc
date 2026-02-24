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
  ctx_ptr_ = std::make_shared<aimrt::context::Context>(core);
  ctx_ptr_->LetMe();

  try {
    // Read cfg
    std::string file_path = std::string(core_.GetConfigurator().GetConfigFilePath());
    if (!file_path.empty()) {
      YAML::Node cfg_node = YAML::LoadFile(file_path);
      rpc_frq_ = cfg_node["rpc_frq"].as<double>();
    }

    // Get executor
    executor_ = ctx_ptr_->CreateExecutor("client_thread_pool");

    // Register client
    client_ = ctx_ptr_->CreateClient<aimrt_samples::protocols::CalculationServiceCoClient>();

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Init succeeded.");

  return true;
}

bool NormalRpcClientModule::Start() {
  try {
    // Start main loop in executor_
    executor_.Execute([this]() {ctx_ptr_->LetMe();MainLoop(); });
  } catch (const std::exception& e) {
    AIMRT_ERROR("Start failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Start succeeded.");
  return true;
}

void NormalRpcClientModule::Shutdown() {
  ctx_ptr_->StopRunning();

  AIMRT_INFO("Shutdown succeeded.");
}

// Main loop
void NormalRpcClientModule::MainLoop() {
  try {
    uint32_t count = 0;
    while (ctx_ptr_->Running()) {
      // Sleep
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint32_t>(1000 / rpc_frq_)));

      count++;
      AIMRT_INFO("Loop count : {} -------------------------", count);

      // Create req and rsp
      aimrt_samples::protocols::CalculateSumReq req;
      aimrt_samples::protocols::CalculateSumRsp rsp;
      req.set_num1(random_int(1, 10));
      req.set_num2(random_int(1, 10));

      // Create ctx
      auto ctx_ptr = proxy_->NewContextSharedPtr();
      ctx_ptr->SetTimeout(std::chrono::seconds(3));

      AIMRT_INFO("Client start new rpc call. req: {}", aimrt::Pb2CompactJson(req));

      // Call rpc
      auto status = proxy_->CalculateSum(ctx_ptr, req, rsp);

      // Check result
      if (status.OK()) {
        AIMRT_INFO("Client get rpc ret, status: {}, rsp: {}", status.ToString(),
                   aimrt::Pb2CompactJson(rsp));
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
