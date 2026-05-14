find_path(
  CcamCooL4SensorIo_INCLUDE_DIR
  NAMES
    sensor_io.pb.h receiver.hpp sender.hpp
  PATHS
    /usr/local/include
  PATH_SUFFIXES
    ccam_cool4_sensor_io
)

message(STATUS "CcamCooL4SensorIo include dirs: ${CcamCooL4SensorIo_INCLUDE_DIR}")
if(${CcamCooL4SensorIo_INCLUDE_DIR} STREQUAL "CcamCooL4SensorIo_INCLUDE_DIR-NOTFOUND")
  set(CcamCooL4SensorIo_FOUND "NO")
  return()
endif()


set(CcamCooL4SensorIo_LIBRARIES)
set(SEARCH_TARGETS ccam_cool4_sensor_io_types)
foreach(search_target IN LISTS SEARCH_TARGETS)
  find_library(
    ${search_target}_LIB
    NAMES
      ${search_target}
    PATHS
      /usr/local/lib
      /usr/lib
      /usr/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu
  )
  set(n "${search_target}_LIB")
  if("${${n}}" STREQUAL "${search_target}_LIB-NOTFOUND")
    message(STATUS "${n} is not found")
    set(CcamCooL4SensorIo_FOUND "NO")
    return()
  endif()
  LIST(APPEND CcamCooL4SensorIo_LIBRARIES "${${n}}")
endforeach()

LIST(APPEND CcamCooL4SensorIo_LIBRARIES "${XercesC_LIBRARIES}")

message(STATUS "CcamCooL4SensorIo libraries: ${CcamCooL4SensorIo_LIBRARIES}")


set(CcamCooL4SensorIo_FOUND "YES")
