#include "control_module/state_machine/state_machine.h"

namespace my_robot::rl_control_module {

void StateMachine::Init(const YAML::Node& node) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  states_.clear();

  for (const auto& item : node) {
    states_[item.first.as<std::string>()] = State(item.first.as<std::string>(), item.second);
  }

  if (!states_.empty()) {
    current_state_name_ = states_.begin()->first;
    controllers_ = states_.begin()->second.controllers;
  }
}

bool StateMachine::ChangeState(robot_control::RobotState::State state_enum) {
  std::unique_lock<std::shared_mutex> lock(mutex_);

  auto it = state_to_name_.find(state_enum);
  if (it == state_to_name_.end()) return false;

  const std::string& target_name = it->second;
  auto state_it = states_.find(target_name);
  if (state_it == states_.end()) return false;

  const State& target_state = state_it->second;
  auto pre_it = std::find(target_state.pre_states.begin(), target_state.pre_states.end(),
                          current_state_name_);

  if (pre_it != target_state.pre_states.end()) {
    current_state_name_ = target_name;
    controllers_ = target_state.controllers;
    return true;
  }
  return false;
}

robot_control::RobotState::State StateMachine::GetCurrentStateEnum() const {
  std::shared_lock<std::shared_mutex> lock(mutex_);

  for (const auto& [enum_val, name] : state_to_name_) {
    if (name == current_state_name_) {
      return enum_val;
    }
  }
  return robot_control::RobotState::IDLE;
}

std::vector<std::string> StateMachine::GetCurrentControllerNames() const {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  return controllers_;
}

}  // namespace my_robot::rl_control_module