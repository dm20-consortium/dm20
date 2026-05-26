cmake_minimum_required (VERSION 3.5)
project (tcp_dmi CXX)

set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 8)
set(${PROJECT_NAME}_VERSION_PATCH 0)

set(BUFFER_SIZE 2048)

configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/generated/tcp_dmi/config.h")

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/CompilerSettings.cmake)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

find_package(Threads)
find_package(Boost REQUIRED COMPONENTS thread regex system filesystem)
find_package(dmi_utils REQUIRED)
find_package(gflags REQUIRED)
find_package(Glog REQUIRED)

find_package(islib REQUIRED)
find_package(XercesC REQUIRED)

include_directories(
  include
  ${Boost_INCLUDE_DIRS}
  ${Threads_INCLUDE_DIRS}
  ${gflags_INCLUDE_DIRS}
  ${GeographicLib_INCLUDE_DIRS}
  ${islib_INCLUDE_DIR}
  ${dmi_utils_INCLUDE_DIRS}
)

# common libraries
add_library(${PROJECT_NAME} SHARED
  src/${PROJECT_NAME}/system/server.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:include>

  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    ${Boost_LIBRARIES}
    ${GeographicLib_LIBRARIES}
    glog::glog
    ${XercesC_LIBRARIES}
    ${islib_LIBRARIES}
    dmi_utils
)

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

add_executable(${PROJECT_NAME}_receiver_signal_info
  src/${PROJECT_NAME}/receiver/signal_info.cpp
)

target_link_libraries(${PROJECT_NAME}_receiver_signal_info
  ${PROJECT_NAME}
  gflags
)

add_executable(${PROJECT_NAME}_sample_send_signal_info
  src/${PROJECT_NAME}/sample/send_signal_info.cpp
)

target_link_libraries(${PROJECT_NAME}_sample_send_signal_info
  ${PROJECT_NAME}
  gflags
)

install(TARGETS ${PROJECT_NAME}_receiver_signal_info ${PROJECT_NAME}_sample_send_signal_info
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

  set(SOCKET_TYPES simple_server)

  foreach(socket_type IN LISTS SOCKET_TYPES)
    add_executable(test_tcp_dmi_${socket_type}
      test/test_tcp_dmi_${socket_type}.cpp
    )
    target_link_libraries(test_tcp_dmi_${socket_type}
      ${PROJECT_NAME} 
      dmi_utils
      GTest::GTest
      GTest::Main
    )
    gtest_discover_tests(test_tcp_dmi_${socket_type})
  endforeach()

endif()
