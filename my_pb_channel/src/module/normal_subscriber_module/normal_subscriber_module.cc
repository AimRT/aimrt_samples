// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "normal_subscriber_module/normal_subscriber_module.h"
#include "aimrt_module_protobuf_interface/channel/protobuf_channel.h"
#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"
#include "yaml-cpp/yaml.h"

bool NormalSubscriberModule::Initialize(aimrt::CoreRef core) {
  ctx_ptr_ = std::make_shared<aimrt::context::Context>(core);
  ctx_ptr_->LetMe();

  try {
    // Read cfg
    auto file_path = ctx_ptr_->GetConfigFilePath();
    if (!file_path.empty()) {
      YAML::Node cfg_node = YAML::LoadFile(std::string(file_path));
      topic_name_ = cfg_node["topic_name"].as<std::string>();
    }

    // Register subscriber
    subscriber_ = ctx_ptr_->CreateSubscriber<aimrt_samples::protocols::EventMsg>(topic_name_,
                                                                                 std::bind(&NormalSubscriberModule::EventHandle, this, std::placeholders::_1, std::placeholders::_2));

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }

  return true;
}

bool NormalSubscriberModule::Start() { return true; }

void NormalSubscriberModule::Shutdown() { ctx_ptr_->StopRunning(); }

void NormalSubscriberModule::EventHandle(
    aimrt::channel::ContextRef ctx,
    const std::shared_ptr<const aimrt_samples::protocols::EventMsg>& data) {
  AIMRT_INFO("Receive new pb event, ctx: {}, data: {}", ctx.ToString(), aimrt::Pb2CompactJson(*data));
}
