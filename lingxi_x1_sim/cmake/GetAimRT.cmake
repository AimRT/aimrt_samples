include(FetchContent)

message(STATUS "get aimrt ...")

set(aimrt_DOWNLOAD_URL
    "https://github.com/AimRT/AimRT/archive/refs/tags/v1.3.0.tar.gz"
    CACHE STRING "")


if(aimrt_LOCAL_SOURCE)
    FetchContent_Declare(
      aimrt
      SOURCE_DIR ${aimrt_LOCAL_SOURCE}
      OVERRIDE_FIND_PACKAGE)
else()
    FetchContent_Declare(
      aimrt
      URL ${aimrt_DOWNLOAD_URL}
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
      OVERRIDE_FIND_PACKAGE)
endif()
  
FetchContent_GetProperties(aimrt)
if(NOT aimrt_POPULATED)
  set(AIMRT_BUILD_TESTS
      OFF
      CACHE BOOL "")
  set(AIMRT_BUILD_EXAMPLES
      OFF
      CACHE BOOL "")
  set(AIMRT_BUILD_DOCUMENT
      OFF
      CACHE BOOL "")
  set(AIMRT_BUILD_RUNTIME
      ON
      CACHE BOOL "")
  set(AIMRT_BUILD_CLI_TOOLS
      OFF
      CACHE BOOL "")
  set(AIMRT_BUILD_WITH_PROTOBUF
      ON
      CACHE BOOL "")
  set(AIMRT_USE_LOCAL_PROTOC_COMPILER
      OFF
      CACHE BOOL "")
  set(AIMRT_USE_PROTOC_PYTHON_PLUGIN
      OFF
      CACHE BOOL "")
  set(AIMRT_BUILD_WITH_ROS2
      ON
      CACHE BOOL "")
  set(AIMRT_BUILD_NET_PLUGIN
      OFF
      CACHE BOOL "")
  set(AIMRT_BUILD_ROS2_PLUGIN
      ON
      CACHE BOOL "")
  set(AIMRT_BUILD_MQTT_PLUGIN
      OFF
      CACHE BOOL "")
  set(AIMRT_BUILD_ZENOH_PLUGIN
      OFF
      CACHE BOOL "")
  set(AIMRT_BUILD_ICEORYX_PLUGIN
      OFF
      CACHE BOOL "")

  FetchContent_MakeAvailable(aimrt)
endif()
