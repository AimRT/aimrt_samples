#include "control_module/controller/rl_controller.h"
#include <string.h>
#include <iostream>

namespace my_robot::rl_control_module {

RLController::RLController(const bool use_sim_handles)
    : ControllerBase(use_sim_handles),
      memory_info_(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault)) {}

void RLController::Init(const YAML::Node& cfg_node) {
  // 初始化 joint_names_
  joint_names_.clear();
  joint_names_ = cfg_node["joint_list"].as<std::vector<std::string>>();
  // printf("joint_names_: ");
  // for (const auto &name : joint_names_) {
  //   printf("%s ", name.c_str());
  // }
  // printf("\n");
  for (size_t i = 0; i < joint_names_.size(); ++i) {
    auto* joint = joint_state_data_.add_joints();
    joint->set_name(joint_names_[i]);
    joint->set_position(0.0);
    joint->set_velocity(0.0);
    joint->set_effort(0.0);
  }

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

  // 其他 RL 参数
  // clang-format off
  walk_step_conf_.action_scale  = cfg_node["walk_step_conf"]["action_scale"].as<double>();
  walk_step_conf_.decimation    = cfg_node["walk_step_conf"]["decimation"].as<int32_t>();
  walk_step_conf_.cycle_time    = cfg_node["walk_step_conf"]["cycle_time"].as<double>();
  walk_step_conf_.sw_mode       = cfg_node["walk_step_conf"]["sw_mode"].as<bool>();
  walk_step_conf_.cmd_threshold = cfg_node["walk_step_conf"]["cmd_threshold"].as<double>();
  obs_scales_.lin_vel           = cfg_node["obs_scales"]["lin_vel"].as<double>();
  obs_scales_.ang_vel           = cfg_node["obs_scales"]["ang_vel"].as<double>();
  obs_scales_.dof_pos           = cfg_node["obs_scales"]["dof_pos"].as<double>();
  obs_scales_.dof_vel           = cfg_node["obs_scales"]["dof_vel"].as<double>();
  obs_scales_.quat              = cfg_node["obs_scales"]["quat"].as<double>();
  onnx_conf_.policy_file        = cfg_node["onnx_conf"]["policy_file"].as<std::string>();
  onnx_conf_.actions_size       = cfg_node["onnx_conf"]["actions_size"].as<int32_t>();
  onnx_conf_.observations_size  = cfg_node["onnx_conf"]["observations_size"].as<int32_t>();
  onnx_conf_.num_hist           = cfg_node["onnx_conf"]["num_hist"].as<int32_t>();
  onnx_conf_.observations_clip  = cfg_node["onnx_conf"]["observations_clip"].as<double>();
  onnx_conf_.actions_clip       = cfg_node["onnx_conf"]["actions_clip"].as<double>();
  lpf_conf_.wc                  = cfg_node["lpf_conf"]["wc"].as<double>();
  lpf_conf_.ts                  = cfg_node["lpf_conf"]["ts"].as<double>();
  auto paralle_list = cfg_node["lpf_conf"]["paralle_list"].as<std::vector<std::string>>();
  lpf_conf_.paralle_list        = std::set<std::string>(paralle_list.begin(), paralle_list.end());
  LoadModel();
  // clang-format on
  actions_.resize(onnx_conf_.actions_size);
  observations_.resize(onnx_conf_.observations_size * onnx_conf_.num_hist);
  last_actions_.resize(onnx_conf_.actions_size);
  last_actions_.setZero();
  propri_history_buffer_.resize(onnx_conf_.observations_size * onnx_conf_.num_hist);
  loop_count_ = 0;
  low_pass_filters_.clear();
  for (size_t i = 0; i < onnx_conf_.actions_size; ++i) {
    low_pass_filters_.emplace_back(100, 0.001);
  }
  propri_.joint_pos.resize(onnx_conf_.actions_size);
  propri_.joint_vel.resize(onnx_conf_.actions_size);
}

void RLController::RestartController() { is_first_frame_ = true; }

void RLController::Update() {
  UpdateStateEstimation();
  // compute observation & actions
  if (loop_count_ % walk_step_conf_.decimation == 0) {
    ComputeObservation();
    ComputeActions();
  }
  loop_count_++;
}

