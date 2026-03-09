#pragma once
#include <yaml-cpp/yaml.h>
#include <map>
#include <shared_mutex>
#include <string>
#include <vector>
#include "robot_control.pb.h"

namespace my_robot::rl_control_module {

struct State {
  std::string name;
  std::vector<std::string> pre_states;
  std::vector<std::string> controllers;

  State() = default;
  State(std::string_view name, const YAML::Node& node) : name(name) {
    pre_states = node["pre_states"].as<std::vector<std::string>>();
    controllers = node["controllers"].as<std::vector<std::string>>();
  }
};

class StateMachine {
 public:
  StateMachine() = default;
  ~StateMachine() = default;

  void Init(const YAML::Node& node);
  bool ChangeState(robot_control::RobotState::State state_enum);
  robot_control::RobotState::State GetCurrentStateEnum() const;
  std::vector<std::string> GetCurrentControllerNames() const;

 private:
  const std::map<robot_control::RobotState::State, std::string> state_to_name_ = {
      {robot_control::RobotState::IDLE, "idle"},
      {robot_control::RobotState::STAND, "stand"},
      {robot_control::RobotState::ZERO, "zero"},
      {robot_control::RobotState::WALK, "walk_leg"},
      {robot_control::RobotState::KEEP, "keep"}};
  std::map<std::string, State> states_;
  std::string current_state_name_;

  mutable std::shared_mutex mutex_;
  std::vector<std::string> controllers_;
};

}  // namespace my_robot::rl_control_module