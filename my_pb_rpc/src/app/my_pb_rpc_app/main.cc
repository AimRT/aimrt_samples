// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include <csignal>
#include <iostream>

#include "core/aimrt_core.h"
#include "normal_client_module/normal_client_module.h"
#include "normal_server_module/normal_server_module.h"

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
    // here we register two modules: normal_client_module and normal_server_module
    NormalRpcClientModule normal_client_module;
    core.GetModuleManager().RegisterModule(normal_client_module.NativeHandle());

    NormalRpcServerModule normal_server_module;
    core.GetModuleManager().RegisterModule(normal_server_module.NativeHandle());

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
