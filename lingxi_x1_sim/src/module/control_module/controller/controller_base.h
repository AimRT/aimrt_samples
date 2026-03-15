#pragma once
#include <yaml-cpp/yaml.h>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>
#include "control_module/controller/utilities.h"
#include "imu.pb.h"
#include "joint_command.pb.h"
#include "joint_state.pb.h"
#include "robot_control.pb.h"

namespace my_robot::rl_control_module {

class ControllerBase {
 public:
  explicit ControllerBase(bool use_sim_handles);
  virtual ~ControllerBase() = default;

  virtual void Init(const YAML::Node &cfg_node) = 0;
  virtual void RestartController() = 0;
  virtual void SetTarget(const robot_control::VelocityCommand &joy_data);
  virtual void SetImuData(const aimrt::protocols::sensor::ImuState &imu_data);
  virtual void SetJointStateData(
      const aimrt::protocols::sensor::JointStateArray &joint_state_data,
      const std::unordered_map<std::string, int> &joint_state_index_map_);
  virtual std::vector<std::string> GetJointList();
  virtual void Update() = 0;
  virtual aimrt::protocols::sensor::JointCommandArray GetJointCmdData() = 0;

 protected:
  bool use_sim_handles_;
  struct JointConf {
    vector_t init_state;
    vector_t stiffness;
    vector_t damping;
  } joint_conf_;

  std::vector<std::string> joint_names_;

  mutable std::shared_mutex joy_mutex_;
  mutable std::shared_mutex imu_mutex_;
  mutable std::shared_mutex joint_state_mutex_;

  robot_control::VelocityCommand joy_data_;
  aimrt::protocols::sensor::ImuState imu_data_;
  aimrt::protocols::sensor::JointStateArray joint_state_data_;
};

}  // namespace my_robot::rl_control_module
