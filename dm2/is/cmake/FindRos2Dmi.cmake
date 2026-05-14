# input: ROS2_WORKSPACE_DIR
message(STATUS "try to search ${ROS2_WORKSPACE_DIR}")
find_path(
  ros2dmi_INCLUDE_DIR
  NAMES
    object_info_converter.hpp freespace_info_converter.hpp signal_info_converter.hpp
  PATHS
    ${ROS2_WORKSPACE_DIR}/src/ros2_dmi/include/ros2_dmi
  PATH_SUFFIXES
    ros2_dmi
)

set(ros2dmi_FOUND "NO")
message(STATUS "ros2 dmi include dirs: ${ros2dmi_INCLUDE_DIR}")
if(${ros2dmi_INCLUDE_DIR} STREQUAL "ros2dmi_INCLUDE_DIR-NOTFOUND")
  return()
endif()

set(ros2dmi_LIBRARIES)
set(SEARCH_TARGETS ros2_dmi;ros2_dmi_publisher_wrapper;ros2_dmi_subscriber_wrapper)
foreach(search_target IN LISTS SEARCH_TARGETS)
  find_library(
    ${search_target}_LIB
    NAMES
      ${search_target}
    PATHS
      ${ROS2_WORKSPACE_DIR}/build/ros2_dmi
  )
  set(n "${search_target}_LIB")
  if("${${n}}" STREQUAL "${search_target}_LIB-NOTFOUND")
    message(STATUS "${n} is not found")
    set(ros2dmi_FOUND "NO")
    return()
  endif()
  LIST(APPEND ros2dmi_LIBRARIES "${${n}}")
endforeach()

message(STATUS "ros2dmi libraries: ${ros2dmi_LIBRARIES}")


set(ros2dmi_FOUND "YES")
