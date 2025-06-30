# Copyright (c) 2023, AgiBot Inc.
# All rights reserved.

set(PROTO_NAME "my_pb_rpc")

# Use AimRT CMake tool to generate protobuf code and
# generate a target named ${PROTO_NAME}_pb_gencode
add_protobuf_gencode_target_for_proto_path(
  TARGET_NAME ${PROTO_NAME}_pb_gencode
  PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}
  GENCODE_PATH ${CMAKE_CURRENT_BINARY_DIR})

# Generate AimRT rpc code based on protobuf files
# Creates target: ${PROTO_NAME}_aimrt_rpc_gencode
# Depends on: ${PROTO_NAME}_pb_gencode
add_protobuf_aimrt_rpc_gencode_target_for_proto_files(
  TARGET_NAME ${PROTO_NAME}_aimrt_rpc_gencode
  PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/rpc.proto
  GENCODE_PATH ${CMAKE_CURRENT_BINARY_DIR}
  DEP_PROTO_TARGETS ${PROTO_NAME}_pb_gencode)
