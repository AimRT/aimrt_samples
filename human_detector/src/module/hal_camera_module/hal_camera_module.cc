// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "hal_camera_module/hal_camera_module.h"
#include "aimrt_module_protobuf_interface/channel/protobuf_channel.h"
#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"
#include "img.pb.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep

bool HalCameraModule::Initialize(aimrt::CoreRef core) {
  core_ = core;

  try {
    // Write your other initialization logic here ...
    // Get executor handle
    executor_ = core_.GetExecutorManager().GetExecutor("work_executor");
    AIMRT_CHECK_ERROR_THROW(executor_, "Get executor 'work_executor' failed.");

    timer_ = aimrt::executor::CreateTimer(
        executor_, std::chrono::milliseconds(1000 / 30), [this] { MainTask(); }, false);

    // Register publish type
    publisher_ = core_.GetChannelHandle().GetPublisher("/msg/camera");
    AIMRT_CHECK_ERROR_THROW(publisher_, "Get publisher failed.");
    bool ret = aimrt::channel::RegisterPublishType<custom_protocol::img_msg::Image>(publisher_);
    AIMRT_CHECK_ERROR_THROW(ret, "Register publish type failed.");

    camera_ptr_ = std::make_unique<cv::VideoCapture>(0);  // 0 表示默认摄像头
    if (!camera_ptr_->isOpened()) {
      AIMRT_ERROR("Open camera failed.");
    }

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }

  AIMRT_INFO("Init succeeded.");

  return true;
}

bool HalCameraModule::Start() {
  // Write your runtime logic here
  AIMRT_INFO("Start succeeded.");
  timer_->Reset();
  return true;
}

void HalCameraModule::Shutdown() {
  // Write your resource release logic here
  AIMRT_INFO("Shutdown succeeded.");
  timer_->Cancel();
}

void HalCameraModule::MainTask() {
  try {
    cv::Mat frame;
    camera_ptr_->read(frame);

    if (frame.empty()) {
      return;
    }

    custom_protocol::img_msg::Image msg;

    msg.set_width(frame.cols);
    msg.set_height(frame.rows);

    msg.set_data(frame.data, frame.total() * frame.elemSize());

    // publish event
    AIMRT_TRACE_INTERVAL(1000, "Publish new pb event, data: {}", aimrt::Pb2CompactJson(msg));
    aimrt::channel::Publish(publisher_, msg);

  } catch (const std::exception& e) {
    AIMRT_ERROR("Exit MainLoop with exception, {}", e.what());
  }
}
