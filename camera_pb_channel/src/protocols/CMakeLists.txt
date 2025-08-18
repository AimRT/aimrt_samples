# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

# Set proto name
set(PROTO_NAME "image")

# Use AimRT CMake tool to generate protobuf code and
# generate a target named ${PROTO_NAME}_pb_gencode
add_protobuf_gencode_target_for_proto_path(
  TARGET_NAME ${PROTO_NAME}_pb_gencode
  PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}
  GENCODE_PATH ${CMAKE_CURRENT_BINARY_DIR})
