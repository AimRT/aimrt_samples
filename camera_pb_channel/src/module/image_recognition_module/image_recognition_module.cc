// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "image_recognition_module/image_recognition_module.h"
#include "aimrt_module_protobuf_interface/channel/protobuf_channel.h"
#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"

#include "yaml-cpp/yaml.h"

bool ImageRecognitionModule::Initialize(aimrt::CoreRef core) {
  core_ = core;

  try {
    // Read cfg
    auto file_path = core_.GetConfigurator().GetConfigFilePath();
    if (!file_path.empty()) {
      YAML::Node cfg_node = YAML::LoadFile(std::string(file_path));
      topic_name_ = cfg_node["topic_name"].as<std::string>();
    }

    // Register publish(it contains two steps)
    // Step1: get subscriber handle with topic name
    subscriber_ = core_.GetChannelHandle().GetSubscriber(topic_name_);
    AIMRT_CHECK_ERROR_THROW(subscriber_, "Get subscriber for topic '{}' failed.", topic_name_);

    // Step2: register callback function for subscriber (aimrt_samples::protocols::EventMsg is the message type)
    bool ret = aimrt::channel::Subscribe<aimrt_samples::protocols::Image>(
        subscriber_,
        std::bind(&ImageRecognitionModule::EventHandle, this, std::placeholders::_1, std::placeholders::_2));
    AIMRT_CHECK_ERROR_THROW(ret, "Subscribe failed.");

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Init succeeded.");

  return true;
}

bool ImageRecognitionModule::Start() { return true; }

void ImageRecognitionModule::Shutdown() {}

// This is the callback function for subscriber. When new message arrives, this function will be called.
// aimrt_samples::protocols::Image is the message type (you can define your own message type).
void ImageRecognitionModule::EventHandle(
    aimrt::channel::ContextRef ctx,
    const std::shared_ptr<const aimrt_samples::protocols::Image>& data) {
  AIMRT_INFO("Receive new pb event, ctx: {}, data: {}", ctx.ToString(), aimrt::Pb2CompactJson(*data));
}
