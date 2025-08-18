// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include <csignal>
#include <iostream>

#include "camera_module/camera_module.h"
#include "core/aimrt_core.h"
#include "image_recognition_module/image_recognition_module.h"

using namespace aimrt::runtime::core;

AimRTCore *global_core_ptr_ = nullptr;

// Handles SIGINT (Ctrl+C) and SIGTERM signals for graceful shutdown
void SignalHandler(int sig) {
  if (global_core_ptr_ && (sig == SIGINT || sig == SIGTERM)) {
    global_core_ptr_->Shutdown();
    return;
  }
  raise(sig);
};

int32_t main(int32_t argc, char **argv) {
  // Register signal handlers
  signal(SIGINT, SignalHandler);
  signal(SIGTERM, SignalHandler);

  std::cout << "AimRT start." << std::endl;

  try {
    AimRTCore core;
    global_core_ptr_ = &core;

    // Register module you want to use:
    // here we register two modules: camera and image recognition module
    CameraModule camera_module;
    core.GetModuleManager().RegisterModule(camera_module.NativeHandle());

    ImageRecognitionModule image_recognition_module;
    core.GetModuleManager().RegisterModule(image_recognition_module.NativeHandle());

    // get configuration file path from command line argument
    AimRTCore::Options options;
    options.cfg_file_path = argv[1];

    // Initialization phase
    core.Initialize(options);

    // Runtime phase
    core.Start();

    // Cleanup and termination phase
    core.Shutdown();

    global_core_ptr_ = nullptr;
  } catch (const std::exception &e) {
    std::cout << "AimRT run with exception and exit. " << e.what() << std::endl;
    return -1;
  }

  std::cout << "AimRT exit." << std::endl;
  return 0;
}
