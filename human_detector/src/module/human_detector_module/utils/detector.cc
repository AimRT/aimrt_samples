#include "human_detector_module/utils/detector.h"

bool Detector::LoadModel(const std::string& cascade_path) {
  return face_cascade_.load(cascade_path);
}

void Detector::ProcessImage(const cv::Mat& input_frame) {
  if (input_frame.empty()) {
    return;
  }

  // 转换为灰度图
  cv::Mat gray;
  cv::cvtColor(input_frame, gray, cv::COLOR_BGR2GRAY);

  // 直方图均衡化
  cv::equalizeHist(gray, gray);

  // 双边滤波
  cv::Mat processed;
  cv::bilateralFilter(gray, processed, 20, 150, 150);

  // 使用级联分类器进行人脸检测
  std::vector<cv::Rect> faces;
  face_cascade_.detectMultiScale(processed, faces, 1.1, 3, 0, cv::Size(30, 30));

  // 在原图上绘制检测结果
  cv::Mat result = input_frame.clone();
  for (const auto& face : faces) {
    cv::rectangle(result, face, cv::Scalar(255, 0, 0), 2);
  }

  std::lock_guard<std::mutex> lock(image_mutex_);
  latest_image_ = result;
}

bool Detector::InitializeDisplay(const std::string& window_name, int width, int height) {
  window_name_ = window_name;
  cv::namedWindow(window_name_, cv::WINDOW_NORMAL);
  cv::resizeWindow(window_name_, width, height);
  return true;
}

void Detector::DisplayImage() {
  cv::Mat display_image;

  {
    std::lock_guard<std::mutex> lock(image_mutex_);
    if (latest_image_.empty()) {
      return;
    }
    display_image = latest_image_.clone();
  }

  cv::imshow(window_name_, display_image);
  cv::waitKey(10);
}

void Detector::ShutdownDisplay() {
  cv::destroyWindow(window_name_);
}