#include "human_detector_module/human_detector_module.h"
#include <opencv2/opencv.hpp>
#include "aimrt_module_protobuf_interface/channel/protobuf_channel.h"

bool HumanDetectorModule::Initialize(aimrt::CoreRef core) {
  core_ = core;

  subscriber_ = core_.GetChannelHandle().GetSubscriber("/msg/camera");
  AIMRT_CHECK_ERROR_THROW(subscriber_, "Get subscriber failed.");

  bool ret = aimrt::channel::Subscribe<custom_protocol::img_msg::Image>(
      subscriber_,
      std::bind(&HumanDetectorModule::EventHandle, this,
                std::placeholders::_1, std::placeholders::_2));
  AIMRT_CHECK_ERROR_THROW(ret, "Subscribe failed.");

  executor_ = core_.GetExecutorManager().GetExecutor("display_executor");
  timer_ = aimrt::executor::CreateTimer(
      executor_, std::chrono::milliseconds(1000 / 30),
      [this] { detector_.DisplayImage(); }, false);

  // initial detector
  detector_.LoadModel("./cfg/model/haarcascade_frontalface_default.xml");

  // initial display window
  detector_.InitializeDisplay("Human Detection", 320, 240);

  return true;
}

bool HumanDetectorModule::Start() {
  timer_->Reset();
  return true;
}

void HumanDetectorModule::Shutdown() {
  timer_->Cancel();
  detector_.ShutdownDisplay();
}

void HumanDetectorModule::EventHandle(
    aimrt::channel::ContextRef ctx,
    const std::shared_ptr<const custom_protocol::img_msg::Image>& data) {
  cv::Mat frame(data->height(), data->width(), CV_8UC3,
                const_cast<uint8_t*>(
                    reinterpret_cast<const uint8_t*>(data->data().data())));

  detector_.ProcessImage(frame);
}
