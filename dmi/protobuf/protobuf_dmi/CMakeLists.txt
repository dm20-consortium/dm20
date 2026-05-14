cmake_minimum_required (VERSION 3.10)
project (protobuf_dmi CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall")

find_package(Protobuf REQUIRED)
find_package(CcamCooL4SensorIo REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(gflags REQUIRED)
find_package(Glog REQUIRED)
find_package(islib REQUIRED)
find_package(XercesC REQUIRED)

include_directories(
  include
  ${CcamCooL4SensorIo_INCLUDE_DIR}
  ${Protobuf_INCLUDE_DIRS}
  ${islib_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${gflags_INCLUDE_DIRS}
)
add_library(${PROJECT_NAME} SHARED
  src/${PROJECT_NAME}/utils.cpp
  src/${PROJECT_NAME}/dm_bridge.cpp
  src/${PROJECT_NAME}/uploader.cpp
  src/${PROJECT_NAME}/socket.cpp
  src/${PROJECT_NAME}/plugin.cpp
)
target_link_libraries(${PROJECT_NAME}
  ${CcamCooL4SensorIo_LIBRARIES}
  ${Protobuf_LIBRARIES}
  ${islib_LIBRARIES}
  ${Boost_LIBRARIES}
  ${islib_LIBRARIES}
  ${XercesC_LIBRARIES}
  glog::glog
)
install(
  TARGETS
    ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

function(convert_camel_to_snake input output)
  set(result "")
  string(LENGTH "${input}" length)
  foreach(i RANGE 0 ${length})
    string(SUBSTRING "${input}" ${i} 1 char)
    if(char MATCHES "[A-Z]")
      if(NOT "${i}" STREQUAL "0")
        set(result "${result}_")
      endif()
      string(TOLOWER "${char}" char)
    endif()
    set(result "${result}${char}")
  endforeach()
  set(${output} "${result}" PARENT_SCOPE)
endfunction()

set(SUPPORT_TYPES SensingMessage)
foreach(type IN LISTS SUPPORT_TYPES)
  message(STATUS "${type}")
  convert_camel_to_snake("${type}" type_)
  message(STATUS "${type_}")

  add_executable(${PROJECT_NAME}_upload_${type_}
    src/${PROJECT_NAME}/upload.cpp
  )
  target_link_libraries(${PROJECT_NAME}_upload_${type_}
    ${PROJECT_NAME}
    gflags
  )
  target_compile_options(
    ${PROJECT_NAME}_upload_${type_}
    PRIVATE
      -DDATA_TYPE=${type}
      -DBRIDGE_TYPE=${type}DmBridge)
  install(
    TARGETS
      ${PROJECT_NAME}_upload_${type_}
    EXPORT ${PROJECT_NAME}_upload_${type}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)
endforeach()

install(
  DIRECTORY include/
  DESTINATION include)

install(EXPORT ${PROJECT_NAME}
  FILE ${PROJECT_NAME}-config.cmake
  DESTINATION share/cmake/${PROJECT_NAME}/
  EXPORT_LINK_INTERFACE_LIBRARIES)


if(TEST)
  find_package(GTest REQUIRED)
  include(GoogleTest)
  enable_testing()
  set(GTEST_LIBRARIES gtest gtest_main pthread)


  set(TESTS_NAME utils dm_bridge socket uploader combine)

  foreach(test_name IN LISTS TESTS_NAME)
    add_executable(${PROJECT_NAME}_${test_name}Test
      test/test_${test_name}.cpp)
    target_link_libraries(${PROJECT_NAME}_${test_name}Test
      ${GTEST_LIBRARIES}
      ${PROJECT_NAME}
     )

    if(ACCEPTABLE_RECEIVE_TIME_MSEC)
      target_compile_options(
      ${PROJECT_NAME}_${test_name}Test
      PRIVATE
        -DACCEPTABLE_RECEIVE_TIME_MSEC=${ACCEPTABLE_RECEIVE_TIME_MSEC})
    endif()

    if(TEST_SPIN_MSEC)
      target_compile_options(
      ${PROJECT_NAME}_${test_name}Test
      PRIVATE
        -DTEST_SPIN_MSEC=${TEST_SPIN_MSEC})
    endif()

    if(SEND_INTERVAL_MSEC)
      target_compile_options(
      ${PROJECT_NAME}_${test_name}Test
      PRIVATE
        -DSEND_INTERVAL_MSEC=${SEND_INTERVAL_MSEC})
    endif()

    if(WAITING_PERIOD_MSEC)
      target_compile_options(
      ${PROJECT_NAME}_${test_name}Test
      PRIVATE
        -DWAITING_PERIOD_MSEC=${WAITING_PERIOD_MSEC})
    endif()

    if(TEST_OBJ_NUM)
      target_compile_options(
      ${PROJECT_NAME}_${test_name}Test
      PRIVATE
        -DTEST_OBJ_NUM=${TEST_OBJ_NUM})
    endif()

    if(TEST_MSG_LEN)
      target_compile_options(
      ${PROJECT_NAME}_${test_name}Test
      PRIVATE
        -DTEST_MSG_LEN=${TEST_OBJ_NUM})
    endif()

    if(CHECK_POINT)
      target_compile_options(
      ${PROJECT_NAME}_${test_name}Test
      PRIVATE
        -DCHECK_POINT=${CHECK_POINT})
    endif()

    if(THROUGHPUT_CALCULATE_TIME_MSEC)
      target_compile_options(
      ${PROJECT_NAME}_${test_name}Test
      PRIVATE
        -DTHROUGHPUT_CALCULATE_TIME_MSEC=${THROUGHPUT_CALCULATE_TIME_MSEC})
    endif()

    gtest_discover_tests(${PROJECT_NAME}_${test_name}Test)
  endforeach()

endif()


