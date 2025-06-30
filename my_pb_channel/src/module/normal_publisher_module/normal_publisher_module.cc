// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "normal_publisher_module/normal_publisher_module.h"
#include "aimrt_module_protobuf_interface/channel/protobuf_channel.h"
#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"

#include "yaml-cpp/yaml.h"

#include "event.pb.h"

bool NormalPublisherModule::Initialize(aimrt::CoreRef core) {
  core_ = core;

  try {
    // Read cfg
    auto file_path = core_.GetConfigurator().GetConfigFilePath();
    if (!file_path.empty()) {
      YAML::Node cfg_node = YAML::LoadFile(std::string(file_path));
      topic_name_ = cfg_node["topic_name"].as<std::string>();
      channel_frq_ = cfg_node["channel_frq"].as<double>();
    }

    // Get executor handle with name "work_thread_pool",(it must be registered in aimrt config file)
    executor_ = core_.GetExecutorManager().GetExecutor("work_thread_pool");
    AIMRT_CHECK_ERROR_THROW(executor_ && executor_.SupportTimerSchedule(),
                            "Get executor 'work_thread_pool' failed.");

    // Register publish(it contains two steps)
    // Step1: get publisher handle with topic name
    publisher_ = core_.GetChannelHandle().GetPublisher(topic_name_);
    AIMRT_CHECK_ERROR_THROW(publisher_, "Get publisher for topic '{}' failed.", topic_name_);

    // Step2: register publish type
    bool ret = aimrt::channel::RegisterPublishType<aimrt_samples::protocols::EventMsg>(publisher_);
    AIMRT_CHECK_ERROR_THROW(ret, "Register publish type failed.");

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Init succeeded.");

  return true;
}

bool NormalPublisherModule::Start() {
  try {
    run_flag_ = true;
    // Start main loop in executor_
    executor_.Execute(std::bind(&NormalPublisherModule::MainLoop, this));
  } catch (const std::exception& e) {
    AIMRT_ERROR("Start failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Start succeeded.");
  return true;
}

void NormalPublisherModule::Shutdown() {
  try {
    if (run_flag_) {
      run_flag_ = false;
      stop_sig_.get_future().wait();
    }
  } catch (const std::exception& e) {
    AIMRT_ERROR("Shutdown failed, {}", e.what());
    return;
  }

  AIMRT_INFO("Shutdown succeeded.");
}

// Main loop
void NormalPublisherModule::MainLoop() {
  try {
    AIMRT_INFO("Start MainLoop.");

    uint32_t count = 0;
    while (run_flag_) {
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
      aimrt::channel::Publish(publisher_, msg);
    }

    AIMRT_INFO("Exit MainLoop.");
  } catch (const std::exception& e) {
    AIMRT_ERROR("Exit MainLoop with exception, {}", e.what());
  }

  stop_sig_.set_value();
}
