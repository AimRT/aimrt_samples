#include "aimrt_pkg_c_interface/pkg_macro.h"
#include "normal_client_module/normal_client_module.h"
#include "normal_server_module/normal_server_module.h"

static std::tuple<std::string_view, std::function<aimrt::ModuleBase*()>> aimrt_module_register_array[]{
    // Here register all modules in the package
    {"NormalRpcClientModule", []() -> aimrt::ModuleBase* { return new NormalRpcClientModule(); }},
    {"NormalRpcServerModule", []() -> aimrt::ModuleBase* { return new NormalRpcServerModule(); }},
};

AIMRT_PKG_MAIN(aimrt_module_register_array)