aimrt::protocols::sensor::JointCommandArray RLController::GetJointCmdData() {
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

  // get action
  for (int ii = 0; ii < onnx_conf_.actions_size; ii++) {
    scalar_t pos_des = actions_[ii] * walk_step_conf_.action_scale + joint_conf_.init_state(ii);
    double stiffness = joint_conf_.stiffness(ii);
    double damping = joint_conf_.damping(ii);

    // 获取当前关节的可修改引用
    auto* joint = joint_cmd.mutable_joints(ii);

    if (lpf_conf_.paralle_list.find(joint_names_[ii]) == lpf_conf_.paralle_list.end()) {
      low_pass_filters_[ii].input(pos_des);
      double pos_des_lp = low_pass_filters_[ii].output();

      joint->set_position(pos_des_lp);
      joint->set_velocity(0.0);
      joint->set_effort(0.0);
      joint->set_stiffness(stiffness);
      joint->set_damping(damping);
    } else {
      double tau_des =
          stiffness * (pos_des - propri_.joint_pos[ii]) + damping * (0.0 - propri_.joint_vel[ii]);
      low_pass_filters_[ii].input(tau_des);
      double tau_des_lp = low_pass_filters_[ii].output();

      joint->set_position(0.0);
      joint->set_velocity(0.0);
      joint->set_effort(tau_des_lp);
      joint->set_stiffness(0.0);
      joint->set_damping(0.0);
    }
    last_actions_(ii, 0) = actions_[ii];
  }

  return joint_cmd;
}

void RLController::LoadModel() {
  // create env
  std::shared_ptr<Ort::Env> onnxEnvPrt(
      new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "LeggedOnnxController"));
  Ort::SessionOptions sessionOptions;
  sessionOptions.SetInterOpNumThreads(1);
  session_ptr_ =
      std::make_unique<Ort::Session>(*onnxEnvPrt, onnx_conf_.policy_file.c_str(), sessionOptions);

  // get input and output info
  input_names_.clear();
  output_names_.clear();
  input_shapes_.clear();
  output_shapes_.clear();

  Ort::AllocatorWithDefaultOptions allocator;
  for (size_t ii = 0; ii < session_ptr_->GetInputCount(); ++ii) {
    char* tempstring =
        new char[strlen(session_ptr_->GetInputNameAllocated(ii, allocator).get()) + 1];
    strcpy(tempstring, session_ptr_->GetInputNameAllocated(ii, allocator).get());
    input_names_.push_back(tempstring);
    input_shapes_.push_back(
        session_ptr_->GetInputTypeInfo(ii).GetTensorTypeAndShapeInfo().GetShape());
  }

  for (size_t ii = 0; ii < session_ptr_->GetOutputCount(); ++ii) {
    char* tempstring =
        new char[strlen(session_ptr_->GetOutputNameAllocated(ii, allocator).get()) + 1];
    strcpy(tempstring, session_ptr_->GetOutputNameAllocated(ii, allocator).get());
    output_names_.push_back(tempstring);
    output_shapes_.push_back(
        session_ptr_->GetOutputTypeInfo(ii).GetTensorTypeAndShapeInfo().GetShape());
  }
}

void RLController::UpdateStateEstimation() {
  {
    std::shared_lock<std::shared_mutex> lock(joint_state_mutex_);
    for (size_t ii = 0; ii < onnx_conf_.actions_size; ++ii) {
      std::string joint_name = joint_names_[ii];
      // 从新的 JointStateArray 格式中获取数据
      propri_.joint_pos(ii) = joint_state_data_.joints(ii).position();
      propri_.joint_vel(ii) = joint_state_data_.joints(ii).velocity();
    }
  }

  {
    std::shared_lock<std::shared_mutex> lock(imu_mutex_);
    propri_.base_ang_vel(0) = imu_data_.angular_velocity().x();
    propri_.base_ang_vel(1) = imu_data_.angular_velocity().y();
    propri_.base_ang_vel(2) = imu_data_.angular_velocity().z();

    vector3_t gravity_vector(0, 0, -1);
    quaternion_t quat;
    quat.x() = imu_data_.orientation().x();
    quat.y() = imu_data_.orientation().y();
    quat.z() = imu_data_.orientation().z();
    quat.w() = imu_data_.orientation().w();
    matrix_t inverse_rot = GetRotationMatrixFromZyxEulerAngles(QuatToZyx(quat)).inverse();
    propri_.projected_gravity = inverse_rot * gravity_vector;
    propri_.base_euler_xyz = QuatToXyz(quat);
  }
}

