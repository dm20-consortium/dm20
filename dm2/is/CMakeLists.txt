# Ubuntu 20.04想定のCMakeListsである
cmake_minimum_required (VERSION 3.10)
project(dm2is CXX)

set(CMAKE_CXX_FLAGS "-std=c++14 -MMD -MP -O3 -Wextra -Winit-self -Wall -Wno-deprecated-declarations -Wno-shift-count-overflow -Wno-misleading-indentation -Wno-unused-variable -Wno-missing-field-initializers -Wno-unused-parameter -Wno-maybe-uninitialized")
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# 依存パッケージの探索(required)
find_package(Threads REQUIRED)
find_package(Log4cxx REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(UUID REQUIRED uuid)
find_package(XercesC REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(GEOS REQUIRED)
find_package(yaml-cpp REQUIRED)
# zstdのcmakeファイルが公開されていないので，いずれつくる
# find_package(ZSTD REQUIRED)
set(zstd_INCLUDE_DIRS /usr/include)
set(zstd_LIBRARIES /usr/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu/libzstd.so)
set(pqxx_INCLUDE_DIRS /usr/include)
set(pqxx_LIBRARIES /usr/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu/libpqxx.so)
# commonlibはプロジェクトツリー内に成果物があるのでそれをリンクする
set(commonlib_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/../commonLib/include/dm2SocketInterface)
set(commonlib_LIBRARIES ${PROJECT_SOURCE_DIR}/../commonLib/lib/libdm2SocketInterface.so)
# cslibもなんかインクルードしている
set(cslib_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/../csLib/include)
# seclib
set(seclib_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/../secLib/include)
find_package(islib REQUIRED)
find_package(Dm2Proto REQUIRED)
set(geos_c_LIBRARIES /usr/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu/libgeos_c.so)
add_compile_definitions(GEOS_VER=${GEOS_VER})
message(
  "CMAKE_THREAD_LIBS_INIT ${CMAKE_THREAD_LIBS_INIT}\n"
  "CMAKE_DL_LIBS ${CMAKE_DL_LIBS}\n"
  "UUID_INCLUDE_DIRS ${UUID_INCLUDE_DIRS}\n"
  "UUID_LIBRARIES ${UUID_LIBRARIES}\n"
  "pqxx_INCLUDE_DIRS ${pqxx_INCLUDE_DIRS}\n"
  "pqxx_LIBRARIES ${pqxx_LIBRARIES}\n"
  "Log4cxx_INCLUDE_DIR ${Log4cxx_INCLUDE_DIR}\n"
  "Log4cxx_LIBRARY ${Log4cxx_LIBRARY}\n"
  "XercesC_INCLUDE_DIRS ${XercesC_INCLUDE_DIRS}\n"
  "XercesC_LIBRARIES ${XercesC_LIBRARIES}\n"
  "OpenSSL_INCLUDE_DIR ${OPENSSL_INCLUDE_DIR}\n"
  "OpenSSL_LIBRARIES ${OPENSSL_LIBRARIES}\n"
  "ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS}\n"
  "ZLIB_LIBRARIES ${ZLIB_LIBRARIES}\n"
  "GEOS_INCLUDE_DIR ${GEOS_INCLUDE_DIR}\n"
  "GEOS_LIBRARIES ${GEOS_LIBRARIES}\n"
  "zstd_INCLUDE_DIR ${zstd_INCLUDE_DIRS}\n"
  "zstd_LIBRARIES ${zstd_LIBRARIES}\n"
  "commonlib_INCLUDE_DIR ${commonlib_INCLUDE_DIRS}\n"
  "commonlib_LIBRARIES ${commonlib_LIBRARIES}\n"
  "cslib_INCLUDE_DIRS ${cslib_INCLUDE_DIRS}\n"
  "YAML_CPP_INCLUDE_DIR ${YAML_CPP_INCLUDE_DIR}\n"
  "YAML_CPP_LIBRARIES ${YAML_CPP_LIBRARIES}\n"
  "islib_INCLUDE_DIR ${islib_INCLUDE_DIR}\n"
  "islib_LIBRARIES ${islib_LIBRARIES}\n"
  "dm2proto_INCLUDE_DIR ${dm2proto_INCLUDE_DIR}\n"
  "dm2proto_LIBRARIES ${dm2proto_LIBRARIES}\n"
  )

# (optional)
# UDP-DMIがいた場合の処理
set(USE_UDP_DMI 0)
find_package(UdpDmi QUIET)
if(${udp_dmi_FOUND})
  set(USE_UDP_DMI 1)
endif()
message(STATUS "USE_UDP_DMI: ${USE_UDP_DMI}")

# cmake .. -DROS2_WORKSPACE_DIR=/path/to/ros2/workspace/directory
set(ROS2_WORKSPACE_DIR)
set(USE_ROS2DMI 0)
find_package(Ros2Dmi QUIET)
if(${ros2dmi_FOUND})
  set(USE_ROS2DMI 1)
endif()
message(STATUS "USE_ROS2DMI: ${USE_ROS2DMI}")

set(USE_PROTOBUF_DMI 0)
find_package(ProtobufDmi QUIET)
if(${protobuf_dmi_FOUND})
  set(USE_PROTOBUF_DMI 1)
endif()
message(STATUS "USE_PROTOBUF_DMI: ${USE_PROTOBUF_DMI}")

include_directories(
  include
  ${Log4cxx_INCLUDE_DIRS}
  ${UUID_INCLUDE_DIRS}
  ${pqxx_INCLUDE_DIRS}
  ${XercesC_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIRS}
  ${GEOS_INCLUDE_DIR}
  ${zstd_INCLUDE_DIRS}
  ${protobuf_INCLUDE_DIRS}
  ${commonlib_INCLUDE_DIRS}
  ${cslib_INCLUDE_DIRS}
  ${seclib_INCLUDE_DIRS}
  ${islib_INCLUDE_DIR}
  ${dm2proto_INCLUDE_DIR}
  ${YAML_CPP_INCLUDE_DIR}
  )

file(GLOB SOURCE_LIST source/*.cpp)
list(APPEND SOUCE_LIST ${PROTO_OUTPUT_FILES})
add_executable(${PROJECT_NAME}
  ${SOURCE_LIST}
  )
target_link_libraries(${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_DL_LIBS}
  ${pqxx_LIBRARIES}
  ${XercesC_LIBRARIES}
  ${UUID_LIBRARIES}
  ${Log4cxx_LIBRARY}
  ${GEOS_LIBRARIES}
  ${geos_c_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${dmclient_LIBRARIES}
  ${commonlib_LIBRARIES}
  ${tuple_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${zstd_LIBRARIES}
  ${protobuf_LIBRARIES}
  ${YAML_CPP_LIBRARIES}
  ${islib_LIBRARIES}
  ${dm2proto_LIBRARIES}
  rt
  dm2security
  )
target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${PROTO_OUTPUT_DIR}
  )
install(
  TARGETS
    ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
# ubuntu 18.04の場合はこっち
#target_compile_definitions(
#  ${PROJECT_NAME}
#  PRIVATE
#    USE_ROS2DMI=${USE_ROS2DMI}
#    ROS2_WORKSPACE_DIR="${ROS2_WORKSPACE_DIR}"
#)
# ubuntu 20.04以降はこっち
 add_compile_definitions(
   USE_UDP_DMI=${USE_UDP_DMI}
   USE_ROS2DMI=${USE_ROS2DMI}
   USE_PROTOBUF_DMI=${USE_PROTOBUF_DMI}
   USE_ASMOBI_DMI=${USE_ASMOBI_DMI}
   USE_KOITO_DMI=${USE_KOITO_DMI}
   USE_NS_DMI=${USE_NS_DMI}
   ROS2_WORKSPACE_DIR="${ROS2_WORKSPACE_DIR}"
 )
