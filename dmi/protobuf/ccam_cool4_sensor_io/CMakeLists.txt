cmake_minimum_required (VERSION 3.12)
project (ccam_cool4_sensor_io CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall")

find_package(Threads)
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(gflags REQUIRED)
find_package(Glog REQUIRED)
find_package(Protobuf REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# ProtocolBuffersによる生成物の格納先の設定
set(PROTO_FILES_DIR "${CMAKE_CURRENT_LIST_DIR}/schema")
set(PROTO_FILES
  ${PROTO_FILES_DIR}/sensor_io.proto
)

foreach(PROTO_FILE ${PROTO_FILES})
  message(STATUS "protofile: ${PROTO_FILE}")
  # C++
  protobuf_generate_cpp(
    PROTO_CC
    PROTO_H
    ${PROTO_FILE}
    )
  list(APPEND PROTO_CCS ${PROTO_CC})
  list(APPEND PROTO_HS ${PROTO_H})

  # Python
  get_filename_component(PROTO_PY_ABS ${PROTO_FILE} ABSOLUTE)
  get_filename_component(PROTO_PY_NAME ${PROTO_FILE} NAME_WE)
  message(STATUS "protoc       : ${Protobuf_PROTOC_EXECUTABLE}")
  message(STATUS "proto py in  : ${PROTO_PY_ABS}")
  message(STATUS "proto py name: ${PROTO_PY_NAME}")
  message(STATUS "proto path   : ${CMAKE_CURRENT_SOURCE_DIR}")
  message(STATUS "python out   : ${CMAKE_CURRENT_BINARY_DIR}")
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_PY_NAME}_pb2.py
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/schema
         --python_out=${CMAKE_CURRENT_BINARY_DIR}
         ${PROTO_PY_ABS}
    DEPENDS ${PROTO_PY_ABS}
    COMMENT "Generating Python code from ${PROTO_FILE}"
  )
  list(APPEND PY_PROTO_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_PY_NAME}_pb2.py)
endforeach()

add_library(${PROJECT_NAME}_types SHARED
  ${PROTO_CCS})
target_link_libraries(${PROJECT_NAME}_types
  ${Protobuf_LIBRARIES})
install(
  TARGETS ${PROJECT_NAME}_types
  EXPORT ${PROJECT_NAME}_types
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

include_directories(
  include
  ${Protobuf_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${gflags_INCLUDE_DIRS}
  ${ZLIB_INCLUDE_DIRS}
)
install(FILES
  ${PROTO_HS}
  DESTINATION include/${PROJECT_NAME}/)

add_custom_target(proto_python ALL DEPENDS ${PY_PROTO_SRCS})
execute_process(
  COMMAND "${Python3_EXECUTABLE}" -c "import site; print(site.getsitepackages()[0])"
  OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
message(STATUS "python3 dist pack: ${PYTHON_SITE_PACKAGES}")
install(FILES ${PY_PROTO_SRCS}
  DESTINATION ${PYTHON_SITE_PACKAGES}/${PROJECT_NAME}
  )

# samples
add_executable(${PROJECT_NAME}_sample_generate
  src/${PROJECT_NAME}/sample_generate.cpp
)
target_link_libraries(${PROJECT_NAME}_sample_generate
  ${PROJECT_NAME}_types
)
add_executable(${PROJECT_NAME}_sample_send_msg
  src/${PROJECT_NAME}/sample_send_msg.cpp
)
target_link_libraries(${PROJECT_NAME}_sample_send_msg
  ${PROJECT_NAME}_types
  ${Boost_LIBRARIES}
  gflags
  glog::glog
  ${ZLIB_LIBRARIES}
)
add_executable(${PROJECT_NAME}_sample_recv_msg
  src/${PROJECT_NAME}/sample_recv_msg.cpp
)
target_link_libraries(${PROJECT_NAME}_sample_recv_msg
  ${PROJECT_NAME}_types
  ${Boost_LIBRARIES}
  glog::glog
  gflags
  ${ZLIB_LIBRARIES}
)

install(
  TARGETS
    ${PROJECT_NAME}_sample_generate
    ${PROJECT_NAME}_sample_send_msg
    ${PROJECT_NAME}_sample_recv_msg
  EXPORT ${PROJECT_NAME}_samples
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(
  DIRECTORY include/
  DESTINATION include)

install(EXPORT ${PROJECT_NAME}_types
  FILE ${PROJECT_NAME}-config.cmake
  DESTINATION share/cmake/${PROJECT_NAME}/
  EXPORT_LINK_INTERFACE_LIBRARIES)
