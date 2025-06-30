// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include "aimrt_module_cpp_interface/module_base.h"

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
  auto GetLogger() { return core_.GetLogger(); }

  void EventHandle(aimrt::channel::ContextRef ctx,
                   const std::shared_ptr<const aimrt_samples::protocols::EventMsg>& data);

 private:
  aimrt::CoreRef core_;

  std::string topic_name_ = "test_topic";
  aimrt::channel::SubscriberRef subscriber_;
};
