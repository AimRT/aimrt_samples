// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include "aimrt_module_cpp_interface/module_base.h"

#include "image.pb.h"

class ImageRecognitionModule : public aimrt::ModuleBase {
 public:
  ImageRecognitionModule() = default;
  ~ImageRecognitionModule() override = default;

  // Override the following functions from ModuleBase
  aimrt::ModuleInfo Info() const override {
    return aimrt::ModuleInfo{.name = "ImageRecognitionModule"};
  }

  bool Initialize(aimrt::CoreRef core) override;
  bool Start() override;
  void Shutdown() override;

 private:
  auto GetLogger() { return core_.GetLogger(); }

  void EventHandle(aimrt::channel::ContextRef ctx,
                   const std::shared_ptr<const aimrt_samples::protocols::Image>& data);

 private:
  aimrt::CoreRef core_;

  std::string topic_name_ = "camera_image";
  aimrt::channel::SubscriberRef subscriber_;
};
