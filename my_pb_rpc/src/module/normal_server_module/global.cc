// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "normal_server_module/global.h"

aimrt::logger::LoggerRef global_logger;
void SetLogger(aimrt::logger::LoggerRef logger) { global_logger = logger; }
aimrt::logger::LoggerRef GetLogger() { return global_logger; }
