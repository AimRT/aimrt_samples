// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.
#pragma once

#include <opencv2/opencv.hpp>
#include "aimrt_module_cpp_interface/module_base.h"
#include "img.pb.h"

class HumanDetectorModule : public aimrt::ModuleBase {
 public:
  HumanDetectorModule() = default;
  ~HumanDetectorModule() override = default;

  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "HumanDetectorModule"};
  }

  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  auto GetLogger() { return core_.GetLogger(); }
  void DisplayLoop();
  void EventHandle(aimrt::channel::ContextRef ctx,
                   const std::shared_ptr<const custom_protocol::img_msg::Image>& data);

 private:
  aimrt::CoreRef core_;

  std::atomic_bool run_flag_ = false;
  std::promise<void> stop_sig_;

  cv::Mat latest_image_;
  std::mutex image_mutex_;

  aimrt::channel::SubscriberRef subscriber_;
  aimrt::executor::ExecutorRef executor_;

  cv::CascadeClassifier face_cascade_;
};