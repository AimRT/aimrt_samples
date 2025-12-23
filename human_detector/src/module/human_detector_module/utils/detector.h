// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.
#pragma once

#include <mutex>
#include <opencv2/opencv.hpp>
#include <string>

class Detector {
 public:
  Detector() = default;
  ~Detector() = default;

  bool LoadModel(const std::string& cascade_path);

  void ProcessImage(const cv::Mat& input_frame);

  bool InitializeDisplay(const std::string& window_name, int width = 300, int height = 300);

  void DisplayImage();

  void ShutdownDisplay();

 private:
  cv::CascadeClassifier face_cascade_;
  std::string window_name_;

  cv::Mat latest_image_;
  std::mutex image_mutex_;
};
