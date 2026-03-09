#include "control_module/control_module.h"
#include "aimrt_module_cpp_interface.h"

namespace my_robot::rl_control_module {

bool ControlModule::Initialize(aimrt::CoreRef core) {
  // Save aimrt framework handle
  ctx_ptr_ = std::make_shared<aimrt::context::Context>(core);
  ctx_ptr_->LetMe();

  auto file_path = ctx_ptr_->GetConfigFilePath();
  try {
    if (!file_path.empty()) {
      // 解析用户自定义模块配置文件
      YAML::Node cfg_node = YAML::LoadFile(file_path.data());

      // 解析控制频率
      freq_ = cfg_node["control_frequecy"].as<int32_t>();
      imu_topic_ = cfg_node["sub_imu_data_name"].as<std::string>();
      joint_state_topic_ = cfg_node["sub_joint_state_name"].as<std::string>();
      joint_cmd_topic_ = cfg_node["pub_joint_cmd_name"].as<std::string>();

      // 解析状态机
      state_machine_.Init(cfg_node["robot_states"]);

      // 解析控制器
      for (auto iter = cfg_node["controllers"].begin(); iter != cfg_node["controllers"].end();
           iter++) {
        auto controller_name = iter->first.as<std::string>();

        if (controller_name.substr(0, 3) == "rl_") {
          controller_map_[controller_name] = std::make_shared<RLController>();
        } else if (controller_name.substr(0, 3) == "pd_") {
          controller_map_[controller_name] = std::make_shared<PDController>();
        } else {
          AIMRT_ERROR("Unknown controller type: {}", controller_name);
        }
        controller_map_[controller_name]->Init(iter->second);
      }

      // 解析 joint_list、joint_offset
      for (const auto& joint : cfg_node["joint_list"]) {
        joint_state_index_map_[joint.as<std::string>()] = -1;
      }
      auto joint_list = cfg_node["joint_list"].as<std::vector<std::string>>();
      for (size_t ii = 0; ii < joint_list.size(); ++ii) {
        joint_cmd_index_map_[joint_list[ii]] = ii;
      }
      joint_offset_map_ = cfg_node["joint_offset"].as<std::map<std::string, double>>();

      // 注册 rpc 服务
      server_handle = ctx_ptr_->CreateServer<robot_control::RobotControlServiceCoServer>();

      server_handle->SetState.ServeInline([this](auto&& PH1, auto&& PH2, auto&& PH3) {
        return SetState(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2),
                        std::forward<decltype(PH3)>(PH3));
      });

      server_handle->GetState.ServeInline([this](auto&& PH1, auto&& PH2, auto&& PH3) {
        return GetState(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2),
                        std::forward<decltype(PH3)>(PH3));
      });

      server_handle->SetVelocity.ServeInline([this](auto&& PH1, auto&& PH2, auto&& PH3) {
        return SetVelocity(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2),
                           std::forward<decltype(PH3)>(PH3));
      });

      server_handle->GetVelocity.ServeInline([this](auto&& PH1, auto&& PH2, auto&& PH3) {
        return GetVelocity(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2),
                           std::forward<decltype(PH3)>(PH3));
      });

      // imu 信息订阅以及回调绑定
      imu_data_sub_ = ctx_ptr_->CreateSubscriber<sensor_msgs::msg::Imu>(
          imu_topic_, [this](auto&& PH1) { ImuDataCallback(std::forward<decltype(PH1)>(PH1)); });

      // 关节状态信息订阅以及回调绑定
      joint_state_sub = ctx_ptr_->CreateSubscriber<aimrt::protocols::sensor::JointStateArray>(
          joint_state_topic_,
          [this](auto&& PH1) { JointStateCallback(std::forward<decltype(PH1)>(PH1)); });

      // 发布关节指令
      joint_cmd_pub_ =
          ctx_ptr_->CreatePublisher<aimrt::protocols::sensor::JointCommandArray>(joint_cmd_topic_);

      // 获取执行器句柄
      executor_ = ctx_ptr_->CreateExecutor("rl_control_pub_thread");

      // 将定时器绑定到某个执行器上
      timer_ = aimrt::executor::CreateTimer(executor_, std::chrono::milliseconds(1000 / freq_),
                                            std::move([this] {
                                              ctx_ptr_->LetMe();
                                              JointCmdPublishTask();
                                            }),
                                            false);
    }
  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Init succeeded.");
  return true;
}

bool ControlModule::Start() {
  AIMRT_INFO("thread safe [{}]", executor_.ThreadSafe());
  try {
    timer_->Reset();
    AIMRT_INFO("Started succeeded.");
  } catch (const std::exception& e) {
    AIMRT_ERROR("Start failed, {}", e.what());
    return false;
  }
  return true;
}

void ControlModule::Shutdown() {
  try {
    timer_->Cancel();
    ctx_ptr_->StopRunning();
    AIMRT_INFO("Started succeeded.");
  } catch (const std::exception& e) {
    AIMRT_ERROR("Start failed, {}", e.what());
  }
}

