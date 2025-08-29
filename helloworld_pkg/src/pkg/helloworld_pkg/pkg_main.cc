#include "aimrt_pkg_c_interface/pkg_macro.h"
#include "helloworld_module/helloworld_module.h"

static std::tuple<std::string_view, std::function<aimrt::ModuleBase*()>> aimrt_module_register_array[]{
    // Here register all modules in the package
    {"HelloWorldModule", []() -> aimrt::ModuleBase* { return new HelloWorldModule(); }}};

AIMRT_PKG_MAIN(aimrt_module_register_array)
