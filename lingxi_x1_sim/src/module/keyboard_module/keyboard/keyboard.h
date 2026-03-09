#pragma once

#include <atomic>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

namespace my_robot::keyboard_module {

// 操作类型
enum class ActionType {
  NONE = 0,
  STATE_CHANGE,
  VELOCITY_CHANGE
};

// 状态切换操作
struct StateAction {
  int state_id;
  const char* state_name;
};

// 速度控制操作
struct VelocityAction {
  double linear_x = 0.0;
  double linear_y = 0.0;
  double angular_z = 0.0;
};

// 统一的键盘事件
struct KeyboardEvent {
  ActionType type = ActionType::NONE;
  std::optional<StateAction> state_action;
  std::optional<VelocityAction> velocity_action;

  static KeyboardEvent StateChange(int id, const char* name) {
    KeyboardEvent ev;
    ev.type = ActionType::STATE_CHANGE;
    ev.state_action = {id, name};
    return ev;
  }

  static KeyboardEvent VelocityChange(double lx, double ly, double az) {
    KeyboardEvent ev;
    ev.type = ActionType::VELOCITY_CHANGE;
    ev.velocity_action = {lx, ly, az};
    return ev;
  }

  static KeyboardEvent None() { return {}; }
};

class Keyboard {
 public:
  Keyboard();
  ~Keyboard();

  // 禁止拷贝
  Keyboard(const Keyboard&) = delete;
  Keyboard& operator=(const Keyboard&) = delete;

  // 获取并消耗当前缓存的事件
  KeyboardEvent GetKeyboardEvent();

 private:
  void inputLoop();  // 替代原有的事件循环
  void printUsage();

  std::thread input_thread_;
  std::atomic_bool running_{true};
  std::mutex mutex_;
  KeyboardEvent current_event_;
};

}  // namespace my_robot::keyboard_module