void RLController::ComputeObservation() {
  // actions
  vector_t propri_obs(onnx_conf_.observations_size);
  {
    std::shared_lock<std::shared_mutex> lock(joy_mutex_);
    double phase = duration<double>(high_resolution_clock::now().time_since_epoch()).count();
    if (walk_step_conf_.sw_mode) {
      double cmd_norm = std::sqrt(Square(joy_data_.linear_x()) + Square(joy_data_.linear_y()) +
                                  Square(joy_data_.angular_z()));
      if (cmd_norm <= walk_step_conf_.cmd_threshold) {
        phase = 0;
      }
    }
    phase = phase / walk_step_conf_.cycle_time;

    // clang-format off
    propri_obs << sin(2 * M_PI * phase),  // 1
                  cos(2 * M_PI * phase),  // 1
                  joy_data_.linear_x() * obs_scales_.lin_vel, // 1
                  joy_data_.linear_y() * obs_scales_.lin_vel, // 1
                  joy_data_.angular_z(), // 1
                  (propri_.joint_pos - joint_conf_.init_state) * obs_scales_.dof_pos, // action_size
                  propri_.joint_vel * obs_scales_.dof_vel, // action_size
                  last_actions_, // action_size
                  propri_.base_ang_vel * obs_scales_.ang_vel, // 3
                  propri_.base_euler_xyz * obs_scales_.quat; // 3
    // clang-format on
  }

  if (is_first_frame_) {
    for (size_t ii = 0; ii < joint_names_.size(); ++ii) {
      if (lpf_conf_.paralle_list.find(joint_names_[ii]) == lpf_conf_.paralle_list.end()) {
        // serial
        low_pass_filters_[ii].init(propri_.joint_pos[ii]);
      } else {
        // parallel
        low_pass_filters_[ii].init(0);
      }
    }

    // Set last_actions_ to 0
    for (int ii = 5 + onnx_conf_.actions_size * 2; ii < 5 + onnx_conf_.actions_size * 3; ++ii) {
      propri_obs(ii, 0) = 0.0;
    }

    for (int ii = 0; ii < onnx_conf_.num_hist; ++ii) {
      propri_history_buffer_.segment(ii * onnx_conf_.observations_size,
                                     onnx_conf_.observations_size) = propri_obs.cast<float>();
    }
    is_first_frame_ = false;
  }

  propri_history_buffer_.head(propri_history_buffer_.size() - onnx_conf_.observations_size) =
      propri_history_buffer_.tail(propri_history_buffer_.size() - onnx_conf_.observations_size);
  propri_history_buffer_.tail(onnx_conf_.observations_size) = propri_obs.cast<float>();

  for (int ii = 0; ii < (onnx_conf_.observations_size * onnx_conf_.num_hist); ++ii) {
    observations_[ii] = static_cast<float>(propri_history_buffer_[ii]);
  }
  // limit observations range
  scalar_t obs_min = -onnx_conf_.observations_clip;
  scalar_t obs_max = onnx_conf_.observations_clip;
  std::transform(
      observations_.begin(), observations_.end(), observations_.begin(),
      [obs_min, obs_max](scalar_t x) { return std::max(obs_min, std::min(obs_max, x)); });
}

void RLController::ComputeActions() {
  // create input tensor object
  std::vector<Ort::Value> input_tensor;
  input_tensor.push_back(
      Ort::Value::CreateTensor<float>(memory_info_, observations_.data(), observations_.size(),
                                      input_shapes_[0].data(), input_shapes_[0].size()));

  std::vector<Ort::Value> output_values = session_ptr_->Run(
      Ort::RunOptions{}, input_names_.data(), input_tensor.data(), 1, output_names_.data(), 1);

  for (int i = 0; i < onnx_conf_.actions_size; ++i) {
    actions_[i] = *(output_values[0].GetTensorMutableData<float>() + i);
  }
  // limit action range
  scalar_t action_min = -onnx_conf_.actions_clip;
  scalar_t action_max = onnx_conf_.actions_clip;
  std::transform(actions_.begin(), actions_.end(), actions_.begin(),
                 [action_min, action_max](scalar_t x) {
                   return std::max(action_min, std::min(action_max, x));
                 });
}

}  // namespace my_robot::rl_control_module