bool ControlModule::JointCmdPublishTask() {
  try {
    static int a = 0;
    static aimrt::protocols::sensor::JointCommandArray cmd_msg;

    // 预分配空间
    for (size_t i = 0; i < joint_cmd_index_map_.size() && a == 0; ++i) {
      cmd_msg.add_joints();
    }
    a = 1;

    auto controller_names = state_machine_.GetCurrentControllerNames();
    for (const auto& name : controller_names) {
      controller_map_[name]->Update();
      aimrt::protocols::sensor::JointCommandArray tmp_cmd =
          controller_map_[name]->GetJointCmdData();

      for (int ii = 0; ii < tmp_cmd.joints_size(); ii++) {
        const auto& joint = tmp_cmd.joints(ii);
        int index = joint_cmd_index_map_[joint.name().c_str()];

        auto* target_joint = cmd_msg.mutable_joints(index);
        target_joint->set_name(joint.name());
        target_joint->set_position(joint.position() + joint_offset_map_[joint.name()]);
        target_joint->set_velocity(joint.velocity());
        target_joint->set_effort(joint.effort());
        target_joint->set_damping(joint.damping());
        target_joint->set_stiffness(joint.stiffness());
      }

      joint_cmd_pub_.Publish(cmd_msg);
    }
  } catch (const std::exception& e) {
    AIMRT_ERROR("Exit MainLoop with exception, {}", e.what());
    return false;
  }
  return true;
}

void ControlModule::ImuDataCallback(const std::shared_ptr<const sensor_msgs::msg::Imu>& msg) {
  auto controller_names = state_machine_.GetCurrentControllerNames();
  for (const auto& name : controller_names) {
    controller_map_[name]->SetImuData(*msg);
  }
}
void ControlModule::JointStateCallback(
    const std::shared_ptr<const aimrt::protocols::sensor::JointStateArray>& msg) {
  if (joint_state_index_map_.begin()->second == -1) {
    for (size_t i = 0; i < msg->joints_size(); i++) {
      joint_state_index_map_[msg->joints(i).name()] = i;
    }
  }

  // 新设置的 offset
  aimrt::protocols::sensor::JointStateArray temp_msg = *msg;

  // 应用关节偏移
  for (const auto& joint : joint_offset_map_) {
    int idx = joint_state_index_map_.at(joint.first);
    auto* joint_state = temp_msg.mutable_joints(idx);
    joint_state->set_position(joint_state->position() - joint.second);
  }

  // 更新控制器数据
  for (const auto& controller : controller_map_) {
    controller.second->SetJointStateData(temp_msg, joint_state_index_map_);
  }
}

aimrt::rpc::Status ControlModule::SetState(aimrt::rpc::ContextRef ctx,
                                           const robot_control::StateSetRequest& req,
                                           robot_control::StateSetResponse& rsp) {
  auto target_state = req.target_state();

  if (state_machine_.ChangeState(target_state)) {
    auto controller_names = state_machine_.GetCurrentControllerNames();

    for (const auto& name : controller_names) {
      controller_map_[name]->RestartController();
    }

    rsp.set_code(0);
    rsp.set_message("OK");
  } else {
    rsp.set_code(1);
    rsp.set_message("Invalid state transition");
  }

  return {};
}
// 状态查询实现
aimrt::rpc::Status ControlModule::GetState(aimrt::rpc::ContextRef ctx,
                                           const robot_control::StateQueryRequest& req,
                                           robot_control::StateQueryResponse& rsp) {
  auto current_state = state_machine_.GetCurrentStateEnum();

  rsp.mutable_state()->set_current_state(current_state);
  rsp.set_code(0);
  rsp.set_message("OK");

  return {};
}

// 速度设置实现
aimrt::rpc::Status ControlModule::SetVelocity(aimrt::rpc::ContextRef ctx,
                                              const robot_control::VelocitySetRequest& req,
                                              robot_control::VelocitySetResponse& rsp) {
  const auto& velocity = req.velocity();

  // 更新当前速度状态
  current_velocity_ = velocity;

  // 将速度命令传递给当前活跃的控制器
  auto controller_names = state_machine_.GetCurrentControllerNames();
  for (const auto& name : controller_names) {
    controller_map_[name]->SetTarget(velocity);
  }

  rsp.set_code(0);
  rsp.set_message("OK");

  return {};
}

// 速度查询实现
aimrt::rpc::Status ControlModule::GetVelocity(aimrt::rpc::ContextRef ctx,
                                              const robot_control::VelocityQueryRequest& req,
                                              robot_control::VelocityQueryResponse& rsp) {
  // 获取当前速度状态

  *rsp.mutable_velocity() = current_velocity_;

  rsp.set_code(0);
  rsp.set_message("OK");

  return {};
}

}  // namespace my_robot::rl_control_module
