#include "keyboard_module/keyboard/keyboard.h"
#include <iostream>

namespace my_robot::keyboard_module {

Keyboard::Keyboard() {
  printUsage();
  input_thread_ = std::thread(&Keyboard::inputLoop, this);
}

Keyboard::~Keyboard() {
  running_ = false;
  // 由于 std::cin 是阻塞的，在某些系统上可能需要按下回车才能完全退出
  if (input_thread_.joinable()) {
    input_thread_.detach();
  }
}

KeyboardEvent Keyboard::GetKeyboardEvent() {
  std::lock_guard<std::mutex> lock(mutex_);
  auto event = current_event_;
  current_event_ = KeyboardEvent::None();  // 消费掉事件
  return event;
}

void Keyboard::printUsage() {
  std::cout << "\n========== Robot CLI Control ==========\n"
            << "[W/S] Forward/Backward  [A/D] Left/Right\n"
            << "[Q/E] Turn Left/Right   [Space] Stop\n"
            << "---------------------------------------\n"
            << "[1] Stand  [2] Zero  [3] Walk  [0] Idle\n"
            << "=======================================\n"
            << "Enter command: " << std::flush;
}

void Keyboard::inputLoop() {
  char cmd;
  while (running_) {
    if (!(std::cin >> cmd)) break;

    std::lock_guard<std::mutex> lock(mutex_);
    switch (cmd) {
      // 速度控制
      case 'w':
      case 'W':
        current_event_ = KeyboardEvent::VelocityChange(0.5, 0.0, 0.0);
        break;
      case 's':
      case 'S':
        current_event_ = KeyboardEvent::VelocityChange(-0.5, 0.0, 0.0);
        break;
      case 'a':
      case 'A':
        current_event_ = KeyboardEvent::VelocityChange(0.0, 0.3, 0.0);
        break;
      case 'd':
      case 'D':
        current_event_ = KeyboardEvent::VelocityChange(0.0, -0.3, 0.0);
        break;
      case 'q':
      case 'Q':
        current_event_ = KeyboardEvent::VelocityChange(0.0, 0.0, 0.5);
        break;
      case 'e':
      case 'E':
        current_event_ = KeyboardEvent::VelocityChange(0.0, 0.0, -0.5);
        break;
      case ' ':
        current_event_ = KeyboardEvent::VelocityChange(0.0, 0.0, 0.0);
        break;

      // 状态控制
      case '1':
        current_event_ = KeyboardEvent::StateChange(1, "Stand");
        break;
      case '2':
        current_event_ = KeyboardEvent::StateChange(2, "Zero");
        break;
      case '3':
        current_event_ = KeyboardEvent::StateChange(3, "Walk");
        break;
      case '0':
        current_event_ = KeyboardEvent::StateChange(0, "Idle");
        break;

      default:
        std::cout << "Unknown command: " << cmd << "\n";
        printUsage();
        continue;
    }
    std::cout << "Command Sent: " << cmd << "\nEnter command: " << std::flush;
  }
}

}  // namespace my_robot::keyboard_module