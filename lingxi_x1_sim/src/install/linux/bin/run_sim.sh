#!/bin/bash

if [ -f ./install/share/ros2_plugin_proto/local_setup.bash ]; then
    source ./install/share/ros2_plugin_proto/local_setup.bash
elif [ -f ../share/ros2_plugin_proto/local_setup.bash ]; then
    source ../share/ros2_plugin_proto/local_setup.bash
fi

./aimrt_main --cfg_file_path=./cfg/x1_cfg_sim.yaml
# gdb --args ./aimrt_main --cfg_file_path=./cfg/x1_cfg_sim.yaml
