// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.
#pragma once

#include "aimrt_module_cpp_interface/aimrt_module_cpp_interface.h"
#include "human_detector_module/utils/detector.h"
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
  void EventHandle(aimrt::channel::ContextRef ctx,
                   const std::shared_ptr<const custom_protocol::img_msg::Image>& data);

 private:
  aimrt::CoreRef core_;
  aimrt::channel::SubscriberRef subscriber_;
  aimrt::executor::ExecutorRef executor_;
  std::shared_ptr<aimrt::executor::TimerBase> timer_;

  Detector detector_;
};