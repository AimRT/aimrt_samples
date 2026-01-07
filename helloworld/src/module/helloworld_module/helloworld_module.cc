// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "helloworld_module/helloworld_module.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep

bool HelloWorldModule::Initialize(aimrt::CoreRef core) {
  ctx_ptr_ = std::make_shared<aimrt::context::Context>(core);
  ctx_ptr_->LetMe();

  try {
    // Write your other initialization logic here ...
    // Read custom module configuration
    auto file_path = ctx_ptr_->GetConfigFilePath();
    if (!file_path.empty()) {
      YAML::Node cfg_node = YAML::LoadFile(file_path.data());
      param1_ = cfg_node["param1"].as<std::string>();
      param2_ = cfg_node["param2"].as<int>();
    }

    // Get executor handle
    executor_ = ctx_ptr_->CreateExecutor("work_executor");

    timer_ = aimrt::executor::CreateTimer(
        executor_,
        std::chrono::milliseconds(1000),
        [this]() { 
          ctx_ptr_->LetMe();
          Task(); },
        false);

  } catch (const std::exception& e) {
    AIMRT_ERROR("Init failed, {}", e.what());
    return false;
  }
  return true;
}

bool HelloWorldModule::Start() {
  // Write your runtime logic here
  // use timer
  timer_->Reset();

  // use executor
  executor_.Execute([this]() { ctx_ptr_->LetMe(); LoopTask(); });

  // use co_task
  scope_.spawn(aimrt::co::On(aimrt::co::InlineScheduler(), CoLoopTask()));
  return true;
}

void HelloWorldModule::Shutdown() {
  // Write your resource release logic here
  ctx_ptr_->StopRunning();
  timer_->Cancel();
  aimrt::co::SyncWait(scope_.complete());
}

void HelloWorldModule::Task() {
  AIMRT_INFO("Single Task param1: {}, param2: {}", param1_, param2_);
}

void HelloWorldModule::LoopTask() {
  while (ctx_ptr_->Running()) {
    AIMRT_WARN("Loop Task param1: {}, param2: {}", param1_, param2_);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

aimrt::co::Task<void> HelloWorldModule::CoLoopTask() {
  auto scheduler = aimrt::co::AimRTScheduler(executor_);
  while (ctx_ptr_->Running()) {
    ctx_ptr_->LetMe();
    AIMRT_ERROR("CoLoop Task param1: {}, param2: {}", param1_, param2_);
    co_await aimrt::co::ScheduleAfter(scheduler, std::chrono::milliseconds(1000));
  }
  co_return;
}
