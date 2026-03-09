include(FetchContent)

message(STATUS "get aimrt mujoco sim ...")

set(aimrt_mujoco_sim_DOWNLOAD_URL
    "https://github.com/AimRT/aimrt_mujoco_sim/archive/refs/tags/v1.0.2.tar.gz"
    CACHE STRING "")


if(aimrt_mujoco_sim_LOCAL_SOURCE)
    FetchContent_Declare(
      aimrt_mujoco_sim
      SOURCE_DIR ${aimrt_mujoco_sim_LOCAL_SOURCE}
      OVERRIDE_FIND_PACKAGE)
else()
    FetchContent_Declare(
      aimrt_mujoco_sim
      URL ${aimrt_mujoco_sim_DOWNLOAD_URL}
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
      OVERRIDE_FIND_PACKAGE)
endif()
  
FetchContent_GetProperties(aimrt_mujoco_sim)
if(NOT aimrt_mujoco_sim_POPULATED)

  set(AIMRT_MUJOCO_SIM_BUILD_WITH_ROS2 ON )

  FetchContent_MakeAvailable(aimrt_mujoco_sim)
endif()
