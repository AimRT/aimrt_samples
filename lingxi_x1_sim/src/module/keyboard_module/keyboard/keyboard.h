#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <array>
#include <atomic>
#include <map>
#include <mutex>
#include <optional>
#include <thread>

namespace my_robot::keyboard_module {

// 操作类型
enum class ActionType {
  NONE = 0,
  STATE_CHANGE,    // 状态切换
  VELOCITY_CHANGE  // 速度变化
};

// 状态切换操作
struct StateAction {
  int state_id;  // 0=Stand, 1=Zero, 2=Walk, 7=Idle
  const char* state_name;
};

// 速度控制操作
struct VelocityAction {
  double linear_x;   // W/S 前进后退
  double linear_y;   // A/D 左右移动
  double linear_z;   // 垂直方向
  double angular_x;  // 俯仰
  double angular_y;  // 横滚
  double angular_z;  // Q/E 左右转向
};

// 统一的键盘事件
struct KeyboardEvent {
  ActionType type = ActionType::NONE;
  std::optional<StateAction> state_action;
  std::optional<VelocityAction> velocity_action;

  static KeyboardEvent StateChange(int state_id, const char* name) {
    KeyboardEvent event;
    event.type = ActionType::STATE_CHANGE;
    event.state_action = {state_id, name};
    return event;
  }

  static KeyboardEvent VelocityChange(double lx, double ly, double lz, double ax, double ay,
                                      double az) {
    KeyboardEvent event;
    event.type = ActionType::VELOCITY_CHANGE;
    event.velocity_action = {lx, ly, lz, ax, ay, az};
    return event;
  }

  static KeyboardEvent None() { return KeyboardEvent{}; }
};

// UI配置
struct UIConfig {
  static constexpr int WINDOW_WIDTH = 900;
  static constexpr int WINDOW_HEIGHT = 500;
  static constexpr int FONT_SIZE = 20;
  static constexpr int LABEL_FONT_SIZE = 14;

  // 方向按键配置
  struct DirKey {
    int x, y, w, h;
    const char* key;
    const char* label;
    double linear_x, linear_y, angular_z;  // 对应的速度值
  };

  static constexpr DirKey DIR_KEYS[] = {
      {100, 100, 80, 70, "Q", "Turn L", 0.0, 0.0, 0.5},
      {190, 100, 80, 70, "W", "Forward", 0.5, 0.0, 0.0},
      {280, 100, 80, 70, "E", "Turn R", 0.0, 0.0, -0.5},
      {100, 180, 80, 70, "A", "Left", 0.0, 0.3, 0.0},
      {190, 180, 80, 70, "S", "Backward", -0.5, 0.0, 0.0},
      {280, 180, 80, 70, "D", "Right", 0.0, -0.3, 0.0},
  };

  // 模式按钮配置
  struct ModeBtn {
    int x, y, w, h;
    int state_id;
    const char* text;
    SDL_Color color;
  };

  static constexpr ModeBtn MODE_BTNS[] = {
      {450, 100, 130, 65, 1, "Stand", {255, 165, 0, 255}},
      {590, 100, 130, 65, 2, "Zero", {147, 112, 219, 255}},
      {450, 180, 130, 65, 3, "Walk", {70, 130, 180, 255}},
      {590, 180, 130, 65, 0, "Idle", {128, 128, 128, 255}},
  };
};

class Keyboard {
 public:
  Keyboard();
  ~Keyboard();

  Keyboard(const Keyboard&) = delete;
  Keyboard& operator=(const Keyboard&) = delete;

  // 返回操作事件
  KeyboardEvent GetKeyboardEvent();

 private:
  void eventThread();
  void handleMouseButton(int x, int y, bool is_down);
  void render();

  SDL_Window* window_{nullptr};
  SDL_Renderer* renderer_{nullptr};
  TTF_Font* font_{nullptr};

  std::thread event_thread_;
  std::atomic_bool running_{true};
  std::mutex mutex_;

  KeyboardEvent current_event_;
  std::map<int, bool> last_button_state_;
  std::array<double, 6> last_axis_state_ = {0.0};
};

}  // namespace my_robot::keyboard_module