// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include "aimrt_module_cpp_interface/aimrt_module_cpp_interface.h"

#include "event.pb.h"

class NormalSubscriberModule : public aimrt::ModuleBase {
 public:
  NormalSubscriberModule() = default;
  ~NormalSubscriberModule() override = default;

  // Override the following functions from ModuleBase
  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "NormalSubscriberModule"};
  }

  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  void EventHandle(aimrt::channel::ContextRef ctx,
                   const std::shared_ptr<const aimrt_samples::protocols::EventMsg>& data);

 private:
  std::shared_ptr<aimrt::context::Context> ctx_ptr_;

  std::string topic_name_ = "test_topic";
  aimrt::context::res::Subscriber<aimrt_samples::protocols::EventMsg> subscriber_;
  aimrt::executor::ExecutorRef executor_;
};
