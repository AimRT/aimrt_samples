// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include <memory>

#include "aimrt_module_cpp_interface/module_base.h"
#include "normal_server_module/service.h"

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
  aimrt::CoreRef core_;
  std::shared_ptr<CalculateServiceImpl> service_ptr_;

  std::string service_name_;
};
