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

  // 读取模块自定义配置文件，并赋值给成员变量
  std::string file_path = std::string(ctx_ptr_->GetConfigFilePath());
  if (!file_path.empty()) {
    YAML::Node cfg_node = YAML::LoadFile(file_path);
    rpc_frq_ = cfg_node["rpc_frq"].as<double>();
  }

  // 始化执行器
  client_executor_ = ctx_ptr_->CreateExecutor("client_thread_pool");

  // 初始化client
  client_ = ctx_ptr_->CreateClient<aimrt_samples::protocols::CalculationServiceCoClient>();

  return true;
}

bool NormalRpcClientModule::Start() {
  auto scheduler = aimrt::co::AimRTScheduler(client_executor_);
  scope_.spawn(aimrt::co::On(scheduler, MainLoop()));

  return true;
}

void NormalRpcClientModule::Shutdown() {
  ctx_ptr_->StopRunning();
}

// Main loop
aimrt::co::Task<void> NormalRpcClientModule::MainLoop() {
  while (ctx_ptr_->Running()) {
    ctx_ptr_->LetMe();
    // Create req and rsp
    aimrt_samples::protocols::CalculateSumReq req;
    aimrt_samples::protocols::CalculateSumRsp rsp;
    req.set_num1(random_int(1, 10));
    req.set_num2(random_int(1, 10));

    AIMRT_INFO("[Client] Client start new rpc call. req: {}", aimrt::Pb2CompactJson(req));

    // Call rpc
    auto status = co_await client_.CalculateSum(req, rsp);

    // Check result
    if (status.OK()) {
      AIMRT_INFO("[Client] Client get rpc ret, status: {}, rsp: {}", status.ToString(),
                 aimrt::Pb2CompactJson(rsp));
    } else {
      AIMRT_WARN("[Client] Client get rpc error ret, status: {}", status.ToString());
    }

    // Sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint32_t>(1000 / rpc_frq_)));
  }
  co_return;
}
