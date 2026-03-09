// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "keyboard_module/keyboard_module.h"
#include <yaml-cpp/yaml.h>

namespace my_robot::keyboard_module {

bool KeyboardModule::Initialize(aimrt::CoreRef core) {
  core_ = core;

  try {
    auto file_path = core_.GetConfigurator().GetConfigFilePath();
    if (file_path.empty()) {
      AIMRT_ERROR("Config file path is empty");
      return false;
    }

    // 解析配置文件
    YAML::Node cfg_node = YAML::LoadFile(file_path.data());

    // 从框架中注册资源获取资源句柄
    // 获取执行器句柄
    executor_ = core_.GetExecutorManager().GetExecutor("joy_stick_pub_thread");
    AIMRT_CHECK_ERROR_THROW(executor_, "Cannot get executor 'joy_stick_pub_thread'.");

    // 将定时器绑定到某个执行器上
    timer_ = aimrt::executor::CreateTimer(executor_, std::chrono::milliseconds(100),
                                          std::move([this] { MainTask(); }), false);

    // 注册 rpc 并获取RPC句柄
    auto rpc_handle = core_.GetRpcHandle();
    bool ret = robot_control::RegisterRobotControlServiceClientFunc(rpc_handle);
    proxy_ = std::make_shared<robot_control::RobotControlServiceSyncProxy>(rpc_handle);

    AIMRT_INFO("Init succeeded.");
    return true;

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed: {}", e.what());
    return false;
  }
}

bool KeyboardModule::Start() {
  timer_->Reset();
  AIMRT_INFO("Started succeeded.");
  return true;
}

void KeyboardModule::Shutdown() {
  try {
    timer_->Cancel();
    AIMRT_INFO("Started succeeded.");
  } catch (const std::exception& e) {
    AIMRT_ERROR("Start failed, {}", e.what());
  }
}

void KeyboardModule::MainTask() {
  auto event = keyboard_.GetKeyboardEvent();

  switch (event.type) {
    case ActionType::STATE_CHANGE: {
      // 状态切换
      robot_control::StateSetRequest req;
      robot_control::StateSetResponse rsp;
      req.set_target_state(
          static_cast<robot_control::RobotState::State>(event.state_action->state_id));

      if (proxy_->SetState(req, rsp).OK() && rsp.code() == 0) {
        AIMRT_INFO("State changed: {}", event.state_action->state_name);
      } else {
        AIMRT_WARN("State change failed: {} (code: {})", event.state_action->state_name,
                   rsp.code());
      }
      break;
    }

    case ActionType::VELOCITY_CHANGE: {
      // 速度控制
      robot_control::VelocitySetRequest req;
      robot_control::VelocitySetResponse rsp;

      auto* velocity = req.mutable_velocity();
      velocity->set_linear_x(event.velocity_action->linear_x);
      velocity->set_linear_y(event.velocity_action->linear_y);
      velocity->set_angular_z(event.velocity_action->angular_z);

      if (proxy_->SetVelocity(req, rsp).OK() && rsp.code() == 0) {
        AIMRT_INFO("Velocity: linear=[{:.2f}, {:.2f}, {:.2f}], angular=[{:.2f}, {:.2f}, {:.2f}]",
                   velocity->linear_x(), velocity->linear_y(), velocity->linear_z(),
                   velocity->angular_x(), velocity->angular_y(), velocity->angular_z());
      } else {
        AIMRT_WARN("Velocity set failed (code: {})", rsp.code());
      }
      break;
    }

    case ActionType::NONE:
      break;
  }
}

}  // namespace my_robot::keyboard_module