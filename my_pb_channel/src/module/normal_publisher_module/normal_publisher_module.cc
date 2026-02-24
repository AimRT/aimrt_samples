// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "normal_publisher_module/normal_publisher_module.h"
#include "aimrt_module_protobuf_interface/channel/protobuf_channel.h"
#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"

#include "yaml-cpp/yaml.h"

bool NormalPublisherModule::Initialize(aimrt::CoreRef core) {
  ctx_ptr_ = std::make_shared<aimrt::context::Context>(core);
  ctx_ptr_->LetMe();

  try {
    // Read cfg
    auto file_path = ctx_ptr_->GetConfigFilePath();
    if (!file_path.empty()) {
      YAML::Node cfg_node = YAML::LoadFile(std::string(file_path));
      topic_name_ = cfg_node["topic_name"].as<std::string>();
      channel_frq_ = cfg_node["channel_frq"].as<double>();
    }

    // Get executor handle with name "work_thread_pool",(it must be registered in aimrt config file)
    executor_ = ctx_ptr_->CreateExecutor("pub_thread_pool");

    // Register publish
    publisher_ = ctx_ptr_->CreatePublisher<aimrt_samples::protocols::EventMsg>(topic_name_);

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }
  return true;
}

bool NormalPublisherModule::Start() {
  executor_.Execute([this]() {
    ctx_ptr_->LetMe();
    MainLoop();
  });

  return true;
}

void NormalPublisherModule::Shutdown() {
  ctx_ptr_->StopRunning();
}

// Main loop
void NormalPublisherModule::MainLoop() {
  uint32_t count = 0;
  while (ctx_ptr_->Running()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint32_t>(1000 / channel_frq_)));

    count++;
    AIMRT_INFO("Loop count : {} -------------------------", count);

    // full pb event
    aimrt_samples::protocols::EventMsg msg;
    msg.mutable_header()->set_time_stamp(std::chrono::system_clock::now().time_since_epoch().count());
    msg.mutable_header()->set_frame_id("test_frame");
    msg.mutable_header()->set_sequence_num(count);
    msg.set_msg("count: " + std::to_string(count));

    AIMRT_INFO("Publish new pb event, data: {}", aimrt::Pb2CompactJson(msg));

    // publish pb event
    publisher_.Publish(msg);
  }
}
