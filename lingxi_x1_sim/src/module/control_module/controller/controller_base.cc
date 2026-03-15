#include "control_module/controller/controller_base.h"

namespace my_robot::rl_control_module {

ControllerBase::ControllerBase(const bool use_sim_handles) : use_sim_handles_(use_sim_handles) {}

void ControllerBase::SetTarget(const robot_control::VelocityCommand& joy_data) {
  std::lock_guard<std::shared_mutex> lock(joy_mutex_);
  joy_data_ = joy_data;
}

void ControllerBase::SetImuData(const aimrt::protocols::sensor::ImuState& imu_data) {
  std::lock_guard<std::shared_mutex> lock(imu_mutex_);
  imu_data_ = imu_data;
}

void ControllerBase::SetJointStateData(
    const aimrt::protocols::sensor::JointStateArray& joint_state_data,
    const std::unordered_map<std::string, int>& joint_state_index_map_) {
  std::lock_guard<std::shared_mutex> lock(joint_state_mutex_);

  for (size_t ii = 0; ii < joint_names_.size(); ++ii) {
    // 获取映射的索引
    int idx = joint_state_index_map_.at(joint_names_[ii]);

    // 从输入的 JointStateArray 中获取对应索引的关节数据
    const auto& joint = joint_state_data.joints(idx);

    // 更新到成员变量中
    joint_state_data_.mutable_joints(ii)->set_position(joint.position());
    joint_state_data_.mutable_joints(ii)->set_velocity(joint.velocity());
    joint_state_data_.mutable_joints(ii)->set_effort(joint.effort());

    // 同时设置关节名称（如果需要的话）
    joint_state_data_.mutable_joints(ii)->set_name(joint_names_[ii]);
  }
}
std::vector<std::string> ControllerBase::GetJointList() { return joint_names_; }

}  // namespace my_robot::rl_control_module
