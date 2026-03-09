#pragma once
#include <onnxruntime/onnxruntime_cxx_api.h>
#include <atomic>
#include <memory>
#include <set>

#include "control_module/controller/controller_base.h"
#include "control_module/controller/rotation_tools.h"

namespace my_robot::rl_control_module {

class RLController : public ControllerBase {
 public:
  RLController(const bool use_sim_handles = true);
  ~RLController() = default;

  void Init(const YAML::Node &cfg_node) override;
  void RestartController() override;

  void Update() override;
  aimrt::protocols::sensor::JointCommandArray GetJointCmdData() override;

 private:
  void LoadModel();
  void UpdateStateEstimation();
  void ComputeObservation();
  void ComputeActions();

 private:
  struct WalkStepConf {
    double action_scale;
    int decimation;
    double cycle_time;
    bool sw_mode;
    double cmd_threshold;
  } walk_step_conf_;

  struct ObsScales {
    double lin_vel;
    double ang_vel;
    double dof_pos;
    double dof_vel;
    double quat;
  } obs_scales_;

  struct OnnxConf {
    std::string policy_file;
    int actions_size;
    int observations_size;
    int num_hist;
    double observations_clip;
    double actions_clip;
  } onnx_conf_;

  struct LPFConf {
    double wc;
    double ts;
    std::set<std::string> paralle_list;
  } lpf_conf_;

  // onnx
  std::unique_ptr<Ort::Session> session_ptr_;
  Ort::MemoryInfo memory_info_;
  std::vector<const char *> input_names_;
  std::vector<const char *> output_names_;
  std::vector<std::vector<int64_t>> input_shapes_;
  std::vector<std::vector<int64_t>> output_shapes_;

  // compute in algorithm
  std::vector<float> actions_;
  std::vector<float> observations_;
  vector_t last_actions_;
  // vector_t propri_history_buffer_;
  Eigen::Matrix<float, Eigen::Dynamic, 1> propri_history_buffer_;
  struct Proprioception {
    vector_t joint_pos;
    vector_t joint_vel;
    vector3_t base_ang_vel;
    vector3_t base_euler_xyz;
    vector3_t projected_gravity;
  } propri_;

  // other
  int64_t loop_count_;
  std::vector<digital_lp_filter<double>> low_pass_filters_;
  std::atomic_bool is_first_frame_{true};
};

}  // namespace my_robot::rl_control_module
