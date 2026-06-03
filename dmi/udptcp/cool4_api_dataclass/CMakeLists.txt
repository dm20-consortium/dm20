cmake_minimum_required (VERSION 3.5)
project (cool4_api_dataclass CXX)

set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 8)
set(${PROJECT_NAME}_VERSION_PATCH 0)

configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/generated/cool4_api_dataclass/config.h")

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/CompilerSettings.cmake)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

find_package(Threads)
find_package(XercesC REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(gflags REQUIRED)
find_package(Glog REQUIRED)

include_directories(
  include
  ${Boost_INCLUDE_DIRS}
  ${Threads_INCLUDE_DIRS}
  ${gflags_INCLUDE_DIRS}
)

# common libraries
add_library(${PROJECT_NAME} SHARED
  src/${PROJECT_NAME}/dataclasses_with_header.cpp
  src/${PROJECT_NAME}/header.cpp
  src/${PROJECT_NAME}/object_info.cpp
  src/${PROJECT_NAME}/freespace_info.cpp
  src/${PROJECT_NAME}/signal_info.cpp
  src/${PROJECT_NAME}/sensor_info.cpp
)

target_link_libraries(${PROJECT_NAME}
  ${Boost_LIBRARIES}
  glog::glog
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
  DIRECTORY include/
  DESTINATION include
)
install(
  FILES
    "${PROJECT_BINARY_DIR}/generated/cool4_api_dataclass/config.h"
  DESTINATION include/cool4_api_dataclass
)
install(EXPORT ${PROJECT_NAME}
  FILE ${PROJECT_NAME}-config.cmake
  DESTINATION share/cmake/${PROJECT_NAME}/
  EXPORT_LINK_INTERFACE_LIBRARIES
)

if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P
            "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif()

if (TEST)
  find_package(GTest REQUIRED)
  enable_testing()
  include(GoogleTest)

  set(COOL4_API_DATACLASS_TYPES header byte_extractor byte_stacker)

  foreach(cool4_api_dataclass_type IN LISTS COOL4_API_DATACLASS_TYPES)
    add_executable(test_${PROJECT_NAME}_${cool4_api_dataclass_type}
      test/test_${PROJECT_NAME}_${cool4_api_dataclass_type}.cpp
    )
    target_link_libraries(test_${PROJECT_NAME}_${cool4_api_dataclass_type}
      ${XercesC_LIBRARIES}
      ${PROJECT_NAME}
      GTest::GTest
      GTest::Main
    )
    gtest_discover_tests(test_${PROJECT_NAME}_${cool4_api_dataclass_type})
  endforeach()
endif()
