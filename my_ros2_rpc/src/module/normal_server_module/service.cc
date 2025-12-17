// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "normal_server_module/service.h"
#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"
#include "normal_server_module/global.h"

aimrt::rpc::Status CalculateServiceImpl::CalculateSum(
    aimrt::rpc::ContextRef ctx,
    const ::my_ros2_srvs::srv::CalculateSum_Request& req,
    ::my_ros2_srvs::srv::CalculateSum_Response& rsp) {
  // Here you define your specific service logic
  rsp.set__sum(req.num1 + req.num2);

  AIMRT_INFO("Server handle new rpc call. context: {}, req: {}, return rsp: {}",
             ctx.ToString(), my_ros2_srvs::srv::to_yaml(req), my_ros2_srvs::srv::to_yaml(rsp));

  return aimrt::rpc::Status();
}