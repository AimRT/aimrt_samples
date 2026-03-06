// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include <memory>
#include "aimrt_module_cpp_interface/aimrt_module_cpp_interface.h"
#include "rpc.aimrt_rpc.pb.h"

class NormalRpcServerModule : public aimrt::ModuleBase {
 public:
  NormalRpcServerModule() = default;
  ~NormalRpcServerModule() override = default;

  // Override the following functions from ModuleBase
  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "NormalRpcServerModule"};
  }

  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  std::shared_ptr<aimrt::context::Context> ctx_ptr_;
  aimrt::executor::ExecutorRef server_executor_;
};
