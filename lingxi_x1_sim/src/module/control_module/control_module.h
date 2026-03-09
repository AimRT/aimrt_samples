#pragma once

#include "aimrt_module_cpp_interface.h"
#include "aimrt_module_cpp_interface/executor/timer.h"
#include "aimrt_module_cpp_interface/module_base.h"
#include "aimrt_module_protobuf_interface/channel/protobuf_channel.h"
#include "aimrt_module_ros2_interface/channel/ros2_channel.h"
#include "control_module/controller/pd_controller.h"
#include "control_module/controller/rl_controller.h"
#include "control_module/state_machine/state_machine.h"
#include "joint_state.pb.h"
#include "robot_control.aimrt_rpc.pb.h"

namespace my_robot::rl_control_module {
class ControlModule : public aimrt::ModuleBase {
 public:
  ControlModule() = default;
  ~ControlModule() override = default;
  aimrt::ModuleInfo Info() const override { return aimrt::ModuleInfo{.name = "ControlModule"}; }
  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  bool JointCmdPublishTask();

  void ImuDataCallback(const std::shared_ptr<const sensor_msgs::msg::Imu>& msg);
  void JointStateCallback(
      const std::shared_ptr<const aimrt::protocols::sensor::JointStateArray>& msg);

  aimrt::rpc::Status SetState(aimrt::rpc::ContextRef ctx, const robot_control::StateSetRequest& req,
                              ::robot_control::StateSetResponse& rsp);

  aimrt::rpc::Status GetState(aimrt::rpc::ContextRef ctx,
                              const robot_control::StateQueryRequest& req,
                              ::robot_control::StateQueryResponse& rsp);

  aimrt::rpc::Status SetVelocity(aimrt::rpc::ContextRef ctx,
                                 const robot_control::VelocitySetRequest& req,
                                 ::robot_control::VelocitySetResponse& rsp);

  aimrt::rpc::Status GetVelocity(aimrt::rpc::ContextRef ctx,
                                 const robot_control::VelocityQueryRequest& req,
                                 ::robot_control::VelocityQueryResponse& rsp);

 private:
  // 模块配置基础信息
  int32_t freq_;
  std::string imu_topic_;
  std::string joint_state_topic_;
  std::string joint_cmd_topic_;

  std::shared_ptr<aimrt::context::Context> ctx_ptr_;
  aimrt::executor::ExecutorRef executor_;
  std::shared_ptr<aimrt::executor::TimerBase> timer_;

  aimrt::context::res::Publisher<aimrt::protocols::sensor::JointCommandArray> joint_cmd_pub_;
  aimrt::context::res::Subscriber<sensor_msgs::msg::Imu> imu_data_sub_;
  aimrt::context::res::Subscriber<aimrt::protocols::sensor::JointStateArray> joint_state_sub;
  std::shared_ptr<robot_control::RobotControlServiceCoServer> server_handle;

  StateMachine state_machine_;
  std::map<std::string, std::shared_ptr<ControllerBase>> controller_map_;

  robot_control::VelocityCommand current_velocity_;

  std::unordered_map<std::string, int> joint_state_index_map_;
  std::unordered_map<std::string, int> joint_cmd_index_map_;
  std::map<std::string, double> joint_offset_map_;
};

}  // namespace my_robot::rl_control_module
