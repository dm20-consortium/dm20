# Ubuntu 20.04想定のCMakeListsである
cmake_minimum_required (VERSION 3.10)
project(dm2proto)

find_package(PkgConfig REQUIRED)

# protobufは自前でビルドしたものをリンクする
# set(protobuf_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/../../protoc/include)
# file(GLOB protobuf_LIBRARIES ${PROJECT_SOURCE_DIR}/../../protoc/lib/*.so)
find_package(Protobuf REQUIRED)

message(STATUS
  "protobuf_INCLUDE_DIRS ${Protobuf_INCLUDE_DIRS}\n"
  "-- protobuf_LIBRARIES ${Protobuf_LIBRARIES}\n"
)

include_directories(
  ${Protobuf_INCLUDE_DIRS}
)

# .protoファイルがあるディレクトリを入力すると，ビルドしてくれる関数
function (BuildProto PROTO_FILES_DIR OTHER_PROTO_PATH)
  message(STATUS "current dir: ${PROTO_FILES_DIR}")

  get_filename_component(DIR_NAME "${PROTO_FILES_DIR}" NAME)
  message(STATUS "dir name   : ${DIR_NAME}")
  set(PROTO_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  # set(PROTO_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${DIR_NAME}")
  file(MAKE_DIRECTORY ${PROTO_OUTPUT_DIR})

  file(GLOB PROTO_FILES ${PROTO_FILES_DIR}/*.proto)
  message(STATUS "proto files: ${PROTO_FILES}")

  set(PROTO_OUTPUT_FILES)
  foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(NAME ${PROTO_FILE} NAME_WE)
    list(APPEND PROTO_OUTPUT_FILES
      ${PROTO_OUTPUT_DIR}/${NAME}.pb.h
      ${PROTO_OUTPUT_DIR}/${NAME}.pb.cc)
  endforeach()
  message(STATUS "protofiles: ${PROTO_OUTPUT_FILES}")
  add_custom_command(
    OUTPUT ${PROTO_OUTPUT_FILES}
    DEPENDS ${PROTO_FILES}
    COMMAND protoc --proto_path=${PROTO_FILES_DIR} --proto_path=${OTHER_PROTO_PATH} ${PROTO_FILES} --cpp_out=${PROTO_OUTPUT_DIR}
    VERBATIM)
  #  add_custom_target(${PROJECT_NAME}_${DIR_NAME}_
  #  DEPENDS ${PROTO_OUTPUT_FILES})
  source_group(TREE ${PROTO_FILES_DIR} FILES ${PROTO_FILES})
  add_library(${PROJECT_NAME}_${DIR_NAME} SHARED
    ${PROTO_OUTPUT_FILES})
  target_link_libraries(${PROJECT_NAME}_${DIR_NAME}
    ${Protobuf_LIBRARIES})
  link_directories(
    ${Protobuf_LIBRARIES})

  # Extract Header files from output list
  set(PROTO_HEADERS)
  foreach(PROTO_OUTPUT_FILE ${PROTO_OUTPUT_FILES})
    if(${PROTO_OUTPUT_FILE} MATCHES "\\.pb.h")
      list(APPEND PROTO_HEADERS ${PROTO_OUTPUT_FILE})
    endif()
  endforeach()
  message(STATUS "headers: ${PROTO_HEADERS}")

  install(
    FILES
      ${PROTO_HEADERS}
    DESTINATION
      include/is/proto
  )
  install(
    TARGETS
      ${PROJECT_NAME}_${DIR_NAME}
    EXPORT ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
  )

endfunction()

# apiのビルド
BuildProto(${CMAKE_CURRENT_LIST_DIR}/api "")
# isのビルド(depends on api)
BuildProto(${CMAKE_CURRENT_LIST_DIR}/is ${CMAKE_CURRENT_LIST_DIR}/api)
