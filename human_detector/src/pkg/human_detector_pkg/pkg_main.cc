#include "aimrt_pkg_c_interface/pkg_macro.h"
#include "hal_camera_module/hal_camera_module.h"
#include "human_detector_module/human_detector_module.h"

static std::tuple<std::string_view, std::function<aimrt::ModuleBase*()>> aimrt_module_register_array[]{
    // Here register all modules in the package
    {"HalCameraModule", []() -> aimrt::ModuleBase* { return new HalCameraModule(); }},
    {"HumanDetectorModule", []() -> aimrt::ModuleBase* { return new HumanDetectorModule(); }},
};

AIMRT_PKG_MAIN(aimrt_module_register_array)
