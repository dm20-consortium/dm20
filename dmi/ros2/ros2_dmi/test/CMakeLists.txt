include_directories(include)

# Getters
ament_add_gtest(${PROJECT_NAME}_getter_objectid_test src/test_getter_objectid.cpp)
ament_target_dependencies(${PROJECT_NAME}_getter_objectid_test
  dm_object_info_msgs
)
target_link_libraries(${PROJECT_NAME}_getter_objectid_test
  ${Boost_LIBRARIES}
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)
ament_add_gtest(${PROJECT_NAME}_getter_location_test src/test_getter_location.cpp)
ament_target_dependencies(${PROJECT_NAME}_getter_location_test
  dm_object_info_msgs
)
target_link_libraries(${PROJECT_NAME}_getter_location_test
  ${Boost_LIBRARIES}
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)


# ObjectInfo Converter
ament_add_gtest(${PROJECT_NAME}_objectinfo_test src/test_converter_objectinfo.cpp)
ament_target_dependencies(${PROJECT_NAME}_objectinfo_test
  dm_object_info_msgs
)
target_link_libraries(${PROJECT_NAME}_objectinfo_test
  ${Boost_LIBRARIES}
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)
ament_add_gtest(${PROJECT_NAME}_objectinfoarray_test src/test_converter_objectinfoarray.cpp)
ament_target_dependencies(${PROJECT_NAME}_objectinfoarray_test
  dm_object_info_msgs
)
target_link_libraries(${PROJECT_NAME}_objectinfoarray_test
  ${Boost_LIBRARIES}
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)

# SignalInfo Converter
ament_add_gtest(${PROJECT_NAME}_signalinfo_test src/test_converter_signalinfo.cpp)
ament_target_dependencies(${PROJECT_NAME}_signalinfo_test
  dm_signal_info_msgs
)
target_link_libraries(${PROJECT_NAME}_signalinfo_test
  ${Boost_LIBRARIES}
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)
ament_add_gtest(${PROJECT_NAME}_signalinfoarray_test src/test_converter_signalinfoarray.cpp)
ament_target_dependencies(${PROJECT_NAME}_signalinfoarray_test
  dm_signal_info_msgs
)
target_link_libraries(${PROJECT_NAME}_signalinfoarray_test
  ${Boost_LIBRARIES}
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)

# FreespaceInfo Converter
ament_add_gtest(${PROJECT_NAME}_freespaceinfo_test src/test_converter_freespaceinfo.cpp)
ament_target_dependencies(${PROJECT_NAME}_freespaceinfo_test
  dm_freespace_info_msgs
)
target_link_libraries(${PROJECT_NAME}_freespaceinfo_test
  ${Boost_LIBRARIES}
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)
ament_add_gtest(${PROJECT_NAME}_freespaceinfoarray_test src/test_converter_freespaceinfoarray.cpp)
ament_target_dependencies(${PROJECT_NAME}_freespaceinfoarray_test
  dm_freespace_info_msgs
)
target_link_libraries(${PROJECT_NAME}_freespaceinfoarray_test
  ${Boost_LIBRARIES}
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)


# Test Publisher
ament_add_gtest(${PROJECT_NAME}_publisher_test src/test_publisher_publisher.cpp)
ament_target_dependencies(${PROJECT_NAME}_publisher_test
  dm_freespace_info_msgs
  rclcpp
)
target_link_libraries(${PROJECT_NAME}_publisher_test
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)

# Test Publish
ament_add_gtest(${PROJECT_NAME}_publish_test src/test_publisher_publish.cpp)
ament_target_dependencies(${PROJECT_NAME}_publish_test
  dm_freespace_info_msgs
  dm_sensor_info_msgs
  dm_signal_info_msgs
  dm_object_info_msgs
  rclcpp
)
target_link_libraries(${PROJECT_NAME}_publish_test
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)

# Test PublisherFactory
ament_add_gtest(${PROJECT_NAME}_publisherfactory_test src/test_publisherfactory.cpp)
ament_target_dependencies(${PROJECT_NAME}_publisherfactory_test
  dm_object_info_msgs
  dm_freespace_info_msgs
  dm_signal_info_msgs
  dm_sensor_info_msgs
  rclcpp
)
target_link_libraries(${PROJECT_NAME}_publisherfactory_test
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)

# Test Subscriber
ament_add_gtest(${PROJECT_NAME}_subscriber_test src/test_subscriber_subscriber.cpp)
ament_target_dependencies(${PROJECT_NAME}_subscriber_test
  dm_freespace_info_msgs
  rclcpp
)
target_link_libraries(${PROJECT_NAME}_subscriber_test
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)

# Test Update
ament_add_gtest(${PROJECT_NAME}_update_test src/test_subscriber_update.cpp)
ament_target_dependencies(${PROJECT_NAME}_update_test
  dm_freespace_info_msgs
  rclcpp
)
target_link_libraries(${PROJECT_NAME}_update_test
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)

# Test SubscriberFactory
ament_add_gtest(${PROJECT_NAME}_subscriberfactory_test src/test_subscriberfactory.cpp)
ament_target_dependencies(${PROJECT_NAME}_subscriberfactory_test
  dm_object_info_msgs
  dm_freespace_info_msgs
  dm_signal_info_msgs
  dm_sensor_info_msgs
  rclcpp
)
target_link_libraries(${PROJECT_NAME}_subscriberfactory_test
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)

# Test Downloader
ament_add_gtest(${PROJECT_NAME}_downloader_test src/test_downloader.cpp)
ament_target_dependencies(${PROJECT_NAME}_downloader_test
  dm_freespace_info_msgs
  std_msgs
  rclcpp
)
target_link_libraries(${PROJECT_NAME}_downloader_test
  ${Boost_LIBRARIES}
  ${DM_LIBRARIES}
  ${PROJECT_NAME}
  ${islib_LIBRARIES}
  ZLIB::ZLIB
)

# Test Uploader
ament_add_gtest(${PROJECT_NAME}_uploader_test src/test_uploader.cpp)
ament_target_dependencies(${PROJECT_NAME}_uploader_test
  dm_freespace_info_msgs
  std_msgs
  rclcpp
)
target_link_libraries(${PROJECT_NAME}_uploader_test
  ${Boost_LIBRARIES}
  ${DM_LIBRARIES}
  ${islib_LIBRARIES}
  ${PROJECT_NAME}
  ZLIB::ZLIB
)

