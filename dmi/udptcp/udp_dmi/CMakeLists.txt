cmake_minimum_required (VERSION 3.5)
project (udp_dmi CXX)

set(BUFFER_SIZE 2048)

configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/generated/udp_dmi/config.h")

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/CompilerSettings.cmake)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

find_package(Threads)
find_package(XercesC REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(dmi_utils REQUIRED)
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

add_library(${PROJECT_NAME} SHARED
  src/${PROJECT_NAME}/socket/simple_sender.cpp
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    ${Boost_LIBRARIES}
    glog::glog
    dmi_utils
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
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

if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P
            "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif()

# supported types in this package
set(DMI_TYPES object_info;freespace_info;signal_info;sensor_info)

# samples
foreach(dmi_type IN LISTS DMI_TYPES)
  add_executable(${PROJECT_NAME}_sample_send_${dmi_type}
    src/${PROJECT_NAME}/sample/send_${dmi_type}.cpp
  )
  target_link_libraries(${PROJECT_NAME}_sample_send_${dmi_type}
    ${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    dmi_utils
    gflags
  )

  add_executable(${PROJECT_NAME}_sample_receive_${dmi_type}
    src/${PROJECT_NAME}/sample/receive_${dmi_type}.cpp
  )
  target_link_libraries(${PROJECT_NAME}_sample_receive_${dmi_type}
    ${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    dmi_utils
    gflags
  )

  install(
    TARGETS
      ${PROJECT_NAME}_sample_send_${dmi_type}
      ${PROJECT_NAME}_sample_receive_${dmi_type}
    EXPORT ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)
endforeach()

if(${islib_FOUND})
  # Plugin for IS
  set(PLUGIN_NAMES receiver_wrapper;sender_wrapper)
  foreach(plugin_name IN LISTS PLUGIN_NAMES)
    add_library(${PROJECT_NAME}_${plugin_name} SHARED
      src/${PROJECT_NAME}/socket/${plugin_name}.cpp
    )

#   target_link_libraries(${PROJECT_NAME}_${plugin_name}
#     PUBLIC
#       dmi_utils
#     PRIVATE
#       ${PROJECT_NAME}
#       ${XercesC_LIBRARIES}
#       ${islib_LIBRARIES}
#       glog::glog
#   )
    target_link_libraries(${PROJECT_NAME}_${plugin_name}
      dmi_utils
      ${PROJECT_NAME}
      ${XercesC_LIBRARIES}
      ${islib_LIBRARIES}
      glog::glog
    )

    install(
      TARGETS ${PROJECT_NAME}_${plugin_name}
      EXPORT ${PROJECT_NAME}
      ARCHIVE DESTINATION lib
      LIBRARY DESTINATION lib
      RUNTIME DESTINATION bin
      INCLUDES DESTINATION include
    )
  endforeach()

  foreach(dmi_type IN LISTS DMI_TYPES)
    # DM -> Vehicle or RSE
    add_executable(
      ${PROJECT_NAME}_sender_${dmi_type}
      src/${PROJECT_NAME}/sender/${dmi_type}.cpp
    )

    target_link_libraries(${PROJECT_NAME}_sender_${dmi_type}
      ${CMAKE_THREAD_LIBS_INIT}
      ${XercesC_LIBRARIES}
      ${islib_LIBRARIES}
      gflags
      glog::glog
      ${PROJECT_NAME}
      dmi_utils
    )

    install(
      TARGETS ${PROJECT_NAME}_sender_${dmi_type}
      EXPORT ${PROJECT_NAME}
      ARCHIVE DESTINATION lib
      LIBRARY DESTINATION lib
      RUNTIME DESTINATION bin
      INCLUDES DESTINATION include
    )

    # Vehicle or RSE -> DM
    add_executable(
      ${PROJECT_NAME}_receiver_${dmi_type}
      src/${PROJECT_NAME}/receiver/${dmi_type}.cpp
    )
    target_link_libraries(${PROJECT_NAME}_receiver_${dmi_type}
      ${CMAKE_THREAD_LIBS_INIT}
      ${XercesC_LIBRARIES}
      ${islib_LIBRARIES}
      gflags
      ${PROJECT_NAME}
      dmi_utils
    )
    install(
      TARGETS ${PROJECT_NAME}_receiver_${dmi_type}
      EXPORT ${PROJECT_NAME}
      ARCHIVE DESTINATION lib
      LIBRARY DESTINATION lib
      RUNTIME DESTINATION bin
      INCLUDES DESTINATION include
    )
  endforeach()
endif()

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

  set(SOCKET_TYPES receiver sender)

  foreach(socket_type IN LISTS SOCKET_TYPES)
    add_executable(test_udp_dmi_${socket_type}
      test/test_udp_dmi_${socket_type}.cpp
    )
    target_link_libraries(test_udp_dmi_${socket_type}
      ${PROJECT_NAME}
      dmi_utils
      GTest::GTest
      GTest::Main
    )
    gtest_discover_tests(test_udp_dmi_${socket_type})
  endforeach()

endif()
