// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once
#include <eigen3/Eigen/Core>
#include "aimrt_module_cpp_interface/executor/timer.h"
#include "aimrt_module_cpp_interface/module_base.h"
#include "keyboard_module/keyboard/keyboard.h"
#include "robot_control.aimrt_rpc.pb.h"

namespace my_robot::keyboard_module {
using array_t = Eigen::Array<double, Eigen::Dynamic, 1>;

class KeyboardModule : public aimrt::ModuleBase {
 public:
  KeyboardModule() = default;
  ~KeyboardModule() override = default;

  aimrt::ModuleInfo Info() const override { return aimrt::ModuleInfo{.name = "KeyboardModule"}; }
  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  auto GetLogger() { return core_.GetLogger(); }
  void MainTask();

 private:
  aimrt::CoreRef core_;

  Keyboard keyboard_;

  aimrt::executor::ExecutorRef executor_;
  std::shared_ptr<aimrt::executor::TimerBase> timer_;

  std::shared_ptr<robot_control::RobotControlServiceSyncProxy> proxy_;
};

}  // namespace my_robot::keyboard_module