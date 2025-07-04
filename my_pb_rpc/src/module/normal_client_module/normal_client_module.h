// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include <atomic>
#include <future>
#include <memory>

#include "aimrt_module_cpp_interface/module_base.h"

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
  auto GetLogger() { return core_.GetLogger(); }

  void MainLoop();

 private:
  aimrt::CoreRef core_;
  aimrt::executor::ExecutorRef executor_;

  std::atomic_bool run_flag_ = false;
  std::promise<void> stop_sig_;

  double rpc_frq_ = 0.5;

  std::shared_ptr<aimrt_samples::protocols::CalculationServiceSyncProxy> proxy_;
};
