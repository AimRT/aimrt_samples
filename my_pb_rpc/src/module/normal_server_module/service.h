// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#pragma once

#include "aimrt_module_cpp_interface/logger/logger.h"
#include "rpc.aimrt_rpc.pb.h"

class CalculateServiceImpl : public aimrt_samples::protocols::CalculationServiceSyncService {
 public:
  CalculateServiceImpl() = default;
  ~CalculateServiceImpl() override = default;

  aimrt::rpc::Status CalculateSum(
      aimrt::rpc::ContextRef ctx,
      const ::aimrt_samples::protocols::CalculateSumReq& req,
      ::aimrt_samples::protocols::CalculateSumRsp& rsp) override;
};
