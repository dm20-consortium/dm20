cmake_minimum_required(VERSION 3.5)
project(dm_signal_info_msgs)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(dm_object_info_msgs REQUIRED)

set(msg_files
  "msg/ArrowLightIndication.msg"
  "msg/CountDownStopFlag.msg"
  "msg/EventCount.msg"
  "msg/MainLightIndication.msg"
  "msg/MaxTimeToChange.msg"
  "msg/MinTimeToChange.msg"
  "msg/SignalId.msg"
  "msg/SignalInfo.msg"
  "msg/SignalInfoArray.msg"
  "msg/SignalLightInfo.msg"
  "msg/SignalState.msg"
  "msg/SpecificControlFlags.msg"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES dm_object_info_msgs
  ADD_LINTER_TESTS
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()
