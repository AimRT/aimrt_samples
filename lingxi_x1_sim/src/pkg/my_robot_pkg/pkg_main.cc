#include <cstring>

#include "aimrt_pkg_c_interface/pkg_macro.h"
#include "control_module/control_module.h"
#include "keyboard_module/keyboard_module.h"

static std::tuple<std::string_view, std::function<aimrt::ModuleBase*()>>
    aimrt_module_register_array[]{
        {"KeyboardModule",
         []() -> aimrt::ModuleBase* { return new my_robot::keyboard_module::KeyboardModule(); }},
        {"ControlModule",
         []() -> aimrt::ModuleBase* { return new my_robot::rl_control_module::ControlModule(); }},
    };

AIMRT_PKG_MAIN(aimrt_module_register_array)
