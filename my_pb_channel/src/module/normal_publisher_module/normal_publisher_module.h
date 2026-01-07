// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include "aimrt_module_cpp_interface/aimrt_module_cpp_interface.h"
#include "event.pb.h"

class NormalPublisherModule : public aimrt::ModuleBase {
 public:
  NormalPublisherModule() = default;
  ~NormalPublisherModule() override = default;

  // Override the following functions from ModuleBase
  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "NormalPublisherModule"};
  }

  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  void MainLoop();

 private:
  std::shared_ptr<aimrt::context::Context> ctx_ptr_;

  aimrt::executor::ExecutorRef executor_;

  std::string topic_name_ = "test_topic";
  double channel_frq_ = 0.5;

  aimrt::context::res::Publisher<aimrt_samples::protocols::EventMsg> publisher_;
};
