#include "keyboard_module/keyboard/keyboard.h"
#include <cmath>
#include <map>
#include <stdexcept>

namespace my_robot::keyboard_module {

Keyboard::Keyboard() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw std::runtime_error("SDL init failed");
  }

  if (TTF_Init() < 0) {
    SDL_Quit();
    throw std::runtime_error("TTF init failed");
  }

  window_ =
      SDL_CreateWindow("Robot Control Interface", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       UIConfig::WINDOW_WIDTH, UIConfig::WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window_) {
    TTF_Quit();
    SDL_Quit();
    throw std::runtime_error("Window creation failed");
  }

  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer_) {
    SDL_DestroyWindow(window_);
    TTF_Quit();
    SDL_Quit();
    throw std::runtime_error("Renderer creation failed");
  }

  const char* fonts[] = {
      "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
      "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
  };
  for (const auto* path : fonts) {
    font_ = TTF_OpenFont(path, UIConfig::FONT_SIZE);
    if (font_) break;
  }

  current_event_ = KeyboardEvent::None();
  last_axis_state_.fill(0.0);

  for (const auto& btn : UIConfig::MODE_BTNS) {
    last_button_state_[btn.state_id] = false;
  }

  render();

  event_thread_ = std::thread(&Keyboard::eventThread, this);
}

Keyboard::~Keyboard() {
  running_ = false;
  if (event_thread_.joinable()) {
    event_thread_.join();
  }
  if (font_) TTF_CloseFont(font_);
  if (renderer_) SDL_DestroyRenderer(renderer_);
  if (window_) SDL_DestroyWindow(window_);
  TTF_Quit();
  SDL_Quit();
}

KeyboardEvent Keyboard::GetKeyboardEvent() {
  std::lock_guard<std::mutex> lock(mutex_);
  auto event = current_event_;
  current_event_ = KeyboardEvent::None();
  return event;
}

void Keyboard::handleMouseButton(int x, int y, bool is_down) {
  std::lock_guard<std::mutex> lock(mutex_);

  // 处理鼠标按下
  if (is_down) {
    for (const auto& btn : UIConfig::MODE_BTNS) {
      if (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h) {
        if (!last_button_state_[btn.state_id]) {
          current_event_ = KeyboardEvent::StateChange(btn.state_id, btn.text);
          last_button_state_[btn.state_id] = true;
        }
        return;
      }
    }

    for (const auto& key : UIConfig::DIR_KEYS) {
      if (x >= key.x && x <= key.x + key.w && y >= key.y && y <= key.y + key.h) {
        current_event_ =
            KeyboardEvent::VelocityChange(key.linear_x, key.linear_y, 0.0, 0.0, 0.0, key.angular_z);
        return;
      }
    }
  } else {
    for (const auto& btn : UIConfig::MODE_BTNS) {
      if (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h) {
        last_button_state_[btn.state_id] = false;
        return;
      }
    }
  }
}

void Keyboard::render() {
  if (!renderer_) return;

  SDL_SetRenderDrawColor(renderer_, 245, 245, 250, 255);
  SDL_RenderClear(renderer_);

  // 绘制方向按键
  for (const auto& k : UIConfig::DIR_KEYS) {
    SDL_Rect rect = {k.x, k.y, k.w, k.h};

    SDL_SetRenderDrawColor(renderer_, 220, 220, 220, 255);
    SDL_RenderFillRect(renderer_, &rect);

    SDL_SetRenderDrawColor(renderer_, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer_, &rect);

    if (font_) {
      SDL_Color color = {60, 60, 60, 255};

      // 按键字母
      SDL_Surface* s = TTF_RenderText_Blended(font_, k.key, color);
      if (s) {
        SDL_Texture* t = SDL_CreateTextureFromSurface(renderer_, s);
        SDL_Rect r = {k.x + (k.w - s->w) / 2, k.y + 15, s->w, s->h};
        SDL_RenderCopy(renderer_, t, nullptr, &r);
        SDL_DestroyTexture(t);
        SDL_FreeSurface(s);
      }

      // 功能标签
      TTF_SetFontSize(font_, UIConfig::LABEL_FONT_SIZE);
      s = TTF_RenderText_Blended(font_, k.label, color);
      if (s) {
        SDL_Texture* t = SDL_CreateTextureFromSurface(renderer_, s);
        SDL_Rect r = {k.x + (k.w - s->w) / 2, k.y + 45, s->w, s->h};
        SDL_RenderCopy(renderer_, t, nullptr, &r);
        SDL_DestroyTexture(t);
        SDL_FreeSurface(s);
      }
      TTF_SetFontSize(font_, UIConfig::FONT_SIZE);
    }
  }

  // 绘制模式按钮
  for (const auto& b : UIConfig::MODE_BTNS) {
    SDL_Rect rect = {b.x, b.y, b.w, b.h};

    SDL_SetRenderDrawColor(renderer_, b.color.r, b.color.g, b.color.b, 255);
    SDL_RenderFillRect(renderer_, &rect);

    SDL_SetRenderDrawColor(renderer_, 50, 50, 50, 255);
    SDL_RenderDrawRect(renderer_, &rect);

    if (font_) {
      SDL_Color white = {255, 255, 255, 255};
      SDL_Surface* s = TTF_RenderText_Blended(font_, b.text, white);
      if (s) {
        SDL_Texture* t = SDL_CreateTextureFromSurface(renderer_, s);
        SDL_Rect r = {b.x + (b.w - s->w) / 2, b.y + (b.h - s->h) / 2, s->w, s->h};
        SDL_RenderCopy(renderer_, t, nullptr, &r);
        SDL_DestroyTexture(t);
        SDL_FreeSurface(s);
      }
    }
  }

  SDL_RenderPresent(renderer_);
}

void Keyboard::eventThread() {
  while (running_) {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running_ = false;
        break;
      }

      switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
          handleMouseButton(e.button.x, e.button.y, true);
          break;
        case SDL_MOUSEBUTTONUP:
          handleMouseButton(e.button.x, e.button.y, false);
          break;
      }
    }

    {
      std::lock_guard<std::mutex> lock(mutex_);
      render();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(33));
  }
}

}  // namespace my_robot::keyboard_module