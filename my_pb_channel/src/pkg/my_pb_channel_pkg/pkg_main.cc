#include "aimrt_pkg_c_interface/pkg_macro.h"
#include "normal_publisher_module/normal_publisher_module.h"
#include "normal_subscriber_module/normal_subscriber_module.h"

static std::tuple<std::string_view, std::function<aimrt::ModuleBase*()>> aimrt_module_register_array[]{
    // Here register all modules in the package
    {"NormalPublisherModule", []() -> aimrt::ModuleBase* { return new NormalPublisherModule(); }},
    {"NormalSubscriberModule", []() -> aimrt::ModuleBase* { return new NormalSubscriberModule(); }},
};

AIMRT_PKG_MAIN(aimrt_module_register_array)
