cmake_minimum_required (VERSION 3.5)
project (dmi_utils CXX)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wno-cpp")
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

find_package(Threads)
find_package(XercesC REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(cool4_api_dataclass REQUIRED)
find_package(gflags REQUIRED)
find_package(Glog REQUIRED)

find_package(islib QUIET)
message(STATUS "enable build with is: ${islib_FOUND}")

include_directories(
  include
  ${Boost_INCLUDE_DIRS}
  ${islib_INCLUDE_DIR}
  ${Threads_INCLUDE_DIRS}
  ${gflags_INCLUDE_DIRS}
)

# common libraries
add_library(${PROJECT_NAME} SHARED
  src/${PROJECT_NAME}/base.cpp
  src/${PROJECT_NAME}/dm_bridge.cpp
  src/${PROJECT_NAME}/filesystem.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    ${Boost_LIBRARIES}
    glog::glog
  PUBLIC
    cool4_api_dataclass
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

install(TARGETS ${PROJECT_NAME}
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

install(EXPORT ${PROJECT_NAME}
  FILE ${PROJECT_NAME}-config.cmake
  DESTINATION share/cmake/${PROJECT_NAME}/
  EXPORT_LINK_INTERFACE_LIBRARIES
)

if (TEST)
  find_package(GTest REQUIRED)
  enable_testing()
  include(GoogleTest)

  set(DMI_UTILS_TYPES dm_bridge creator uploader downloader)

  foreach(dmi_utils_type IN LISTS DMI_UTILS_TYPES)
    add_executable(test_dmi_utils_${dmi_utils_type}
      test/test_dmi_utils_${dmi_utils_type}.cpp
    )
    target_link_libraries(test_dmi_utils_${dmi_utils_type}
      ${XercesC_LIBRARIES}
      ${islib_LIBRARIES}
      ${PROJECT_NAME}
      dmi_utils
      GTest::GTest
      GTest::Main
    )
    gtest_discover_tests(test_dmi_utils_${dmi_utils_type})
  endforeach()
endif()
