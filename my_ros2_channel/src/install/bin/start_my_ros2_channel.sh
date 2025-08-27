#!/bin/sh

cd $(dirname $0)

source install/share/my_ros2_msgs/local_setup.bash
./my_ros2_channel_app ./cfg/my_ros2_channel_cfg.yaml
