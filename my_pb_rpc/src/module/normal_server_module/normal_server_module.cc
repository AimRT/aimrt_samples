// Copyright (c) 2023, AgiBot Inc.
// All rights reserved.

#include "normal_server_module/normal_server_module.h"
#include "aimrt_module_protobuf_interface/util/protobuf_tools.h"

#include "yaml-cpp/yaml.h"  // IWYU pragma: keep

bool NormalRpcServerModule::Initialize(aimrt::CoreRef core) {
  ctx_ptr_ = std::make_shared<aimrt::context::Context>(core);
  ctx_ptr_->LetMe();

  server_executor_ = ctx_ptr_->CreateExecutor("server_thread_pool");

  auto server_handle = ctx_ptr_->CreateServer<aimrt_samples::protocols::CalculationServiceCoServer>();

  // 初始化server
  server_handle->CalculateSum.ServeOn(server_executor_,
                                      [this](aimrt::rpc::ContextRef ctx, const aimrt_samples::protocols::CalculateSumReq& req, aimrt_samples::protocols::CalculateSumRsp& rsp) {
                                        ctx_ptr_->LetMe();
                                        AIMRT_INFO("[Server] Server get req: {}", aimrt::Pb2CompactJson(req));
                                        rsp.set_sum(req.num1() + req.num2());
                                        return aimrt::rpc::Status();
                                      });

  return true;
}

bool NormalRpcServerModule::Start() {
  return true;
}

void NormalRpcServerModule::Shutdown() {
  ctx_ptr_->StopRunning();
}
