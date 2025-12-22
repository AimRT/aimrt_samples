// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include <opencv2/opencv.hpp>
#include "aimrt_module_cpp_interface/aimrt_module_cpp_interface.h"

class HalCameraModule : public aimrt::ModuleBase {
 public:
  HalCameraModule() = default;
  ~HalCameraModule() override = default;

  // Override the following functions from ModuleBase

  aimrt::ModuleInfo Info() const override { return aimrt::ModuleInfo{.name = "HalCameraModule"}; }

  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  auto GetLogger() { return core_.GetLogger(); }
  void MainTask();

 private:
  aimrt::CoreRef core_;

  aimrt::channel::PublisherRef publisher_;

  aimrt::executor::ExecutorRef executor_;
  std::shared_ptr<aimrt::executor::TimerBase> timer_;

  std::unique_ptr<cv::VideoCapture> camera_ptr_;
};
