cmake_minimum_required(VERSION 3.5)
project(ros2_dmi)

if(NOT CMAKE_CXX_STANDARD)
  include(${CMAKE_CURRENT_LIST_DIR}/../cmake/CompilerSettings.cmake)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Wno-cpp)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

find_package(Boost REQUIRED COMPONENTS system thread regex)
find_package(XercesC REQUIRED)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(dm_object_info_msgs REQUIRED)
find_package(dm_freespace_info_msgs REQUIRED)
find_package(dm_signal_info_msgs REQUIRED)
find_package(dm_sensor_info_msgs REQUIRED)
find_package(std_msgs REQUIRED)

find_package(islib REQUIRED)

include_directories(
  include
  ${Boost_INCLUDE_DIRS}
  ${XercesC_INCLUDE_DIRS}
  ${rclcpp_INCLUDE_DIRS}
  ${rclcpp_components_INCLUDE_DIRS}
  ${dm_object_info_msgs_INCLUDE_DIRS}
  ${dm_freespace_info_msgs_INCLUDE_DIRS}
  ${dm_signal_info_msgs_INCLUDE_DIRS}
  ${dm_sensor_info_msgs_INCLUDE_DIRS}
  ${std_msgs_INCLUDE_DIRS}
  ${islib_INCLUDE_DIR}
)

# コンポーネントのビルド
add_library(${PROJECT_NAME} SHARED
  src/${PROJECT_NAME}/downloader.cpp
  src/${PROJECT_NAME}/uploader.cpp
)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall)
rclcpp_components_register_nodes(${PROJECT_NAME} "ros2_dmi::Downloader")
rclcpp_components_register_nodes(${PROJECT_NAME} "ros2_dmi::Uploader")
target_compile_definitions(${PROJECT_NAME}
  PRIVATE "MY_LIBRARY_BUILDING_LIBRARY")
target_link_libraries(${PROJECT_NAME}
  ${dm_object_info_msgs_LIBRARIES}
  ${dm_freespace_info_msgs_LIBRARIES}
  ${dm_signal_info_msgs_LIBRARIES}
  ${dm_sensor_info_msgs_LIBRARIES}
  ${std_msgs_LIBRARIES}
  ${rclcpp_LIBRARIES}
  ${rclcpp_components_LIBRARIES}
  ${XercesC_LIBRARIES}
  ${Boost_LIBRARIES}
  ${islib_LIBRARIES}
)
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  rclcpp_components
  dm_object_info_msgs
  dm_freespace_info_msgs
  dm_signal_info_msgs
  dm_sensor_info_msgs
  std_msgs
)
install(TARGETS
  ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

# プラグインのビルド
set(PLUGIN_NAMES subscriber_wrapper;publisher_wrapper)
foreach(plugin_name IN LISTS PLUGIN_NAMES)
  add_library(${PROJECT_NAME}_${plugin_name} SHARED
    src/${PROJECT_NAME}/${plugin_name}.cpp
  )
  target_link_libraries(${PROJECT_NAME}_${plugin_name}
    ${PROJECT_NAME}
  )
  install(TARGETS
    ${PROJECT_NAME}_${plugin_name}
    EXPORT export_${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
  )
endforeach()


install(
  DIRECTORY include/
  DESTINATION include
)
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ZLIB REQUIRED)
  add_subdirectory(test)
endif()

ament_package()
