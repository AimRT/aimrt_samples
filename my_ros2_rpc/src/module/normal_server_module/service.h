// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include "CalculateSum.aimrt_rpc.srv.h"
#include "aimrt_module_cpp_interface/logger/logger.h"

class CalculateServiceImpl : public my_ros2_srvs::srv::CalculateSumSyncService {
 public:
  CalculateServiceImpl() = default;
  ~CalculateServiceImpl() override = default;

  aimrt::rpc::Status CalculateSum(
      aimrt::rpc::ContextRef ctx,
      const ::my_ros2_srvs::srv::CalculateSum_Request& req,
      ::my_ros2_srvs::srv::CalculateSum_Response& rsp) override;
};
