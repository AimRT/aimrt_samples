// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include "aimrt_module_cpp_interface/aimrt_module_cpp_interface.h"

class HelloWorldModule : public aimrt::ModuleBase {
 public:
  HelloWorldModule() = default;
  ~HelloWorldModule() override = default;

  // Override the following functions from ModuleBase

  aimrt::ModuleInfo Info() const override { return aimrt::ModuleInfo{.name = "HelloWorldModule"}; }

  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  void Task();
  void LoopTask();
  aimrt::co::Task<void> CoLoopTask();

 private:
  std::shared_ptr<aimrt::context::Context> ctx_ptr_;

  std::string param1_;
  int param2_;

  // 通用执行器
  aimrt::executor::ExecutorRef executor_;

  // 定时器
  std::shared_ptr<aimrt::executor::TimerBase> timer_;

  // 协程作用域
  aimrt::co::AsyncScope scope_;
};
