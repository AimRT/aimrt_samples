#include "control_module/controller/pd_controller.h"
#include <fstream>
#include <iostream>

namespace my_robot::rl_control_module {

PDController::PDController(const bool use_sim_handles) : ControllerBase(use_sim_handles) {}

void PDController::Init(const YAML::Node& cfg_node) {
  // 初始化 joint_names_
  joint_names_.clear();
  joint_names_ = cfg_node["joint_list"].as<std::vector<std::string>>();
  // printf("joint_names_: ");
  // for (const auto &name : joint_names_) {
  //   printf("%s ", name.c_str());
  // }
  // printf("\n");
  joint_state_data_.clear_joints();
  for (size_t i = 0; i < joint_names_.size(); ++i) {
    auto* joint = joint_state_data_.add_joints();
    joint->set_name(joint_names_[i]);
    joint->set_position(0.0);
    joint->set_velocity(0.0);
    joint->set_effort(0.0);
  }

  // start_joint_angles_ 保持不变，因为它是普通的 vector
  start_joint_angles_.resize(joint_names_.size(), 0.0);

  // 初始化 joint_conf_
  joint_conf_.init_state =
      Eigen::Map<vector_t>(cfg_node["init_state"].as<std::vector<double>>().data(),
                           cfg_node["init_state"].as<std::vector<double>>().size());
  joint_conf_.stiffness =
      Eigen::Map<vector_t>(cfg_node["stiffness"].as<std::vector<double>>().data(),
                           cfg_node["stiffness"].as<std::vector<double>>().size());
  joint_conf_.damping = Eigen::Map<vector_t>(cfg_node["damping"].as<std::vector<double>>().data(),
                                             cfg_node["damping"].as<std::vector<double>>().size());
  // std::cout << "init_state: " << joint_conf_.init_state.transpose() << std::endl;
  // std::cout << "stiffness: " << joint_conf_.stiffness.transpose() << std::endl;
  // std::cout << "damping: " << joint_conf_.damping.transpose() << std::endl;

  // 其他 PD 参数
  if (cfg_node["is_keep_controller"]) {
    is_keep_controller_ = cfg_node["is_keep_controller"].as<bool>();
  }

  if (cfg_node["plan_conf"]) {
    is_plan_controller_ = true;
    to_interpolate_data_ =
        cfg_node["plan_conf"]["trajectory_interpolator"].as<std::vector<std::vector<double>>>();
    to_interpolate_data_.insert(to_interpolate_data_.begin(),
                                std::vector<double>{0.0});  // 在开头和结尾插入两个空的数组
    to_interpolate_data_.push_back(std::vector<double>{0.0});
  }
}

void PDController::RestartController() {
  {
    std::lock_guard<std::shared_mutex> lock(joint_state_mutex_);
    for (size_t i = 0; i < joint_state_data_.joints_size(); ++i) {
      start_joint_angles_[i] = joint_state_data_.joints(i).position();
    }
  }
  trans_mode_percent_ = 0.0;

  if (is_plan_controller_) {
    // 在开头和结尾插入当前时刻的关节角度
    std::vector<double> temp_data = start_joint_angles_;
    temp_data.insert(temp_data.begin(), 1.0);  // 1.0 表示插值时间
    to_interpolate_data_[0] = temp_data;
    to_interpolate_data_.back() = temp_data;
    trajectory_generator_.Init(to_interpolate_data_);
  }
}

void PDController::Update() {
  if (is_keep_controller_) {
    return;
  } else if (is_plan_controller_) {
    // trajectory_generator_.GetNextPoint(generated_joint_angles_);
    trajectory_generator_.GetNextPoint(start_joint_angles_);
    return;
  }

  trans_mode_percent_ += 1.0 / trans_mode_duration_s_ / 1000;
  trans_mode_percent_ = std::min(trans_mode_percent_, scalar_t(1));
}

aimrt::protocols::sensor::JointCommandArray PDController::GetJointCmdData() {
  aimrt::protocols::sensor::JointCommandArray joint_cmd;

  // 初始化所有关节
  for (const auto& name : joint_names_) {
    auto* joint = joint_cmd.add_joints();
    joint->set_name(name);
    joint->set_position(0.0);
    joint->set_velocity(0.0);
    joint->set_effort(0.0);
    joint->set_damping(0.0);
    joint->set_stiffness(0.0);
  }

  for (int ii = 0; ii < joint_names_.size(); ii++) {
    double pos_des;
    if (is_keep_controller_) {
      pos_des = start_joint_angles_[ii];
    } else if (is_plan_controller_) {
      // pos_des = generated_joint_angles_[ii];
      pos_des = start_joint_angles_[ii];
    } else {
      pos_des = start_joint_angles_[ii] * (1 - trans_mode_percent_) +
                joint_conf_.init_state(ii) * trans_mode_percent_;
    }

    // 获取当前关节的可修改引用并设置值
    auto* joint = joint_cmd.mutable_joints(ii);
    joint->set_position(pos_des);
    joint->set_velocity(0.0);
    joint->set_effort(0.0);
    joint->set_stiffness(joint_conf_.stiffness(ii));
    joint->set_damping(joint_conf_.damping(ii));
  }

  return joint_cmd;
}
}  // namespace my_robot::rl_control_module
