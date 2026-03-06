// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include "aimrt_module_cpp_interface/aimrt_module_cpp_interface.h"

#include "rpc.aimrt_rpc.pb.h"

class NormalRpcClientModule : public aimrt::ModuleBase {
 public:
  NormalRpcClientModule() = default;
  ~NormalRpcClientModule() override = default;

  // Override the following functions from ModuleBase
  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "NormalRpcClientModule"};
  }

  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  aimrt::co::Task<void> MainLoop();

 private:
  std::shared_ptr<aimrt::context::Context> ctx_ptr_;

  aimrt::executor::ExecutorRef client_executor_;

  aimrt_samples::protocols::CalculationServiceCoClient client_;

  // 协程作用域
  aimrt::co::AsyncScope scope_;

  double rpc_frq_ = 0.5;
};
