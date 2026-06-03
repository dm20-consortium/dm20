cmake_minimum_required(VERSION 3.5)
project(dm_sensor_info_msgs)

if(NOT CMAKE_CXX_STANDARD)
  include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/CompilerSettings.cmake)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(dm_object_info_msgs REQUIRED)
find_package(dm_freespace_info_msgs REQUIRED)

set(msg_files
  "msg/DetectableCapability.msg"
  "msg/SensorInfo.msg"
  "msg/SensorInfoArray.msg"
  "msg/SensorStatus.msg"
  "msg/SensorType.msg"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES dm_object_info_msgs dm_freespace_info_msgs
  ADD_LINTER_TESTS
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()
