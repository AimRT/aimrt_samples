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
  run_flag_ = true;
  executor_.Execute(std::bind(&HalCameraModule::MainLoop, this));
  return true;
}

void HalCameraModule::Shutdown() {
  // Write your resource release logic here
  AIMRT_INFO("Shutdown succeeded.");
  if (run_flag_) {
    run_flag_ = false;
    stop_sig_.get_future().wait();
  }
}

void HalCameraModule::MainLoop() {
  try {
    AIMRT_INFO("Start MainLoop.");

    uint32_t count = 0;
    while (run_flag_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint32_t>(1000 / 30)));

      cv::Mat frame;
      camera_ptr_->read(frame);

      if (frame.empty()) {
        continue;
      }

      custom_protocol::img_msg::Image msg;

      // 图像信息
      msg.set_width(frame.cols);
      msg.set_height(frame.rows);

      // 图像数据（OpenCV 默认是 BGR）
      msg.set_data(frame.data, frame.total() * frame.elemSize());
      // publish event

      AIMRT_TRACE_INTERVAL(1000, "Publish new pb event, data: {}", aimrt::Pb2CompactJson(msg));
      aimrt::channel::Publish(publisher_, msg);
    }

    AIMRT_INFO("Exit MainLoop.");
  } catch (const std::exception& e) {
    AIMRT_ERROR("Exit MainLoop with exception, {}", e.what());
  }

  stop_sig_.set_value();
}
