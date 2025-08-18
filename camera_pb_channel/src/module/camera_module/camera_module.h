// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include <atomic>
#include <future>
#include <opencv2/core.hpp>
#include "aimrt_module_cpp_interface/module_base.h"

class CameraModule : public aimrt::ModuleBase {
 public:
  CameraModule() = default;
  ~CameraModule() override = default;

  // Override the following functions from ModuleBase

  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "CameraModule"};
  }

  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  auto GetLogger() { return core_.GetLogger(); }

  void MainLoop();

 private:
  aimrt::CoreRef core_;
  aimrt::executor::ExecutorRef executor_;

  std::atomic_bool run_flag_ = false;
  std::promise<void> stop_sig_;

  std::string topic_name_ = "camera_image";
  double channel_frq_ = 30;
  aimrt::channel::PublisherRef publisher_;
};
