#include "human_detector_module/human_detector_module.h"
#include <atomic>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>
#include "aimrt_module_protobuf_interface/channel/protobuf_channel.h"
#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"

bool HumanDetectorModule::Initialize(aimrt::CoreRef core) {
  core_ = core;

  if (!face_cascade_.load("./cfg/model/haarcascade_frontalface_default.xml")) {
    AIMRT_ERROR("Failed to load face cascade: haarcascade_frontalface_default.xml");
    return false;
  }

  subscriber_ = core_.GetChannelHandle().GetSubscriber("/msg/camera");
  AIMRT_CHECK_ERROR_THROW(subscriber_, "Get subscriber failed.");

  bool ret = aimrt::channel::Subscribe<custom_protocol::img_msg::Image>(
      subscriber_,
      std::bind(&HumanDetectorModule::EventHandle, this,
                std::placeholders::_1, std::placeholders::_2));
  AIMRT_CHECK_ERROR_THROW(ret, "Subscribe failed.");

  executor_ = core_.GetExecutorManager().GetExecutor("display_executor");
  AIMRT_CHECK_ERROR_THROW(executor_, "Get executor failed.");

  return true;
}

bool HumanDetectorModule::Start() {
  run_flag_ = true;
  executor_.Execute(std::bind(&HumanDetectorModule::DisplayLoop, this));
  return true;
}

void HumanDetectorModule::Shutdown() {
  if (run_flag_) {
    run_flag_ = false;
    stop_sig_.get_future().wait();
  }
  cv::destroyAllWindows();
}

void HumanDetectorModule::EventHandle(aimrt::channel::ContextRef ctx,
                                      const std::shared_ptr<const custom_protocol::img_msg::Image>& data) {
  if (!data || data->data().empty()) return;

  cv::Mat frame(data->height(), data->width(), CV_8UC3,
                const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data->data().data())));
  if (frame.empty()) return;

  cv::Mat gray;
  cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
  cv::equalizeHist(gray, gray);

  cv::Mat dst;
  cv::bilateralFilter(gray, dst, 20, 150, 150);

  std::vector<cv::Rect> faces;
  face_cascade_.detectMultiScale(dst, faces, 1.1, 3, 0, cv::Size(30, 30));

  for (const auto& face : faces) {
    cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2);
  }

  {
    std::lock_guard<std::mutex> lock(image_mutex_);
    latest_image_ = frame.clone();
  }
}

void HumanDetectorModule::DisplayLoop() {
  cv::namedWindow("Human Detection", cv::WINDOW_AUTOSIZE);

  while (run_flag_) {
    cv::Mat display_image;
    {
      std::lock_guard<std::mutex> lock(image_mutex_);
      if (!latest_image_.empty()) {
        display_image = latest_image_.clone();
      }
    }

    if (!display_image.empty()) {
      cv::imshow("Human Detection", display_image);
    }
  }

  cv::destroyWindow("Human Detection");
  stop_sig_.set_value();
}