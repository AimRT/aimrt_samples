// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"
#include "normal_server_module/global.h"
#include "normal_server_module/service.h"

aimrt::rpc::Status CalculateServiceImpl::CalculateSum(
    aimrt::rpc::ContextRef ctx,
    const ::aimrt_samples::protocols::CalculateSumReq& req,
    ::aimrt_samples::protocols::CalculateSumRsp& rsp) {
  // Here you define your specific service logic
  rsp.set_sum(req.num1() + req.num2());

  AIMRT_INFO("Server handle new rpc call. context: {}, req: {}, return rsp: {}",
             ctx.ToString(), aimrt::Pb2CompactJson(req), aimrt::Pb2CompactJson(rsp));

  return aimrt::rpc::Status();
}