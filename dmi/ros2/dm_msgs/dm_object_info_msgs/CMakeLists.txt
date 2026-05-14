cmake_minimum_required(VERSION 3.5)
project(dm_object_info_msgs)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

set(msg_files
  "msg/Acceleration.msg"
  "msg/AccelerationAccuracy.msg"
  "msg/AccelerationValue.msg"
  "msg/Altitude.msg"
  "msg/AltitudeAccuracy.msg"
  "msg/AuxiliaryBrakeState.msg"
  "msg/BrakeState.msg"
  "msg/ClassConfidence.msg"
  "msg/ClassId.msg"
  "msg/ControlSystemState.msg"
  "msg/ControlSystemStates.msg"
  "msg/CrpId.msg"
  "msg/DistanceRatio.msg"
  "msg/DistanceValue.msg"
  "msg/ExistenceConfidence.msg"
  "msg/ExteriorLights.msg"
  "msg/Srid.msg"
  "msg/LaneCount.msg"
  "msg/LaneId.msg"
  "msg/LaneLateralPosition.msg"
  "msg/LanePosition.msg"
  "msg/Latitude.msg"
  "msg/Location.msg"
  "msg/Longitude.msg"
  "msg/ObjectClass.msg"
  "msg/ObjectDimension.msg"
  "msg/ObjectDimensionAccuracy.msg"
  "msg/ObjectDimensionValue.msg"
  "msg/ObjectId.msg"
  "msg/ObjectInfo.msg"
  "msg/ObjectInfoArray.msg"
  "msg/ObjectSize.msg"
  "msg/ReferencePoint.msg"
  "msg/Revision.msg"
  "msg/SemiAxisLength.msg"
  "msg/ShiftPosition.msg"
  "msg/Speed.msg"
  "msg/SpeedAccuracy.msg"
  "msg/SpeedValue.msg"
  "msg/SteeringAngle.msg"
  "msg/SubclassType.msg"
  "msg/ThrottlePosition.msg"
  "msg/TimestampIts.msg"
  "msg/VehicleExtendedInformation.msg"
  "msg/VehicleRole.msg"
  "msg/Azimuth.msg"
  "msg/AzimuthValue.msg"
  "msg/AzimuthAccuracy.msg"
  "msg/YawRate.msg"
  "msg/YawRateAccuracy.msg"
  "msg/YawRateValue.msg"
  "msg/TrackingStatus.msg"
  "msg/DetectionCount.msg"
  "msg/LostCount.msg"
  "msg/ObjectAge.msg"
  "msg/VehicleSizeType.msg"
  "msg/StaticStatus.msg"
)
rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES
  ADD_LINTER_TESTS
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()
