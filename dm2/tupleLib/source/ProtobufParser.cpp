#include "is/ProtobufParser.h"

#include <iostream>
#include <arpa/inet.h>

using namespace std;

namespace IS {
	// -1は、インデックス値を合わせるためのダミー値であり、実際は使用していない。
	int objectInfoUnknownVals_0_6_0[] = {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 900000001, 1800000001, 800001, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, 0, 36001, 9001, 16383, 16383, 32767, 32767, 2001, 1001, 36001, 9001, 65535, 65535, 65535, 65535, 65535, 65535, 0, 7, 91, 91, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 255, 0, -1};
	int objectInfoUnknownVals_0_8_0[] = {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 900000001, 1800000001, 800001, 0, 0, 0, 0, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, 0, 36001, 9001, 16383, 16383, 32767, 32767, 2001, 1001, 36001, 9001, 65535, 65535, 65535, 65535, 65535, 65535, 3602, 7, 91, 91, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 255, 0, 0, 0, 0, 36001, -1};
	int objectInfoUnknownVals_0_8_1[] = {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 900000001, 1800000001, 800001, 0, 0, 0, 0, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, 0, 36001, 9001, 16383, 16383, 32767, 32767, 2001, 1001, 36001, 9001, 65535, 65535, 65535, 65535, 65535, 65535, 3602, 7, 91, 91, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 255, 0, 0, 0, 0, 36001, -1};
	int freespaceInfoUnknownVals_0_6_0[] = {-1, -1, 0, 65535, 0, 900000001, 1800000001, 800001, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, 0, 900000001, 1800000001, 800001, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, 65535, 65535, 0, 0, -1};
	int freespaceInfoUnknownVals_0_8_0[] = {-1, -1, 0, 0, 0, 900000001, 1800000001, 800001, 0, 0, 0, 0, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, -1, -1, 0, 900000001, 1800000001, 800001, 0, 0, 0, 0, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, 0, 900000001, 1800000001, 800001, 0, 0, 0, 0, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, 65535, 65535, 0, 0, 0, 65535, -1};
	int freespaceInfoUnknownVals_0_8_1[] = {-1, -1, 0, 0, 0, 0, 900000001, 1800000001, 800001, 0, 0, 0, 0, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, -1, -1, 0, 900000001, 1800000001, 800001, 0, 0, 0, 0, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, 0, 900000001, 1800000001, 800001, 0, 0, 0, 0, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, 65535, 65535, 0, 0, 0, 65535, -1};
	int signalInfoUnknownVals_0_6_0[] = {-1, -1, -1, 1, 0, 128, 0, 0, 0, 65535, 65535, 0, 0, 65535, 65535, 0, 0, 65535, 65535, 0, 0, 65535, 65535, 0, 0, 65535, 65535, 0, 0, 65535, 65535, 0, 0, 65535, 65535, 0, 0, 65535, 65535, 0, 0, 65535, 65535, 0, 0, 65535, 65535, 0, 0, 65535, 65535, 0, 0, 65535, 65535};
	int sensorInfoUnknownVals_0_8_0[] = {-1, -1, 0, 0, 900000001, 1800000001, 800001, 0, -1, -1, -1, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0};
	int sensorInfoUnknownVals_0_8_1[] = {-1, -1, 0, 0, 900000001, 1800000001, 800001, 0, -1, -1, -1, 0, -132768, -132768, -132768, 15, 17, 101, 0, 0, 10001, 0, -132768, -132768, -132768, 4095, 4095, 36001, 20001, -1, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 0, 0};

	// APL側ではスキーマ情報を保持していないため、暫定で定義
	// カラム名
	string objectInfoNameList_0_6_0[] = {"id", "time", "object_class_id_0", "object_class_confidence_0", "object_class_subclass_type_0", "object_class_subclass_confidence_0", "object_class_id_1", "object_class_confidence_1", "object_class_subclass_type_1", "object_class_subclass_confidence_1", "object_class_id_2", "object_class_confidence_2", "object_class_subclass_type_2", "object_class_subclass_confidence_2", "object_class_id_3", "object_class_confidence_3", "object_class_subclass_type_3", "object_class_subclass_confidence_3", "existency", "geodetic_system", "latitude", "longitude", "altitude", "crp_id", "dx_crp", "dy_crp", "dh_crp", "lane_count", "lane_position", "lane_lateral_position", "crp_id_begin", "crp_id_end", "lane_vertical_position", "lane_id", "dx_lane", "dy_lane", "dh_lane", "semi_axis_length_major", "semi_axis_length_minor", "ellipse_orientation", "altitude_accuracy", "reference_point", "move_direction_value", "move_direction_accuracy", "speed_value", "speed_accuracy", "yaw_rate_value", "yaw_rate_accuracy", "acceleration_value", "acceleration_accuracy", "orientation_value", "orientation_accuracy", "length_value", "length_accuracy", "width_value", "width_accuracy", "height_value", "height_accuracy", "color", "shift_position", "steering_angle_front", "steering_angle_rear", "brake_state", "auxiliary_brake_state", "throttle_position", "exterior_lights", "adaptive_cruise_control_system", "cooperative_adaptive_cruise_control_system", "pre_crash_safety_system", "antilock_brake_system", "traction_control_system", "electronic_stability_control_system", "lane_keeping_assist_system", "lane_departure_warning_system", "vehicle_role", "vehicle_extended_info", "towing_vehicle", "information_source_list", "dm2_creator_id", "dm2_ip_addr", "dm2_hash", "dm2_ts_group"};
	string objectInfoNameList_0_8_0[] = {"object_id", "time", "object_class_id_0", "object_class_confidence_0", "object_class_subclass_type_0", "object_class_subclass_confidence_0", "object_class_id_1", "object_class_confidence_1", "object_class_subclass_type_1", "object_class_subclass_confidence_1", "object_class_id_2", "object_class_confidence_2", "object_class_subclass_type_2", "object_class_subclass_confidence_2", "object_class_id_3", "object_class_confidence_3", "object_class_subclass_type_3", "object_class_subclass_confidence_3", "existency", "geodetic_srid", "latitude", "longitude", "altitude", "projective_srid", "prc_x", "prc_y", "prc_z", "crp_id", "dx_crp", "dy_crp", "dh_crp", "lane_count", "lane_position", "lane_lateral_position", "crp_id_begin", "crp_id_end", "lane_vertical_position", "lane_id", "dx_lane", "dy_lane", "dh_lane", "semi_axis_length_major", "semi_axis_length_minor", "ellipse_orientation", "altitude_accuracy", "reference_point", "move_direction_value", "move_direction_accuracy", "speed_value", "speed_accuracy", "yaw_rate_value", "yaw_rate_accuracy", "acceleration_value", "acceleration_accuracy", "orientation_value", "orientation_accuracy", "length_value", "length_accuracy", "width_value", "width_accuracy", "height_value", "height_accuracy", "static_status", "shift_position", "steering_angle_front", "steering_angle_rear", "brake_state", "auxiliary_brake_state", "throttle_position", "exterior_lights", "adaptive_cruise_control_system", "cooperative_adaptive_cruise_control_system", "pre_crash_safety_system", "antilock_brake_system", "traction_control_system", "electronic_stability_control_system", "lane_keeping_assist_system", "lane_departure_warning_system", "vehicle_size_type", "vehicle_role", "vehicle_extended_info", "towing_vehicle", "tracking_status", "detection_count", "lost_count", "object_age", "information_source_list", "dm2_creator_id", "dm2_ip_addr", "dm2_hash", "dm2_ts_group"};
	string objectInfoNameList_0_8_1[] = {"object_id", "time", "revision", "object_class_id_0", "object_class_confidence_0", "object_class_subclass_type_0", "object_class_subclass_confidence_0", "object_class_id_1", "object_class_confidence_1", "object_class_subclass_type_1", "object_class_subclass_confidence_1", "object_class_id_2", "object_class_confidence_2", "object_class_subclass_type_2", "object_class_subclass_confidence_2", "object_class_id_3", "object_class_confidence_3", "object_class_subclass_type_3", "object_class_subclass_confidence_3", "existency", "geodetic_srid", "latitude", "longitude", "altitude", "projective_srid", "prc_x", "prc_y", "prc_z", "crp_id", "dx_crp", "dy_crp", "dh_crp", "lane_count", "lane_position", "lane_lateral_position", "crp_id_begin", "crp_id_end", "lane_vertical_position", "lane_id", "dx_lane", "dy_lane", "dh_lane", "semi_axis_length_major", "semi_axis_length_minor", "ellipse_orientation", "altitude_accuracy", "reference_point", "move_direction_value", "move_direction_accuracy", "speed_value", "speed_accuracy", "yaw_rate_value", "yaw_rate_accuracy", "acceleration_value", "acceleration_accuracy", "orientation_value", "orientation_accuracy", "length_value", "length_accuracy", "width_value", "width_accuracy", "height_value", "height_accuracy", "static_status", "shift_position", "steering_angle_front", "steering_angle_rear", "brake_state", "auxiliary_brake_state", "throttle_position", "exterior_lights", "adaptive_cruise_control_system", "cooperative_adaptive_cruise_control_system", "pre_crash_safety_system", "antilock_brake_system", "traction_control_system", "electronic_stability_control_system", "lane_keeping_assist_system", "lane_departure_warning_system", "vehicle_size_type", "vehicle_role", "vehicle_extended_info", "towing_vehicle", "tracking_status", "detection_count", "lost_count", "object_age", "information_source_list", "dm2_creator_id", "dm2_ip_addr", "dm2_hash", "dm2_ts_group"};
	string freespaceInfoNameList_0_6_0[] = {"id", "time", "existency", "minimal_detectable_size", "position_begin_geodetic_system", "position_begin_latitude", "position_begin_longitude", "position_begin_altitude", "position_begin_crp_id", "position_begin_dx_crp", "position_begin_dy_crp", "position_begin_dh_crp", "position_begin_lane_count", "position_begin_lane_position", "position_begin_lane_lateral_position", "position_begin_crp_id_begin", "position_begin_crp_id_end", "position_begin_lane_vertical_position", "position_begin_lane_id", "position_begin_dx_lane", "position_begin_dy_lane", "position_begin_dh_lane", "position_begin_semi_axis_length_major", "position_begin_semi_axis_length_minor", "position_begin_ellipse_orientation", "position_begin_altitude_accuracy", "position_end_geodetic_system", "position_end_latitude", "position_end_longitude", "position_end_altitude", "position_end_crp_id", "position_end_dx_crp", "position_end_dy_crp", "position_end_dh_crp", "position_end_lane_count", "position_end_lane_position", "position_end_lane_lateral_position", "position_end_crp_id_begin", "position_end_crp_id_end", "position_end_lane_vertical_position", "position_end_lane_id", "position_end_dx_lane", "position_end_dy_lane", "position_end_dh_lane", "position_end_semi_axis_length_major", "position_end_semi_axis_length_minor", "position_end_ellipse_orientation", "position_end_altitude_accuracy", "length_value", "length_accuracy", "id_begin", "id_end", "information_source_list", "dm2_creator_id", "dm2_ip_addr", "dm2_hash", "dm2_ts_group"};
	string freespaceInfoNameList_0_8_0[] = {"id", "time", "detection_method", "detactable_classes", "vertices_begin_geodetic_srid", "vertices_begin_latitude", "vertices_begin_longitude", "vertices_begin_altitude", "vertices_begin_projective_srid", "vertices_begin_prc_x", "vertices_begin_prc_y", "vertices_begin_prc_z", "vertices_begin_crp_id", "vertices_begin_dx_crp", "vertices_begin_dy_crp", "vertices_begin_dh_crp", "vertices_begin_lane_count", "vertices_begin_lane_position", "vertices_begin_lane_lateral_position", "vertices_begin_crp_id_begin", "vertices_begin_crp_id_end", "vertices_begin_lane_vertical_position", "vertices_begin_lane_id", "vertices_begin_dx_lane", "vertices_begin_dy_lane", "vertices_begin_dh_lane", "vertices_begin_semi_axis_length_major", "vertices_begin_semi_axis_length_minor", "vertices_begin_ellipse_orientation", "vertices_begin_altitude_accuracy", "vertices_dx_list", "vertices_dy_list", "position_begin_geodetic_srid", "position_begin_latitude", "position_begin_longitude", "position_begin_altitude", "position_begin_projective_srid", "position_begin_prc_x", "position_begin_prc_y", "position_begin_prc_z", "position_begin_crp_id", "position_begin_dx_crp", "position_begin_dy_crp", "position_begin_dh_crp", "position_begin_lane_count", "position_begin_lane_position", "position_begin_lane_lateral_position", "position_begin_crp_id_begin", "position_begin_crp_id_end", "position_begin_lane_vertical_position", "position_begin_lane_id", "position_begin_dx_lane", "position_begin_dy_lane", "position_begin_dh_lane", "position_begin_semi_axis_length_major", "position_begin_semi_axis_length_minor", "position_begin_ellipse_orientation", "position_begin_altitude_accuracy", "position_end_geodetic_srid", "position_end_latitude", "position_end_longitude", "position_end_altitude", "position_end_projective_srid", "position_end_prc_x", "position_end_prc_y", "position_end_prc_z", "position_end_crp_id", "position_end_dx_crp", "position_end_dy_crp", "position_end_dh_crp", "position_end_lane_count", "position_end_lane_position", "position_end_lane_lateral_position", "position_end_crp_id_begin", "position_end_crp_id_end", "position_end_lane_vertical_position", "position_end_lane_id", "position_end_dx_lane", "position_end_dy_lane", "position_end_dh_lane", "position_end_semi_axis_length_major", "position_end_semi_axis_length_minor", "position_end_ellipse_orientation", "position_end_altitude_accuracy", "length_value", "length_accuracy", "id_begin", "id_end", "existency", "minimal_detectable_size", "information_source_list", "dm2_creator_id", "dm2_ip_addr", "dm2_hash", "dm2_ts_group"};
	string freespaceInfoNameList_0_8_1[] = {"id", "time", "revision", "detection_method", "detactable_classes", "vertices_begin_geodetic_srid", "vertices_begin_latitude", "vertices_begin_longitude", "vertices_begin_altitude", "vertices_begin_projective_srid", "vertices_begin_prc_x", "vertices_begin_prc_y", "vertices_begin_prc_z", "vertices_begin_crp_id", "vertices_begin_dx_crp", "vertices_begin_dy_crp", "vertices_begin_dh_crp", "vertices_begin_lane_count", "vertices_begin_lane_position", "vertices_begin_lane_lateral_position", "vertices_begin_crp_id_begin", "vertices_begin_crp_id_end", "vertices_begin_lane_vertical_position", "vertices_begin_lane_id", "vertices_begin_dx_lane", "vertices_begin_dy_lane", "vertices_begin_dh_lane", "vertices_begin_semi_axis_length_major", "vertices_begin_semi_axis_length_minor", "vertices_begin_ellipse_orientation", "vertices_begin_altitude_accuracy", "vertices_dx_list", "vertices_dy_list", "position_begin_geodetic_srid", "position_begin_latitude", "position_begin_longitude", "position_begin_altitude", "position_begin_projective_srid", "position_begin_prc_x", "position_begin_prc_y", "position_begin_prc_z", "position_begin_crp_id", "position_begin_dx_crp", "position_begin_dy_crp", "position_begin_dh_crp", "position_begin_lane_count", "position_begin_lane_position", "position_begin_lane_lateral_position", "position_begin_crp_id_begin", "position_begin_crp_id_end", "position_begin_lane_vertical_position", "position_begin_lane_id", "position_begin_dx_lane", "position_begin_dy_lane", "position_begin_dh_lane", "position_begin_semi_axis_length_major", "position_begin_semi_axis_length_minor", "position_begin_ellipse_orientation", "position_begin_altitude_accuracy", "position_end_geodetic_srid", "position_end_latitude", "position_end_longitude", "position_end_altitude", "position_end_projective_srid", "position_end_prc_x", "position_end_prc_y", "position_end_prc_z", "position_end_crp_id", "position_end_dx_crp", "position_end_dy_crp", "position_end_dh_crp", "position_end_lane_count", "position_end_lane_position", "position_end_lane_lateral_position", "position_end_crp_id_begin", "position_end_crp_id_end", "position_end_lane_vertical_position", "position_end_lane_id", "position_end_dx_lane", "position_end_dy_lane", "position_end_dh_lane", "position_end_semi_axis_length_major", "position_end_semi_axis_length_minor", "position_end_ellipse_orientation", "position_end_altitude_accuracy", "length_value", "length_accuracy", "id_begin", "id_end", "existency", "minimal_detectable_size", "information_source_list", "dm2_creator_id", "dm2_ip_addr", "dm2_hash", "dm2_ts_group"};
	string signalInfoNameList_0_6_0[] = {"crp_id", "id", "time", "state", "specific_control_flags", "event_count", "count_down_stop_flag", "light_info_main_0", "light_info_arrow_0", "light_info_min_time_to_change_0", "light_info_max_time_to_change_0", "light_info_main_1", "light_info_arrow_1", "light_info_min_time_to_change_1", "light_info_max_time_to_change_1", "light_info_main_2", "light_info_arrow_2", "light_info_min_time_to_change_2", "light_info_max_time_to_change_2", "light_info_main_3", "light_info_arrow_3", "light_info_min_time_to_change_3", "light_info_max_time_to_change_3", "light_info_main_4", "light_info_arrow_4", "light_info_min_time_to_change_4", "light_info_max_time_to_change_4", "light_info_main_5", "light_info_arrow_5", "light_info_min_time_to_change_5", "light_info_max_time_to_change_5", "light_info_main_6", "light_info_arrow_6", "light_info_min_time_to_change_6", "light_info_max_time_to_change_6", "light_info_main_7", "light_info_arrow_7", "light_info_min_time_to_change_7", "light_info_max_time_to_change_7", "light_info_main_8", "light_info_arrow_8", "light_info_min_time_to_change_8", "light_info_max_time_to_change_8", "light_info_main_9", "light_info_arrow_9", "light_info_min_time_to_change_9", "light_info_max_time_to_change_9", "light_info_main_10", "light_info_arrow_10", "light_info_min_time_to_change_10", "light_info_max_time_to_change_10", "light_info_main_11", "light_info_arrow_11", "light_info_min_time_to_change_11", "light_info_max_time_to_change_11", "dm2_creator_id", "dm2_ip_addr", "dm2_hash", "dm2_ts_group"};
	string sensorInfoNameList_0_8_0[] = {"unit_id", "sensor_id", "sensor_type", "geodetic_srid", "latitude", "longitude", "altitude", "projective_srid", "prc_x", "prc_y", "prc_z", "crp_id", "dx_crp", "dy_crp", "dh_crp", "lane_count", "lane_position", "lane_lateral_position", "crp_id_begin", "crp_id_end", "lane_vertical_position", "lane_id", "dx_lane", "dy_lane", "dh_lane", "semi_axis_length_major", "semi_axis_length_minor", "ellipse_orientation", "altitude_accuracy", "detectable_classes_0", "detectable_domain_x_list_0", "detectable_domain_y_list_0", "confidence_0", "minimal_detectable_size_0", "detectable_classes_1", "detectable_domain_x_list_1", "detectable_domain_y_list_1", "confidence_1", "minimal_detectable_size_1", "detectable_classes_2", "detectable_domain_x_list_2", "detectable_domain_y_list_2", "confidence_2", "minimal_detectable_size_2", "detectable_classes_3", "detectable_domain_x_list_3", "detectable_domain_y_list_3", "confidence_3", "minimal_detectable_size_3", "detectable_classes_4", "detectable_domain_x_list_4", "detectable_domain_y_list_4", "confidence_4", "minimal_detectable_size_4", "detectable_classes_5", "detectable_domain_x_list_5", "detectable_domain_y_list_5", "confidence_5", "minimal_detectable_size_5", "detectable_classes_6", "detectable_domain_x_list_6", "detectable_domain_y_list_6", "confidence_6", "minimal_detectable_size_6", "detectable_classes_7", "detectable_domain_x_list_7", "detectable_domain_y_list_7", "confidence_7", "minimal_detectable_size_7", "sensor_status", "dm2_creator_id", "dm2_ip_addr", "dm2_hash", "dm2_ts_group"};
	string sensorInfoNameList_0_8_1[] = {"unit_id", "sensor_id", "sensor_type", "geodetic_srid", "latitude", "longitude", "altitude", "projective_srid", "prc_x", "prc_y", "prc_z", "crp_id", "dx_crp", "dy_crp", "dh_crp", "lane_count", "lane_position", "lane_lateral_position", "crp_id_begin", "crp_id_end", "lane_vertical_position", "lane_id", "dx_lane", "dy_lane", "dh_lane", "semi_axis_length_major", "semi_axis_length_minor", "ellipse_orientation", "altitude_accuracy", "time", "detectable_classes_0", "detectable_domain_x_list_0", "detectable_domain_y_list_0", "confidence_0", "minimal_detectable_size_0", "detectable_classes_1", "detectable_domain_x_list_1", "detectable_domain_y_list_1", "confidence_1", "minimal_detectable_size_1", "detectable_classes_2", "detectable_domain_x_list_2", "detectable_domain_y_list_2", "confidence_2", "minimal_detectable_size_2", "detectable_classes_3", "detectable_domain_x_list_3", "detectable_domain_y_list_3", "confidence_3", "minimal_detectable_size_3", "detectable_classes_4", "detectable_domain_x_list_4", "detectable_domain_y_list_4", "confidence_4", "minimal_detectable_size_4", "detectable_classes_5", "detectable_domain_x_list_5", "detectable_domain_y_list_5", "confidence_5", "minimal_detectable_size_5", "detectable_classes_6", "detectable_domain_x_list_6", "detectable_domain_y_list_6", "confidence_6", "minimal_detectable_size_6", "detectable_classes_7", "detectable_domain_x_list_7", "detectable_domain_y_list_7", "confidence_7", "minimal_detectable_size_7", "sensor_status", "dm2_creator_id", "dm2_ip_addr", "dm2_hash", "dm2_ts_group"};
	// データタイプ
	const std::vector<std::string>  objectInfoTypeList_0_6_0 = {"string", "long", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "string", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "string", "vector(string)", "string", "string", "string", "string"};
	const std::vector<std::string>  objectInfoTypeList_0_8_0 = {"ulong", "long", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "vector(ulong)", "string", "string", "string", "string"};
	const std::vector<std::string>  objectInfoTypeList_0_8_1 = {"ulong", "long", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "vector(ulong)", "string", "string", "string", "string"};
	const std::vector<std::string>  freespaceInfoTypeList_0_6_0 = {"string", "long", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "string", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "string", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "string", "vector(string)", "string", "string", "string", "string"};
	const std::vector<std::string>  freespaceInfoTypeList_0_8_0 = {"ulong", "long", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "ulong", "int", "int", "vector(ulong)", "string", "string", "string", "string"};
	const std::vector<std::string>  freespaceInfoTypeList_0_8_1 = {"ulong", "long", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "ulong", "int", "int", "vector(ulong)", "string", "string", "string", "string"};
	const std::vector<std::string>  signalInfoTypeList_0_6_0 = {"int", "vector(int)", "long", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "string", "string", "string", "string"};
	const std::vector<std::string>  sensorInfoTypeList_0_8_0 = {"ulong", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "int", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "string", "string", "string", "string"};
	const std::vector<std::string>  sensorInfoTypeList_0_8_1 = {"ulong", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "ulong", "int", "int", "int", "int", "int", "int", "int", "long", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "vector(int)", "vector(int)", "int", "int", "int", "string", "string", "string", "string"};
	const int is_tuple_size = 4;
	const int objectInfo_0_6_0_normal_size = objectInfoTypeList_0_6_0.size() - is_tuple_size;
	const int objectInfo_0_8_0_normal_size = objectInfoTypeList_0_8_0.size() - is_tuple_size;
	const int objectInfo_0_8_1_normal_size = objectInfoTypeList_0_8_1.size() - is_tuple_size;
	const int freespaceInfo_0_6_0_normal_size = freespaceInfoTypeList_0_6_0.size() - is_tuple_size;
	const int freespaceInfo_0_8_0_normal_size = freespaceInfoTypeList_0_8_0.size() - is_tuple_size;
	const int freespaceInfo_0_8_1_normal_size = freespaceInfoTypeList_0_8_1.size() - is_tuple_size;
	const int signalInfo_0_6_0_normal_size = signalInfoTypeList_0_6_0.size() - is_tuple_size;
	const int sensorInfo_0_8_0_normal_size = sensorInfoTypeList_0_8_0.size() - is_tuple_size;
	const int sensorInfo_0_8_1_normal_size = sensorInfoTypeList_0_8_1.size() - is_tuple_size;
	// 注意）下記のtypeStringMapはヘッダファイルに移動すると、コアダンプエラーになります
	std::map<std::type_index, std::string> typeStringMap = {
		{typeid(int), "int"},
		{typeid(long), "long"},
		{typeid(double), "double"},
		{typeid(float), "float"},
		{typeid(string), "string"},
		{typeid(bool), "bool"},
		{typeid(unsigned int), "uint"},
		{typeid(unsigned long), "ulong"},
		{typeid(unsigned long long), "ulong"},
		{typeid(vector<int>), "vector(int)"},
		{typeid(vector<long>), "vector(long)"},
		{typeid(vector<double>), "vector(double)"},
		{typeid(vector<string>), "vector(string)"},
		{typeid(vector<bool>), "vector(bool)"},
		{typeid(vector<unsigned int>), "vector(uint)"},
		{typeid(vector<unsigned long>), "vector(ulong)"},
		{typeid(vector<unsigned long long>), "vector(ulong)"},
		{typeid(vector<vector<int>>), "vector(vector(int))"},
		{typeid(vector<vector<long>>), "vector(vector(long))"},
		{typeid(vector<vector<double>>), "vector(vector(double))"},
		{typeid(vector<vector<string>>), "vector(vector(string))"},
		{typeid(vector<vector<bool>>), "vector(vector(bool))"},
		{typeid(vector<vector<unsigned int>>), "vector(vector(uint))"},
		{typeid(vector<vector<unsigned long>>), "vector(vector(ulong))"},
		{typeid(vector<vector<unsigned long long>>), "vector(vector(ulong))"}
	};
	/**
	* 初期処理
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	*/

	void ProtobufParser::init()
	{
	}

	/**
	* 終了処理
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	 */

	void ProtobufParser::finalize()
	{
	}

	/**
	 * protobufヘッダ作成(セッションキー無し)
	 * 
	 * @author	Nagoya University
	 * @date 2025/10/31
	 * 
	 * @param [in]	mngId		クエリ管理番号
	 * @param [in]	tableName	テーブル名
	 * @param [in]	serializedStr	protobufデータ文字列
	 * 
	 * @return	protobufヘッダ文字列
	 * 
	 */
	string ProtobufParser::createHeader(const unsigned int mngId, const string &tableName, const string &serializedStr)
	{
		return createHeader(mngId, tableName, serializedStr, serializedStr.size(), 0, 1);
	}

	/**
	 * protobufヘッダ作成(セッションキー有り)
	 * 
	 * @author	Nagoya University
	 * @date 2025/10/31
	 * 
	 * @param [in]	mngId			クエリ管理番号
	 * @param [in]	tableName		テーブル名
	 * @param [in]	serializedStr	protobufデータ文字列
	 * @param [in]	strLength		protobufデータ文字長
	 * @param [in]	fragmentIndex	フラグメント番号
	 * @param [in]	totalFragments	全体のフラグメント数
	 * @param [in]	key				セッションキー
	 * 
	 * @return	protobufヘッダ文字列
	 */
	string ProtobufParser::createHeader(const unsigned int mngId, const string &tableName, const string &serializedStr, size_t strLength, short fragmentIndex, short totalFragments, const string &key)
	{
		std::ostringstream oss(std::ios::binary);
		oss << "3" 	// protobufフラグ
			<< std::setfill('0') << std::setw(2) << tableName.length()	// テーブル名長(2桁固定)
			<< tableName;	// テーブル名
		
		// protobufデータ長
		uint32_t strlen = htonl(static_cast<uint32_t>(serializedStr.length()));
		oss.write(reinterpret_cast<const char*>(&strlen), sizeof(strlen));
		
		// クエリ管理番号
		strlen = htonl(mngId);
		oss.write(reinterpret_cast<const char*>(&strlen), sizeof(strlen));

		// フラグメント番号
		oss.put(static_cast<char>(fragmentIndex));
		
		// 全体のフラグメント数
    	oss.put(static_cast<char>(totalFragments));

		// セッションキー長
		strlen = htonl(static_cast<uint32_t>(key.length()));
		oss.write(reinterpret_cast<const char*>(&strlen), sizeof(strlen));

		// セッションキー
		oss << key;

		return oss.str();
	}

	/**
	 * 受信電文からprotobufヘッダ情報を取り出す
	 * 
	 * @author	Nagoya University
	 * @date 2025/10/31
	 * 
	 * @param [in]		receiveData		受信電文
	 * @param [in,out]	info			protobufヘッダ情報
	 * 
	 */
	void ProtobufParser::getProtobufHeaderInfo(const string &receiveData, ProtobufHeaderInfo &info)
	{
		// 文字列インデックス
		size_t pos = 0;

		// フラグを取り出す
		char flag = receiveData.at(0);
		
		if (flag == '3') {
			// フラグを設定
			info.header.flag = flag;
			pos += sizeof(info.header.flag);

			// テーブル名長
			info.header.table_name_length = stoi(receiveData.substr(pos,2));
			pos += 2; // 2桁の文字列で表現されているため
			
			// テーブル名
			info.header.table_name = receiveData.substr(pos, info.header.table_name_length);
			pos += info.header.table_name.length();

			//protobufデータ長
			int payload_size;
			memcpy(&payload_size, receiveData.data() + pos, sizeof(payload_size));
			info.header.payload_size = ntohl(payload_size);
			pos += sizeof(payload_size);

			//クエリ管理番号
			unsigned int mngId;
			memcpy(&mngId, receiveData.data() + pos, sizeof(mngId));
			info.header.mngId = ntohl(mngId);
			pos += sizeof(mngId);

			// フラグメント番号
			uint8_t fragment_idx;
			memcpy(&fragment_idx, receiveData.data() + pos, sizeof(fragment_idx));
			info.header.fragment_index = fragment_idx;
			pos += sizeof(fragment_idx);

			// 全体のフラグメント数
			uint8_t total_fragments;
			memcpy(&total_fragments, receiveData.data() + pos, sizeof(total_fragments));
			info.header.total_fragments = total_fragments;
			pos += sizeof(total_fragments);

			// セッションキー長
			int key_length;
			memcpy(&key_length, receiveData.data() + pos, sizeof(key_length));
			info.header.key_length = ntohl(key_length);
			pos += sizeof(key_length);

			// セッションキー
			info.header.key = receiveData.substr(pos, info.header.key_length);
			pos += info.header.key.length();

			// ヘッダサイズ
			info.headerSize = pos;

			// protobufデータ位置
			info.payload_p = receiveData.data() + pos;
			
		} else {
			// ヘッダー不明につき、なしと見なす
			info.headerSize = 0;
		}

		return;
	}
	/**
	 * 接続要求のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	username	ユーザ名
	* @param [in]	md5			ハッシュ値
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createSessionSerializeToString(const string &username, const string &md5)
	{
		string retStr = "";
		dm2_proto::System_request_dm2is msg;

		dm2_proto::System_request* request = msg.mutable_system_request();
		
		request->set_type("session");
		request->set_key(md5);
		request->set_pid(getpid());
		request->set_body(username);

		msg.SerializeToString(&retStr);
		return retStr;
	}

	/**
	 * 接続要求のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	username	ユーザ名
	* @param [in]	md5			ハッシュ値
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createCreateSession(const string &username, const string &md5)
	{
		string retStr = "";
		retStr = createSessionSerializeToString(username, md5);
		//retStr = createHeader("[system]", retStr) + retStr;
		return retStr;
	}

	/**
	 * 接続要求/クエリ登録/クエリキャンセル 応答のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	value	ハッシュ値/エラーメッセージ
	* @param [in]	errCode	エラーコード
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::systemResponseSerializeToString(const string type, const string &value, const int tcpPortconst, const string &errCode)
	{
		string retStr = "";
		dm2_proto::System_response_dm2is msg;

		dm2_proto::System_response* response = msg.mutable_system_response();
		response->set_type(type);
		response->set_value(value);
		if (errCode != "")
			response->set_error(errCode);

		if (tcpPortconst != -1)
			response->set_port(tcpPortconst);

		msg.SerializeToString(&retStr);
		return retStr;
	}

	/**
	 * 接続要求応答のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	sessionKey	ハッシュ値
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createCreateSessionResponse(const string &sessionKey)
	{
		string retStr = "";
		retStr = systemResponseSerializeToString("session", sessionKey);
		
		return retStr;
	}

	/**
	 * 接続要求エラー応答のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	errCode		エラーコード
	* @param [in]	errMessage	エラーメッセージ
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createCreateSessionErrorResponse(const string &errCode, const string &errMessage)
	{
		string retStr = "";
		retStr = systemResponseSerializeToString("session", errMessage, -1, errCode);
		
		return retStr;
	}

	/**
	 * クエリ登録のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	query				クエリ文
	* @param [in]	key					セッションキー
	* @param [in]	receptionPort_p		受付ポート番号
	* @param [in]	senderSID_p			実行元SID
	* @param [in]	destionationSID_p	宛先SID
	* @param [in]	getTcpSession_p		TCP Sessionをサーバへ要求
	* @param [in]	continuous			継続クエリか判定するフラグ
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::querySerializeToString(const string &query, const string &key, const bool &continuous, const int* receptionPort_p,
		const unsigned long long* senderSID_p, const unsigned long long* destionationSID_p, const bool* getTcpSession_p)
	{
		string retStr = "";
		dm2_proto::System_request_dm2is msg;

		dm2_proto::System_request* request = msg.mutable_system_request();
		
		request->set_type("query");
		request->set_key(key);
		request->set_body(query);
		if (receptionPort_p != nullptr) request->set_port(*receptionPort_p);
		if (senderSID_p != nullptr) request->set_sender(*senderSID_p);
		if (destionationSID_p != nullptr) request->set_destination(*destionationSID_p);
		if (getTcpSession_p != nullptr) request->set_tcp_session(*getTcpSession_p);
		request->set_continuous(continuous);

		msg.SerializeToString(&retStr);
		return retStr;
	}

	/**
	 * クエリ登録のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	query			クエリ文
	* @param [in]	key				セッションキー
	* @param [in]	receptionPort	受付ポート番号
	* @param [in]	getTcpSession	TCP Sessionをサーバへ要求
	* @param [in]	continuous		継続クエリか判定するフラグ
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createQuery(const string &query, const string &key, const int receptionPort, const bool &getTcpSession, const bool &continuous)
	{
		string retStr = "";
		retStr = querySerializeToString(query, key, continuous, &receptionPort, nullptr, nullptr, &getTcpSession);
		//retStr = createHeader("[system]", retStr) + retStr;
		return retStr;
	}

	/**
	 * クエリ登録のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	query			クエリ文
	* @param [in]	key				セッションキー
	* @param [in]	destionationSID	宛先SID
	* @param [in]	continuous		継続クエリか判定するフラグ
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createQuery(const string &query, const string &key, unsigned long long destinationSID, const bool &continuous)
	{
		string retStr = "";
		retStr = querySerializeToString(query, key, continuous, nullptr, nullptr, &destinationSID, nullptr);
		//retStr = createHeader("[system]", retStr) + retStr;
		return retStr;
	}

	/**
	 * クエリ登録応答のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	query			クエリ管理番号
	* @param [in]	tcpPort			ポート番号
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createQueryResponse(const unsigned int mngId, const int tcpPort)
	{
		string retStr = "";
		retStr = systemResponseSerializeToString("query", to_string(mngId), tcpPort);
		//retStr = createHeader("[system]", retStr) + retStr;
		return retStr;
	}

	/**
	 * クエリ登録エラー応答のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	errCode		エラーコード
	* @param [in]	errMessage	エラーメッセージ
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createQueryErrorResponse(const string &errCode, const string &errMessage)
	{
		string retStr = "";
		retStr = systemResponseSerializeToString("query", errMessage, -1, errCode);
		//retStr = createHeader("[system]", retStr) + retStr;
		return retStr;
	}

	/**
	 * クエリ結果のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	tableName		テーブル名
	* @param [in]	ts				クエリ結果タプルセット
	* @param [in]	isDynamicColumn	不定形メッセージフラグ
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::queryResultSerializeToString(const string &tableName, TupleSet& ts, const bool isDynamicColumn)
	{
		if (isDynamicColumn)
		{
			return SerializeToStringDynamically(ts);
		}
		else {
			if (tableName == "object_info" || tableName == "object_info_processed") {
				return objectInfoSerializeToString_0_6_0(ts);
			} else if (tableName == "object_info_0_8_0" || tableName == "object_info_0_8_0_processed") {
				return objectInfoSerializeToString_0_8_0(ts);
			} else if (tableName == "object_info_0_8_1" || tableName == "object_info_0_8_1_processed") {
				return objectInfoSerializeToString_0_8_1(ts);
			} else if (tableName == "freespace_info") {
				return freespaceInfoSerializeToString_0_6_0(ts);
			} else if (tableName == "freespace_info_0_8_0") {
				return freespaceInfoSerializeToString_0_8_0(ts);
			} else if (tableName == "freespace_info_0_8_1") {
				return freespaceInfoSerializeToString_0_8_1(ts);
			} else if (tableName == "signal_info") {
				return signalInfoSerializeToString_0_6_0(ts);
			} else if (tableName == "sensor_info") {
				return sensorInfoSerializeToString_0_8_0(ts);
			} else if (tableName == "sensor_info_0_8_1") {
				return sensorInfoSerializeToString_0_8_1(ts);
			} else {
				return SerializeToStringDynamically(ts);
			}
		}
	}

	/**
	 * クエリ結果のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	mngId			クエリ管理番号
	* @param [in]	tableName		テーブル名
	* @param [in]	ts				クエリ結果タプルセット
	* @param [in]	isDynamicColumn	不定形メッセージフラグ
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createQueryResult(const unsigned int mngId, const string &tableName, TupleSet& ts, const bool isDynamicColumn)
	{
		string retStr = queryResultSerializeToString(tableName, ts, isDynamicColumn);
		retStr = createHeader(mngId, tableName, retStr) + retStr;
		
		return retStr;
	}

	/**
	 * クエリ結果のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	mngId			クエリ管理番号
	* @param [in]	tableName		テーブル名
	* @param [in]	ts				クエリ結果タプルセット
	* @param [in]	isDynamicColumn	不定形メッセージフラグ
	* @param [in]	sepSize			分割メッセージサイズ
	*
	* @return	シリアライズされた文字列を指定サイズで分割したリスト 
	*/
	vector<string> ProtobufParser::createQueryResult(const unsigned int mngId, const string &tableName, TupleSet& ts, const bool isDynamicColumn, const int sepSize)
	{
		vector<string> retStrList;

		string retStr = queryResultSerializeToString(tableName, ts, isDynamicColumn);

		StringUtil stringUtil;
		stringUtil.splitBySize(retStr, sepSize - PROTOBUF_HEADER_FIXED_SIZE, retStrList);
		for(int i = 0; i < retStrList.size() ; i++)
		{
			retStrList[i] = createHeader(mngId, tableName, retStrList[i], retStr.size(), i, retStrList.size()) + retStrList[i];
		}
		return retStrList;
	}

	/**
	 * クエリキャンセルのシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	mngId	クエリ管理番号
	* @param [in]	key		セッションキー
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createCancelSerializeToString(const unsigned int mngId, const string &key)
	{
		string retStr = "";
		dm2_proto::System_request_dm2is msg;

		dm2_proto::System_request* request = msg.mutable_system_request();
		request->set_type("cancel");
		request->set_key(key);
		request->set_body(to_string(mngId));

		msg.SerializeToString(&retStr);
		return retStr;
	}

	/**
	 * クエリキャンセルのシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	mngId	クエリ管理番号
	* @param [in]	key		セッションキー
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createCancel(const unsigned int mngId, const string &key)
	{
		string retStr = "";
		retStr = createCancelSerializeToString(mngId, key);

		//retStr = createHeader("[system]", retStr) + retStr;
		return retStr;
	}
	/**
	 * クエリキャンセル応答のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	mngId	クエリ管理番号
	* @param [in]	key		セッションキー
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createCancelResponse(const unsigned int mngId)
	{
		string retStr = "";
		retStr = systemResponseSerializeToString("cancel", to_string(mngId));
		//retStr = createHeader("[system]", retStr) + retStr;
		return retStr;
	}

	/**
	 * クエリキャンセルエラー応答のシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	errCode		エラーコード
	* @param [in]	errMessage	エラーメッセージ
	*
	* @return	シリアライズされた文字列 
	*/
	string ProtobufParser::createCancelErrorResponse(const string &errCode, const string &errMessage)
	{
		string retStr = "";
		retStr = systemResponseSerializeToString("cancel", errMessage, -1, errCode);
		//retStr = createHeader("[system]", retStr) + retStr;
		return retStr;
	}
	/**
	 * ストリームデータのシリアライズ
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	tableName		テーブル名
	* @param [in]	tuples			ストリームデータタプルリスト
	*
	* @return	シリアライズされた文字列を指定サイズで分割したリスト 
	*/
	string ProtobufParser::streamSerializeToString(const string &tableName, const vector<Tuple> &tuples)
	{
		if (tableName == "object_info" || tableName == "object_info_processed") {
			vector<Tuple> normalizedTuples = normalizeTuple(tuples, objectInfoTypeList_0_6_0);
			return objectInfoSerializeToString_0_6_0(normalizedTuples);
		} else if (tableName == "object_info_0_8_0" || tableName == "object_info_0_8_0_processed") {
			vector<Tuple> normalizedTuples = normalizeTuple(tuples, objectInfoTypeList_0_8_0);
			return objectInfoSerializeToString_0_8_0(normalizedTuples);
		} else if (tableName == "object_info_0_8_1" || tableName == "object_info_0_8_1_processed") {
			vector<Tuple> normalizedTuples = normalizeTuple(tuples, objectInfoTypeList_0_8_1);
			return objectInfoSerializeToString_0_8_1(normalizedTuples);
		} else if (tableName == "freespace_info") {
			vector<Tuple> normalizedTuples = normalizeTuple(tuples, freespaceInfoTypeList_0_6_0);
			return freespaceInfoSerializeToString_0_6_0(normalizedTuples);
		} else if (tableName == "freespace_info_0_8_0") {
			vector<Tuple> normalizedTuples = normalizeTuple(tuples, freespaceInfoTypeList_0_8_0);
			return freespaceInfoSerializeToString_0_8_0(normalizedTuples);
		} else if (tableName == "freespace_info_0_8_1") {
			vector<Tuple> normalizedTuples = normalizeTuple(tuples, freespaceInfoTypeList_0_8_1);
			return freespaceInfoSerializeToString_0_8_1(normalizedTuples);
		} else if (tableName == "signal_info") {
			vector<Tuple> normalizedTuples = normalizeTuple(tuples, signalInfoTypeList_0_6_0);
			return signalInfoSerializeToString_0_6_0(normalizedTuples);
		} else if (tableName == "sensor_info") {
			vector<Tuple> normalizedTuples = normalizeTuple(tuples, sensorInfoTypeList_0_8_0);
			return sensorInfoSerializeToString_0_8_0(normalizedTuples);
		} else if (tableName == "sensor_info_0_8_1") {
			vector<Tuple> normalizedTuples = normalizeTuple(tuples, sensorInfoTypeList_0_8_1);
			return sensorInfoSerializeToString_0_8_1(normalizedTuples);
		} else {
			return SerializeToStringDynamically(tuples, nullptr);
		}
	}

	/**
	 * ストリームデータの生成
	 *
	* @author	Nagoya University
	* @date		2025/10/31
	*
	* @param [in]	tableName		テーブル名
	* @param [in]	tuples			ストリームデータタプルリスト
	* @param [in]	sepSize			ストリームデータタプルリスト
	*
	* @return	シリアライズされた文字列リスト 
	*/
	vector<string> ProtobufParser::createStreamList(const string &tableName, const vector<Tuple> &tuples, const string &key, const int sepSize)
	{
		vector<string> retStrList;
		
		string retStr = streamSerializeToString(tableName, tuples);
		// cout << "[createStreamList] retStrSize: " << retStr.size() << ",sepSize: " << sepSize << ",PROTOBUF_HEADER_FIXED_SIZE: " << PROTOBUF_HEADER_FIXED_SIZE << ",keySize:" << key.size() << endl;
		StringUtil stringUtil;
		stringUtil.splitBySize(retStr, sepSize - PROTOBUF_HEADER_FIXED_SIZE - key.size(), retStrList);

		for(int i = 0; i < retStrList.size() ; i++)
		{
			retStrList[i] = createHeader(0, tableName, retStrList[i], retStr.size(), i, retStrList.size(), key) + retStrList[i];
			//cout << "[createStreamList] retStrList[i] size:" << retStrList[i].size() << ", retStrList.size(): " << retStrList.size() << endl;
		}
		return retStrList;
	}

	/**
	* 物標情報のシリアライズ (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	tuples	タプルリスト
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::objectInfoSerializeToString_0_6_0(const vector<Tuple> &tuples)
	{
		string retStr = "";
		dm2_proto::Object_info_0_6_0_dm2is msg;
		for (Tuple tuple : tuples)
		{
			try {
				setObjectInfo(msg.add_object_info(), msg.add_is_tuple_info(), tuple);
			} catch (...) {
				msg.mutable_object_info()->RemoveLast();
				loggerWarn(__func__, "]Serialize Failed");
			}
		}
		if (msg.object_info_size() > 0) msg.SerializeToString(&retStr);
		return retStr;
	}
	/**
	* 物標情報のシリアライズ (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::objectInfoSerializeToString_0_6_0(TupleSet &tupleset)
	{
		return objectInfoSerializeToString_0_6_0(tupleset.getTuples());
	}

	/**
	* 物標情報のシリアライズ (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	tuples	タプルリスト
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::objectInfoSerializeToString_0_8_0(const vector<Tuple> &tuples)
	{
		string retStr = "";
		dm2_proto::Object_info_0_8_0_dm2is msg;
		for (Tuple tuple : tuples)
		{
			try {
				setObjectInfo(msg.add_object_info(), msg.add_is_tuple_info(), tuple);
			} catch (...) {
				msg.mutable_object_info()->RemoveLast();
				loggerWarn(__func__, "]Serialize Failed");
			}
		}
		if (msg.object_info_size() > 0) msg.SerializeToString(&retStr);
		return retStr;
	}
	/**
	* 物標情報のシリアライズ (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::objectInfoSerializeToString_0_8_0(TupleSet &tupleset)
	{
		return objectInfoSerializeToString_0_8_0(tupleset.getTuples());
	}

	/**
	* 物標情報のシリアライズ (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	tuples	タプルリスト
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::objectInfoSerializeToString_0_8_1(const vector<Tuple> &tuples)
	{
		string retStr = "";
		dm2_proto::Object_info_0_8_1_dm2is msg;
		for (Tuple tuple : tuples)
		{
			try {
				setObjectInfo(msg.add_object_info(), msg.add_is_tuple_info(), tuple);
			} catch (...) {
				msg.mutable_object_info()->RemoveLast();
				loggerWarn(__func__, "]Serialize Failed");
			}
		}
		if (msg.object_info_size() > 0) msg.SerializeToString(&retStr);
		return retStr;
	}
	/**
	* 物標情報のシリアライズ (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::objectInfoSerializeToString_0_8_1(TupleSet &tupleset)
	{
		return objectInfoSerializeToString_0_8_1(tupleset.getTuples());
	}

	/**
	* フリースペース情報のシリアライズ (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	tuples	タプルリスト
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::freespaceInfoSerializeToString_0_6_0(const vector<Tuple> &tuples)
	{
		string retStr = "";
		dm2_proto::Freespace_info_0_6_0_dm2is msg;
		for (Tuple tuple : tuples)
		{
			try {
				setFreespaceInfo(msg.add_freespace_info(), msg.add_is_tuple_info(), tuple);
			} catch (...) {
				msg.mutable_freespace_info()->RemoveLast();
				loggerWarn(__func__, "]Serialize Failed");
			}
		}
		if (msg.freespace_info_size() > 0) msg.SerializeToString(&retStr);
		return retStr;
	}
	/**
	* フリースペース情報のシリアライズ (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::freespaceInfoSerializeToString_0_6_0(TupleSet &tupleset)
	{
		return freespaceInfoSerializeToString_0_6_0(tupleset.getTuples());
	}

	/**
	* フリースペース情報のシリアライズ (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/05/10
	*
	* @param [in]	tuples	タプルリスト
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::freespaceInfoSerializeToString_0_8_0(const vector<Tuple> &tuples)
	{
		string retStr = "";
		dm2_proto::Freespace_info_0_8_0_dm2is msg;
		for (Tuple tuple : tuples)
		{
			try {
				setFreespaceInfo(msg.add_freespace_info(), msg.add_is_tuple_info(), tuple);
			} catch (...) {
				msg.mutable_freespace_info()->RemoveLast();
				loggerWarn(__func__, "]Serialize Failed");
			}
		}
		if (msg.freespace_info_size() > 0) msg.SerializeToString(&retStr);
		return retStr;
	}
	/**
	* フリースペース情報のシリアライズ (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/05/10
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::freespaceInfoSerializeToString_0_8_0(TupleSet &tupleset)
	{
		return freespaceInfoSerializeToString_0_8_0(tupleset.getTuples());
	}

	/**
	* フリースペース情報のシリアライズ (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2025/06/30
	*
	* @param [in]	tuples	タプルリスト
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::freespaceInfoSerializeToString_0_8_1(const vector<Tuple> &tuples)
	{
		string retStr = "";
		dm2_proto::Freespace_info_0_8_1_dm2is msg;
		for (Tuple tuple : tuples)
		{
			try {
				setFreespaceInfo(msg.add_freespace_info(), msg.add_is_tuple_info(), tuple);
			} catch (...) {
				msg.mutable_freespace_info()->RemoveLast();
				loggerWarn(__func__, "]Serialize Failed");
			}
		}
		if (msg.freespace_info_size() > 0) msg.SerializeToString(&retStr);
		return retStr;
	}
	/**
	* フリースペース情報のシリアライズ (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2025/06/30
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::freespaceInfoSerializeToString_0_8_1(TupleSet &tupleset)
	{
		return freespaceInfoSerializeToString_0_8_1(tupleset.getTuples());
	}
	/**
	* 信号情報のシリアライズ (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	tuples	タプルリスト
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::signalInfoSerializeToString_0_6_0(const vector<Tuple> &tuples)
	{
		string retStr = "";
		dm2_proto::Signal_info_0_6_0_dm2is msg;
		for (Tuple tuple : tuples)
		{
			try {
				setSignalInfo(msg.add_signal_info(), msg.add_is_tuple_info(), tuple);
			} catch (...) {
				msg.mutable_signal_info()->RemoveLast();
				loggerWarn(__func__, "]Serialize Failed");
			}
		}
		if (msg.signal_info_size() > 0) msg.SerializeToString(&retStr);
		return retStr;
	}
	/**
	* 信号情報のシリアライズ (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::signalInfoSerializeToString_0_6_0(TupleSet &tupleset)
	{
		return signalInfoSerializeToString_0_6_0(tupleset.getTuples());
	}

	/**
	* センサー情報のシリアライズ (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	tuples	タプルリスト
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::sensorInfoSerializeToString_0_8_0(const vector<Tuple> &tuples)
	{
		string retStr = "";
		dm2_proto::Sensor_info_0_8_0_dm2is msg;
		for (Tuple tuple : tuples)
		{
			try {
				setSensorInfo(msg.add_sensor_info(), msg.add_is_tuple_info(), tuple);
			} catch (...) {
				msg.mutable_sensor_info()->RemoveLast();
				loggerWarn(__func__, "]Serialize Failed");
			}
		}
		if (msg.sensor_info_size() > 0) msg.SerializeToString(&retStr);
		return retStr;
	}
	/**
	* センサー情報のシリアライズ (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::sensorInfoSerializeToString_0_8_0(TupleSet &tupleset)
	{
		return sensorInfoSerializeToString_0_8_0(tupleset.getTuples());
	}

	/**
	* センサー情報のシリアライズ (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	tuples	タプルリスト
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::sensorInfoSerializeToString_0_8_1(const vector<Tuple> &tuples)
	{
		string retStr = "";
		dm2_proto::Sensor_info_0_8_1_dm2is msg;
		for (Tuple tuple : tuples)
		{
			try {
				setSensorInfo(msg.add_sensor_info(), msg.add_is_tuple_info(), tuple);
			} catch (...) {
				msg.mutable_sensor_info()->RemoveLast();
				loggerWarn(__func__, "]Serialize Failed");
			}
		}
		if (msg.sensor_info_size() > 0) msg.SerializeToString(&retStr);
		return retStr;
	}
	/**
	* センサー情報のシリアライズ (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	 */
	string ProtobufParser::sensorInfoSerializeToString_0_8_1(TupleSet &tupleset)
	{
		return sensorInfoSerializeToString_0_8_1(tupleset.getTuples());
	}
	/**
	* 物標情報のデシリアライズ (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	*
	 */
	void ProtobufParser::objectInfoDeserializeToTuple_0_6_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema)
	{
		dm2_proto::Object_info_0_6_0_dm2is msg;
		msg.ParseFromString(serializedStr);
		string strVal;
		// CS からの転送の場合、ここで管理者情報列込の列数がセットされる
		int attr_size = schema.getAttributeSize();
		for (int i = 0; i < msg.object_info_size(); i++) {
			const cool4_api_0_6_0::Object_info& obj = msg.object_info(i);
			const cool4_api_0_6_0::Position_and_movement obj_pm = obj.position_and_movement();
			const cool4_api_0_6_0::Vehicle_condition obj_vc = obj.vehicle_condition();
			const dm2_proto::Is_tuple_info& is_info = msg.is_tuple_info(i);
			if (attr_size == 0) {
				// 受信アプリの場合、スキーマを持たないため、ここで列数をセット（電文の中に時刻列が入っていれば、管理者情報列分、追加する）
				attr_size = objectInfo_0_6_0_normal_size;
				if (is_info.dm2_ts_group_size() > 0) attr_size = attr_size + is_tuple_size;
			}
			Tuple tuple(attr_size);
			long ts = is_info.dm2_create_ts();
			strVal = DmUtil::getStringFromUnsignedLong(obj.id());
			tuple.setValue((int)OBJECT_INFO_POS_0_6_0::id, strVal, ts);
			tuple.setValue((int)OBJECT_INFO_POS_0_6_0::time, (long)obj.time(), ts);
			if (obj.has_object_class_id_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_id_0, (int)obj.object_class_id_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_id_0, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_id_0], ts); }
			if (obj.has_object_class_confidence_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_confidence_0, (int)obj.object_class_confidence_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_confidence_0, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_0], ts); }
			if (obj.has_object_class_subclass_type_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_0, (int)obj.object_class_subclass_type_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_0, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_0], ts); }
			if (obj.has_object_class_subclass_confidence_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_0, (int)obj.object_class_subclass_confidence_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_0, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_0], ts); }
			if (obj.has_object_class_id_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_id_1, (int)obj.object_class_id_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_id_1, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_id_1], ts); }
			if (obj.has_object_class_confidence_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_confidence_1, (int)obj.object_class_confidence_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_confidence_1, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_1], ts); }
			if (obj.has_object_class_subclass_type_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_1, (int)obj.object_class_subclass_type_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_1, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_1], ts); }
			if (obj.has_object_class_subclass_confidence_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_1, (int)obj.object_class_subclass_confidence_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_1, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_1], ts); }
			if (obj.has_object_class_id_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_id_2, (int)obj.object_class_id_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_id_2, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_id_2], ts); }
			if (obj.has_object_class_confidence_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_confidence_2, (int)obj.object_class_confidence_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_confidence_2, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_2], ts); }
			if (obj.has_object_class_subclass_type_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_2, (int)obj.object_class_subclass_type_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_2, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_2], ts); }
			if (obj.has_object_class_subclass_confidence_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_2, (int)obj.object_class_subclass_confidence_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_2, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_2], ts); }
			if (obj.has_object_class_id_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_id_3, (int)obj.object_class_id_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_id_3, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_id_3], ts); }
			if (obj.has_object_class_confidence_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_confidence_3, (int)obj.object_class_confidence_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_confidence_3, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_3], ts); }
			if (obj.has_object_class_subclass_type_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_3, (int)obj.object_class_subclass_type_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_3, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_3], ts); }
			if (obj.has_object_class_subclass_confidence_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_3, (int)obj.object_class_subclass_confidence_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_3, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_3], ts); }
			if (obj.has_existency()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::existency, (int)obj.existency(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::existency, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::existency], ts); }
			if (obj_pm.has_geodetic_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::geodetic_system, (int)obj_pm.geodetic_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::geodetic_system, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::geodetic_system], ts); }
			if (obj_pm.has_latitude()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::latitude, (int)obj_pm.latitude(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::latitude, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::latitude], ts); }
			if (obj_pm.has_longitude()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::longitude, (int)obj_pm.longitude(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::longitude, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::longitude], ts); }
			if (obj_pm.has_altitude()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::altitude, (int)obj_pm.altitude(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::altitude, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::altitude], ts); }
			if (obj_pm.has_crp_id()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::crp_id, (int)obj_pm.crp_id(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::crp_id, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::crp_id], ts); }
			if (obj_pm.has_dx_crp()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dx_crp, (int)obj_pm.dx_crp(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dx_crp, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dx_crp], ts); }
			if (obj_pm.has_dy_crp()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dy_crp, (int)obj_pm.dy_crp(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dy_crp, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dy_crp], ts); }
			if (obj_pm.has_dh_crp()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dh_crp, (int)obj_pm.dh_crp(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dh_crp, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dh_crp], ts); }
			if (obj_pm.has_lane_count()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_count, (int)obj_pm.lane_count(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_count, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_count], ts); }
			if (obj_pm.has_lane_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_position, (int)obj_pm.lane_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_position, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_position], ts); }
			if (obj_pm.has_lane_lateral_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_lateral_position, (int)obj_pm.lane_lateral_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_lateral_position, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_lateral_position], ts); }
			if (obj_pm.has_crp_id_begin()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::crp_id_begin, (int)obj_pm.crp_id_begin(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::crp_id_begin, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::crp_id_begin], ts); }
			if (obj_pm.has_crp_id_end()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::crp_id_end, (int)obj_pm.crp_id_end(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::crp_id_end, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::crp_id_end], ts); }
			if (obj_pm.has_lane_vertical_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_vertical_position, (int)obj_pm.lane_vertical_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_vertical_position, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_vertical_position], ts); }
			if (obj_pm.has_lane_id()) {
				strVal = DmUtil::getStringFromUnsignedLong(obj_pm.lane_id());
			} else {
				strVal = "0000000000000000";
			}
			tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_id, strVal, ts);
			if (obj_pm.has_dx_lane()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dx_lane, (int)obj_pm.dx_lane(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dx_lane, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dx_lane], ts); }
			if (obj_pm.has_dy_lane()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dy_lane, (int)obj_pm.dy_lane(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dy_lane, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dy_lane], ts); }
			if (obj_pm.has_dh_lane()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dh_lane, (int)obj_pm.dh_lane(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::dh_lane, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dh_lane], ts); }
			if (obj_pm.has_semi_axis_length_major()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::semi_axis_length_major, (int)obj_pm.semi_axis_length_major(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::semi_axis_length_major, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::semi_axis_length_major], ts); }
			if (obj_pm.has_semi_axis_length_minor()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::semi_axis_length_minor, (int)obj_pm.semi_axis_length_minor(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::semi_axis_length_minor, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::semi_axis_length_minor], ts); }
			if (obj_pm.has_ellipse_orientation()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::ellipse_orientation, (int)obj_pm.ellipse_orientation(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::ellipse_orientation, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::ellipse_orientation], ts); }
			if (obj_pm.has_altitude_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::altitude_accuracy, (int)obj_pm.altitude_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::altitude_accuracy, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::altitude_accuracy], ts); }
			if (obj_pm.has_reference_point()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::reference_point, (int)obj_pm.reference_point(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::reference_point, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::reference_point], ts); }
			if (obj_pm.has_move_direction_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::move_direction_value, (int)obj_pm.move_direction_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::move_direction_value, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::move_direction_value], ts); }
			if (obj_pm.has_move_direction_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::move_direction_accuracy, (int)obj_pm.move_direction_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::move_direction_accuracy, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::move_direction_accuracy], ts); }
			if (obj_pm.has_speed_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::speed_value, (int)obj_pm.speed_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::speed_value, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::speed_value], ts); }
			if (obj_pm.has_speed_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::speed_accuracy, (int)obj_pm.speed_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::speed_accuracy, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::speed_accuracy], ts); }
			if (obj_pm.has_yaw_rate_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::yaw_rate_value, (int)obj_pm.yaw_rate_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::yaw_rate_value, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::yaw_rate_value], ts); }
			if (obj_pm.has_yaw_rate_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::yaw_rate_accuracy, (int)obj_pm.yaw_rate_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::yaw_rate_accuracy, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::yaw_rate_accuracy], ts); }
			if (obj_pm.has_acceleration_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::acceleration_value, (int)obj_pm.acceleration_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::acceleration_value, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::acceleration_value], ts); }
			if (obj_pm.has_acceleration_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::acceleration_accuracy, (int)obj_pm.acceleration_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::acceleration_accuracy, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::acceleration_accuracy], ts); }
			if (obj_vc.has_orientation_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::orientation_value, (int)obj_vc.orientation_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::orientation_value, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::orientation_value], ts); }
			if (obj_vc.has_orientation_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::orientation_accuracy, (int)obj_vc.orientation_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::orientation_accuracy, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::orientation_accuracy], ts); }
			if (obj_vc.has_length_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::length_value, (int)obj_vc.length_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::length_value, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::length_value], ts); }
			if (obj_vc.has_length_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::length_accuracy, (int)obj_vc.length_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::length_accuracy, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::length_accuracy], ts); }
			if (obj_vc.has_width_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::width_value, (int)obj_vc.width_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::width_value, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::width_value], ts); }
			if (obj_vc.has_width_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::width_accuracy, (int)obj_vc.width_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::width_accuracy, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::width_accuracy], ts); }
			if (obj_vc.has_height_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::height_value, (int)obj_vc.height_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::height_value, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::height_value], ts); }
			if (obj_vc.has_height_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::height_accuracy, (int)obj_vc.height_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::height_accuracy, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::height_accuracy], ts); }
			if (obj_vc.has_color()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::color, (int)obj_vc.color(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::color, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::color], ts); }
			if (obj_vc.has_shift_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::shift_position, (int)obj_vc.shift_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::shift_position, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::shift_position], ts); }
			if (obj_vc.has_steering_angle_front()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::steering_angle_front, (int)obj_vc.steering_angle_front(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::steering_angle_front, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::steering_angle_front], ts); }
			if (obj_vc.has_steering_angle_rear()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::steering_angle_rear, (int)obj_vc.steering_angle_rear(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::steering_angle_rear, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::steering_angle_rear], ts); }
			if (obj_vc.has_brake_state()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::brake_state, (int)obj_vc.brake_state(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::brake_state, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::brake_state], ts); }
			if (obj_vc.has_auxiliary_brake_state()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::auxiliary_brake_state, (int)obj_vc.auxiliary_brake_state(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::auxiliary_brake_state, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::auxiliary_brake_state], ts); }
			if (obj_vc.has_throttle_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::throttle_position, (int)obj_vc.throttle_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::throttle_position, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::throttle_position], ts); }
			if (obj_vc.has_exterior_lights()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::exterior_lights, (int)obj_vc.exterior_lights(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::exterior_lights, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::exterior_lights], ts); }
			if (obj_vc.has_adaptive_cruise_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::adaptive_cruise_control_system, (int)obj_vc.adaptive_cruise_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::adaptive_cruise_control_system, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::adaptive_cruise_control_system], ts); }
			if (obj_vc.has_cooperative_adaptive_cruise_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::cooperative_adaptive_cruise_control_system, (int)obj_vc.cooperative_adaptive_cruise_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::cooperative_adaptive_cruise_control_system, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::cooperative_adaptive_cruise_control_system], ts); }
			if (obj_vc.has_pre_crash_safety_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::pre_crash_safety_system, (int)obj_vc.pre_crash_safety_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::pre_crash_safety_system, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::pre_crash_safety_system], ts); }
			if (obj_vc.has_antilock_brake_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::antilock_brake_system, (int)obj_vc.antilock_brake_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::antilock_brake_system, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::antilock_brake_system], ts); }
			if (obj_vc.has_traction_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::traction_control_system, (int)obj_vc.traction_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::traction_control_system, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::traction_control_system], ts); }
			if (obj_vc.has_electronic_stability_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::electronic_stability_control_system, (int)obj_vc.electronic_stability_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::electronic_stability_control_system, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::electronic_stability_control_system], ts); }
			if (obj_vc.has_lane_keeping_assist_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_keeping_assist_system, (int)obj_vc.lane_keeping_assist_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_keeping_assist_system, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_keeping_assist_system], ts); }
			if (obj_vc.has_lane_departure_warning_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_departure_warning_system, (int)obj_vc.lane_departure_warning_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::lane_departure_warning_system, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_departure_warning_system], ts); }
			if (obj_vc.has_vehicle_role()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::vehicle_role, (int)obj_vc.vehicle_role(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::vehicle_role, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::vehicle_role], ts); }
			if (obj_vc.has_vehicle_extended_info()) { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::vehicle_extended_info, (int)obj_vc.vehicle_extended_info(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_6_0::vehicle_extended_info, objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::vehicle_extended_info], ts); }
			if (obj_vc.has_towing_vehicle()) {
				strVal = DmUtil::getStringFromUnsignedLong(obj_vc.towing_vehicle());
			} else {
				strVal = "0000000000000000";
			}
			tuple.setValue((int)OBJECT_INFO_POS_0_6_0::towing_vehicle, strVal, ts);
			{
				strVal = "";
				for (int i = 0; i < obj.information_source_list_size(); i++) {
					if (i != 0) strVal += ",";
					strVal += DmUtil::getStringFromUnsignedLong(obj.information_source_list(i));
				}
				if (strVal != "") strVal = "[" + strVal + "]";
				any val = stringUtil.getAnyValFromString(strVal, objectInfoTypeList_0_6_0[(int)OBJECT_INFO_POS_0_6_0::information_source_list]);
				tuple.setValue((int)OBJECT_INFO_POS_0_6_0::information_source_list, val, ts);
			}
			isTupleInfoDeserializeToTuple(tuple, is_info, ts, (int)OBJECT_INFO_POS_0_6_0::information_source_list + 1);
			tuples.push_back(tuple);
		}
	}
	/**
	* 物標情報のデシリアライズ (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	*
	 */
	void ProtobufParser::objectInfoDeserializeToTuple_0_8_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema)
	{
		dm2_proto::Object_info_0_8_0_dm2is msg;
		msg.ParseFromString(serializedStr);
		string strVal;
		// CS からの転送の場合、ここで管理者情報列込の列数がセットされる
		int attr_size = schema.getAttributeSize();
		for (int i = 0; i < msg.object_info_size(); i++) {
			const cool4_api_0_8_0::Object_info_0_8_0& obj = msg.object_info(i);
			const cool4_api_0_8_0::Position_and_movement obj_pm = obj.position_and_movement();
			const cool4_api_0_8_0::Vehicle_condition obj_vc = obj.vehicle_condition();
			const cool4_api_0_8_0::Tracking_information obj_ti = obj.tracking_information();
			const dm2_proto::Is_tuple_info& is_info = msg.is_tuple_info(i);
			if (attr_size == 0) {
				// 受信アプリの場合、スキーマを持たないため、ここで列数をセット（電文の中に時刻列が入っていれば、管理者情報列分、追加する）
				attr_size = objectInfo_0_8_0_normal_size;
				if (is_info.dm2_ts_group_size() > 0) attr_size = attr_size + is_tuple_size;
			}
			Tuple tuple(attr_size);
			long ts = is_info.dm2_create_ts();
			tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_id, (unsigned long long)obj.object_id(), ts);
			tuple.setValue((int)OBJECT_INFO_POS_0_8_0::time, (long)obj.time(), ts);
			if (obj.has_object_class_id_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_id_0, (int)obj.object_class_id_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_id_0, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_id_0], ts); }
			if (obj.has_object_class_confidence_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_confidence_0, (int)obj.object_class_confidence_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_confidence_0, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_0], ts); }
			if (obj.has_object_class_subclass_type_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_0, (int)obj.object_class_subclass_type_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_0, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_0], ts); }
			if (obj.has_object_class_subclass_confidence_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_0, (int)obj.object_class_subclass_confidence_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_0, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_0], ts); }
			if (obj.has_object_class_id_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_id_1, (int)obj.object_class_id_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_id_1, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_id_1], ts); }
			if (obj.has_object_class_confidence_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_confidence_1, (int)obj.object_class_confidence_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_confidence_1, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_1], ts); }
			if (obj.has_object_class_subclass_type_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_1, (int)obj.object_class_subclass_type_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_1, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_1], ts); }
			if (obj.has_object_class_subclass_confidence_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_1, (int)obj.object_class_subclass_confidence_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_1, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_1], ts); }
			if (obj.has_object_class_id_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_id_2, (int)obj.object_class_id_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_id_2, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_id_2], ts); }
			if (obj.has_object_class_confidence_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_confidence_2, (int)obj.object_class_confidence_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_confidence_2, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_2], ts); }
			if (obj.has_object_class_subclass_type_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_2, (int)obj.object_class_subclass_type_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_2, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_2], ts); }
			if (obj.has_object_class_subclass_confidence_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_2, (int)obj.object_class_subclass_confidence_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_2, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_2], ts); }
			if (obj.has_object_class_id_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_id_3, (int)obj.object_class_id_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_id_3, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_id_3], ts); }
			if (obj.has_object_class_confidence_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_confidence_3, (int)obj.object_class_confidence_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_confidence_3, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_3], ts); }
			if (obj.has_object_class_subclass_type_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_3, (int)obj.object_class_subclass_type_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_3, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_3], ts); }
			if (obj.has_object_class_subclass_confidence_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_3, (int)obj.object_class_subclass_confidence_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_3, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_3], ts); }
			if (obj.has_existency()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::existency, (int)obj.existency(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::existency, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::existency], ts); }
			if (obj_pm.has_geodetic_srid()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::geodetic_srid, (int)obj_pm.geodetic_srid(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::geodetic_srid, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::geodetic_srid], ts); }
			if (obj_pm.has_latitude()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::latitude, (int)obj_pm.latitude(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::latitude, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::latitude], ts); }
			if (obj_pm.has_longitude()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::longitude, (int)obj_pm.longitude(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::longitude, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::longitude], ts); }
			if (obj_pm.has_altitude()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::altitude, (int)obj_pm.altitude(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::altitude, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::altitude], ts); }
			if (obj_pm.has_projective_srid()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::projective_srid, (int)obj_pm.projective_srid(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::projective_srid, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::projective_srid], ts); }
			if (obj_pm.has_prc_x()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::prc_x, (int)obj_pm.prc_x(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::prc_x, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::prc_x], ts); }
			if (obj_pm.has_prc_y()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::prc_y, (int)obj_pm.prc_y(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::prc_y, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::prc_y], ts); }
			if (obj_pm.has_prc_z()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::prc_z, (int)obj_pm.prc_z(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::prc_z, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::prc_z], ts); }
			if (obj_pm.has_crp_id()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::crp_id, (int)obj_pm.crp_id(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::crp_id, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::crp_id], ts); }
			if (obj_pm.has_dx_crp()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dx_crp, (int)obj_pm.dx_crp(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dx_crp, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dx_crp], ts); }
			if (obj_pm.has_dy_crp()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dy_crp, (int)obj_pm.dy_crp(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dy_crp, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dy_crp], ts); }
			if (obj_pm.has_dh_crp()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dh_crp, (int)obj_pm.dh_crp(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dh_crp, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dh_crp], ts); }
			if (obj_pm.has_lane_count()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_count, (int)obj_pm.lane_count(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_count, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_count], ts); }
			if (obj_pm.has_lane_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_position, (int)obj_pm.lane_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_position, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_position], ts); }
			if (obj_pm.has_lane_lateral_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_lateral_position, (int)obj_pm.lane_lateral_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_lateral_position, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_lateral_position], ts); }
			if (obj_pm.has_crp_id_begin()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::crp_id_begin, (int)obj_pm.crp_id_begin(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::crp_id_begin, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::crp_id_begin], ts); }
			if (obj_pm.has_crp_id_end()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::crp_id_end, (int)obj_pm.crp_id_end(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::crp_id_end, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::crp_id_end], ts); }
			if (obj_pm.has_lane_vertical_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_vertical_position, (int)obj_pm.lane_vertical_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_vertical_position, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_vertical_position], ts); }
			if (obj_pm.has_lane_id()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_id, (unsigned long long)obj_pm.lane_id(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_id, (unsigned long long)objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_id], ts); }
			if (obj_pm.has_dx_lane()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dx_lane, (int)obj_pm.dx_lane(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dx_lane, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dx_lane], ts); }
			if (obj_pm.has_dy_lane()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dy_lane, (int)obj_pm.dy_lane(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dy_lane, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dy_lane], ts); }
			if (obj_pm.has_dh_lane()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dh_lane, (int)obj_pm.dh_lane(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::dh_lane, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dh_lane], ts); }
			if (obj_pm.has_semi_axis_length_major()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::semi_axis_length_major, (int)obj_pm.semi_axis_length_major(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::semi_axis_length_major, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::semi_axis_length_major], ts); }
			if (obj_pm.has_semi_axis_length_minor()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::semi_axis_length_minor, (int)obj_pm.semi_axis_length_minor(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::semi_axis_length_minor, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::semi_axis_length_minor], ts); }
			if (obj_pm.has_ellipse_orientation()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::ellipse_orientation, (int)obj_pm.ellipse_orientation(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::ellipse_orientation, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::ellipse_orientation], ts); }
			if (obj_pm.has_altitude_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::altitude_accuracy, (int)obj_pm.altitude_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::altitude_accuracy, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::altitude_accuracy], ts); }
			if (obj_pm.has_reference_point()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::reference_point, (int)obj_pm.reference_point(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::reference_point, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::reference_point], ts); }
			if (obj_pm.has_move_direction_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::move_direction_value, (int)obj_pm.move_direction_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::move_direction_value, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::move_direction_value], ts); }
			if (obj_pm.has_move_direction_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::move_direction_accuracy, (int)obj_pm.move_direction_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::move_direction_accuracy, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::move_direction_accuracy], ts); }
			if (obj_pm.has_speed_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::speed_value, (int)obj_pm.speed_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::speed_value, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::speed_value], ts); }
			if (obj_pm.has_speed_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::speed_accuracy, (int)obj_pm.speed_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::speed_accuracy, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::speed_accuracy], ts); }
			if (obj_pm.has_yaw_rate_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::yaw_rate_value, (int)obj_pm.yaw_rate_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::yaw_rate_value, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::yaw_rate_value], ts); }
			if (obj_pm.has_yaw_rate_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::yaw_rate_accuracy, (int)obj_pm.yaw_rate_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::yaw_rate_accuracy, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::yaw_rate_accuracy], ts); }
			if (obj_pm.has_acceleration_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::acceleration_value, (int)obj_pm.acceleration_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::acceleration_value, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::acceleration_value], ts); }
			if (obj_pm.has_acceleration_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::acceleration_accuracy, (int)obj_pm.acceleration_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::acceleration_accuracy, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::acceleration_accuracy], ts); }
			if (obj_vc.has_orientation_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::orientation_value, (int)obj_vc.orientation_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::orientation_value, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::orientation_value], ts); }
			if (obj_vc.has_orientation_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::orientation_accuracy, (int)obj_vc.orientation_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::orientation_accuracy, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::orientation_accuracy], ts); }
			if (obj_vc.has_length_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::length_value, (int)obj_vc.length_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::length_value, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::length_value], ts); }
			if (obj_vc.has_length_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::length_accuracy, (int)obj_vc.length_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::length_accuracy, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::length_accuracy], ts); }
			if (obj_vc.has_width_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::width_value, (int)obj_vc.width_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::width_value, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::width_value], ts); }
			if (obj_vc.has_width_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::width_accuracy, (int)obj_vc.width_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::width_accuracy, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::width_accuracy], ts); }
			if (obj_vc.has_height_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::height_value, (int)obj_vc.height_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::height_value, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::height_value], ts); }
			if (obj_vc.has_height_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::height_accuracy, (int)obj_vc.height_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::height_accuracy, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::height_accuracy], ts); }
			if (obj_vc.has_static_status()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::static_status, (int)obj_vc.static_status(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::static_status, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::static_status], ts); }
			if (obj_vc.has_shift_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::shift_position, (int)obj_vc.shift_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::shift_position, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::shift_position], ts); }
			if (obj_vc.has_steering_angle_front()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::steering_angle_front, (int)obj_vc.steering_angle_front(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::steering_angle_front, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::steering_angle_front], ts); }
			if (obj_vc.has_steering_angle_rear()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::steering_angle_rear, (int)obj_vc.steering_angle_rear(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::steering_angle_rear, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::steering_angle_rear], ts); }
			if (obj_vc.has_brake_state()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::brake_state, (int)obj_vc.brake_state(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::brake_state, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::brake_state], ts); }
			if (obj_vc.has_auxiliary_brake_state()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::auxiliary_brake_state, (int)obj_vc.auxiliary_brake_state(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::auxiliary_brake_state, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::auxiliary_brake_state], ts); }
			if (obj_vc.has_throttle_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::throttle_position, (int)obj_vc.throttle_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::throttle_position, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::throttle_position], ts); }
			if (obj_vc.has_exterior_lights()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::exterior_lights, (int)obj_vc.exterior_lights(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::exterior_lights, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::exterior_lights], ts); }
			if (obj_vc.has_adaptive_cruise_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::adaptive_cruise_control_system, (int)obj_vc.adaptive_cruise_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::adaptive_cruise_control_system, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::adaptive_cruise_control_system], ts); }
			if (obj_vc.has_cooperative_adaptive_cruise_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::cooperative_adaptive_cruise_control_system, (int)obj_vc.cooperative_adaptive_cruise_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::cooperative_adaptive_cruise_control_system, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::cooperative_adaptive_cruise_control_system], ts); }
			if (obj_vc.has_pre_crash_safety_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::pre_crash_safety_system, (int)obj_vc.pre_crash_safety_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::pre_crash_safety_system, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::pre_crash_safety_system], ts); }
			if (obj_vc.has_antilock_brake_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::antilock_brake_system, (int)obj_vc.antilock_brake_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::antilock_brake_system, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::antilock_brake_system], ts); }
			if (obj_vc.has_traction_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::traction_control_system, (int)obj_vc.traction_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::traction_control_system, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::traction_control_system], ts); }
			if (obj_vc.has_electronic_stability_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::electronic_stability_control_system, (int)obj_vc.electronic_stability_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::electronic_stability_control_system, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::electronic_stability_control_system], ts); }
			if (obj_vc.has_lane_keeping_assist_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_keeping_assist_system, (int)obj_vc.lane_keeping_assist_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_keeping_assist_system, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_keeping_assist_system], ts); }
			if (obj_vc.has_lane_departure_warning_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_departure_warning_system, (int)obj_vc.lane_departure_warning_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lane_departure_warning_system, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_departure_warning_system], ts); }
			if (obj_vc.has_vehicle_size_type()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::vehicle_size_type, (int)obj_vc.vehicle_size_type(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::vehicle_size_type, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::vehicle_size_type], ts); }
			if (obj_vc.has_vehicle_role()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::vehicle_role, (int)obj_vc.vehicle_role(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::vehicle_role, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::vehicle_role], ts); }
			if (obj_vc.has_vehicle_extended_info()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::vehicle_extended_info, (int)obj_vc.vehicle_extended_info(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::vehicle_extended_info, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::vehicle_extended_info], ts); }
			if (obj_vc.has_towing_vehicle()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::towing_vehicle, (unsigned long long)obj_vc.towing_vehicle(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::towing_vehicle, (unsigned long long)objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::towing_vehicle], ts); }
			if (obj_ti.has_tracking_status()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::tracking_status, (int)obj_ti.tracking_status(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::tracking_status, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::tracking_status], ts); }
			if (obj_ti.has_detection_count()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::detection_count, (int)obj_ti.detection_count(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::detection_count, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::detection_count], ts); }
			if (obj_ti.has_lost_count()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lost_count, (int)obj_ti.lost_count(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::lost_count, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lost_count], ts); }
			if (obj_ti.has_object_age()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_age, (int)obj_ti.object_age(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_0::object_age, objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_age], ts); }
			{
				vector<unsigned long long> vull;
				for (int i = 0; i < obj.information_source_list_size(); i++) {
					vull.push_back((unsigned long long)obj.information_source_list(i));
				}
				tuple.setValue((int)OBJECT_INFO_POS_0_8_0::information_source_list, vull, ts);
			}
			isTupleInfoDeserializeToTuple(tuple, is_info, ts, (int)OBJECT_INFO_POS_0_8_0::information_source_list + 1);
			tuples.push_back(tuple);
		}
	}
	/**
	* 物標情報のデシリアライズ (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	*
	 */
	void ProtobufParser::objectInfoDeserializeToTuple_0_8_1(string serializedStr, vector<Tuple> &tuples, const Schema &schema)
	{
		dm2_proto::Object_info_0_8_1_dm2is msg;
		msg.ParseFromString(serializedStr);
		//cout << msg.object_info_size() << endl;
		string strVal;
		// CS からの転送の場合、ここで管理者情報列込の列数がセットされる
		int attr_size = schema.getAttributeSize();
		for (int i = 0; i < msg.object_info_size(); i++) {
			const cool4_api_0_8_0::Object_info_0_8_1& obj = msg.object_info(i);
			const cool4_api_0_8_0::Position_and_movement obj_pm = obj.position_and_movement();
			const cool4_api_0_8_0::Vehicle_condition obj_vc = obj.vehicle_condition();
			const cool4_api_0_8_0::Tracking_information obj_ti = obj.tracking_information();
			const dm2_proto::Is_tuple_info& is_info = msg.is_tuple_info(i);
			if (attr_size == 0) {
				// 受信アプリの場合、スキーマを持たないため、ここで列数をセット（電文の中に時刻列が入っていれば、管理者情報列分、追加する）
				attr_size = objectInfo_0_8_1_normal_size;
				if (is_info.dm2_ts_group_size() > 0) attr_size = attr_size + is_tuple_size;
			}
			Tuple tuple(attr_size);
			long ts = is_info.dm2_create_ts();
			tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_id, (unsigned long long)obj.object_id(), ts);
			tuple.setValue((int)OBJECT_INFO_POS_0_8_1::time, (long)obj.time(), ts);
			tuple.setValue((int)OBJECT_INFO_POS_0_8_1::revision, (int)obj.revision(), ts);
			if (obj.has_object_class_id_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_id_0, (int)obj.object_class_id_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_id_0, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_id_0], ts); }
			if (obj.has_object_class_confidence_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_confidence_0, (int)obj.object_class_confidence_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_confidence_0, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_0], ts); }
			if (obj.has_object_class_subclass_type_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_0, (int)obj.object_class_subclass_type_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_0, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_0], ts); }
			if (obj.has_object_class_subclass_confidence_0()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_0, (int)obj.object_class_subclass_confidence_0(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_0, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_0], ts); }
			if (obj.has_object_class_id_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_id_1, (int)obj.object_class_id_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_id_1, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_id_1], ts); }
			if (obj.has_object_class_confidence_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_confidence_1, (int)obj.object_class_confidence_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_confidence_1, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_1], ts); }
			if (obj.has_object_class_subclass_type_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_1, (int)obj.object_class_subclass_type_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_1, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_1], ts); }
			if (obj.has_object_class_subclass_confidence_1()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_1, (int)obj.object_class_subclass_confidence_1(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_1, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_1], ts); }
			if (obj.has_object_class_id_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_id_2, (int)obj.object_class_id_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_id_2, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_id_2], ts); }
			if (obj.has_object_class_confidence_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_confidence_2, (int)obj.object_class_confidence_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_confidence_2, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_2], ts); }
			if (obj.has_object_class_subclass_type_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_2, (int)obj.object_class_subclass_type_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_2, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_2], ts); }
			if (obj.has_object_class_subclass_confidence_2()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_2, (int)obj.object_class_subclass_confidence_2(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_2, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_2], ts); }
			if (obj.has_object_class_id_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_id_3, (int)obj.object_class_id_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_id_3, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_id_3], ts); }
			if (obj.has_object_class_confidence_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_confidence_3, (int)obj.object_class_confidence_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_confidence_3, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_3], ts); }
			if (obj.has_object_class_subclass_type_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_3, (int)obj.object_class_subclass_type_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_3, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_3], ts); }
			if (obj.has_object_class_subclass_confidence_3()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_3, (int)obj.object_class_subclass_confidence_3(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_3, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_3], ts); }
			if (obj.has_existency()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::existency, (int)obj.existency(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::existency, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::existency], ts); }
			if (obj_pm.has_geodetic_srid()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::geodetic_srid, (int)obj_pm.geodetic_srid(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::geodetic_srid, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::geodetic_srid], ts); }
			if (obj_pm.has_latitude()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::latitude, (int)obj_pm.latitude(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::latitude, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::latitude], ts); }
			if (obj_pm.has_longitude()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::longitude, (int)obj_pm.longitude(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::longitude, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::longitude], ts); }
			if (obj_pm.has_altitude()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::altitude, (int)obj_pm.altitude(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::altitude, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::altitude], ts); }
			if (obj_pm.has_projective_srid()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::projective_srid, (int)obj_pm.projective_srid(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::projective_srid, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::projective_srid], ts); }
			if (obj_pm.has_prc_x()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::prc_x, (int)obj_pm.prc_x(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::prc_x, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::prc_x], ts); }
			if (obj_pm.has_prc_y()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::prc_y, (int)obj_pm.prc_y(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::prc_y, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::prc_y], ts); }
			if (obj_pm.has_prc_z()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::prc_z, (int)obj_pm.prc_z(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::prc_z, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::prc_z], ts); }
			if (obj_pm.has_crp_id()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::crp_id, (int)obj_pm.crp_id(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::crp_id, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::crp_id], ts); }
			if (obj_pm.has_dx_crp()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dx_crp, (int)obj_pm.dx_crp(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dx_crp, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dx_crp], ts); }
			if (obj_pm.has_dy_crp()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dy_crp, (int)obj_pm.dy_crp(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dy_crp, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dy_crp], ts); }
			if (obj_pm.has_dh_crp()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dh_crp, (int)obj_pm.dh_crp(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dh_crp, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dh_crp], ts); }
			if (obj_pm.has_lane_count()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_count, (int)obj_pm.lane_count(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_count, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_count], ts); }
			if (obj_pm.has_lane_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_position, (int)obj_pm.lane_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_position, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_position], ts); }
			if (obj_pm.has_lane_lateral_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_lateral_position, (int)obj_pm.lane_lateral_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_lateral_position, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_lateral_position], ts); }
			if (obj_pm.has_crp_id_begin()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::crp_id_begin, (int)obj_pm.crp_id_begin(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::crp_id_begin, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::crp_id_begin], ts); }
			if (obj_pm.has_crp_id_end()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::crp_id_end, (int)obj_pm.crp_id_end(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::crp_id_end, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::crp_id_end], ts); }
			if (obj_pm.has_lane_vertical_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_vertical_position, (int)obj_pm.lane_vertical_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_vertical_position, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_vertical_position], ts); }
			if (obj_pm.has_lane_id()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_id, (unsigned long long)obj_pm.lane_id(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_id, (unsigned long long)objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_id], ts); }
			if (obj_pm.has_dx_lane()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dx_lane, (int)obj_pm.dx_lane(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dx_lane, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dx_lane], ts); }
			if (obj_pm.has_dy_lane()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dy_lane, (int)obj_pm.dy_lane(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dy_lane, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dy_lane], ts); }
			if (obj_pm.has_dh_lane()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dh_lane, (int)obj_pm.dh_lane(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::dh_lane, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dh_lane], ts); }
			if (obj_pm.has_semi_axis_length_major()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::semi_axis_length_major, (int)obj_pm.semi_axis_length_major(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::semi_axis_length_major, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::semi_axis_length_major], ts); }
			if (obj_pm.has_semi_axis_length_minor()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::semi_axis_length_minor, (int)obj_pm.semi_axis_length_minor(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::semi_axis_length_minor, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::semi_axis_length_minor], ts); }
			if (obj_pm.has_ellipse_orientation()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::ellipse_orientation, (int)obj_pm.ellipse_orientation(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::ellipse_orientation, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::ellipse_orientation], ts); }
			if (obj_pm.has_altitude_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::altitude_accuracy, (int)obj_pm.altitude_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::altitude_accuracy, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::altitude_accuracy], ts); }
			if (obj_pm.has_reference_point()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::reference_point, (int)obj_pm.reference_point(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::reference_point, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::reference_point], ts); }
			if (obj_pm.has_move_direction_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::move_direction_value, (int)obj_pm.move_direction_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::move_direction_value, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::move_direction_value], ts); }
			if (obj_pm.has_move_direction_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::move_direction_accuracy, (int)obj_pm.move_direction_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::move_direction_accuracy, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::move_direction_accuracy], ts); }
			if (obj_pm.has_speed_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::speed_value, (int)obj_pm.speed_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::speed_value, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::speed_value], ts); }
			if (obj_pm.has_speed_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::speed_accuracy, (int)obj_pm.speed_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::speed_accuracy, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::speed_accuracy], ts); }
			if (obj_pm.has_yaw_rate_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::yaw_rate_value, (int)obj_pm.yaw_rate_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::yaw_rate_value, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::yaw_rate_value], ts); }
			if (obj_pm.has_yaw_rate_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::yaw_rate_accuracy, (int)obj_pm.yaw_rate_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::yaw_rate_accuracy, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::yaw_rate_accuracy], ts); }
			if (obj_pm.has_acceleration_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::acceleration_value, (int)obj_pm.acceleration_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::acceleration_value, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::acceleration_value], ts); }
			if (obj_pm.has_acceleration_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::acceleration_accuracy, (int)obj_pm.acceleration_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::acceleration_accuracy, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::acceleration_accuracy], ts); }
			if (obj_vc.has_orientation_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::orientation_value, (int)obj_vc.orientation_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::orientation_value, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::orientation_value], ts); }
			if (obj_vc.has_orientation_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::orientation_accuracy, (int)obj_vc.orientation_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::orientation_accuracy, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::orientation_accuracy], ts); }
			if (obj_vc.has_length_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::length_value, (int)obj_vc.length_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::length_value, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::length_value], ts); }
			if (obj_vc.has_length_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::length_accuracy, (int)obj_vc.length_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::length_accuracy, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::length_accuracy], ts); }
			if (obj_vc.has_width_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::width_value, (int)obj_vc.width_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::width_value, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::width_value], ts); }
			if (obj_vc.has_width_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::width_accuracy, (int)obj_vc.width_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::width_accuracy, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::width_accuracy], ts); }
			if (obj_vc.has_height_value()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::height_value, (int)obj_vc.height_value(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::height_value, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::height_value], ts); }
			if (obj_vc.has_height_accuracy()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::height_accuracy, (int)obj_vc.height_accuracy(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::height_accuracy, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::height_accuracy], ts); }
			if (obj_vc.has_static_status()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::static_status, (int)obj_vc.static_status(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::static_status, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::static_status], ts); }
			if (obj_vc.has_shift_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::shift_position, (int)obj_vc.shift_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::shift_position, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::shift_position], ts); }
			if (obj_vc.has_steering_angle_front()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::steering_angle_front, (int)obj_vc.steering_angle_front(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::steering_angle_front, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::steering_angle_front], ts); }
			if (obj_vc.has_steering_angle_rear()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::steering_angle_rear, (int)obj_vc.steering_angle_rear(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::steering_angle_rear, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::steering_angle_rear], ts); }
			if (obj_vc.has_brake_state()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::brake_state, (int)obj_vc.brake_state(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::brake_state, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::brake_state], ts); }
			if (obj_vc.has_auxiliary_brake_state()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::auxiliary_brake_state, (int)obj_vc.auxiliary_brake_state(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::auxiliary_brake_state, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::auxiliary_brake_state], ts); }
			if (obj_vc.has_throttle_position()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::throttle_position, (int)obj_vc.throttle_position(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::throttle_position, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::throttle_position], ts); }
			if (obj_vc.has_exterior_lights()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::exterior_lights, (int)obj_vc.exterior_lights(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::exterior_lights, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::exterior_lights], ts); }
			if (obj_vc.has_adaptive_cruise_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::adaptive_cruise_control_system, (int)obj_vc.adaptive_cruise_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::adaptive_cruise_control_system, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::adaptive_cruise_control_system], ts); }
			if (obj_vc.has_cooperative_adaptive_cruise_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::cooperative_adaptive_cruise_control_system, (int)obj_vc.cooperative_adaptive_cruise_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::cooperative_adaptive_cruise_control_system, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::cooperative_adaptive_cruise_control_system], ts); }
			if (obj_vc.has_pre_crash_safety_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::pre_crash_safety_system, (int)obj_vc.pre_crash_safety_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::pre_crash_safety_system, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::pre_crash_safety_system], ts); }
			if (obj_vc.has_antilock_brake_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::antilock_brake_system, (int)obj_vc.antilock_brake_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::antilock_brake_system, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::antilock_brake_system], ts); }
			if (obj_vc.has_traction_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::traction_control_system, (int)obj_vc.traction_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::traction_control_system, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::traction_control_system], ts); }
			if (obj_vc.has_electronic_stability_control_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::electronic_stability_control_system, (int)obj_vc.electronic_stability_control_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::electronic_stability_control_system, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::electronic_stability_control_system], ts); }
			if (obj_vc.has_lane_keeping_assist_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_keeping_assist_system, (int)obj_vc.lane_keeping_assist_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_keeping_assist_system, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_keeping_assist_system], ts); }
			if (obj_vc.has_lane_departure_warning_system()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_departure_warning_system, (int)obj_vc.lane_departure_warning_system(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lane_departure_warning_system, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_departure_warning_system], ts); }
			if (obj_vc.has_vehicle_size_type()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::vehicle_size_type, (int)obj_vc.vehicle_size_type(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::vehicle_size_type, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::vehicle_size_type], ts); }
			if (obj_vc.has_vehicle_role()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::vehicle_role, (int)obj_vc.vehicle_role(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::vehicle_role, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::vehicle_role], ts); }
			if (obj_vc.has_vehicle_extended_info()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::vehicle_extended_info, (int)obj_vc.vehicle_extended_info(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::vehicle_extended_info, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::vehicle_extended_info], ts); }
			if (obj_vc.has_towing_vehicle()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::towing_vehicle, (unsigned long long)obj_vc.towing_vehicle(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::towing_vehicle, (unsigned long long)objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::towing_vehicle], ts); }
			if (obj_ti.has_tracking_status()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::tracking_status, (int)obj_ti.tracking_status(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::tracking_status, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::tracking_status], ts); }
			if (obj_ti.has_detection_count()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::detection_count, (int)obj_ti.detection_count(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::detection_count, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::detection_count], ts); }
			if (obj_ti.has_lost_count()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lost_count, (int)obj_ti.lost_count(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::lost_count, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lost_count], ts); }
			if (obj_ti.has_object_age()) { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_age, (int)obj_ti.object_age(), ts); } else { tuple.setValue((int)OBJECT_INFO_POS_0_8_1::object_age, objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_age], ts); }
			{
				vector<unsigned long long> vull;
				for (int i = 0; i < obj.information_source_list_size(); i++) {
					vull.push_back((unsigned long long)obj.information_source_list(i));
				}
				tuple.setValue((int)OBJECT_INFO_POS_0_8_1::information_source_list, vull, ts);
			}
			isTupleInfoDeserializeToTuple(tuple, is_info, ts, (int)OBJECT_INFO_POS_0_8_1::information_source_list + 1);
			tuples.push_back(tuple);
		}
	}
	/**
	* フリースペース情報のデシリアライズ (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	*
	 */
	void ProtobufParser::freespaceInfoDeserializeToTuple_0_6_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema)
	{
		dm2_proto::Freespace_info_0_6_0_dm2is msg;
		msg.ParseFromString(serializedStr);
		string strVal;
		// CS からの転送の場合、ここで管理者情報列込の列数がセットされる
		int attr_size = schema.getAttributeSize();
		for (int i = 0; i < msg.freespace_info_size(); i++) {
			const cool4_api_0_6_0::Freespace_info& fre = msg.freespace_info(i);
			const cool4_api_0_6_0::Location pos_begin = fre.position_begin();
			const cool4_api_0_6_0::Location pos_end = fre.position_end();
			const dm2_proto::Is_tuple_info& is_info = msg.is_tuple_info(i);
			if (attr_size == 0) {
				// 受信アプリの場合、スキーマを持たないため、ここで列数をセット（電文の中に時刻列が入っていれば、管理者情報列分、追加する）
				attr_size = freespaceInfo_0_6_0_normal_size;
				if (is_info.dm2_ts_group_size() > 0) attr_size = attr_size + is_tuple_size;
			}
			Tuple tuple(attr_size);
			long ts = is_info.dm2_create_ts();
			strVal = DmUtil::getStringFromUnsignedLong(fre.id());
			tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::id, strVal, ts);
			tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::time, (long)fre.time(), ts);
			if (fre.has_existency()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::existency, (int)fre.existency(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::existency, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::existency], ts); }
			if (fre.has_minimal_detectable_size()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::minimal_detectable_size, (int)fre.minimal_detectable_size(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::minimal_detectable_size, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::minimal_detectable_size], ts); }
			if (pos_begin.has_geodetic_system()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_geodetic_system, (int)pos_begin.geodetic_system(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_geodetic_system, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_geodetic_system], ts); }
			if (pos_begin.has_latitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_latitude, (int)pos_begin.latitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_latitude, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_latitude], ts); }
			if (pos_begin.has_longitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_longitude, (int)pos_begin.longitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_longitude, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_longitude], ts); }
			if (pos_begin.has_altitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_altitude, (int)pos_begin.altitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_altitude, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_altitude], ts); }
			if (pos_begin.has_crp_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id, (int)pos_begin.crp_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id], ts); }
			if (pos_begin.has_dx_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dx_crp, (int)pos_begin.dx_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dx_crp, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dx_crp], ts); }
			if (pos_begin.has_dy_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dy_crp, (int)pos_begin.dy_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dy_crp, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dy_crp], ts); }
			if (pos_begin.has_dh_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dh_crp, (int)pos_begin.dh_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dh_crp, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dh_crp], ts); }
			if (pos_begin.has_lane_count()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_count, (int)pos_begin.lane_count(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_count, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_count], ts); }
			if (pos_begin.has_lane_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_position, (int)pos_begin.lane_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_position, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_position], ts); }
			if (pos_begin.has_lane_lateral_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_lateral_position, (int)pos_begin.lane_lateral_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_lateral_position, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_lateral_position], ts); }
			if (pos_begin.has_crp_id_begin()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id_begin, (int)pos_begin.crp_id_begin(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id_begin, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id_begin], ts); }
			if (pos_begin.has_crp_id_end()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id_end, (int)pos_begin.crp_id_end(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id_end, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id_end], ts); }
			if (pos_begin.has_lane_vertical_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_vertical_position, (int)pos_begin.lane_vertical_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_vertical_position, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_vertical_position], ts); }
			if (pos_begin.has_lane_id()) {
				strVal = DmUtil::getStringFromUnsignedLong(pos_begin.lane_id());
			} else {
				strVal = "0000000000000000";
			}
			tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_id, strVal, ts);
			if (pos_begin.has_dx_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dx_lane, (int)pos_begin.dx_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dx_lane, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dx_lane], ts); }
			if (pos_begin.has_dy_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dy_lane, (int)pos_begin.dy_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dy_lane, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dy_lane], ts); }
			if (pos_begin.has_dh_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dh_lane, (int)pos_begin.dh_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_dh_lane, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dh_lane], ts); }
			if (pos_begin.has_semi_axis_length_major()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_semi_axis_length_major, (int)pos_begin.semi_axis_length_major(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_semi_axis_length_major, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_semi_axis_length_major], ts); }
			if (pos_begin.has_semi_axis_length_minor()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_semi_axis_length_minor, (int)pos_begin.semi_axis_length_minor(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_semi_axis_length_minor, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_semi_axis_length_minor], ts); }
			if (pos_begin.has_ellipse_orientation()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_ellipse_orientation, (int)pos_begin.ellipse_orientation(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_ellipse_orientation, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_ellipse_orientation], ts); }
			if (pos_begin.has_altitude_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_altitude_accuracy, (int)pos_begin.altitude_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_begin_altitude_accuracy, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_altitude_accuracy], ts); }
			if (pos_end.has_geodetic_system()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_geodetic_system, (int)pos_end.geodetic_system(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_geodetic_system, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_geodetic_system], ts); }
			if (pos_end.has_latitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_latitude, (int)pos_end.latitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_latitude, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_latitude], ts); }
			if (pos_end.has_longitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_longitude, (int)pos_end.longitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_longitude, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_longitude], ts); }
			if (pos_end.has_altitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_altitude, (int)pos_end.altitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_altitude, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_altitude], ts); }
			if (pos_end.has_crp_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id, (int)pos_end.crp_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id], ts); }
			if (pos_end.has_dx_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dx_crp, (int)pos_end.dx_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dx_crp, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dx_crp], ts); }
			if (pos_end.has_dy_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dy_crp, (int)pos_end.dy_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dy_crp, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dy_crp], ts); }
			if (pos_end.has_dh_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dh_crp, (int)pos_end.dh_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dh_crp, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dh_crp], ts); }
			if (pos_end.has_lane_count()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_lane_count, (int)pos_end.lane_count(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_lane_count, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_count], ts); }
			if (pos_end.has_lane_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_lane_position, (int)pos_end.lane_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_lane_position, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_position], ts); }
			if (pos_end.has_lane_lateral_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_lane_lateral_position, (int)pos_end.lane_lateral_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_lane_lateral_position, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_lateral_position], ts); }
			if (pos_end.has_crp_id_begin()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id_begin, (int)pos_end.crp_id_begin(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id_begin, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id_begin], ts); }
			if (pos_end.has_crp_id_end()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id_end, (int)pos_end.crp_id_end(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id_end, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id_end], ts); }
			if (pos_end.has_lane_vertical_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_lane_vertical_position, (int)pos_end.lane_vertical_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_lane_vertical_position, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_vertical_position], ts); }
			if (pos_end.has_lane_id()) {
				strVal = DmUtil::getStringFromUnsignedLong(pos_end.lane_id());
			} else {
				strVal = "0000000000000000";
			}
			tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_lane_id, strVal, ts);
			if (pos_end.has_dx_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dx_lane, (int)pos_end.dx_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dx_lane, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dx_lane], ts); }
			if (pos_end.has_dy_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dy_lane, (int)pos_end.dy_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dy_lane, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dy_lane], ts); }
			if (pos_end.has_dh_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dh_lane, (int)pos_end.dh_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_dh_lane, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dh_lane], ts); }
			if (pos_end.has_semi_axis_length_major()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_semi_axis_length_major, (int)pos_end.semi_axis_length_major(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_semi_axis_length_major, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_semi_axis_length_major], ts); }
			if (pos_end.has_semi_axis_length_minor()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_semi_axis_length_minor, (int)pos_end.semi_axis_length_minor(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_semi_axis_length_minor, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_semi_axis_length_minor], ts); }
			if (pos_end.has_ellipse_orientation()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_ellipse_orientation, (int)pos_end.ellipse_orientation(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_ellipse_orientation, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_ellipse_orientation], ts); }
			if (pos_end.has_altitude_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_altitude_accuracy, (int)pos_end.altitude_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::position_end_altitude_accuracy, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_altitude_accuracy], ts); }
			if (fre.has_length_value()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::length_value, (int)fre.length_value(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::length_value, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::length_value], ts); }
			if (fre.has_length_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::length_accuracy, (int)fre.length_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::length_accuracy, freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::length_accuracy], ts); }
			if (fre.has_id_begin()) {
				strVal = DmUtil::getStringFromUnsignedLong(fre.id_begin());
			} else {
				strVal = "0000000000000000";
			}
			tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::id_begin, strVal, ts);
			if (fre.has_id_end()) {
				strVal = DmUtil::getStringFromUnsignedLong(fre.id_end());
			} else {
				strVal = "0000000000000000";
			}
			tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::id_end, strVal, ts);
			{
				strVal = "";
				for (int i = 0; i < fre.information_source_list_size(); i++) {
					if (i != 0) strVal += ",";
					strVal += DmUtil::getStringFromUnsignedLong(fre.information_source_list(i));
				}
				if (strVal != "") strVal = "[" + strVal + "]";
				any val = stringUtil.getAnyValFromString(strVal, freespaceInfoTypeList_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::information_source_list]);
				tuple.setValue((int)FREESPACE_INFO_POS_0_6_0::information_source_list, val, ts);
			}
			isTupleInfoDeserializeToTuple(tuple, is_info, ts, (int)FREESPACE_INFO_POS_0_6_0::information_source_list + 1);
			tuples.push_back(tuple);
		}
	}
	/**
	* フリースペース情報のデシリアライズ (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/05/10
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	*
	 */
	void ProtobufParser::freespaceInfoDeserializeToTuple_0_8_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema)
	{
		dm2_proto::Freespace_info_0_8_0_dm2is msg;
		msg.ParseFromString(serializedStr);
		string strVal;
		// CS からの転送の場合、ここで管理者情報列込の列数がセットされる
		int attr_size = schema.getAttributeSize();
		for (int i = 0; i < msg.freespace_info_size(); i++) {
			const cool4_api_0_8_0::Freespace_info_0_8_0& fre = msg.freespace_info(i);
			const cool4_api_0_8_0::Location vertices_begin = fre.vertices_begin();
			const cool4_api_0_8_0::Location pos_begin = fre.position_begin();
			const cool4_api_0_8_0::Location pos_end = fre.position_end();
			const dm2_proto::Is_tuple_info& is_info = msg.is_tuple_info(i);
			if (attr_size == 0) {
				// 受信アプリの場合、スキーマを持たないため、ここで列数をセット（電文の中に時刻列が入っていれば、管理者情報列分、追加する）
				attr_size = freespaceInfo_0_8_0_normal_size;
				if (is_info.dm2_ts_group_size() > 0) attr_size = attr_size + is_tuple_size;
			}
			Tuple tuple(attr_size);
			long ts = is_info.dm2_create_ts();
			tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::id, (unsigned long long)fre.id(), ts);
			tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::time, (long)fre.time(), ts);
			if (fre.has_detection_method()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::detection_method, (int)fre.detection_method(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::detection_method, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::detection_method], ts); }
			if (fre.has_detactable_classes()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::detactable_classes, (int)fre.detactable_classes(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::detactable_classes, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::detactable_classes], ts); }
			if (vertices_begin.has_geodetic_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_geodetic_srid, (int)vertices_begin.geodetic_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_geodetic_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_geodetic_srid], ts); }
			if (vertices_begin.has_latitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_latitude, (int)vertices_begin.latitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_latitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_latitude], ts); }
			if (vertices_begin.has_longitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_longitude, (int)vertices_begin.longitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_longitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_longitude], ts); }
			if (vertices_begin.has_altitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_altitude, (int)vertices_begin.altitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_altitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_altitude], ts); }
			if (vertices_begin.has_projective_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_projective_srid, (int)vertices_begin.projective_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_projective_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_projective_srid], ts); }
			if (vertices_begin.has_prc_x()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_x, (int)vertices_begin.prc_x(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_x, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_x], ts); }
			if (vertices_begin.has_prc_y()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_y, (int)vertices_begin.prc_y(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_y, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_y], ts); }
			if (vertices_begin.has_prc_z()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_z, (int)vertices_begin.prc_z(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_z, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_z], ts); }
			if (vertices_begin.has_crp_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id, (int)vertices_begin.crp_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id], ts); }
			if (vertices_begin.has_dx_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dx_crp, (int)vertices_begin.dx_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dx_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dx_crp], ts); }
			if (vertices_begin.has_dy_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dy_crp, (int)vertices_begin.dy_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dy_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dy_crp], ts); }
			if (vertices_begin.has_dh_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dh_crp, (int)vertices_begin.dh_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dh_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dh_crp], ts); }
			if (vertices_begin.has_lane_count()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_count, (int)vertices_begin.lane_count(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_count, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_count], ts); }
			if (vertices_begin.has_lane_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_position, (int)vertices_begin.lane_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_position], ts); }
			if (vertices_begin.has_lane_lateral_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_lateral_position, (int)vertices_begin.lane_lateral_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_lateral_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_lateral_position], ts); }
			if (vertices_begin.has_crp_id_begin()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id_begin, (int)vertices_begin.crp_id_begin(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id_begin, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id_begin], ts); }
			if (vertices_begin.has_crp_id_end()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id_end, (int)vertices_begin.crp_id_end(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id_end, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id_end], ts); }
			if (vertices_begin.has_lane_vertical_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_vertical_position, (int)vertices_begin.lane_vertical_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_vertical_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_vertical_position], ts); }
			if (vertices_begin.has_lane_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_id, (unsigned long long)vertices_begin.lane_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_id, (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_id], ts); }
			if (vertices_begin.has_dx_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dx_lane, (int)vertices_begin.dx_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dx_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dx_lane], ts); }
			if (vertices_begin.has_dy_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dy_lane, (int)vertices_begin.dy_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dy_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dy_lane], ts); }
			if (vertices_begin.has_dh_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dh_lane, (int)vertices_begin.dh_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dh_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dh_lane], ts); }
			if (vertices_begin.has_semi_axis_length_major()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_semi_axis_length_major, (int)vertices_begin.semi_axis_length_major(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_semi_axis_length_major, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_semi_axis_length_major], ts); }
			if (vertices_begin.has_semi_axis_length_minor()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_semi_axis_length_minor, (int)vertices_begin.semi_axis_length_minor(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_semi_axis_length_minor, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_semi_axis_length_minor], ts); }
			if (vertices_begin.has_ellipse_orientation()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_ellipse_orientation, (int)vertices_begin.ellipse_orientation(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_ellipse_orientation, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_ellipse_orientation], ts); }
			if (vertices_begin.has_altitude_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_altitude_accuracy, (int)vertices_begin.altitude_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_begin_altitude_accuracy, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_altitude_accuracy], ts); }
			{
				vector<int> vi;
				for (int i = 0; i < fre.vertices_dx_list_size(); i++) {
					vi.push_back((int)fre.vertices_dx_list(i));
				}
				tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_dx_list, vi, ts);
			}
			{
				vector<int> vi;
				for (int i = 0; i < fre.vertices_dy_list_size(); i++) {
					vi.push_back((int)fre.vertices_dy_list(i));
				}
				tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::vertices_dy_list, vi, ts);
			}
			if (pos_begin.has_geodetic_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_geodetic_srid, (int)pos_begin.geodetic_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_geodetic_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_geodetic_srid], ts); }
			if (pos_begin.has_latitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_latitude, (int)pos_begin.latitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_latitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_latitude], ts); }
			if (pos_begin.has_longitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_longitude, (int)pos_begin.longitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_longitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_longitude], ts); }
			if (pos_begin.has_altitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_altitude, (int)pos_begin.altitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_altitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_altitude], ts); }
			if (pos_begin.has_projective_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_projective_srid, (int)pos_begin.projective_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_projective_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_projective_srid], ts); }
			if (pos_begin.has_prc_x()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_x, (int)pos_begin.prc_x(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_x, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_x], ts); }
			if (pos_begin.has_prc_y()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_y, (int)pos_begin.prc_y(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_y, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_y], ts); }
			if (pos_begin.has_prc_z()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_z, (int)pos_begin.prc_z(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_z, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_z], ts); }
			if (pos_begin.has_crp_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id, (int)pos_begin.crp_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id], ts); }
			if (pos_begin.has_dx_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dx_crp, (int)pos_begin.dx_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dx_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dx_crp], ts); }
			if (pos_begin.has_dy_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dy_crp, (int)pos_begin.dy_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dy_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dy_crp], ts); }
			if (pos_begin.has_dh_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dh_crp, (int)pos_begin.dh_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dh_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dh_crp], ts); }
			if (pos_begin.has_lane_count()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_count, (int)pos_begin.lane_count(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_count, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_count], ts); }
			if (pos_begin.has_lane_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_position, (int)pos_begin.lane_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_position], ts); }
			if (pos_begin.has_lane_lateral_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_lateral_position, (int)pos_begin.lane_lateral_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_lateral_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_lateral_position], ts); }
			if (pos_begin.has_crp_id_begin()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id_begin, (int)pos_begin.crp_id_begin(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id_begin, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id_begin], ts); }
			if (pos_begin.has_crp_id_end()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id_end, (int)pos_begin.crp_id_end(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id_end, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id_end], ts); }
			if (pos_begin.has_lane_vertical_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_vertical_position, (int)pos_begin.lane_vertical_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_vertical_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_vertical_position], ts); }
			if (pos_begin.has_lane_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_id, (unsigned long long)pos_begin.lane_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_id, (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_id], ts); }
			if (pos_begin.has_dx_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dx_lane, (int)pos_begin.dx_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dx_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dx_lane], ts); }
			if (pos_begin.has_dy_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dy_lane, (int)pos_begin.dy_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dy_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dy_lane], ts); }
			if (pos_begin.has_dh_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dh_lane, (int)pos_begin.dh_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_dh_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dh_lane], ts); }
			if (pos_begin.has_semi_axis_length_major()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_semi_axis_length_major, (int)pos_begin.semi_axis_length_major(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_semi_axis_length_major, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_semi_axis_length_major], ts); }
			if (pos_begin.has_semi_axis_length_minor()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_semi_axis_length_minor, (int)pos_begin.semi_axis_length_minor(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_semi_axis_length_minor, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_semi_axis_length_minor], ts); }
			if (pos_begin.has_ellipse_orientation()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_ellipse_orientation, (int)pos_begin.ellipse_orientation(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_ellipse_orientation, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_ellipse_orientation], ts); }
			if (pos_begin.has_altitude_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_altitude_accuracy, (int)pos_begin.altitude_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_begin_altitude_accuracy, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_altitude_accuracy], ts); }
			if (pos_end.has_geodetic_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_geodetic_srid, (int)pos_end.geodetic_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_geodetic_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_geodetic_srid], ts); }
			if (pos_end.has_latitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_latitude, (int)pos_end.latitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_latitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_latitude], ts); }
			if (pos_end.has_longitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_longitude, (int)pos_end.longitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_longitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_longitude], ts); }
			if (pos_end.has_altitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_altitude, (int)pos_end.altitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_altitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_altitude], ts); }
			if (pos_end.has_projective_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_projective_srid, (int)pos_end.projective_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_projective_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_projective_srid], ts); }
			if (pos_end.has_prc_x()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_prc_x, (int)pos_end.prc_x(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_prc_x, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_prc_x], ts); }
			if (pos_end.has_prc_y()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_prc_y, (int)pos_end.prc_y(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_prc_y, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_prc_y], ts); }
			if (pos_end.has_prc_z()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_prc_z, (int)pos_end.prc_z(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_prc_z, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_prc_z], ts); }
			if (pos_end.has_crp_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id, (int)pos_end.crp_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id], ts); }
			if (pos_end.has_dx_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dx_crp, (int)pos_end.dx_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dx_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dx_crp], ts); }
			if (pos_end.has_dy_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dy_crp, (int)pos_end.dy_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dy_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dy_crp], ts); }
			if (pos_end.has_dh_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dh_crp, (int)pos_end.dh_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dh_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dh_crp], ts); }
			if (pos_end.has_lane_count()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_lane_count, (int)pos_end.lane_count(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_lane_count, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_count], ts); }
			if (pos_end.has_lane_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_lane_position, (int)pos_end.lane_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_lane_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_position], ts); }
			if (pos_end.has_lane_lateral_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_lane_lateral_position, (int)pos_end.lane_lateral_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_lane_lateral_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_lateral_position], ts); }
			if (pos_end.has_crp_id_begin()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id_begin, (int)pos_end.crp_id_begin(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id_begin, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id_begin], ts); }
			if (pos_end.has_crp_id_end()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id_end, (int)pos_end.crp_id_end(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id_end, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id_end], ts); }
			if (pos_end.has_lane_vertical_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_lane_vertical_position, (int)pos_end.lane_vertical_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_lane_vertical_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_vertical_position], ts); }
			if (pos_end.has_lane_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_lane_id, (unsigned long long)pos_end.lane_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_lane_id, (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_id], ts); }
			if (pos_end.has_dx_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dx_lane, (int)pos_end.dx_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dx_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dx_lane], ts); }
			if (pos_end.has_dy_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dy_lane, (int)pos_end.dy_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dy_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dy_lane], ts); }
			if (pos_end.has_dh_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dh_lane, (int)pos_end.dh_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_dh_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dh_lane], ts); }
			if (pos_end.has_semi_axis_length_major()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_semi_axis_length_major, (int)pos_end.semi_axis_length_major(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_semi_axis_length_major, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_semi_axis_length_major], ts); }
			if (pos_end.has_semi_axis_length_minor()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_semi_axis_length_minor, (int)pos_end.semi_axis_length_minor(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_semi_axis_length_minor, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_semi_axis_length_minor], ts); }
			if (pos_end.has_ellipse_orientation()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_ellipse_orientation, (int)pos_end.ellipse_orientation(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_ellipse_orientation, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_ellipse_orientation], ts); }
			if (pos_end.has_altitude_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_altitude_accuracy, (int)pos_end.altitude_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::position_end_altitude_accuracy, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_altitude_accuracy], ts); }
			if (fre.has_length_value()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::length_value, (int)fre.length_value(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::length_value, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::length_value], ts); }
			if (fre.has_length_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::length_accuracy, (int)fre.length_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::length_accuracy, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::length_accuracy], ts); }
			if (fre.has_existency()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::existency, (int)fre.existency(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::existency, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::existency], ts); }
			if (fre.has_minimal_detectable_size()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::minimal_detectable_size, (int)fre.minimal_detectable_size(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::minimal_detectable_size, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::minimal_detectable_size], ts); }
			if (fre.has_id_begin()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::id_begin, (unsigned long long)fre.id_begin(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::id_begin, (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::id_begin], ts); }
			if (fre.has_id_end()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::id_end, (unsigned long long)fre.id_end(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::id_end, (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::id_end], ts); }
			{
				vector<unsigned long long> vull;
				for (int i = 0; i < fre.information_source_list_size(); i++) {
					vull.push_back((unsigned long long)fre.information_source_list(i));
				}
				tuple.setValue((int)FREESPACE_INFO_POS_0_8_0::information_source_list, vull, ts);
			}
			isTupleInfoDeserializeToTuple(tuple, is_info, ts, (int)FREESPACE_INFO_POS_0_8_0::information_source_list + 1);
			tuples.push_back(tuple);
		}
	}
	/**
	* フリースペース情報のデシリアライズ (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2025/06/30
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	*
	 */
	void ProtobufParser::freespaceInfoDeserializeToTuple_0_8_1(string serializedStr, vector<Tuple> &tuples, const Schema &schema)
	{
		dm2_proto::Freespace_info_0_8_1_dm2is msg;
		msg.ParseFromString(serializedStr);
		string strVal;
		// CS からの転送の場合、ここで管理者情報列込の列数がセットされる
		int attr_size = schema.getAttributeSize();
		for (int i = 0; i < msg.freespace_info_size(); i++) {
			const cool4_api_0_8_0::Freespace_info_0_8_1& fre = msg.freespace_info(i);
			const cool4_api_0_8_0::Location vertices_begin = fre.vertices_begin();
			const cool4_api_0_8_0::Location pos_begin = fre.position_begin();
			const cool4_api_0_8_0::Location pos_end = fre.position_end();
			const dm2_proto::Is_tuple_info& is_info = msg.is_tuple_info(i);
			if (attr_size == 0) {
				// 受信アプリの場合、スキーマを持たないため、ここで列数をセット（電文の中に時刻列が入っていれば、管理者情報列分、追加する）
				attr_size = freespaceInfo_0_8_1_normal_size;
				if (is_info.dm2_ts_group_size() > 0) attr_size = attr_size + is_tuple_size;
			}
			Tuple tuple(attr_size);
			long ts = is_info.dm2_create_ts();
			tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::id, (unsigned long long)fre.id(), ts);
			tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::time, (long)fre.time(), ts);
			tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::revision, (int)fre.revision(), ts);
			if (fre.has_detection_method()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::detection_method, (int)fre.detection_method(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::detection_method, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::detection_method], ts); }
			if (fre.has_detactable_classes()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::detactable_classes, (int)fre.detactable_classes(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::detactable_classes, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::detactable_classes], ts); }
			if (vertices_begin.has_geodetic_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_geodetic_srid, (int)vertices_begin.geodetic_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_geodetic_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_geodetic_srid], ts); }
			if (vertices_begin.has_latitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_latitude, (int)vertices_begin.latitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_latitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_latitude], ts); }
			if (vertices_begin.has_longitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_longitude, (int)vertices_begin.longitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_longitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_longitude], ts); }
			if (vertices_begin.has_altitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_altitude, (int)vertices_begin.altitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_altitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_altitude], ts); }
			if (vertices_begin.has_projective_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_projective_srid, (int)vertices_begin.projective_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_projective_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_projective_srid], ts); }
			if (vertices_begin.has_prc_x()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_x, (int)vertices_begin.prc_x(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_x, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_x], ts); }
			if (vertices_begin.has_prc_y()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_y, (int)vertices_begin.prc_y(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_y, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_y], ts); }
			if (vertices_begin.has_prc_z()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_z, (int)vertices_begin.prc_z(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_z, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_z], ts); }
			if (vertices_begin.has_crp_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id, (int)vertices_begin.crp_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id], ts); }
			if (vertices_begin.has_dx_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dx_crp, (int)vertices_begin.dx_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dx_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dx_crp], ts); }
			if (vertices_begin.has_dy_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dy_crp, (int)vertices_begin.dy_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dy_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dy_crp], ts); }
			if (vertices_begin.has_dh_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dh_crp, (int)vertices_begin.dh_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dh_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dh_crp], ts); }
			if (vertices_begin.has_lane_count()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_count, (int)vertices_begin.lane_count(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_count, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_count], ts); }
			if (vertices_begin.has_lane_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_position, (int)vertices_begin.lane_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_position], ts); }
			if (vertices_begin.has_lane_lateral_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_lateral_position, (int)vertices_begin.lane_lateral_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_lateral_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_lateral_position], ts); }
			if (vertices_begin.has_crp_id_begin()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id_begin, (int)vertices_begin.crp_id_begin(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id_begin, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id_begin], ts); }
			if (vertices_begin.has_crp_id_end()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id_end, (int)vertices_begin.crp_id_end(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id_end, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id_end], ts); }
			if (vertices_begin.has_lane_vertical_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_vertical_position, (int)vertices_begin.lane_vertical_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_vertical_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_vertical_position], ts); }
			if (vertices_begin.has_lane_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_id, (unsigned long long)vertices_begin.lane_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_id, (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_id], ts); }
			if (vertices_begin.has_dx_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dx_lane, (int)vertices_begin.dx_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dx_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dx_lane], ts); }
			if (vertices_begin.has_dy_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dy_lane, (int)vertices_begin.dy_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dy_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dy_lane], ts); }
			if (vertices_begin.has_dh_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dh_lane, (int)vertices_begin.dh_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dh_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dh_lane], ts); }
			if (vertices_begin.has_semi_axis_length_major()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_semi_axis_length_major, (int)vertices_begin.semi_axis_length_major(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_semi_axis_length_major, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_semi_axis_length_major], ts); }
			if (vertices_begin.has_semi_axis_length_minor()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_semi_axis_length_minor, (int)vertices_begin.semi_axis_length_minor(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_semi_axis_length_minor, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_semi_axis_length_minor], ts); }
			if (vertices_begin.has_ellipse_orientation()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_ellipse_orientation, (int)vertices_begin.ellipse_orientation(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_ellipse_orientation, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_ellipse_orientation], ts); }
			if (vertices_begin.has_altitude_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_altitude_accuracy, (int)vertices_begin.altitude_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_begin_altitude_accuracy, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_altitude_accuracy], ts); }
			{
				vector<int> vi;
				for (int i = 0; i < fre.vertices_dx_list_size(); i++) {
					vi.push_back((int)fre.vertices_dx_list(i));
				}
				tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_dx_list, vi, ts);
			}
			{
				vector<int> vi;
				for (int i = 0; i < fre.vertices_dy_list_size(); i++) {
					vi.push_back((int)fre.vertices_dy_list(i));
				}
				tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::vertices_dy_list, vi, ts);
			}
			if (pos_begin.has_geodetic_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_geodetic_srid, (int)pos_begin.geodetic_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_geodetic_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_geodetic_srid], ts); }
			if (pos_begin.has_latitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_latitude, (int)pos_begin.latitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_latitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_latitude], ts); }
			if (pos_begin.has_longitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_longitude, (int)pos_begin.longitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_longitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_longitude], ts); }
			if (pos_begin.has_altitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_altitude, (int)pos_begin.altitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_altitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_altitude], ts); }
			if (pos_begin.has_projective_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_projective_srid, (int)pos_begin.projective_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_projective_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_projective_srid], ts); }
			if (pos_begin.has_prc_x()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_x, (int)pos_begin.prc_x(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_x, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_x], ts); }
			if (pos_begin.has_prc_y()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_y, (int)pos_begin.prc_y(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_y, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_y], ts); }
			if (pos_begin.has_prc_z()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_z, (int)pos_begin.prc_z(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_z, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_z], ts); }
			if (pos_begin.has_crp_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id, (int)pos_begin.crp_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id], ts); }
			if (pos_begin.has_dx_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dx_crp, (int)pos_begin.dx_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dx_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dx_crp], ts); }
			if (pos_begin.has_dy_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dy_crp, (int)pos_begin.dy_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dy_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dy_crp], ts); }
			if (pos_begin.has_dh_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dh_crp, (int)pos_begin.dh_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dh_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dh_crp], ts); }
			if (pos_begin.has_lane_count()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_count, (int)pos_begin.lane_count(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_count, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_count], ts); }
			if (pos_begin.has_lane_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_position, (int)pos_begin.lane_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_position], ts); }
			if (pos_begin.has_lane_lateral_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_lateral_position, (int)pos_begin.lane_lateral_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_lateral_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_lateral_position], ts); }
			if (pos_begin.has_crp_id_begin()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id_begin, (int)pos_begin.crp_id_begin(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id_begin, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id_begin], ts); }
			if (pos_begin.has_crp_id_end()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id_end, (int)pos_begin.crp_id_end(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id_end, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id_end], ts); }
			if (pos_begin.has_lane_vertical_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_vertical_position, (int)pos_begin.lane_vertical_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_vertical_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_vertical_position], ts); }
			if (pos_begin.has_lane_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_id, (unsigned long long)pos_begin.lane_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_id, (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_id], ts); }
			if (pos_begin.has_dx_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dx_lane, (int)pos_begin.dx_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dx_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dx_lane], ts); }
			if (pos_begin.has_dy_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dy_lane, (int)pos_begin.dy_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dy_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dy_lane], ts); }
			if (pos_begin.has_dh_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dh_lane, (int)pos_begin.dh_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_dh_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dh_lane], ts); }
			if (pos_begin.has_semi_axis_length_major()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_semi_axis_length_major, (int)pos_begin.semi_axis_length_major(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_semi_axis_length_major, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_semi_axis_length_major], ts); }
			if (pos_begin.has_semi_axis_length_minor()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_semi_axis_length_minor, (int)pos_begin.semi_axis_length_minor(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_semi_axis_length_minor, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_semi_axis_length_minor], ts); }
			if (pos_begin.has_ellipse_orientation()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_ellipse_orientation, (int)pos_begin.ellipse_orientation(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_ellipse_orientation, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_ellipse_orientation], ts); }
			if (pos_begin.has_altitude_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_altitude_accuracy, (int)pos_begin.altitude_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_begin_altitude_accuracy, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_altitude_accuracy], ts); }
			if (pos_end.has_geodetic_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_geodetic_srid, (int)pos_end.geodetic_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_geodetic_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_geodetic_srid], ts); }
			if (pos_end.has_latitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_latitude, (int)pos_end.latitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_latitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_latitude], ts); }
			if (pos_end.has_longitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_longitude, (int)pos_end.longitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_longitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_longitude], ts); }
			if (pos_end.has_altitude()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_altitude, (int)pos_end.altitude(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_altitude, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_altitude], ts); }
			if (pos_end.has_projective_srid()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_projective_srid, (int)pos_end.projective_srid(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_projective_srid, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_projective_srid], ts); }
			if (pos_end.has_prc_x()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_prc_x, (int)pos_end.prc_x(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_prc_x, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_prc_x], ts); }
			if (pos_end.has_prc_y()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_prc_y, (int)pos_end.prc_y(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_prc_y, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_prc_y], ts); }
			if (pos_end.has_prc_z()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_prc_z, (int)pos_end.prc_z(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_prc_z, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_prc_z], ts); }
			if (pos_end.has_crp_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id, (int)pos_end.crp_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id], ts); }
			if (pos_end.has_dx_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dx_crp, (int)pos_end.dx_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dx_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dx_crp], ts); }
			if (pos_end.has_dy_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dy_crp, (int)pos_end.dy_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dy_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dy_crp], ts); }
			if (pos_end.has_dh_crp()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dh_crp, (int)pos_end.dh_crp(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dh_crp, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dh_crp], ts); }
			if (pos_end.has_lane_count()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_lane_count, (int)pos_end.lane_count(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_lane_count, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_count], ts); }
			if (pos_end.has_lane_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_lane_position, (int)pos_end.lane_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_lane_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_position], ts); }
			if (pos_end.has_lane_lateral_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_lane_lateral_position, (int)pos_end.lane_lateral_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_lane_lateral_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_lateral_position], ts); }
			if (pos_end.has_crp_id_begin()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id_begin, (int)pos_end.crp_id_begin(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id_begin, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id_begin], ts); }
			if (pos_end.has_crp_id_end()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id_end, (int)pos_end.crp_id_end(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id_end, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id_end], ts); }
			if (pos_end.has_lane_vertical_position()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_lane_vertical_position, (int)pos_end.lane_vertical_position(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_lane_vertical_position, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_vertical_position], ts); }
			if (pos_end.has_lane_id()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_lane_id, (unsigned long long)pos_end.lane_id(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_lane_id, (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_id], ts); }
			if (pos_end.has_dx_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dx_lane, (int)pos_end.dx_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dx_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dx_lane], ts); }
			if (pos_end.has_dy_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dy_lane, (int)pos_end.dy_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dy_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dy_lane], ts); }
			if (pos_end.has_dh_lane()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dh_lane, (int)pos_end.dh_lane(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_dh_lane, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dh_lane], ts); }
			if (pos_end.has_semi_axis_length_major()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_semi_axis_length_major, (int)pos_end.semi_axis_length_major(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_semi_axis_length_major, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_semi_axis_length_major], ts); }
			if (pos_end.has_semi_axis_length_minor()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_semi_axis_length_minor, (int)pos_end.semi_axis_length_minor(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_semi_axis_length_minor, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_semi_axis_length_minor], ts); }
			if (pos_end.has_ellipse_orientation()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_ellipse_orientation, (int)pos_end.ellipse_orientation(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_ellipse_orientation, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_ellipse_orientation], ts); }
			if (pos_end.has_altitude_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_altitude_accuracy, (int)pos_end.altitude_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::position_end_altitude_accuracy, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_altitude_accuracy], ts); }
			if (fre.has_length_value()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::length_value, (int)fre.length_value(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::length_value, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::length_value], ts); }
			if (fre.has_length_accuracy()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::length_accuracy, (int)fre.length_accuracy(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::length_accuracy, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::length_accuracy], ts); }
			if (fre.has_existency()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::existency, (int)fre.existency(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::existency, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::existency], ts); }
			if (fre.has_minimal_detectable_size()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::minimal_detectable_size, (int)fre.minimal_detectable_size(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::minimal_detectable_size, freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::minimal_detectable_size], ts); }
			if (fre.has_id_begin()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::id_begin, (unsigned long long)fre.id_begin(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::id_begin, (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::id_begin], ts); }
			if (fre.has_id_end()) { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::id_end, (unsigned long long)fre.id_end(), ts); } else { tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::id_end, (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::id_end], ts); }
			{
				vector<unsigned long long> vull;
				for (int i = 0; i < fre.information_source_list_size(); i++) {
					vull.push_back((unsigned long long)fre.information_source_list(i));
				}
				tuple.setValue((int)FREESPACE_INFO_POS_0_8_1::information_source_list, vull, ts);
			}
			isTupleInfoDeserializeToTuple(tuple, is_info, ts, (int)FREESPACE_INFO_POS_0_8_1::information_source_list + 1);
			tuples.push_back(tuple);
		}
	}
	/**
	* 信号情報のデシリアライズ (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	*
	 */
	void ProtobufParser::signalInfoDeserializeToTuple_0_6_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema)
	{
		dm2_proto::Signal_info_0_6_0_dm2is msg;
		msg.ParseFromString(serializedStr);
		string strVal;
		// CS からの転送の場合、ここで管理者情報列込の列数がセットされる
		int attr_size = schema.getAttributeSize();
		for (int i = 0; i < msg.signal_info_size(); i++) {
			const cool4_api_0_6_0::Signal_info& sig = msg.signal_info(i);
			const dm2_proto::Is_tuple_info& is_info = msg.is_tuple_info(i);
			if (attr_size == 0) {
				// 受信アプリの場合、スキーマを持たないため、ここで列数をセット（電文の中に時刻列が入っていれば、管理者情報列分、追加する）
				attr_size = signalInfo_0_6_0_normal_size;
				if (is_info.dm2_ts_group_size() > 0) attr_size = attr_size + is_tuple_size;
			}
			Tuple tuple(attr_size);
			long ts = is_info.dm2_create_ts();
			tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::crp_id, (int)sig.crp_id(), ts);
			{
				strVal = "";
				for (int i = 0; i < sig.id_size(); i++) {
					if (i != 0) strVal += ",";
					strVal += to_string(sig.id(i));
				}
				tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::id, stringUtil.getAnyValFromString(strVal, signalInfoTypeList_0_6_0[(int)SIGNAL_INFO_POS_0_6_0::id]), ts);
			}
			tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::time, (long)sig.time(), ts);
			if (sig.has_state()) { tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::state, (int)sig.state(), ts); } else { tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::state, signalInfoUnknownVals_0_6_0[(int)SIGNAL_INFO_POS_0_6_0::state], ts); }
			if (sig.has_specific_control_flags()) { tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::specific_control_flags, (int)sig.specific_control_flags(), ts); } else { tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::specific_control_flags, signalInfoUnknownVals_0_6_0[(int)SIGNAL_INFO_POS_0_6_0::specific_control_flags], ts); }
			if (sig.has_event_count()) { tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::event_count, (int)sig.event_count(), ts); } else { tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::event_count, signalInfoUnknownVals_0_6_0[(int)SIGNAL_INFO_POS_0_6_0::event_count], ts); }
			if (sig.has_count_down_stop_flag()) { tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::count_down_stop_flag, (int)sig.count_down_stop_flag(), ts); } else { tuple.setValue((int)SIGNAL_INFO_POS_0_6_0::count_down_stop_flag, signalInfoUnknownVals_0_6_0[(int)SIGNAL_INFO_POS_0_6_0::count_down_stop_flag], ts); }
			int lig_pos = (int)SIGNAL_INFO_POS_0_6_0::light_info_main_0;
			{
				for (int i = 0; i < sig.light_info_size(); i++) {
					const cool4_api_0_6_0::Light_info lig = sig.light_info(i);
					if (lig.has_main()) { tuple.setValue(lig_pos, (int)lig.main(), ts); } else { tuple.setValue(lig_pos, signalInfoUnknownVals_0_6_0[lig_pos], ts); }
					lig_pos++;
					if (lig.has_arrow()) { tuple.setValue(lig_pos, (int)lig.arrow(), ts); } else { tuple.setValue(lig_pos, signalInfoUnknownVals_0_6_0[lig_pos], ts); }
					lig_pos++;
					if (lig.has_min_time_to_change()) { tuple.setValue(lig_pos, (int)lig.min_time_to_change(), ts); } else { tuple.setValue(lig_pos, signalInfoUnknownVals_0_6_0[lig_pos], ts); }
					lig_pos++;
					if (lig.has_max_time_to_change()) { tuple.setValue(lig_pos, (int)lig.max_time_to_change(), ts); } else { tuple.setValue(lig_pos, signalInfoUnknownVals_0_6_0[lig_pos], ts); }
					lig_pos++;
				}
			}
			isTupleInfoDeserializeToTuple(tuple, is_info, ts, lig_pos);
			tuples.push_back(tuple);
		}
	}
	/**
	* センサー情報のデシリアライズ (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	*
	 */
	void ProtobufParser::sensorInfoDeserializeToTuple_0_8_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema)
	{
		dm2_proto::Sensor_info_0_8_0_dm2is msg;
		msg.ParseFromString(serializedStr);
		string strVal;
		// CS からの転送の場合、ここで管理者情報列込の列数がセットされる
		int attr_size = schema.getAttributeSize();
		for (int i = 0; i < msg.sensor_info_size(); i++) {
			const cool4_api_0_8_0::Sensor_info_0_8_0& sen = msg.sensor_info(i);
			const cool4_api_0_8_0::Position sen_po = sen.position();
			const dm2_proto::Is_tuple_info& is_info = msg.is_tuple_info(i);
			if (attr_size == 0) {
				// 受信アプリの場合、スキーマを持たないため、ここで列数をセット（電文の中に時刻列が入っていれば、管理者情報列分、追加する）
				attr_size = sensorInfo_0_8_0_normal_size;
				if (is_info.dm2_ts_group_size() > 0) attr_size = attr_size + is_tuple_size;
			}
			Tuple tuple(attr_size);
			long ts = is_info.dm2_create_ts();
			tuple.setValue((int)SENSOR_INFO_POS_0_8_0::unit_id, (unsigned long long)sen.unit_id(), ts);
			tuple.setValue((int)SENSOR_INFO_POS_0_8_0::sensor_id, (int)sen.sensor_id(), ts);
			if (sen.has_sensor_type()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::sensor_type, (int)sen.sensor_type(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::sensor_type, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::sensor_type], ts); }
			if (sen_po.has_geodetic_srid()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::geodetic_srid, (int)sen_po.geodetic_srid(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::geodetic_srid, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::geodetic_srid], ts); }
			if (sen_po.has_latitude()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::latitude, (int)sen_po.latitude(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::latitude, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::latitude], ts); }
			if (sen_po.has_longitude()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::longitude, (int)sen_po.longitude(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::longitude, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::longitude], ts); }
			if (sen_po.has_altitude()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::altitude, (int)sen_po.altitude(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::altitude, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::altitude], ts); }
			if (sen_po.has_projective_srid()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::projective_srid, (int)sen_po.projective_srid(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::projective_srid, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::projective_srid], ts); }
			if (sen_po.has_prc_x()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::prc_x, (int)sen_po.prc_x(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::prc_x, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::prc_x], ts); }
			if (sen_po.has_prc_y()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::prc_y, (int)sen_po.prc_y(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::prc_y, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::prc_y], ts); }
			if (sen_po.has_prc_z()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::prc_z, (int)sen_po.prc_z(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::prc_z, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::prc_z], ts); }
			if (sen_po.has_crp_id()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::crp_id, (int)sen_po.crp_id(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::crp_id, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::crp_id], ts); }
			if (sen_po.has_dx_crp()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dx_crp, (int)sen_po.dx_crp(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dx_crp, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dx_crp], ts); }
			if (sen_po.has_dy_crp()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dy_crp, (int)sen_po.dy_crp(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dy_crp, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dy_crp], ts); }
			if (sen_po.has_dh_crp()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dh_crp, (int)sen_po.dh_crp(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dh_crp, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dh_crp], ts); }
			if (sen_po.has_lane_count()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::lane_count, (int)sen_po.lane_count(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::lane_count, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::lane_count], ts); }
			if (sen_po.has_lane_position()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::lane_position, (int)sen_po.lane_position(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::lane_position, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::lane_position], ts); }
			if (sen_po.has_lane_lateral_position()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::lane_lateral_position, (int)sen_po.lane_lateral_position(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::lane_lateral_position, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::lane_lateral_position], ts); }
			if (sen_po.has_crp_id_begin()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::crp_id_begin, (int)sen_po.crp_id_begin(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::crp_id_begin, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::crp_id_begin], ts); }
			if (sen_po.has_crp_id_end()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::crp_id_end, (int)sen_po.crp_id_end(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::crp_id_end, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::crp_id_end], ts); }
			if (sen_po.has_lane_vertical_position()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::lane_vertical_position, (int)sen_po.lane_vertical_position(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::lane_vertical_position, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::lane_vertical_position], ts); }
			if (sen_po.has_lane_id()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::lane_id, (unsigned long long)sen_po.lane_id(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::lane_id, (unsigned long long)sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::lane_id], ts); }
			if (sen_po.has_dx_lane()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dx_lane, (int)sen_po.dx_lane(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dx_lane, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dx_lane], ts); }
			if (sen_po.has_dy_lane()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dy_lane, (int)sen_po.dy_lane(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dy_lane, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dy_lane], ts); }
			if (sen_po.has_dh_lane()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dh_lane, (int)sen_po.dh_lane(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::dh_lane, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dh_lane], ts); }
			if (sen_po.has_semi_axis_length_major()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::semi_axis_length_major, (int)sen_po.semi_axis_length_major(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::semi_axis_length_major, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::semi_axis_length_major], ts); }
			if (sen_po.has_semi_axis_length_minor()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::semi_axis_length_minor, (int)sen_po.semi_axis_length_minor(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::semi_axis_length_minor, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::semi_axis_length_minor], ts); }
			if (sen_po.has_ellipse_orientation()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::ellipse_orientation, (int)sen_po.ellipse_orientation(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::ellipse_orientation, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::ellipse_orientation], ts); }
			if (sen_po.has_altitude_accuracy()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::altitude_accuracy, (int)sen_po.altitude_accuracy(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::altitude_accuracy, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::altitude_accuracy], ts); }
			int dc_pos = (int)SENSOR_INFO_POS_0_8_0::detectable_classes_0;
			{
				for (int i = 0; i < sen.detectable_capabilities_size(); i++) {
					const cool4_api_0_8_0::Detectable_capabilities dc = sen.detectable_capabilities(i);
					if (dc.has_detectable_classes()) { tuple.setValue(dc_pos, (int)dc.detectable_classes(), ts); } else { tuple.setValue(dc_pos, sensorInfoUnknownVals_0_8_0[dc_pos], ts); }
					dc_pos++;
					{
						vector<int> vi;
						for (int i = 0; i < dc.detectable_domain_x_list_size(); i++) {
							vi.push_back((int)dc.detectable_domain_x_list(i));
						}
						tuple.setValue(dc_pos, vi, ts);
					}
					dc_pos++;
					{
						vector<int> vi;
						for (int i = 0; i < dc.detectable_domain_y_list_size(); i++) {
							vi.push_back((int)dc.detectable_domain_y_list(i));
						}
						tuple.setValue(dc_pos, vi, ts);
					}
					dc_pos++;
					if (dc.has_confidence()) { tuple.setValue(dc_pos, (int)dc.confidence(), ts); } else { tuple.setValue(dc_pos, sensorInfoUnknownVals_0_8_0[dc_pos], ts); }
					dc_pos++;
					if (dc.has_minimal_detectable_size()) { tuple.setValue(dc_pos, (int)dc.minimal_detectable_size(), ts); } else { tuple.setValue(dc_pos, sensorInfoUnknownVals_0_8_0[dc_pos], ts); }
					dc_pos++;
				}
			}
			if (sen.has_sensor_status()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::sensor_status, (int)sen.sensor_status(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_0::sensor_status, sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::sensor_status], ts); }
			isTupleInfoDeserializeToTuple(tuple, is_info, ts, (int)SENSOR_INFO_POS_0_8_0::sensor_status + 1);
			tuples.push_back(tuple);
		}
	}
	/**
	* センサー情報のデシリアライズ (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	*
	 */
	void ProtobufParser::sensorInfoDeserializeToTuple_0_8_1(string serializedStr, vector<Tuple> &tuples, const Schema &schema)
	{
		dm2_proto::Sensor_info_0_8_1_dm2is msg;
		msg.ParseFromString(serializedStr);
		string strVal;
		// CS からの転送の場合、ここで管理者情報列込の列数がセットされる
		int attr_size = schema.getAttributeSize();
		for (int i = 0; i < msg.sensor_info_size(); i++) {
			const cool4_api_0_8_0::Sensor_info_0_8_1& sen = msg.sensor_info(i);
			const cool4_api_0_8_0::Position sen_po = sen.position();
			const dm2_proto::Is_tuple_info& is_info = msg.is_tuple_info(i);
			if (attr_size == 0) {
				// 受信アプリの場合、スキーマを持たないため、ここで列数をセット（電文の中に時刻列が入っていれば、管理者情報列分、追加する）
				attr_size = sensorInfo_0_8_1_normal_size;
				if (is_info.dm2_ts_group_size() > 0) attr_size = attr_size + is_tuple_size;
			}
			Tuple tuple(attr_size);
			long ts = is_info.dm2_create_ts();
			tuple.setValue((int)SENSOR_INFO_POS_0_8_1::unit_id, (unsigned long long)sen.unit_id(), ts);
			tuple.setValue((int)SENSOR_INFO_POS_0_8_1::sensor_id, (int)sen.sensor_id(), ts);
			if (sen.has_sensor_type()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::sensor_type, (int)sen.sensor_type(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::sensor_type, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::sensor_type], ts); }
			if (sen_po.has_geodetic_srid()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::geodetic_srid, (int)sen_po.geodetic_srid(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::geodetic_srid, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::geodetic_srid], ts); }
			if (sen_po.has_latitude()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::latitude, (int)sen_po.latitude(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::latitude, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::latitude], ts); }
			if (sen_po.has_longitude()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::longitude, (int)sen_po.longitude(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::longitude, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::longitude], ts); }
			if (sen_po.has_altitude()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::altitude, (int)sen_po.altitude(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::altitude, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::altitude], ts); }
			if (sen_po.has_projective_srid()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::projective_srid, (int)sen_po.projective_srid(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::projective_srid, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::projective_srid], ts); }
			if (sen_po.has_prc_x()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::prc_x, (int)sen_po.prc_x(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::prc_x, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::prc_x], ts); }
			if (sen_po.has_prc_y()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::prc_y, (int)sen_po.prc_y(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::prc_y, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::prc_y], ts); }
			if (sen_po.has_prc_z()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::prc_z, (int)sen_po.prc_z(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::prc_z, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::prc_z], ts); }
			if (sen_po.has_crp_id()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::crp_id, (int)sen_po.crp_id(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::crp_id, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::crp_id], ts); }
			if (sen_po.has_dx_crp()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dx_crp, (int)sen_po.dx_crp(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dx_crp, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dx_crp], ts); }
			if (sen_po.has_dy_crp()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dy_crp, (int)sen_po.dy_crp(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dy_crp, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dy_crp], ts); }
			if (sen_po.has_dh_crp()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dh_crp, (int)sen_po.dh_crp(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dh_crp, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dh_crp], ts); }
			if (sen_po.has_lane_count()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::lane_count, (int)sen_po.lane_count(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::lane_count, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::lane_count], ts); }
			if (sen_po.has_lane_position()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::lane_position, (int)sen_po.lane_position(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::lane_position, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::lane_position], ts); }
			if (sen_po.has_lane_lateral_position()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::lane_lateral_position, (int)sen_po.lane_lateral_position(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::lane_lateral_position, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::lane_lateral_position], ts); }
			if (sen_po.has_crp_id_begin()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::crp_id_begin, (int)sen_po.crp_id_begin(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::crp_id_begin, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::crp_id_begin], ts); }
			if (sen_po.has_crp_id_end()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::crp_id_end, (int)sen_po.crp_id_end(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::crp_id_end, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::crp_id_end], ts); }
			if (sen_po.has_lane_vertical_position()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::lane_vertical_position, (int)sen_po.lane_vertical_position(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::lane_vertical_position, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::lane_vertical_position], ts); }
			if (sen_po.has_lane_id()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::lane_id, (unsigned long long)sen_po.lane_id(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::lane_id, (unsigned long long)sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::lane_id], ts); }
			if (sen_po.has_dx_lane()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dx_lane, (int)sen_po.dx_lane(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dx_lane, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dx_lane], ts); }
			if (sen_po.has_dy_lane()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dy_lane, (int)sen_po.dy_lane(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dy_lane, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dy_lane], ts); }
			if (sen_po.has_dh_lane()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dh_lane, (int)sen_po.dh_lane(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::dh_lane, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dh_lane], ts); }
			if (sen_po.has_semi_axis_length_major()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::semi_axis_length_major, (int)sen_po.semi_axis_length_major(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::semi_axis_length_major, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::semi_axis_length_major], ts); }
			if (sen_po.has_semi_axis_length_minor()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::semi_axis_length_minor, (int)sen_po.semi_axis_length_minor(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::semi_axis_length_minor, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::semi_axis_length_minor], ts); }
			if (sen_po.has_ellipse_orientation()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::ellipse_orientation, (int)sen_po.ellipse_orientation(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::ellipse_orientation, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::ellipse_orientation], ts); }
			if (sen_po.has_altitude_accuracy()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::altitude_accuracy, (int)sen_po.altitude_accuracy(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::altitude_accuracy, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::altitude_accuracy], ts); }
			tuple.setValue((int)SENSOR_INFO_POS_0_8_1::time, (long)sen.time(), ts);	
			int dc_pos = (int)SENSOR_INFO_POS_0_8_1::detectable_classes_0;
			{
				for (int i = 0; i < sen.detectable_capabilities_size(); i++) {
					const cool4_api_0_8_0::Detectable_capabilities dc = sen.detectable_capabilities(i);
					if (dc.has_detectable_classes()) { tuple.setValue(dc_pos, (int)dc.detectable_classes(), ts); } else { tuple.setValue(dc_pos, sensorInfoUnknownVals_0_8_1[dc_pos], ts); }
					dc_pos++;
					{
						vector<int> vi;
						for (int i = 0; i < dc.detectable_domain_x_list_size(); i++) {
							vi.push_back((int)dc.detectable_domain_x_list(i));
						}
						tuple.setValue(dc_pos, vi, ts);
					}
					dc_pos++;
					{
						vector<int> vi;
						for (int i = 0; i < dc.detectable_domain_y_list_size(); i++) {
							vi.push_back((int)dc.detectable_domain_y_list(i));
						}
						tuple.setValue(dc_pos, vi, ts);
					}
					dc_pos++;
					if (dc.has_confidence()) { tuple.setValue(dc_pos, (int)dc.confidence(), ts); } else { tuple.setValue(dc_pos, sensorInfoUnknownVals_0_8_1[dc_pos], ts); }
					dc_pos++;
					if (dc.has_minimal_detectable_size()) { tuple.setValue(dc_pos, (int)dc.minimal_detectable_size(), ts); } else { tuple.setValue(dc_pos, sensorInfoUnknownVals_0_8_1[dc_pos], ts); }
					dc_pos++;
				}
			}
			if (sen.has_sensor_status()) { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::sensor_status, (int)sen.sensor_status(), ts); } else { tuple.setValue((int)SENSOR_INFO_POS_0_8_1::sensor_status, sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::sensor_status], ts); }
			isTupleInfoDeserializeToTuple(tuple, is_info, ts, (int)SENSOR_INFO_POS_0_8_1::sensor_status + 1);
			tuples.push_back(tuple);
		}
	}
	/**
	* ISタプル情報のデシリアライズ
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	tuple	タプル
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	ts		タイムスタンプ
	*
	 */
	void ProtobufParser::isTupleInfoDeserializeToTuple(Tuple &tuple, const dm2_proto::Is_tuple_info &is_info, long &ts, const int &curIdx)
	{	
		if (tuple.size() <= curIdx + (int)IS_TUPLE_INFO_POS::dm2_ts_group) return;
		string strVal;
		// DBシステムが付与する項目
		if (is_info.has_dm2_creator_id()) {
			tuple.setValue(curIdx + (int)IS_TUPLE_INFO_POS::dm2_creator_id, is_info.dm2_creator_id(), ts);
		} else {
			strVal = "dm2developer1";
			tuple.setValue(curIdx + (int)IS_TUPLE_INFO_POS::dm2_creator_id, strVal, ts);
		}
		if (is_info.has_dm2_ip_addr()) {
			tuple.setValue(curIdx + (int)IS_TUPLE_INFO_POS::dm2_ip_addr, is_info.dm2_ip_addr(), ts);
		} else {
			strVal = "0.0.0.0";
			tuple.setValue(curIdx + (int)IS_TUPLE_INFO_POS::dm2_ip_addr, strVal, ts);
		}
		tuple.setValue(curIdx + (int)IS_TUPLE_INFO_POS::dm2_hash, to_string(is_info.dm2_hash()), ts);
		{
			strVal = "";
			for (int i = 0; i < is_info.dm2_ts_group_size(); i++) {
				strVal += to_string(is_info.dm2_ts_group(i)) + ",";
			}
			tuple.setValue(curIdx + (int)IS_TUPLE_INFO_POS::dm2_ts_group, strVal, ts);
		}
	}
	/**
	* 物標情報のセット (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	obj		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	tuple	タプル
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::setObjectInfo(cool4_api_0_6_0::Object_info *obj, dm2_proto::Is_tuple_info *is_info, Tuple tuple)
	{
		int tuple_size = tuple.size();
		cool4_api_0_6_0::Position_and_movement* obj_pm = obj->mutable_position_and_movement();
		cool4_api_0_6_0::Vehicle_condition* obj_vc = obj->mutable_vehicle_condition();
		vector<any> vals(tuple_size);
		string strVal;
		long ts_first = 0;
		long ts_tmp = 0;
		int iVal;
		for (int i = 0; i < tuple_size; i++) {
			tuple.getValue(i, vals[i], ts_tmp);
			if (i == 0) ts_first = ts_tmp;
		}
		is_info->set_dm2_create_ts(ts_first);
		strVal = stringUtil.getAnyString(vals[(int)OBJECT_INFO_POS_0_6_0::id]);
		obj->set_id(DmUtil::getUnsignedLongFromString(strVal));
		obj->set_time(any_cast<long>(vals[(int)OBJECT_INFO_POS_0_6_0::time]));
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_id_0]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_id_0]) obj->set_object_class_id_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_0]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_0]) obj->set_object_class_confidence_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_0]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_0]) obj->set_object_class_subclass_type_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_0]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_0]) obj->set_object_class_subclass_confidence_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_id_1]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_id_1]) obj->set_object_class_id_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_1]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_1]) obj->set_object_class_confidence_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_1]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_1]) obj->set_object_class_subclass_type_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_1]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_1]) obj->set_object_class_subclass_confidence_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_id_2]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_id_2]) obj->set_object_class_id_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_2]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_2]) obj->set_object_class_confidence_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_2]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_2]) obj->set_object_class_subclass_type_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_2]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_2]) obj->set_object_class_subclass_confidence_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_id_3]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_id_3]) obj->set_object_class_id_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_3]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_confidence_3]) obj->set_object_class_confidence_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_3]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_type_3]) obj->set_object_class_subclass_type_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_3]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::object_class_subclass_confidence_3]) obj->set_object_class_subclass_confidence_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::existency]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::existency]) obj->set_existency(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::geodetic_system]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::geodetic_system]) obj_pm->set_geodetic_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::latitude]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::latitude]) obj_pm->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::longitude]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::longitude]) obj_pm->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::altitude]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::altitude]) obj_pm->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::crp_id]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::crp_id]) obj_pm->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::dx_crp]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dx_crp]) obj_pm->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::dy_crp]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dy_crp]) obj_pm->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::dh_crp]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dh_crp]) obj_pm->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::lane_count]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_count]) obj_pm->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::lane_position]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_position]) obj_pm->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::lane_lateral_position]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_lateral_position]) obj_pm->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::crp_id_begin]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::crp_id_begin]) obj_pm->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::crp_id_end]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::crp_id_end]) obj_pm->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::lane_vertical_position]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_vertical_position]) obj_pm->set_lane_vertical_position(iVal);
		strVal = stringUtil.getAnyString(vals[(int)OBJECT_INFO_POS_0_6_0::lane_id]);
		if (strVal != "0000000000000000") obj_pm->set_lane_id(DmUtil::getUnsignedLongFromString(strVal));
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::dx_lane]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dx_lane]) obj_pm->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::dy_lane]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dy_lane]) obj_pm->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::dh_lane]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::dh_lane]) obj_pm->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::semi_axis_length_major]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::semi_axis_length_major]) obj_pm->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::semi_axis_length_minor]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::semi_axis_length_minor]) obj_pm->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::ellipse_orientation]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::ellipse_orientation]) obj_pm->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::altitude_accuracy]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::altitude_accuracy]) obj_pm->set_altitude_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::reference_point]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::reference_point]) obj_pm->set_reference_point(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::move_direction_value]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::move_direction_value]) obj_pm->set_move_direction_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::move_direction_accuracy]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::move_direction_accuracy]) obj_pm->set_move_direction_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::speed_value]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::speed_value]) obj_pm->set_speed_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::speed_accuracy]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::speed_accuracy]) obj_pm->set_speed_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::yaw_rate_value]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::yaw_rate_value]) obj_pm->set_yaw_rate_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::yaw_rate_accuracy]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::yaw_rate_accuracy]) obj_pm->set_yaw_rate_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::acceleration_value]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::acceleration_value]) obj_pm->set_acceleration_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::acceleration_accuracy]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::acceleration_accuracy]) obj_pm->set_acceleration_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::orientation_value]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::orientation_value]) obj_vc->set_orientation_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::orientation_accuracy]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::orientation_accuracy]) obj_vc->set_orientation_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::length_value]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::length_value]) obj_vc->set_length_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::length_accuracy]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::length_accuracy]) obj_vc->set_length_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::width_value]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::width_value]) obj_vc->set_width_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::width_accuracy]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::width_accuracy]) obj_vc->set_width_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::height_value]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::height_value]) obj_vc->set_height_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::height_accuracy]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::height_accuracy]) obj_vc->set_height_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::color]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::color]) obj_vc->set_color(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::shift_position]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::shift_position]) obj_vc->set_shift_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::steering_angle_front]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::steering_angle_front]) obj_vc->set_steering_angle_front(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::steering_angle_rear]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::steering_angle_rear]) obj_vc->set_steering_angle_rear(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::brake_state]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::brake_state]) obj_vc->set_brake_state(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::auxiliary_brake_state]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::auxiliary_brake_state]) obj_vc->set_auxiliary_brake_state(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::throttle_position]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::throttle_position]) obj_vc->set_throttle_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::exterior_lights]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::exterior_lights]) obj_vc->set_exterior_lights(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::adaptive_cruise_control_system]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::adaptive_cruise_control_system]) obj_vc->set_adaptive_cruise_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::cooperative_adaptive_cruise_control_system]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::cooperative_adaptive_cruise_control_system]) obj_vc->set_cooperative_adaptive_cruise_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::pre_crash_safety_system]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::pre_crash_safety_system]) obj_vc->set_pre_crash_safety_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::antilock_brake_system]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::antilock_brake_system]) obj_vc->set_antilock_brake_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::traction_control_system]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::traction_control_system]) obj_vc->set_traction_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::electronic_stability_control_system]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::electronic_stability_control_system]) obj_vc->set_electronic_stability_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::lane_keeping_assist_system]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_keeping_assist_system]) obj_vc->set_lane_keeping_assist_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::lane_departure_warning_system]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::lane_departure_warning_system]) obj_vc->set_lane_departure_warning_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::vehicle_role]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::vehicle_role]) obj_vc->set_vehicle_role(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_6_0::vehicle_extended_info]); if (iVal != objectInfoUnknownVals_0_6_0[(int)OBJECT_INFO_POS_0_6_0::vehicle_extended_info]) obj_vc->set_vehicle_extended_info(iVal);
		strVal = stringUtil.getAnyString(vals[(int)OBJECT_INFO_POS_0_6_0::towing_vehicle]);
		if (strVal != "0000000000000000") obj_vc->set_towing_vehicle(DmUtil::getUnsignedLongFromString(strVal));
		{
			strVal = stringUtil.getAnyString(vals[(int)OBJECT_INFO_POS_0_6_0::information_source_list]);
			vector<string> vs = stringUtil.getVectorString(strVal);
			for (int i = 0; i < (int)vs.size(); i++) {
				if (vs[i].empty()) continue;
				obj->add_information_source_list(DmUtil::getUnsignedLongFromString(vs[i]));
			}
		}
		setIsTupleInfo(is_info, vals, (int)OBJECT_INFO_POS_0_6_0::information_source_list + 1);
		//debugObjectInfo(obj, is_info);
		return;
	}
	/**
	* 物標情報のセット (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	obj		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	tuple	タプル
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::setObjectInfo(cool4_api_0_8_0::Object_info_0_8_0 *obj, dm2_proto::Is_tuple_info *is_info, Tuple tuple)
	{
		int tuple_size = tuple.size();
		cool4_api_0_8_0::Position_and_movement* obj_pm = obj->mutable_position_and_movement();
		cool4_api_0_8_0::Vehicle_condition* obj_vc = obj->mutable_vehicle_condition();
		cool4_api_0_8_0::Tracking_information* obj_ti = obj->mutable_tracking_information();
		vector<any> vals(tuple_size);
		string strVal;
		long ts_first = 0;
		long ts_tmp = 0;
		int iVal;
		unsigned long long ullVal;
		for (int i = 0; i < tuple_size; i++) {
			tuple.getValue(i, vals[i], ts_tmp);
			if (i == 0) ts_first = ts_tmp;
		}
		is_info->set_dm2_create_ts(ts_first);
		obj->set_object_id(any_cast<unsigned long long>(vals[(int)OBJECT_INFO_POS_0_8_0::object_id]));
		obj->set_time(any_cast<long>(vals[(int)OBJECT_INFO_POS_0_8_0::time]));
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_id_0]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_id_0]) obj->set_object_class_id_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_0]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_0]) obj->set_object_class_confidence_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_0]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_0]) obj->set_object_class_subclass_type_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_0]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_0]) obj->set_object_class_subclass_confidence_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_id_1]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_id_1]) obj->set_object_class_id_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_1]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_1]) obj->set_object_class_confidence_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_1]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_1]) obj->set_object_class_subclass_type_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_1]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_1]) obj->set_object_class_subclass_confidence_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_id_2]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_id_2]) obj->set_object_class_id_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_2]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_2]) obj->set_object_class_confidence_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_2]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_2]) obj->set_object_class_subclass_type_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_2]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_2]) obj->set_object_class_subclass_confidence_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_id_3]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_id_3]) obj->set_object_class_id_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_3]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_confidence_3]) obj->set_object_class_confidence_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_3]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_type_3]) obj->set_object_class_subclass_type_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_3]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_class_subclass_confidence_3]) obj->set_object_class_subclass_confidence_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::existency]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::existency]) obj->set_existency(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::geodetic_srid]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::geodetic_srid]) obj_pm->set_geodetic_srid(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::latitude]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::latitude]) obj_pm->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::longitude]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::longitude]) obj_pm->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::altitude]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::altitude]) obj_pm->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::projective_srid]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::projective_srid]) obj_pm->set_projective_srid(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::prc_x]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::prc_x]) obj_pm->set_prc_x(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::prc_y]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::prc_y]) obj_pm->set_prc_y(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::prc_z]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::prc_z]) obj_pm->set_prc_z(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::crp_id]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::crp_id]) obj_pm->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::dx_crp]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dx_crp]) obj_pm->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::dy_crp]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dy_crp]) obj_pm->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::dh_crp]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dh_crp]) obj_pm->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::lane_count]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_count]) obj_pm->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::lane_position]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_position]) obj_pm->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::lane_lateral_position]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_lateral_position]) obj_pm->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::crp_id_begin]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::crp_id_begin]) obj_pm->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::crp_id_end]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::crp_id_end]) obj_pm->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::lane_vertical_position]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_vertical_position]) obj_pm->set_lane_vertical_position(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)OBJECT_INFO_POS_0_8_0::lane_id]); if (ullVal != (unsigned long long)objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_id]) obj_pm->set_lane_id(ullVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::dx_lane]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dx_lane]) obj_pm->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::dy_lane]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dy_lane]) obj_pm->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::dh_lane]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::dh_lane]) obj_pm->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::semi_axis_length_major]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::semi_axis_length_major]) obj_pm->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::semi_axis_length_minor]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::semi_axis_length_minor]) obj_pm->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::ellipse_orientation]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::ellipse_orientation]) obj_pm->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::altitude_accuracy]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::altitude_accuracy]) obj_pm->set_altitude_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::reference_point]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::reference_point]) obj_pm->set_reference_point(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::move_direction_value]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::move_direction_value]) obj_pm->set_move_direction_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::move_direction_accuracy]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::move_direction_accuracy]) obj_pm->set_move_direction_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::speed_value]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::speed_value]) obj_pm->set_speed_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::speed_accuracy]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::speed_accuracy]) obj_pm->set_speed_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::yaw_rate_value]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::yaw_rate_value]) obj_pm->set_yaw_rate_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::yaw_rate_accuracy]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::yaw_rate_accuracy]) obj_pm->set_yaw_rate_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::acceleration_value]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::acceleration_value]) obj_pm->set_acceleration_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::acceleration_accuracy]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::acceleration_accuracy]) obj_pm->set_acceleration_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::orientation_value]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::orientation_value]) obj_vc->set_orientation_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::orientation_accuracy]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::orientation_accuracy]) obj_vc->set_orientation_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::length_value]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::length_value]) obj_vc->set_length_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::length_accuracy]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::length_accuracy]) obj_vc->set_length_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::width_value]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::width_value]) obj_vc->set_width_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::width_accuracy]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::width_accuracy]) obj_vc->set_width_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::height_value]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::height_value]) obj_vc->set_height_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::height_accuracy]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::height_accuracy]) obj_vc->set_height_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::static_status]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::static_status]) obj_vc->set_static_status(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::shift_position]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::shift_position]) obj_vc->set_shift_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::steering_angle_front]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::steering_angle_front]) obj_vc->set_steering_angle_front(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::steering_angle_rear]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::steering_angle_rear]) obj_vc->set_steering_angle_rear(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::brake_state]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::brake_state]) obj_vc->set_brake_state(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::auxiliary_brake_state]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::auxiliary_brake_state]) obj_vc->set_auxiliary_brake_state(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::throttle_position]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::throttle_position]) obj_vc->set_throttle_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::exterior_lights]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::exterior_lights]) obj_vc->set_exterior_lights(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::adaptive_cruise_control_system]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::adaptive_cruise_control_system]) obj_vc->set_adaptive_cruise_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::cooperative_adaptive_cruise_control_system]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::cooperative_adaptive_cruise_control_system]) obj_vc->set_cooperative_adaptive_cruise_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::pre_crash_safety_system]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::pre_crash_safety_system]) obj_vc->set_pre_crash_safety_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::antilock_brake_system]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::antilock_brake_system]) obj_vc->set_antilock_brake_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::traction_control_system]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::traction_control_system]) obj_vc->set_traction_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::electronic_stability_control_system]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::electronic_stability_control_system]) obj_vc->set_electronic_stability_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::lane_keeping_assist_system]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_keeping_assist_system]) obj_vc->set_lane_keeping_assist_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::lane_departure_warning_system]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lane_departure_warning_system]) obj_vc->set_lane_departure_warning_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::vehicle_size_type]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::vehicle_size_type]) obj_vc->set_vehicle_size_type(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::vehicle_role]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::vehicle_role]) obj_vc->set_vehicle_role(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::vehicle_extended_info]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::vehicle_extended_info]) obj_vc->set_vehicle_extended_info(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)OBJECT_INFO_POS_0_8_0::towing_vehicle]); if (ullVal != (unsigned long long)objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::towing_vehicle]) obj_vc->set_towing_vehicle(ullVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::tracking_status]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::tracking_status]) obj_ti->set_tracking_status(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::detection_count]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::detection_count]) obj_ti->set_detection_count(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::lost_count]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::lost_count]) obj_ti->set_lost_count(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_0::object_age]); if (iVal != objectInfoUnknownVals_0_8_0[(int)OBJECT_INFO_POS_0_8_0::object_age]) obj_ti->set_object_age(iVal);
		try {
			vector<unsigned long long> vany =  any_cast<vector<unsigned long long>>(vals[(int)OBJECT_INFO_POS_0_8_0::information_source_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				obj->add_information_source_list(any_cast<unsigned long long>(vany[i]));
			}
		} catch (...) {
			vector<any> vany =  any_cast<vector<any>>(vals[(int)OBJECT_INFO_POS_0_8_0::information_source_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				obj->add_information_source_list(any_cast<unsigned long long>(vany[i]));
			}
		}
		setIsTupleInfo(is_info, vals, (int)OBJECT_INFO_POS_0_8_0::information_source_list + 1);
		//debugObjectInfo(obj, is_info);
		return;
	}
	/**
	* 物標情報のセット (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	obj		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	tuple	タプル
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::setObjectInfo(cool4_api_0_8_0::Object_info_0_8_1 *obj, dm2_proto::Is_tuple_info *is_info, Tuple tuple)
	{
		int tuple_size = tuple.size();
		cool4_api_0_8_0::Position_and_movement* obj_pm = obj->mutable_position_and_movement();
		cool4_api_0_8_0::Vehicle_condition* obj_vc = obj->mutable_vehicle_condition();
		cool4_api_0_8_0::Tracking_information* obj_ti = obj->mutable_tracking_information();
		vector<any> vals(tuple_size);
		string strVal;
		long ts_first = 0;
		long ts_tmp = 0;
		int iVal;
		unsigned long long ullVal;
		for (int i = 0; i < tuple_size; i++) {
			tuple.getValue(i, vals[i], ts_tmp);
			if (i == 0) ts_first = ts_tmp;
		}
		is_info->set_dm2_create_ts(ts_first);
		obj->set_object_id(any_cast<unsigned long long>(vals[(int)OBJECT_INFO_POS_0_8_1::object_id]));
		obj->set_time(any_cast<long>(vals[(int)OBJECT_INFO_POS_0_8_1::time]));
		obj->set_revision(any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::revision]));
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_id_0]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_id_0]) obj->set_object_class_id_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_0]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_0]) obj->set_object_class_confidence_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_0]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_0]) obj->set_object_class_subclass_type_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_0]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_0]) obj->set_object_class_subclass_confidence_0(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_id_1]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_id_1]) obj->set_object_class_id_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_1]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_1]) obj->set_object_class_confidence_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_1]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_1]) obj->set_object_class_subclass_type_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_1]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_1]) obj->set_object_class_subclass_confidence_1(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_id_2]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_id_2]) obj->set_object_class_id_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_2]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_2]) obj->set_object_class_confidence_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_2]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_2]) obj->set_object_class_subclass_type_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_2]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_2]) obj->set_object_class_subclass_confidence_2(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_id_3]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_id_3]) obj->set_object_class_id_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_3]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_confidence_3]) obj->set_object_class_confidence_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_3]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_type_3]) obj->set_object_class_subclass_type_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_3]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_class_subclass_confidence_3]) obj->set_object_class_subclass_confidence_3(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::existency]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::existency]) obj->set_existency(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::geodetic_srid]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::geodetic_srid]) obj_pm->set_geodetic_srid(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::latitude]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::latitude]) obj_pm->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::longitude]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::longitude]) obj_pm->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::altitude]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::altitude]) obj_pm->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::projective_srid]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::projective_srid]) obj_pm->set_projective_srid(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::prc_x]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::prc_x]) obj_pm->set_prc_x(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::prc_y]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::prc_y]) obj_pm->set_prc_y(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::prc_z]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::prc_z]) obj_pm->set_prc_z(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::crp_id]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::crp_id]) obj_pm->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::dx_crp]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dx_crp]) obj_pm->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::dy_crp]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dy_crp]) obj_pm->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::dh_crp]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dh_crp]) obj_pm->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::lane_count]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_count]) obj_pm->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::lane_position]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_position]) obj_pm->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::lane_lateral_position]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_lateral_position]) obj_pm->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::crp_id_begin]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::crp_id_begin]) obj_pm->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::crp_id_end]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::crp_id_end]) obj_pm->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::lane_vertical_position]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_vertical_position]) obj_pm->set_lane_vertical_position(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)OBJECT_INFO_POS_0_8_1::lane_id]); if (ullVal != (unsigned long long)objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_id]) obj_pm->set_lane_id(ullVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::dx_lane]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dx_lane]) obj_pm->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::dy_lane]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dy_lane]) obj_pm->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::dh_lane]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::dh_lane]) obj_pm->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::semi_axis_length_major]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::semi_axis_length_major]) obj_pm->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::semi_axis_length_minor]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::semi_axis_length_minor]) obj_pm->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::ellipse_orientation]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::ellipse_orientation]) obj_pm->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::altitude_accuracy]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::altitude_accuracy]) obj_pm->set_altitude_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::reference_point]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::reference_point]) obj_pm->set_reference_point(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::move_direction_value]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::move_direction_value]) obj_pm->set_move_direction_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::move_direction_accuracy]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::move_direction_accuracy]) obj_pm->set_move_direction_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::speed_value]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::speed_value]) obj_pm->set_speed_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::speed_accuracy]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::speed_accuracy]) obj_pm->set_speed_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::yaw_rate_value]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::yaw_rate_value]) obj_pm->set_yaw_rate_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::yaw_rate_accuracy]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::yaw_rate_accuracy]) obj_pm->set_yaw_rate_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::acceleration_value]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::acceleration_value]) obj_pm->set_acceleration_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::acceleration_accuracy]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::acceleration_accuracy]) obj_pm->set_acceleration_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::orientation_value]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::orientation_value]) obj_vc->set_orientation_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::orientation_accuracy]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::orientation_accuracy]) obj_vc->set_orientation_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::length_value]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::length_value]) obj_vc->set_length_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::length_accuracy]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::length_accuracy]) obj_vc->set_length_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::width_value]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::width_value]) obj_vc->set_width_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::width_accuracy]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::width_accuracy]) obj_vc->set_width_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::height_value]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::height_value]) obj_vc->set_height_value(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::height_accuracy]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::height_accuracy]) obj_vc->set_height_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::static_status]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::static_status]) obj_vc->set_static_status(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::shift_position]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::shift_position]) obj_vc->set_shift_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::steering_angle_front]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::steering_angle_front]) obj_vc->set_steering_angle_front(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::steering_angle_rear]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::steering_angle_rear]) obj_vc->set_steering_angle_rear(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::brake_state]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::brake_state]) obj_vc->set_brake_state(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::auxiliary_brake_state]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::auxiliary_brake_state]) obj_vc->set_auxiliary_brake_state(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::throttle_position]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::throttle_position]) obj_vc->set_throttle_position(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::exterior_lights]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::exterior_lights]) obj_vc->set_exterior_lights(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::adaptive_cruise_control_system]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::adaptive_cruise_control_system]) obj_vc->set_adaptive_cruise_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::cooperative_adaptive_cruise_control_system]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::cooperative_adaptive_cruise_control_system]) obj_vc->set_cooperative_adaptive_cruise_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::pre_crash_safety_system]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::pre_crash_safety_system]) obj_vc->set_pre_crash_safety_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::antilock_brake_system]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::antilock_brake_system]) obj_vc->set_antilock_brake_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::traction_control_system]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::traction_control_system]) obj_vc->set_traction_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::electronic_stability_control_system]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::electronic_stability_control_system]) obj_vc->set_electronic_stability_control_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::lane_keeping_assist_system]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_keeping_assist_system]) obj_vc->set_lane_keeping_assist_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::lane_departure_warning_system]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lane_departure_warning_system]) obj_vc->set_lane_departure_warning_system(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::vehicle_size_type]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::vehicle_size_type]) obj_vc->set_vehicle_size_type(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::vehicle_role]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::vehicle_role]) obj_vc->set_vehicle_role(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::vehicle_extended_info]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::vehicle_extended_info]) obj_vc->set_vehicle_extended_info(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)OBJECT_INFO_POS_0_8_1::towing_vehicle]); if (ullVal != (unsigned long long)objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::towing_vehicle]) obj_vc->set_towing_vehicle(ullVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::tracking_status]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::tracking_status]) obj_ti->set_tracking_status(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::detection_count]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::detection_count]) obj_ti->set_detection_count(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::lost_count]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::lost_count]) obj_ti->set_lost_count(iVal);
		iVal = any_cast<int>(vals[(int)OBJECT_INFO_POS_0_8_1::object_age]); if (iVal != objectInfoUnknownVals_0_8_1[(int)OBJECT_INFO_POS_0_8_1::object_age]) obj_ti->set_object_age(iVal);
		try {
			vector<unsigned long long> vany =  any_cast<vector<unsigned long long>>(vals[(int)OBJECT_INFO_POS_0_8_1::information_source_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				obj->add_information_source_list(any_cast<unsigned long long>(vany[i]));
			}
		} catch (...) {
			vector<any> vany =  any_cast<vector<any>>(vals[(int)OBJECT_INFO_POS_0_8_1::information_source_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				obj->add_information_source_list(any_cast<unsigned long long>(vany[i]));
			}
		}
		setIsTupleInfo(is_info, vals, (int)OBJECT_INFO_POS_0_8_1::information_source_list + 1);
		//debugObjectInfo(obj, is_info);
		return;
	}
	/**
	* フリースペース情報のセット (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	fre		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	tuple	タプル
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::setFreespaceInfo(cool4_api_0_6_0::Freespace_info *fre, dm2_proto::Is_tuple_info *is_info, Tuple tuple)
	{
		int tuple_size = tuple.size();
		cool4_api_0_6_0::Location* pos_begin = fre->mutable_position_begin();
		cool4_api_0_6_0::Location* pos_end = fre->mutable_position_end();
		vector<any> vals(tuple_size);
		string strVal;
		long ts_first = 0;
		long ts_tmp = 0;
		int iVal;
		for (int i = 0; i < tuple_size; i++) {
			tuple.getValue(i, vals[i], ts_tmp);
			if (i == 0) ts_first = ts_tmp;
		}
		is_info->set_dm2_create_ts(ts_first);
		
		strVal = stringUtil.getAnyString(vals[(int)FREESPACE_INFO_POS_0_6_0::id]);
		fre->set_id(DmUtil::getUnsignedLongFromString(strVal));
		fre->set_time(any_cast<long>(vals[(int)FREESPACE_INFO_POS_0_6_0::time]));
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::existency]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::existency]) fre->set_existency(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::minimal_detectable_size]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::minimal_detectable_size]) fre->set_minimal_detectable_size(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_geodetic_system]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_geodetic_system]) pos_begin->set_geodetic_system(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_latitude]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_latitude]) pos_begin->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_longitude]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_longitude]) pos_begin->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_altitude]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_altitude]) pos_begin->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id]) pos_begin->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dx_crp]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dx_crp]) pos_begin->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dy_crp]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dy_crp]) pos_begin->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dh_crp]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dh_crp]) pos_begin->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_count]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_count]) pos_begin->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_position]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_position]) pos_begin->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_lateral_position]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_lateral_position]) pos_begin->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id_begin]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id_begin]) pos_begin->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id_end]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_crp_id_end]) pos_begin->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_vertical_position]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_vertical_position]) pos_begin->set_lane_vertical_position(iVal);
		strVal = stringUtil.getAnyString(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_lane_id]);
		if (strVal != "0000000000000000") pos_begin->set_lane_id(DmUtil::getUnsignedLongFromString(strVal));
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dx_lane]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dx_lane]) pos_begin->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dy_lane]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dy_lane]) pos_begin->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dh_lane]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_dh_lane]) pos_begin->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_semi_axis_length_major]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_semi_axis_length_major]) pos_begin->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_semi_axis_length_minor]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_semi_axis_length_minor]) pos_begin->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_ellipse_orientation]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_ellipse_orientation]) pos_begin->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_begin_altitude_accuracy]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_begin_altitude_accuracy]) pos_begin->set_altitude_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_geodetic_system]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_geodetic_system]) pos_end->set_geodetic_system(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_latitude]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_latitude]) pos_end->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_longitude]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_longitude]) pos_end->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_altitude]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_altitude]) pos_end->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id]) pos_end->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_dx_crp]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dx_crp]) pos_end->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_dy_crp]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dy_crp]) pos_end->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_dh_crp]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dh_crp]) pos_end->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_count]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_count]) pos_end->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_position]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_position]) pos_end->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_lateral_position]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_lateral_position]) pos_end->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id_begin]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id_begin]) pos_end->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id_end]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_crp_id_end]) pos_end->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_vertical_position]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_vertical_position]) pos_end->set_lane_vertical_position(iVal);
		strVal = stringUtil.getAnyString(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_lane_id]);
		if (strVal != "0000000000000000") pos_end->set_lane_id(DmUtil::getUnsignedLongFromString(strVal));
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_dx_lane]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dx_lane]) pos_end->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_dy_lane]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dy_lane]) pos_end->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_dh_lane]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_dh_lane]) pos_end->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_semi_axis_length_major]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_semi_axis_length_major]) pos_end->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_semi_axis_length_minor]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_semi_axis_length_minor]) pos_end->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_ellipse_orientation]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_ellipse_orientation]) pos_end->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::position_end_altitude_accuracy]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::position_end_altitude_accuracy]) pos_end->set_altitude_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::length_value]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::length_value]) fre->set_length_value(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_6_0::length_accuracy]); if (iVal != freespaceInfoUnknownVals_0_6_0[(int)FREESPACE_INFO_POS_0_6_0::length_accuracy]) fre->set_length_accuracy(iVal);
		strVal = stringUtil.getAnyString(vals[(int)FREESPACE_INFO_POS_0_6_0::id_begin]);
		if (strVal != "0000000000000000") fre->set_id_begin(DmUtil::getUnsignedLongFromString(strVal));
		strVal = stringUtil.getAnyString(vals[(int)FREESPACE_INFO_POS_0_6_0::id_end]);
		if (strVal != "0000000000000000") fre->set_id_end(DmUtil::getUnsignedLongFromString(strVal));
		{
			strVal = stringUtil.getAnyString(vals[(int)FREESPACE_INFO_POS_0_6_0::information_source_list]);
			vector<string> vs = stringUtil.getVectorString(strVal);
			for (int i = 0; i < (int)vs.size(); i++) {
				if (vs[i].empty()) continue;
				fre->add_information_source_list(DmUtil::getUnsignedLongFromString(vs[i]));
			}
		}
		setIsTupleInfo(is_info, vals, (int)FREESPACE_INFO_POS_0_6_0::information_source_list + 1);
		//debugFreespaceInfo(fre, is_info);
		return;
	}
	/**
	* フリースペース情報のセット (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/05/10
	*
	* @param [in]	fre		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	tuple	タプル
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::setFreespaceInfo(cool4_api_0_8_0::Freespace_info_0_8_0 *fre, dm2_proto::Is_tuple_info *is_info, Tuple tuple)
	{
		int tuple_size = tuple.size();
		cool4_api_0_8_0::Location* vertices_begin = fre->mutable_vertices_begin();
		cool4_api_0_8_0::Location* pos_begin = fre->mutable_position_begin();
		cool4_api_0_8_0::Location* pos_end = fre->mutable_position_end();
		vector<any> vals(tuple_size);
		string strVal;
		long ts_first = 0;
		long ts_tmp = 0;
		int iVal;
		unsigned long long ullVal;
		for (int i = 0; i < tuple_size; i++) {
			tuple.getValue(i, vals[i], ts_tmp);
			if (i == 0) ts_first = ts_tmp;
		}
		is_info->set_dm2_create_ts(ts_first);
		fre->set_id(any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_0::id]));
		fre->set_time(any_cast<long>(vals[(int)FREESPACE_INFO_POS_0_8_0::time]));
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::detection_method]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::detection_method]) fre->set_detection_method(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::detactable_classes]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::detactable_classes]) fre->set_detactable_classes(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_geodetic_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_geodetic_srid]) vertices_begin->set_geodetic_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_latitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_latitude]) vertices_begin->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_longitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_longitude]) vertices_begin->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_altitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_altitude]) vertices_begin->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_projective_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_projective_srid]) vertices_begin->set_projective_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_x]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_x]) vertices_begin->set_prc_x(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_y]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_y]) vertices_begin->set_prc_y(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_z]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_prc_z]) vertices_begin->set_prc_z(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id]) vertices_begin->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dx_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dx_crp]) vertices_begin->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dy_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dy_crp]) vertices_begin->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dh_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dh_crp]) vertices_begin->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_count]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_count]) vertices_begin->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_position]) vertices_begin->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_lateral_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_lateral_position]) vertices_begin->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id_begin]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id_begin]) vertices_begin->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id_end]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_crp_id_end]) vertices_begin->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_vertical_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_vertical_position]) vertices_begin->set_lane_vertical_position(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_id]); if (ullVal != (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_lane_id]) vertices_begin->set_lane_id(ullVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dx_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dx_lane]) vertices_begin->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dy_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dy_lane]) vertices_begin->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dh_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_dh_lane]) vertices_begin->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_semi_axis_length_major]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_semi_axis_length_major]) vertices_begin->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_semi_axis_length_minor]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_semi_axis_length_minor]) vertices_begin->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_ellipse_orientation]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_ellipse_orientation]) vertices_begin->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_altitude_accuracy]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::vertices_begin_altitude_accuracy]) vertices_begin->set_altitude_accuracy(iVal);
		try {
			vector<int> vany =  any_cast<vector<int>>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_dx_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_vertices_dx_list(any_cast<int>(vany[i]));
			}
		} catch (...) {
			vector<any> vany =  any_cast<vector<any>>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_dx_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_vertices_dx_list(any_cast<int>(vany[i]));
			}
		}
		try {
			vector<int> vany =  any_cast<vector<int>>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_dy_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_vertices_dy_list(any_cast<int>(vany[i]));
			}
		} catch (...) {
			vector<any> vany =  any_cast<vector<any>>(vals[(int)FREESPACE_INFO_POS_0_8_0::vertices_dy_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_vertices_dy_list(any_cast<int>(vany[i]));
			}
		}
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_geodetic_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_geodetic_srid]) pos_begin->set_geodetic_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_latitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_latitude]) pos_begin->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_longitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_longitude]) pos_begin->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_altitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_altitude]) pos_begin->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_projective_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_projective_srid]) pos_begin->set_projective_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_x]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_x]) pos_begin->set_prc_x(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_y]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_y]) pos_begin->set_prc_y(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_z]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_prc_z]) pos_begin->set_prc_z(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id]) pos_begin->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dx_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dx_crp]) pos_begin->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dy_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dy_crp]) pos_begin->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dh_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dh_crp]) pos_begin->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_count]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_count]) pos_begin->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_position]) pos_begin->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_lateral_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_lateral_position]) pos_begin->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id_begin]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id_begin]) pos_begin->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id_end]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_crp_id_end]) pos_begin->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_vertical_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_vertical_position]) pos_begin->set_lane_vertical_position(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_id]); if (ullVal != (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_lane_id]) pos_begin->set_lane_id(ullVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dx_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dx_lane]) pos_begin->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dy_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dy_lane]) pos_begin->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dh_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_dh_lane]) pos_begin->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_semi_axis_length_major]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_semi_axis_length_major]) pos_begin->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_semi_axis_length_minor]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_semi_axis_length_minor]) pos_begin->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_ellipse_orientation]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_ellipse_orientation]) pos_begin->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_begin_altitude_accuracy]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_begin_altitude_accuracy]) pos_begin->set_altitude_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_geodetic_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_geodetic_srid]) pos_end->set_geodetic_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_latitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_latitude]) pos_end->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_longitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_longitude]) pos_end->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_altitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_altitude]) pos_end->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_projective_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_projective_srid]) pos_end->set_projective_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_prc_x]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_prc_x]) pos_end->set_prc_x(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_prc_y]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_prc_y]) pos_end->set_prc_y(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_prc_z]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_prc_z]) pos_end->set_prc_z(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id]) pos_end->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_dx_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dx_crp]) pos_end->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_dy_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dy_crp]) pos_end->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_dh_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dh_crp]) pos_end->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_count]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_count]) pos_end->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_position]) pos_end->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_lateral_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_lateral_position]) pos_end->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id_begin]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id_begin]) pos_end->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id_end]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_crp_id_end]) pos_end->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_vertical_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_vertical_position]) pos_end->set_lane_vertical_position(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_id]); if (ullVal != (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_lane_id]) pos_end->set_lane_id(ullVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_dx_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dx_lane]) pos_end->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_dy_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dy_lane]) pos_end->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_dh_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_dh_lane]) pos_end->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_semi_axis_length_major]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_semi_axis_length_major]) pos_end->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_semi_axis_length_minor]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_semi_axis_length_minor]) pos_end->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_ellipse_orientation]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_ellipse_orientation]) pos_end->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::position_end_altitude_accuracy]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::position_end_altitude_accuracy]) pos_end->set_altitude_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::length_value]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::length_value]) fre->set_length_value(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::length_accuracy]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::length_accuracy]) fre->set_length_accuracy(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_0::id_begin]); if (ullVal != (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::id_begin]) fre->set_id_begin(ullVal);
		ullVal = any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_0::id_end]); if (ullVal != (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::id_end]) fre->set_id_end(ullVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::existency]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::existency]) fre->set_existency(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_0::minimal_detectable_size]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_0::minimal_detectable_size]) fre->set_minimal_detectable_size(iVal);
		try {
			vector<unsigned long long> vany =  any_cast<vector<unsigned long long>>(vals[(int)FREESPACE_INFO_POS_0_8_0::information_source_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_information_source_list(any_cast<unsigned long long>(vany[i]));
			}
		} catch (...) {
			vector<any> vany =  any_cast<vector<any>>(vals[(int)FREESPACE_INFO_POS_0_8_0::information_source_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_information_source_list(any_cast<unsigned long long>(vany[i]));
			}
		}
		setIsTupleInfo(is_info, vals, (int)FREESPACE_INFO_POS_0_8_0::information_source_list + 1);
		//debugFreespaceInfo(fre, is_info);
		return;
	}
	/**
	* フリースペース情報のセット (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/05/10
	*
	* @param [in]	fre		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	tuple	タプル
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::setFreespaceInfo(cool4_api_0_8_0::Freespace_info_0_8_1 *fre, dm2_proto::Is_tuple_info *is_info, Tuple tuple)
	{
		int tuple_size = tuple.size();
		cool4_api_0_8_0::Location* vertices_begin = fre->mutable_vertices_begin();
		cool4_api_0_8_0::Location* pos_begin = fre->mutable_position_begin();
		cool4_api_0_8_0::Location* pos_end = fre->mutable_position_end();
		vector<any> vals(tuple_size);
		string strVal;
		long ts_first = 0;
		long ts_tmp = 0;
		int iVal;
		unsigned long long ullVal;
		for (int i = 0; i < tuple_size; i++) {
			tuple.getValue(i, vals[i], ts_tmp);
			if (i == 0) ts_first = ts_tmp;
		}
		is_info->set_dm2_create_ts(ts_first);
		fre->set_id(any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_1::id]));
		fre->set_time(any_cast<long>(vals[(int)FREESPACE_INFO_POS_0_8_1::time]));
		fre->set_revision(any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::revision]));
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::detection_method]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::detection_method]) fre->set_detection_method(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::detactable_classes]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::detactable_classes]) fre->set_detactable_classes(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_geodetic_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_geodetic_srid]) vertices_begin->set_geodetic_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_latitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_latitude]) vertices_begin->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_longitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_longitude]) vertices_begin->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_altitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_altitude]) vertices_begin->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_projective_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_projective_srid]) vertices_begin->set_projective_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_x]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_x]) vertices_begin->set_prc_x(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_y]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_y]) vertices_begin->set_prc_y(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_z]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_prc_z]) vertices_begin->set_prc_z(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id]) vertices_begin->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dx_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dx_crp]) vertices_begin->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dy_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dy_crp]) vertices_begin->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dh_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dh_crp]) vertices_begin->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_count]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_count]) vertices_begin->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_position]) vertices_begin->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_lateral_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_lateral_position]) vertices_begin->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id_begin]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id_begin]) vertices_begin->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id_end]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_crp_id_end]) vertices_begin->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_vertical_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_vertical_position]) vertices_begin->set_lane_vertical_position(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_id]); if (ullVal != (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_lane_id]) vertices_begin->set_lane_id(ullVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dx_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dx_lane]) vertices_begin->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dy_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dy_lane]) vertices_begin->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dh_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_dh_lane]) vertices_begin->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_semi_axis_length_major]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_semi_axis_length_major]) vertices_begin->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_semi_axis_length_minor]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_semi_axis_length_minor]) vertices_begin->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_ellipse_orientation]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_ellipse_orientation]) vertices_begin->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_altitude_accuracy]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::vertices_begin_altitude_accuracy]) vertices_begin->set_altitude_accuracy(iVal);
		try {
			vector<int> vany =  any_cast<vector<int>>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_dx_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_vertices_dx_list(any_cast<int>(vany[i]));
			}
		} catch (...) {
			vector<any> vany =  any_cast<vector<any>>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_dx_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_vertices_dx_list(any_cast<int>(vany[i]));
			}
		}
		try {
			vector<int> vany =  any_cast<vector<int>>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_dy_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_vertices_dy_list(any_cast<int>(vany[i]));
			}
		} catch (...) {
			vector<any> vany =  any_cast<vector<any>>(vals[(int)FREESPACE_INFO_POS_0_8_1::vertices_dy_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_vertices_dy_list(any_cast<int>(vany[i]));
			}
		}
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_geodetic_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_geodetic_srid]) pos_begin->set_geodetic_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_latitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_latitude]) pos_begin->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_longitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_longitude]) pos_begin->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_altitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_altitude]) pos_begin->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_projective_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_projective_srid]) pos_begin->set_projective_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_x]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_x]) pos_begin->set_prc_x(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_y]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_y]) pos_begin->set_prc_y(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_z]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_prc_z]) pos_begin->set_prc_z(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id]) pos_begin->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dx_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dx_crp]) pos_begin->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dy_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dy_crp]) pos_begin->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dh_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dh_crp]) pos_begin->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_count]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_count]) pos_begin->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_position]) pos_begin->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_lateral_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_lateral_position]) pos_begin->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id_begin]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id_begin]) pos_begin->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id_end]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_crp_id_end]) pos_begin->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_vertical_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_vertical_position]) pos_begin->set_lane_vertical_position(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_id]); if (ullVal != (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_lane_id]) pos_begin->set_lane_id(ullVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dx_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dx_lane]) pos_begin->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dy_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dy_lane]) pos_begin->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dh_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_dh_lane]) pos_begin->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_semi_axis_length_major]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_semi_axis_length_major]) pos_begin->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_semi_axis_length_minor]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_semi_axis_length_minor]) pos_begin->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_ellipse_orientation]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_ellipse_orientation]) pos_begin->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_begin_altitude_accuracy]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_begin_altitude_accuracy]) pos_begin->set_altitude_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_geodetic_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_geodetic_srid]) pos_end->set_geodetic_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_latitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_latitude]) pos_end->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_longitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_longitude]) pos_end->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_altitude]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_altitude]) pos_end->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_projective_srid]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_projective_srid]) pos_end->set_projective_srid(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_prc_x]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_prc_x]) pos_end->set_prc_x(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_prc_y]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_prc_y]) pos_end->set_prc_y(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_prc_z]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_prc_z]) pos_end->set_prc_z(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id]) pos_end->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_dx_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dx_crp]) pos_end->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_dy_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dy_crp]) pos_end->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_dh_crp]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dh_crp]) pos_end->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_count]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_count]) pos_end->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_position]) pos_end->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_lateral_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_lateral_position]) pos_end->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id_begin]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id_begin]) pos_end->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id_end]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_crp_id_end]) pos_end->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_vertical_position]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_vertical_position]) pos_end->set_lane_vertical_position(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_id]); if (ullVal != (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_lane_id]) pos_end->set_lane_id(ullVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_dx_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dx_lane]) pos_end->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_dy_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dy_lane]) pos_end->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_dh_lane]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_dh_lane]) pos_end->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_semi_axis_length_major]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_semi_axis_length_major]) pos_end->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_semi_axis_length_minor]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_semi_axis_length_minor]) pos_end->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_ellipse_orientation]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_ellipse_orientation]) pos_end->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::position_end_altitude_accuracy]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::position_end_altitude_accuracy]) pos_end->set_altitude_accuracy(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::length_value]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::length_value]) fre->set_length_value(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::length_accuracy]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::length_accuracy]) fre->set_length_accuracy(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_1::id_begin]); if (ullVal != (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::id_begin]) fre->set_id_begin(ullVal);
		ullVal = any_cast<unsigned long long>(vals[(int)FREESPACE_INFO_POS_0_8_1::id_end]); if (ullVal != (unsigned long long)freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::id_end]) fre->set_id_end(ullVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::existency]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::existency]) fre->set_existency(iVal);
		iVal = any_cast<int>(vals[(int)FREESPACE_INFO_POS_0_8_1::minimal_detectable_size]); if (iVal != freespaceInfoUnknownVals_0_8_0[(int)FREESPACE_INFO_POS_0_8_1::minimal_detectable_size]) fre->set_minimal_detectable_size(iVal);
		try {
			vector<unsigned long long> vany =  any_cast<vector<unsigned long long>>(vals[(int)FREESPACE_INFO_POS_0_8_1::information_source_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_information_source_list(any_cast<unsigned long long>(vany[i]));
			}
		} catch (...) {
			vector<any> vany =  any_cast<vector<any>>(vals[(int)FREESPACE_INFO_POS_0_8_1::information_source_list]);
			for (int i = 0; i < (int)vany.size(); i++) {
				fre->add_information_source_list(any_cast<unsigned long long>(vany[i]));
			}
		}
		setIsTupleInfo(is_info, vals, (int)FREESPACE_INFO_POS_0_8_1::information_source_list + 1);
		//debugFreespaceInfo(fre, is_info);
		return;
	}
	/**
	* 信号情報のセット (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	sig		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	tuple	タプル
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::setSignalInfo(cool4_api_0_6_0::Signal_info *sig, dm2_proto::Is_tuple_info *is_info, Tuple tuple)
	{
		int tuple_size = tuple.size();
		vector<any> vals(tuple_size);
		string strVal;
		long ts_first = 0;
		long ts_tmp = 0;
		int iVal;
		for (int i = 0; i < tuple_size; i++) {
			tuple.getValue(i, vals[i], ts_tmp);
			if (i == 0) ts_first = ts_tmp;
		}
		is_info->set_dm2_create_ts(ts_first);
		
		sig->set_crp_id(any_cast<int>(vals[(int)SIGNAL_INFO_POS_0_6_0::crp_id]));
		try {
			vector<int> va = any_cast<vector<int>>(vals[(int)SIGNAL_INFO_POS_0_6_0::id]);
			for (int i = 0; i < (int)va.size(); i++) { 
				sig->add_id(any_cast<int>(va[i]));
			}
		} catch (...) {
			vector<any> va = any_cast<vector<any>>(vals[(int)SIGNAL_INFO_POS_0_6_0::id]);
			for (int i = 0; i < (int)va.size(); i++) { 
				sig->add_id(any_cast<int>(va[i]));
			}
		}
		sig->set_time(any_cast<long>(vals[(int)SIGNAL_INFO_POS_0_6_0::time]));
		iVal = any_cast<int>(vals[(int)SIGNAL_INFO_POS_0_6_0::state]); if (iVal != signalInfoUnknownVals_0_6_0[(int)SIGNAL_INFO_POS_0_6_0::state]) sig->set_state(iVal);
		iVal = any_cast<int>(vals[(int)SIGNAL_INFO_POS_0_6_0::specific_control_flags]); if (iVal != signalInfoUnknownVals_0_6_0[(int)SIGNAL_INFO_POS_0_6_0::specific_control_flags]) sig->set_specific_control_flags(iVal);
		iVal = any_cast<int>(vals[(int)SIGNAL_INFO_POS_0_6_0::event_count]); if (iVal != signalInfoUnknownVals_0_6_0[(int)SIGNAL_INFO_POS_0_6_0::event_count]) sig->set_event_count(iVal);
		iVal = any_cast<int>(vals[(int)SIGNAL_INFO_POS_0_6_0::count_down_stop_flag]); if (iVal != signalInfoUnknownVals_0_6_0[(int)SIGNAL_INFO_POS_0_6_0::count_down_stop_flag]) sig->set_count_down_stop_flag(iVal);
		int lig_pos = (int)SIGNAL_INFO_POS_0_6_0::light_info_main_0;
		for (int i = 0; i < 12; i++) {
			cool4_api_0_6_0::Light_info* lig = sig->add_light_info();
			iVal = any_cast<int>(vals[lig_pos]); if (iVal != signalInfoUnknownVals_0_6_0[lig_pos]) lig->set_main(iVal);
			lig_pos++;
			iVal = any_cast<int>(vals[lig_pos]); if (iVal != signalInfoUnknownVals_0_6_0[lig_pos]) lig->set_arrow(iVal);
			lig_pos++;
			iVal = any_cast<int>(vals[lig_pos]); if (iVal != signalInfoUnknownVals_0_6_0[lig_pos]) lig->set_min_time_to_change(iVal);
			lig_pos++;
			iVal = any_cast<int>(vals[lig_pos]); if (iVal != signalInfoUnknownVals_0_6_0[lig_pos]) lig->set_max_time_to_change(iVal);
			lig_pos++;
		}
		setIsTupleInfo(is_info, vals, lig_pos);
		//debugSignalInfo(sig, is_info);
		return;
	}
	/**
	* センサー情報のセット (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	obj		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	tuple	タプル
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::setSensorInfo(cool4_api_0_8_0::Sensor_info_0_8_0 *sen, dm2_proto::Is_tuple_info *is_info, Tuple tuple)
	{
		int tuple_size = tuple.size();
		cool4_api_0_8_0::Position* sen_po = sen->mutable_position();
		vector<any> vals(tuple_size);
		string strVal;
		long ts_first = 0;
		long ts_tmp = 0;
		int iVal;
		unsigned long long ullVal;
		for (int i = 0; i < tuple_size; i++) {
			tuple.getValue(i, vals[i], ts_tmp);
			if (i == 0) ts_first = ts_tmp;
		}
		is_info->set_dm2_create_ts(ts_first);
		sen->set_unit_id(any_cast<unsigned long long>(vals[(int)SENSOR_INFO_POS_0_8_0::unit_id]));
		sen->set_sensor_id(any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::sensor_id]));
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::sensor_type]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::sensor_type]) sen->set_sensor_type(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::geodetic_srid]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::geodetic_srid]) sen_po->set_geodetic_srid(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::latitude]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::latitude]) sen_po->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::longitude]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::longitude]) sen_po->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::altitude]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::altitude]) sen_po->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::projective_srid]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::projective_srid]) sen_po->set_projective_srid(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::prc_x]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::prc_x]) sen_po->set_prc_x(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::prc_y]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::prc_y]) sen_po->set_prc_y(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::prc_z]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::prc_z]) sen_po->set_prc_z(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::crp_id]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::crp_id]) sen_po->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::dx_crp]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dx_crp]) sen_po->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::dy_crp]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dy_crp]) sen_po->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::dh_crp]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dh_crp]) sen_po->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::lane_count]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::lane_count]) sen_po->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::lane_position]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::lane_position]) sen_po->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::lane_lateral_position]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::lane_lateral_position]) sen_po->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::crp_id_begin]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::crp_id_begin]) sen_po->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::crp_id_end]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::crp_id_end]) sen_po->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::lane_vertical_position]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::lane_vertical_position]) sen_po->set_lane_vertical_position(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)SENSOR_INFO_POS_0_8_0::lane_id]); if (ullVal != (unsigned long long)sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::lane_id]) sen_po->set_lane_id(ullVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::dx_lane]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dx_lane]) sen_po->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::dy_lane]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dy_lane]) sen_po->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::dh_lane]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::dh_lane]) sen_po->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::semi_axis_length_major]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::semi_axis_length_major]) sen_po->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::semi_axis_length_minor]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::semi_axis_length_minor]) sen_po->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::ellipse_orientation]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::ellipse_orientation]) sen_po->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::altitude_accuracy]); if (iVal != sensorInfoUnknownVals_0_8_0[(int)SENSOR_INFO_POS_0_8_0::altitude_accuracy]) sen_po->set_altitude_accuracy(iVal);
		
		int dc_pos = (int)SENSOR_INFO_POS_0_8_0::detectable_classes_0;
		for (int i = 0; i < 8; i++) {
			cool4_api_0_8_0::Detectable_capabilities* dc = sen->add_detectable_capabilities();
			iVal = any_cast<int>(vals[dc_pos]); if (iVal != sensorInfoUnknownVals_0_8_0[dc_pos]) dc->set_detectable_classes(iVal);
			dc_pos++;
			try {
				vector<int> vi =  any_cast<vector<int>>(vals[dc_pos]);
				for (int i = 0; i < (int)vi.size(); i++) {
					dc->add_detectable_domain_x_list(any_cast<int>(vi[i]));
				}
			} catch (...) {
				vector<any> vany =  any_cast<vector<any>>(vals[dc_pos]);
				for (int i = 0; i < (int)vany.size(); i++) {
					dc->add_detectable_domain_x_list(any_cast<int>(vany[i]));
				}
			}
			dc_pos++;
			try {
				vector<int> vi =  any_cast<vector<int>>(vals[dc_pos]);
				for (int i = 0; i < (int)vi.size(); i++) {
					dc->add_detectable_domain_y_list(any_cast<int>(vi[i]));
				}
			} catch (...) {
				vector<any> vany =  any_cast<vector<any>>(vals[dc_pos]);
				for (int i = 0; i < (int)vany.size(); i++) {
					dc->add_detectable_domain_y_list(any_cast<int>(vany[i]));
				}
			}
			dc_pos++;
			iVal = any_cast<int>(vals[dc_pos]); if (iVal != sensorInfoUnknownVals_0_8_0[dc_pos]) dc->set_confidence(iVal);
			dc_pos++;
			iVal = any_cast<int>(vals[dc_pos]); if (iVal != sensorInfoUnknownVals_0_8_0[dc_pos]) dc->set_minimal_detectable_size(iVal);
			dc_pos++;
		}
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_0::sensor_status]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_0::sensor_status]) sen->set_sensor_status(iVal);
		setIsTupleInfo(is_info, vals, (int)SENSOR_INFO_POS_0_8_0::sensor_status + 1);
		//debugSensorInfo(sen, is_info);
		return;
	}
	/**
	* センサー情報のセット (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	obj		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	tuple	タプル
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::setSensorInfo(cool4_api_0_8_0::Sensor_info_0_8_1 *sen, dm2_proto::Is_tuple_info *is_info, Tuple tuple)
	{
		int tuple_size = tuple.size();
		cool4_api_0_8_0::Position* sen_po = sen->mutable_position();
		vector<any> vals(tuple_size);
		string strVal;
		long ts_first = 0;
		long ts_tmp = 0;
		int iVal;
		unsigned long long ullVal;
		for (int i = 0; i < tuple_size; i++) {
			tuple.getValue(i, vals[i], ts_tmp);
			if (i == 0) ts_first = ts_tmp;
		}
		is_info->set_dm2_create_ts(ts_first);
		sen->set_unit_id(any_cast<unsigned long long>(vals[(int)SENSOR_INFO_POS_0_8_1::unit_id]));
		sen->set_sensor_id(any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::sensor_id]));
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::sensor_type]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::sensor_type]) sen->set_sensor_type(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::geodetic_srid]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::geodetic_srid]) sen_po->set_geodetic_srid(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::latitude]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::latitude]) sen_po->set_latitude(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::longitude]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::longitude]) sen_po->set_longitude(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::altitude]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::altitude]) sen_po->set_altitude(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::projective_srid]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::projective_srid]) sen_po->set_projective_srid(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::prc_x]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::prc_x]) sen_po->set_prc_x(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::prc_y]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::prc_y]) sen_po->set_prc_y(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::prc_z]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::prc_z]) sen_po->set_prc_z(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::crp_id]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::crp_id]) sen_po->set_crp_id(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::dx_crp]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dx_crp]) sen_po->set_dx_crp(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::dy_crp]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dy_crp]) sen_po->set_dy_crp(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::dh_crp]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dh_crp]) sen_po->set_dh_crp(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::lane_count]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::lane_count]) sen_po->set_lane_count(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::lane_position]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::lane_position]) sen_po->set_lane_position(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::lane_lateral_position]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::lane_lateral_position]) sen_po->set_lane_lateral_position(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::crp_id_begin]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::crp_id_begin]) sen_po->set_crp_id_begin(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::crp_id_end]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::crp_id_end]) sen_po->set_crp_id_end(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::lane_vertical_position]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::lane_vertical_position]) sen_po->set_lane_vertical_position(iVal);
		ullVal = any_cast<unsigned long long>(vals[(int)SENSOR_INFO_POS_0_8_1::lane_id]); if (ullVal != (unsigned long long)sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::lane_id]) sen_po->set_lane_id(ullVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::dx_lane]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dx_lane]) sen_po->set_dx_lane(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::dy_lane]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dy_lane]) sen_po->set_dy_lane(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::dh_lane]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::dh_lane]) sen_po->set_dh_lane(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::semi_axis_length_major]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::semi_axis_length_major]) sen_po->set_semi_axis_length_major(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::semi_axis_length_minor]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::semi_axis_length_minor]) sen_po->set_semi_axis_length_minor(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::ellipse_orientation]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::ellipse_orientation]) sen_po->set_ellipse_orientation(iVal);
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::altitude_accuracy]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::altitude_accuracy]) sen_po->set_altitude_accuracy(iVal);
		sen->set_time(any_cast<long>(vals[(int)SENSOR_INFO_POS_0_8_1::time]));
		
		int dc_pos = (int)SENSOR_INFO_POS_0_8_1::detectable_classes_0;
		for (int i = 0; i < 8; i++) {
			cool4_api_0_8_0::Detectable_capabilities* dc = sen->add_detectable_capabilities();
			iVal = any_cast<int>(vals[dc_pos]); if (iVal != sensorInfoUnknownVals_0_8_1[dc_pos]) dc->set_detectable_classes(iVal);
			dc_pos++;
			try {
				vector<int> vi =  any_cast<vector<int>>(vals[dc_pos]);
				for (int i = 0; i < (int)vi.size(); i++) {
					dc->add_detectable_domain_x_list(any_cast<int>(vi[i]));
				}
			} catch (...) {
				vector<any> vany =  any_cast<vector<any>>(vals[dc_pos]);
				for (int i = 0; i < (int)vany.size(); i++) {
					dc->add_detectable_domain_x_list(any_cast<int>(vany[i]));
				}
			}
			dc_pos++;
			try {
				vector<int> vi =  any_cast<vector<int>>(vals[dc_pos]);
				for (int i = 0; i < (int)vi.size(); i++) {
					dc->add_detectable_domain_y_list(any_cast<int>(vi[i]));
				}
			} catch (...) {
				vector<any> vany =  any_cast<vector<any>>(vals[dc_pos]);
				for (int i = 0; i < (int)vany.size(); i++) {
					dc->add_detectable_domain_y_list(any_cast<int>(vany[i]));
				}
			}
			dc_pos++;
			iVal = any_cast<int>(vals[dc_pos]); if (iVal != sensorInfoUnknownVals_0_8_1[dc_pos]) dc->set_confidence(iVal);
			dc_pos++;
			iVal = any_cast<int>(vals[dc_pos]); if (iVal != sensorInfoUnknownVals_0_8_1[dc_pos]) dc->set_minimal_detectable_size(iVal);
			dc_pos++;
		}
		iVal = any_cast<int>(vals[(int)SENSOR_INFO_POS_0_8_1::sensor_status]); if (iVal != sensorInfoUnknownVals_0_8_1[(int)SENSOR_INFO_POS_0_8_1::sensor_status]) sen->set_sensor_status(iVal);
		setIsTupleInfo(is_info, vals, (int)SENSOR_INFO_POS_0_8_1::sensor_status + 1);
		//debugSensorInfo(sen, is_info);
		return;
	}
	/**
	* DBシステムが付与するタプル情報のセット
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	is_info	ISのタプル情報
	* @param [in]	vals	タプル
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::setIsTupleInfo(dm2_proto::Is_tuple_info *is_info, vector<any> &vals, const int &curIdx)
	{
		if (vals.size() <= curIdx + (int)IS_TUPLE_INFO_POS::dm2_ts_group) return;
		string strVal;
		strVal = any_cast<string>(vals[curIdx + (int)IS_TUPLE_INFO_POS::dm2_creator_id]); if (strVal != "dm2developer1") is_info->set_dm2_creator_id(strVal);
		strVal = any_cast<string>(vals[curIdx + (int)IS_TUPLE_INFO_POS::dm2_ip_addr]); if (strVal != "0.0.0.0") is_info->set_dm2_ip_addr(strVal);
		string hashStr = any_cast<string>(vals[curIdx + (int)IS_TUPLE_INFO_POS::dm2_hash]);
		is_info->set_dm2_hash(stoull(hashStr));
		{
			strVal = any_cast<string>(vals[curIdx + (int)IS_TUPLE_INFO_POS::dm2_ts_group]);
			vector<string> vs = stringUtil.getVectorString(strVal);
			for (int i = 0; i < (int)vs.size(); i++) {
				if (vs[i].empty()) continue;
				is_info->add_dm2_ts_group(stoull(vs[i]));
			}
		}
	}
	/**
	* protobufのフィールド情報から型名を判別する
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	field	フィールド情報
	*
	* @return	型名
	 */
	string ProtobufParser::FieldTypeToString(const FieldDescriptor *field) {
		//cout << "Field Name: " << field->name() << ",Type: " << FieldDescriptor::TypeName(field->type()) << ",IsRepeated: " << field->is_repeated() << endl;
		string field_type;
		string prefix = "";
		string suffix = "";
		if (field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
			prefix = "vector(vector(";
			suffix = "))";
			const Descriptor* child = field->message_type();
			const FieldDescriptor* childField = child->field(0);
			field_type = FieldDescriptor::TypeName(childField->type());
		} else {
			field_type = FieldDescriptor::TypeName(field->type());
			if (field->is_repeated()) {
				prefix = "vector(";
				suffix = ")";
			}
		}
		return prefix + field_type + suffix;
	}

	void ProtobufParser::loggerWarn(const char * func, const string &msg)
	{
		string s = __func__;
		cerr << "[" << func << "]" + msg << endl;
	}
	/**
	* 物標情報のデバッグ出力 (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	obj		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	*
	* @return	シリアライズされた文字列
	 */

	void ProtobufParser::debugObjectInfo(const cool4_api_0_6_0::Object_info *obj, dm2_proto::Is_tuple_info *is_info)
	{
		const cool4_api_0_6_0::Position_and_movement _obj_pm = obj->position_and_movement();
		const cool4_api_0_6_0::Vehicle_condition _obj_vc = obj->vehicle_condition();
		const cool4_api_0_6_0::Position_and_movement *obj_pm = &_obj_pm;
		const cool4_api_0_6_0::Vehicle_condition *obj_vc = &_obj_vc;
		cout << "[ts:" << is_info->dm2_create_ts() << "]";
		cout << obj->id() << ",";
		cout << obj->time() << ",";
		cout << obj->object_class_id_0() << ",";
		cout << obj->object_class_confidence_0() << ",";
		cout << obj->object_class_subclass_type_0() << ",";
		cout << obj->object_class_subclass_confidence_0() << ",";
		cout << obj->object_class_id_1() << ",";
		cout << obj->object_class_confidence_1() << ",";
		cout << obj->object_class_subclass_type_1() << ",";
		cout << obj->object_class_subclass_confidence_1() << ",";
		cout << obj->object_class_id_2() << ",";
		cout << obj->object_class_confidence_2() << ",";
		cout << obj->object_class_subclass_type_2() << ",";
		cout << obj->object_class_subclass_confidence_2() << ",";
		cout << obj->object_class_id_3() << ",";
		cout << obj->object_class_confidence_3() << ",";
		cout << obj->object_class_subclass_type_3() << ",";
		cout << obj->object_class_subclass_confidence_3() << ",";
		cout << obj->existency() << ",";
		cout << obj_pm->geodetic_system() << ",";
		cout << obj_pm->latitude() << ",";
		cout << obj_pm->longitude() << ",";
		cout << obj_pm->altitude() << ",";
		cout << obj_pm->crp_id() << ",";
		cout << obj_pm->dx_crp() << ",";
		cout << obj_pm->dy_crp() << ",";
		cout << obj_pm->dh_crp() << ",";
		cout << obj_pm->lane_count() << ",";
		cout << obj_pm->lane_position() << ",";
		cout << obj_pm->lane_lateral_position() << ",";
		cout << obj_pm->crp_id_begin() << ",";
		cout << obj_pm->crp_id_end() << ",";
		cout << obj_pm->lane_vertical_position() << ",";
		cout << obj_pm->lane_id() << ",";
		cout << obj_pm->dx_lane() << ",";
		cout << obj_pm->dy_lane() << ",";
		cout << obj_pm->dh_lane() << ",";
		cout << obj_pm->semi_axis_length_major() << ",";
		cout << obj_pm->semi_axis_length_minor() << ",";
		cout << obj_pm->ellipse_orientation() << ",";
		cout << obj_pm->altitude_accuracy() << ",";
		cout << obj_pm->reference_point() << ",";
		cout << obj_pm->move_direction_value() << ",";
		cout << obj_pm->move_direction_accuracy() << ",";
		cout << obj_pm->speed_value() << ",";
		cout << obj_pm->speed_accuracy() << ",";
		cout << obj_pm->yaw_rate_value() << ",";
		cout << obj_pm->yaw_rate_accuracy() << ",";
		cout << obj_pm->acceleration_value() << ",";
		cout << obj_pm->acceleration_accuracy() << ",";
		cout << obj_vc->orientation_value() << ",";
		cout << obj_vc->orientation_accuracy() << ",";
		cout << obj_vc->length_value() << ",";
		cout << obj_vc->length_accuracy() << ",";
		cout << obj_vc->width_value() << ",";
		cout << obj_vc->width_accuracy() << ",";
		cout << obj_vc->height_value() << ",";
		cout << obj_vc->height_accuracy() << ",";
		cout << obj_vc->color() << ",";
		cout << obj_vc->shift_position() << ",";
		cout << obj_vc->steering_angle_front() << ",";
		cout << obj_vc->steering_angle_rear() << ",";
		cout << obj_vc->brake_state() << ",";
		cout << obj_vc->auxiliary_brake_state() << ",";
		cout << obj_vc->throttle_position() << ",";
		cout << obj_vc->exterior_lights() << ",";
		cout << obj_vc->adaptive_cruise_control_system() << ",";
		cout << obj_vc->cooperative_adaptive_cruise_control_system() << ",";
		cout << obj_vc->pre_crash_safety_system() << ",";
		cout << obj_vc->antilock_brake_system() << ",";
		cout << obj_vc->traction_control_system() << ",";
		cout << obj_vc->electronic_stability_control_system() << ",";
		cout << obj_vc->lane_keeping_assist_system() << ",";
		cout << obj_vc->lane_departure_warning_system() << ",";
		cout << obj_vc->vehicle_role() << ",";
		cout << obj_vc->vehicle_extended_info() << ",";
		cout << obj_vc->towing_vehicle() << ",";
		for (int i = 0; i < obj->information_source_list_size(); i++) {
			cout << obj->information_source_list(i) << ",";
		}
		cout << is_info->dm2_creator_id() << ",";
		cout << is_info->dm2_ip_addr() << ",";
		cout << is_info->dm2_hash() << ",";
		for (int i = 0; i < is_info->dm2_ts_group_size(); i++) {
			cout << is_info->dm2_ts_group(i) << ",";
		}
		cout << "" << endl;
	}
	/**
	* 物標情報のデバッグ出力 (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	obj		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	*
	* @return	シリアライズされた文字列
	 */

	void ProtobufParser::debugObjectInfo(const cool4_api_0_8_0::Object_info_0_8_0 *obj, dm2_proto::Is_tuple_info *is_info)
	{
		const cool4_api_0_8_0::Position_and_movement _obj_pm = obj->position_and_movement();
		const cool4_api_0_8_0::Vehicle_condition _obj_vc = obj->vehicle_condition();
		const cool4_api_0_8_0::Tracking_information _obj_ti = obj->tracking_information();
		const cool4_api_0_8_0::Position_and_movement *obj_pm = &_obj_pm;
		const cool4_api_0_8_0::Vehicle_condition *obj_vc = &_obj_vc;
		const cool4_api_0_8_0::Tracking_information *obj_ti = &_obj_ti;
		cout << "[ts:" << is_info->dm2_create_ts() << "]";
		cout << obj->object_id() << ",";
		cout << obj->time() << ",";
		cout << obj->object_class_id_0() << ",";
		cout << obj->object_class_confidence_0() << ",";
		cout << obj->object_class_subclass_type_0() << ",";
		cout << obj->object_class_subclass_confidence_0() << ",";
		cout << obj->object_class_id_1() << ",";
		cout << obj->object_class_confidence_1() << ",";
		cout << obj->object_class_subclass_type_1() << ",";
		cout << obj->object_class_subclass_confidence_1() << ",";
		cout << obj->object_class_id_2() << ",";
		cout << obj->object_class_confidence_2() << ",";
		cout << obj->object_class_subclass_type_2() << ",";
		cout << obj->object_class_subclass_confidence_2() << ",";
		cout << obj->object_class_id_3() << ",";
		cout << obj->object_class_confidence_3() << ",";
		cout << obj->object_class_subclass_type_3() << ",";
		cout << obj->object_class_subclass_confidence_3() << ",";
		cout << obj->existency() << ",";
		cout << obj_pm->geodetic_srid() << ",";
		cout << obj_pm->latitude() << ",";
		cout << obj_pm->longitude() << ",";
		cout << obj_pm->altitude() << ",";
		cout << obj_pm->projective_srid() << ",";
		cout << obj_pm->prc_x() << ",";
		cout << obj_pm->prc_y() << ",";
		cout << obj_pm->prc_z() << ",";
		cout << obj_pm->crp_id() << ",";
		cout << obj_pm->dx_crp() << ",";
		cout << obj_pm->dy_crp() << ",";
		cout << obj_pm->dh_crp() << ",";
		cout << obj_pm->lane_count() << ",";
		cout << obj_pm->lane_position() << ",";
		cout << obj_pm->lane_lateral_position() << ",";
		cout << obj_pm->crp_id_begin() << ",";
		cout << obj_pm->crp_id_end() << ",";
		cout << obj_pm->lane_vertical_position() << ",";
		cout << obj_pm->lane_id() << ",";
		cout << obj_pm->dx_lane() << ",";
		cout << obj_pm->dy_lane() << ",";
		cout << obj_pm->dh_lane() << ",";
		cout << obj_pm->semi_axis_length_major() << ",";
		cout << obj_pm->semi_axis_length_minor() << ",";
		cout << obj_pm->ellipse_orientation() << ",";
		cout << obj_pm->altitude_accuracy() << ",";
		cout << obj_pm->reference_point() << ",";
		cout << obj_pm->move_direction_value() << ",";
		cout << obj_pm->move_direction_accuracy() << ",";
		cout << obj_pm->speed_value() << ",";
		cout << obj_pm->speed_accuracy() << ",";
		cout << obj_pm->yaw_rate_value() << ",";
		cout << obj_pm->yaw_rate_accuracy() << ",";
		cout << obj_pm->acceleration_value() << ",";
		cout << obj_pm->acceleration_accuracy() << ",";
		cout << obj_vc->orientation_value() << ",";
		cout << obj_vc->orientation_accuracy() << ",";
		cout << obj_vc->length_value() << ",";
		cout << obj_vc->length_accuracy() << ",";
		cout << obj_vc->width_value() << ",";
		cout << obj_vc->width_accuracy() << ",";
		cout << obj_vc->height_value() << ",";
		cout << obj_vc->height_accuracy() << ",";
		cout << obj_vc->static_status() << ",";
		cout << obj_vc->shift_position() << ",";
		cout << obj_vc->steering_angle_front() << ",";
		cout << obj_vc->steering_angle_rear() << ",";
		cout << obj_vc->brake_state() << ",";
		cout << obj_vc->auxiliary_brake_state() << ",";
		cout << obj_vc->throttle_position() << ",";
		cout << obj_vc->exterior_lights() << ",";
		cout << obj_vc->adaptive_cruise_control_system() << ",";
		cout << obj_vc->cooperative_adaptive_cruise_control_system() << ",";
		cout << obj_vc->pre_crash_safety_system() << ",";
		cout << obj_vc->antilock_brake_system() << ",";
		cout << obj_vc->traction_control_system() << ",";
		cout << obj_vc->electronic_stability_control_system() << ",";
		cout << obj_vc->lane_keeping_assist_system() << ",";
		cout << obj_vc->lane_departure_warning_system() << ",";
		cout << obj_vc->vehicle_size_type() << ",";
		cout << obj_vc->vehicle_role() << ",";
		cout << obj_vc->vehicle_extended_info() << ",";
		cout << obj_vc->towing_vehicle() << ",";
		cout << obj_ti->tracking_status() << ",";
		cout << obj_ti->detection_count() << ",";
		cout << obj_ti->lost_count() << ",";
		cout << obj_ti->object_age() << ",[";
		for (int i = 0; i < obj->information_source_list_size(); i++) {
			if (i != 0)  cout << ",";
			cout << obj->information_source_list(i);
		}
		cout << "]," << is_info->dm2_creator_id() << ",";
		cout << is_info->dm2_ip_addr() << ",";
		cout << is_info->dm2_hash() << ",";
		for (int i = 0; i < is_info->dm2_ts_group_size(); i++) {
			cout << is_info->dm2_ts_group(i) << ",";
		}
		cout << "" << endl;
	}
	/**
	* 物標情報のデバッグ出力 (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	obj		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	*
	* @return	シリアライズされた文字列
	 */

	void ProtobufParser::debugObjectInfo(const cool4_api_0_8_0::Object_info_0_8_1 *obj, dm2_proto::Is_tuple_info *is_info)
	{
		const cool4_api_0_8_0::Position_and_movement _obj_pm = obj->position_and_movement();
		const cool4_api_0_8_0::Vehicle_condition _obj_vc = obj->vehicle_condition();
		const cool4_api_0_8_0::Tracking_information _obj_ti = obj->tracking_information();
		const cool4_api_0_8_0::Position_and_movement *obj_pm = &_obj_pm;
		const cool4_api_0_8_0::Vehicle_condition *obj_vc = &_obj_vc;
		const cool4_api_0_8_0::Tracking_information *obj_ti = &_obj_ti;
		cout << "[ts:" << is_info->dm2_create_ts() << "]";
		cout << obj->object_id() << ",";
		cout << obj->time() << ",";
		cout << obj->revision() << ",";
		cout << obj->object_class_id_0() << ",";
		cout << obj->object_class_confidence_0() << ",";
		cout << obj->object_class_subclass_type_0() << ",";
		cout << obj->object_class_subclass_confidence_0() << ",";
		cout << obj->object_class_id_1() << ",";
		cout << obj->object_class_confidence_1() << ",";
		cout << obj->object_class_subclass_type_1() << ",";
		cout << obj->object_class_subclass_confidence_1() << ",";
		cout << obj->object_class_id_2() << ",";
		cout << obj->object_class_confidence_2() << ",";
		cout << obj->object_class_subclass_type_2() << ",";
		cout << obj->object_class_subclass_confidence_2() << ",";
		cout << obj->object_class_id_3() << ",";
		cout << obj->object_class_confidence_3() << ",";
		cout << obj->object_class_subclass_type_3() << ",";
		cout << obj->object_class_subclass_confidence_3() << ",";
		cout << obj->existency() << ",";
		cout << obj_pm->geodetic_srid() << ",";
		cout << obj_pm->latitude() << ",";
		cout << obj_pm->longitude() << ",";
		cout << obj_pm->altitude() << ",";
		cout << obj_pm->projective_srid() << ",";
		cout << obj_pm->prc_x() << ",";
		cout << obj_pm->prc_y() << ",";
		cout << obj_pm->prc_z() << ",";
		cout << obj_pm->crp_id() << ",";
		cout << obj_pm->dx_crp() << ",";
		cout << obj_pm->dy_crp() << ",";
		cout << obj_pm->dh_crp() << ",";
		cout << obj_pm->lane_count() << ",";
		cout << obj_pm->lane_position() << ",";
		cout << obj_pm->lane_lateral_position() << ",";
		cout << obj_pm->crp_id_begin() << ",";
		cout << obj_pm->crp_id_end() << ",";
		cout << obj_pm->lane_vertical_position() << ",";
		cout << obj_pm->lane_id() << ",";
		cout << obj_pm->dx_lane() << ",";
		cout << obj_pm->dy_lane() << ",";
		cout << obj_pm->dh_lane() << ",";
		cout << obj_pm->semi_axis_length_major() << ",";
		cout << obj_pm->semi_axis_length_minor() << ",";
		cout << obj_pm->ellipse_orientation() << ",";
		cout << obj_pm->altitude_accuracy() << ",";
		cout << obj_pm->reference_point() << ",";
		cout << obj_pm->move_direction_value() << ",";
		cout << obj_pm->move_direction_accuracy() << ",";
		cout << obj_pm->speed_value() << ",";
		cout << obj_pm->speed_accuracy() << ",";
		cout << obj_pm->yaw_rate_value() << ",";
		cout << obj_pm->yaw_rate_accuracy() << ",";
		cout << obj_pm->acceleration_value() << ",";
		cout << obj_pm->acceleration_accuracy() << ",";
		cout << obj_vc->orientation_value() << ",";
		cout << obj_vc->orientation_accuracy() << ",";
		cout << obj_vc->length_value() << ",";
		cout << obj_vc->length_accuracy() << ",";
		cout << obj_vc->width_value() << ",";
		cout << obj_vc->width_accuracy() << ",";
		cout << obj_vc->height_value() << ",";
		cout << obj_vc->height_accuracy() << ",";
		cout << obj_vc->static_status() << ",";
		cout << obj_vc->shift_position() << ",";
		cout << obj_vc->steering_angle_front() << ",";
		cout << obj_vc->steering_angle_rear() << ",";
		cout << obj_vc->brake_state() << ",";
		cout << obj_vc->auxiliary_brake_state() << ",";
		cout << obj_vc->throttle_position() << ",";
		cout << obj_vc->exterior_lights() << ",";
		cout << obj_vc->adaptive_cruise_control_system() << ",";
		cout << obj_vc->cooperative_adaptive_cruise_control_system() << ",";
		cout << obj_vc->pre_crash_safety_system() << ",";
		cout << obj_vc->antilock_brake_system() << ",";
		cout << obj_vc->traction_control_system() << ",";
		cout << obj_vc->electronic_stability_control_system() << ",";
		cout << obj_vc->lane_keeping_assist_system() << ",";
		cout << obj_vc->lane_departure_warning_system() << ",";
		cout << obj_vc->vehicle_size_type() << ",";
		cout << obj_vc->vehicle_role() << ",";
		cout << obj_vc->vehicle_extended_info() << ",";
		cout << obj_vc->towing_vehicle() << ",";
		cout << obj_ti->tracking_status() << ",";
		cout << obj_ti->detection_count() << ",";
		cout << obj_ti->lost_count() << ",";
		cout << obj_ti->object_age() << ",[";
		for (int i = 0; i < obj->information_source_list_size(); i++) {
			if (i != 0)  cout << ",";
			cout << obj->information_source_list(i);
		}
		cout << "]," << is_info->dm2_creator_id() << ",";
		cout << is_info->dm2_ip_addr() << ",";
		cout << is_info->dm2_hash() << ",";
		for (int i = 0; i < is_info->dm2_ts_group_size(); i++) {
			cout << is_info->dm2_ts_group(i) << ",";
		}
		cout << "" << endl;
	}
	/**
	* フリースペース情報のデバッグ出力 (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	fre		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::debugFreespaceInfo(const cool4_api_0_6_0::Freespace_info* fre, dm2_proto::Is_tuple_info *is_info)
	{
		const cool4_api_0_6_0::Location _pos_begin = fre->position_begin();
		const cool4_api_0_6_0::Location _pos_end = fre->position_end();
		const cool4_api_0_6_0::Location *pos_begin = &_pos_begin;
		const cool4_api_0_6_0::Location *pos_end = &_pos_end;
		cout << "[ts:" << is_info->dm2_create_ts() << "]";
		cout << fre->id() << ",";
		cout << fre->time() << ",";
		cout << fre->existency() << ",";
		cout << fre->minimal_detectable_size() << ",";
		cout << pos_begin->geodetic_system() << ",";
		cout << pos_begin->latitude() << ",";
		cout << pos_begin->longitude() << ",";
		cout << pos_begin->altitude() << ",";
		cout << pos_begin->crp_id() << ",";
		cout << pos_begin->dx_crp() << ",";
		cout << pos_begin->dy_crp() << ",";
		cout << pos_begin->dh_crp() << ",";
		cout << pos_begin->lane_count() << ",";
		cout << pos_begin->lane_position() << ",";
		cout << pos_begin->lane_lateral_position() << ",";
		cout << pos_begin->crp_id_begin() << ",";
		cout << pos_begin->crp_id_end() << ",";
		cout << pos_begin->lane_vertical_position() << ",";
		cout << pos_begin->lane_id() << ",";
		cout << pos_begin->dx_lane() << ",";
		cout << pos_begin->dy_lane() << ",";
		cout << pos_begin->dh_lane() << ",";
		cout << pos_begin->semi_axis_length_major() << ",";
		cout << pos_begin->semi_axis_length_minor() << ",";
		cout << pos_begin->ellipse_orientation() << ",";
		cout << pos_begin->altitude_accuracy() << ",";
		cout << pos_end->geodetic_system() << ",";
		cout << pos_end->latitude() << ",";
		cout << pos_end->longitude() << ",";
		cout << pos_end->altitude() << ",";
		cout << pos_end->crp_id() << ",";
		cout << pos_end->dx_crp() << ",";
		cout << pos_end->dy_crp() << ",";
		cout << pos_end->dh_crp() << ",";
		cout << pos_end->lane_count() << ",";
		cout << pos_end->lane_position() << ",";
		cout << pos_end->lane_lateral_position() << ",";
		cout << pos_end->crp_id_begin() << ",";
		cout << pos_end->crp_id_end() << ",";
		cout << pos_end->lane_vertical_position() << ",";
		cout << pos_end->lane_id() << ",";
		cout << pos_end->dx_lane() << ",";
		cout << pos_end->dy_lane() << ",";
		cout << pos_end->dh_lane() << ",";
		cout << pos_end->semi_axis_length_major() << ",";
		cout << pos_end->semi_axis_length_minor() << ",";
		cout << pos_end->ellipse_orientation() << ",";
		cout << pos_end->altitude_accuracy() << ",";
		cout << fre->length_value() << ",";
		cout << fre->length_accuracy() << ",";
		cout << fre->id_begin() << ",";
		cout << fre->id_end() << ",";
		for (int i = 0; i < fre->information_source_list_size(); i++) {
			cout << fre->information_source_list(i) << ",";
		}
		cout << is_info->dm2_creator_id() << ",";
		cout << is_info->dm2_ip_addr() << ",";
		cout << is_info->dm2_hash() << ",";
		for (int i = 0; i < is_info->dm2_ts_group_size(); i++) {
			cout << is_info->dm2_ts_group(i) << ",";
		}
		cout << "" << endl;
	}
	/**
	* フリースペース情報のデバッグ出力 (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/05/10
	*
	* @param [in]	fre		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::debugFreespaceInfo(const cool4_api_0_8_0::Freespace_info_0_8_0* fre, dm2_proto::Is_tuple_info *is_info)
	{
		const cool4_api_0_8_0::Location _vertices_begin = fre->vertices_begin();
		const cool4_api_0_8_0::Location _pos_begin = fre->position_begin();
		const cool4_api_0_8_0::Location _pos_end = fre->position_end();
		const cool4_api_0_8_0::Location *vertices_begin = &_vertices_begin;
		const cool4_api_0_8_0::Location *pos_begin = &_pos_begin;
		const cool4_api_0_8_0::Location *pos_end = &_pos_end;
		cout << "[ts:" << is_info->dm2_create_ts() << "]";
		cout << fre->id() << ",";
		cout << fre->time() << ",";
		cout << fre->detection_method() << ",";
		cout << fre->detactable_classes() << ",";
		cout << vertices_begin->geodetic_srid() << ",";
		cout << vertices_begin->latitude() << ",";
		cout << vertices_begin->longitude() << ",";
		cout << vertices_begin->altitude() << ",";
		cout << vertices_begin->projective_srid() << ",";
		cout << vertices_begin->prc_x() << ",";
		cout << vertices_begin->prc_y() << ",";
		cout << vertices_begin->prc_z() << ",";
		cout << vertices_begin->crp_id() << ",";
		cout << vertices_begin->dx_crp() << ",";
		cout << vertices_begin->dy_crp() << ",";
		cout << vertices_begin->dh_crp() << ",";
		cout << vertices_begin->lane_count() << ",";
		cout << vertices_begin->lane_position() << ",";
		cout << vertices_begin->lane_lateral_position() << ",";
		cout << vertices_begin->crp_id_begin() << ",";
		cout << vertices_begin->crp_id_end() << ",";
		cout << vertices_begin->lane_vertical_position() << ",";
		cout << vertices_begin->lane_id() << ",";
		cout << vertices_begin->dx_lane() << ",";
		cout << vertices_begin->dy_lane() << ",";
		cout << vertices_begin->dh_lane() << ",";
		cout << vertices_begin->semi_axis_length_major() << ",";
		cout << vertices_begin->semi_axis_length_minor() << ",";
		cout << vertices_begin->ellipse_orientation() << ",";
		cout << vertices_begin->altitude_accuracy() << ",";
		for (int i = 0; i < fre->vertices_dx_list_size(); i++) {
			cout << fre->vertices_dx_list(i) << ",";
		}
		for (int i = 0; i < fre->vertices_dy_list_size(); i++) {
			cout << fre->vertices_dy_list(i) << ",";
		}
		cout << pos_begin->geodetic_srid() << ",";
		cout << pos_begin->latitude() << ",";
		cout << pos_begin->longitude() << ",";
		cout << pos_begin->altitude() << ",";
		cout << pos_begin->projective_srid() << ",";
		cout << pos_begin->prc_x() << ",";
		cout << pos_begin->prc_y() << ",";
		cout << pos_begin->prc_z() << ",";
		cout << pos_begin->crp_id() << ",";
		cout << pos_begin->dx_crp() << ",";
		cout << pos_begin->dy_crp() << ",";
		cout << pos_begin->dh_crp() << ",";
		cout << pos_begin->lane_count() << ",";
		cout << pos_begin->lane_position() << ",";
		cout << pos_begin->lane_lateral_position() << ",";
		cout << pos_begin->crp_id_begin() << ",";
		cout << pos_begin->crp_id_end() << ",";
		cout << pos_begin->lane_vertical_position() << ",";
		cout << pos_begin->lane_id() << ",";
		cout << pos_begin->dx_lane() << ",";
		cout << pos_begin->dy_lane() << ",";
		cout << pos_begin->dh_lane() << ",";
		cout << pos_begin->semi_axis_length_major() << ",";
		cout << pos_begin->semi_axis_length_minor() << ",";
		cout << pos_begin->ellipse_orientation() << ",";
		cout << pos_begin->altitude_accuracy() << ",";
		cout << pos_end->geodetic_srid() << ",";
		cout << pos_end->latitude() << ",";
		cout << pos_end->longitude() << ",";
		cout << pos_end->altitude() << ",";
		cout << pos_end->projective_srid() << ",";
		cout << pos_end->prc_x() << ",";
		cout << pos_end->prc_y() << ",";
		cout << pos_end->prc_z() << ",";
		cout << pos_end->crp_id() << ",";
		cout << pos_end->dx_crp() << ",";
		cout << pos_end->dy_crp() << ",";
		cout << pos_end->dh_crp() << ",";
		cout << pos_end->lane_count() << ",";
		cout << pos_end->lane_position() << ",";
		cout << pos_end->lane_lateral_position() << ",";
		cout << pos_end->crp_id_begin() << ",";
		cout << pos_end->crp_id_end() << ",";
		cout << pos_end->lane_vertical_position() << ",";
		cout << pos_end->lane_id() << ",";
		cout << pos_end->dx_lane() << ",";
		cout << pos_end->dy_lane() << ",";
		cout << pos_end->dh_lane() << ",";
		cout << pos_end->semi_axis_length_major() << ",";
		cout << pos_end->semi_axis_length_minor() << ",";
		cout << pos_end->ellipse_orientation() << ",";
		cout << pos_end->altitude_accuracy() << ",";
		cout << fre->length_value() << ",";
		cout << fre->length_accuracy() << ",";
		cout << fre->id_begin() << ",";
		cout << fre->id_end() << ",";
		cout << fre->existency() << ",";
		cout << fre->minimal_detectable_size() << ",";
		for (int i = 0; i < fre->information_source_list_size(); i++) {
			cout << fre->information_source_list(i) << ",";
		}
		cout << is_info->dm2_creator_id() << ",";
		cout << is_info->dm2_ip_addr() << ",";
		cout << is_info->dm2_hash() << ",";
		for (int i = 0; i < is_info->dm2_ts_group_size(); i++) {
			cout << is_info->dm2_ts_group(i) << ",";
		}
		cout << "" << endl;
	}
	/**
	* フリースペース情報のデバッグ出力 (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2025/06/30
	*
	* @param [in]	fre		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	*
	* @return	シリアライズされた文字列
	 */
	void ProtobufParser::debugFreespaceInfo(const cool4_api_0_8_0::Freespace_info_0_8_1* fre, dm2_proto::Is_tuple_info *is_info)
	{
		const cool4_api_0_8_0::Location _vertices_begin = fre->vertices_begin();
		const cool4_api_0_8_0::Location _pos_begin = fre->position_begin();
		const cool4_api_0_8_0::Location _pos_end = fre->position_end();
		const cool4_api_0_8_0::Location *vertices_begin = &_vertices_begin;
		const cool4_api_0_8_0::Location *pos_begin = &_pos_begin;
		const cool4_api_0_8_0::Location *pos_end = &_pos_end;
		cout << "[ts:" << is_info->dm2_create_ts() << "]";
		cout << fre->id() << ",";
		cout << fre->time() << ",";
		cout << fre->revision() << ",";
		cout << fre->detection_method() << ",";
		cout << fre->detactable_classes() << ",";
		cout << vertices_begin->geodetic_srid() << ",";
		cout << vertices_begin->latitude() << ",";
		cout << vertices_begin->longitude() << ",";
		cout << vertices_begin->altitude() << ",";
		cout << vertices_begin->projective_srid() << ",";
		cout << vertices_begin->prc_x() << ",";
		cout << vertices_begin->prc_y() << ",";
		cout << vertices_begin->prc_z() << ",";
		cout << vertices_begin->crp_id() << ",";
		cout << vertices_begin->dx_crp() << ",";
		cout << vertices_begin->dy_crp() << ",";
		cout << vertices_begin->dh_crp() << ",";
		cout << vertices_begin->lane_count() << ",";
		cout << vertices_begin->lane_position() << ",";
		cout << vertices_begin->lane_lateral_position() << ",";
		cout << vertices_begin->crp_id_begin() << ",";
		cout << vertices_begin->crp_id_end() << ",";
		cout << vertices_begin->lane_vertical_position() << ",";
		cout << vertices_begin->lane_id() << ",";
		cout << vertices_begin->dx_lane() << ",";
		cout << vertices_begin->dy_lane() << ",";
		cout << vertices_begin->dh_lane() << ",";
		cout << vertices_begin->semi_axis_length_major() << ",";
		cout << vertices_begin->semi_axis_length_minor() << ",";
		cout << vertices_begin->ellipse_orientation() << ",";
		cout << vertices_begin->altitude_accuracy() << ",";
		for (int i = 0; i < fre->vertices_dx_list_size(); i++) {
			cout << fre->vertices_dx_list(i) << ",";
		}
		for (int i = 0; i < fre->vertices_dy_list_size(); i++) {
			cout << fre->vertices_dy_list(i) << ",";
		}
		cout << pos_begin->geodetic_srid() << ",";
		cout << pos_begin->latitude() << ",";
		cout << pos_begin->longitude() << ",";
		cout << pos_begin->altitude() << ",";
		cout << pos_begin->projective_srid() << ",";
		cout << pos_begin->prc_x() << ",";
		cout << pos_begin->prc_y() << ",";
		cout << pos_begin->prc_z() << ",";
		cout << pos_begin->crp_id() << ",";
		cout << pos_begin->dx_crp() << ",";
		cout << pos_begin->dy_crp() << ",";
		cout << pos_begin->dh_crp() << ",";
		cout << pos_begin->lane_count() << ",";
		cout << pos_begin->lane_position() << ",";
		cout << pos_begin->lane_lateral_position() << ",";
		cout << pos_begin->crp_id_begin() << ",";
		cout << pos_begin->crp_id_end() << ",";
		cout << pos_begin->lane_vertical_position() << ",";
		cout << pos_begin->lane_id() << ",";
		cout << pos_begin->dx_lane() << ",";
		cout << pos_begin->dy_lane() << ",";
		cout << pos_begin->dh_lane() << ",";
		cout << pos_begin->semi_axis_length_major() << ",";
		cout << pos_begin->semi_axis_length_minor() << ",";
		cout << pos_begin->ellipse_orientation() << ",";
		cout << pos_begin->altitude_accuracy() << ",";
		cout << pos_end->geodetic_srid() << ",";
		cout << pos_end->latitude() << ",";
		cout << pos_end->longitude() << ",";
		cout << pos_end->altitude() << ",";
		cout << pos_end->projective_srid() << ",";
		cout << pos_end->prc_x() << ",";
		cout << pos_end->prc_y() << ",";
		cout << pos_end->prc_z() << ",";
		cout << pos_end->crp_id() << ",";
		cout << pos_end->dx_crp() << ",";
		cout << pos_end->dy_crp() << ",";
		cout << pos_end->dh_crp() << ",";
		cout << pos_end->lane_count() << ",";
		cout << pos_end->lane_position() << ",";
		cout << pos_end->lane_lateral_position() << ",";
		cout << pos_end->crp_id_begin() << ",";
		cout << pos_end->crp_id_end() << ",";
		cout << pos_end->lane_vertical_position() << ",";
		cout << pos_end->lane_id() << ",";
		cout << pos_end->dx_lane() << ",";
		cout << pos_end->dy_lane() << ",";
		cout << pos_end->dh_lane() << ",";
		cout << pos_end->semi_axis_length_major() << ",";
		cout << pos_end->semi_axis_length_minor() << ",";
		cout << pos_end->ellipse_orientation() << ",";
		cout << pos_end->altitude_accuracy() << ",";
		cout << fre->length_value() << ",";
		cout << fre->length_accuracy() << ",";
		cout << fre->id_begin() << ",";
		cout << fre->id_end() << ",";
		cout << fre->existency() << ",";
		cout << fre->minimal_detectable_size() << ",";
		for (int i = 0; i < fre->information_source_list_size(); i++) {
			cout << fre->information_source_list(i) << ",";
		}
		cout << is_info->dm2_creator_id() << ",";
		cout << is_info->dm2_ip_addr() << ",";
		cout << is_info->dm2_hash() << ",";
		for (int i = 0; i < is_info->dm2_ts_group_size(); i++) {
			cout << is_info->dm2_ts_group(i) << ",";
		}
		cout << "" << endl;
	}
	/**
	* 信号情報のデバッグ出力 (API 仕様案 Ver.0.6.0)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/10
	*
	* @param [in]	sig		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	*
	* @return	シリアライズされた文字列
	 */

	void ProtobufParser::debugSignalInfo(const cool4_api_0_6_0::Signal_info* sig, dm2_proto::Is_tuple_info *is_info)
	{
		cout << "[ts:" << is_info->dm2_create_ts() << "]";
		cout << sig->crp_id() << ",";
		for (int i = 0; i < sig->id_size(); i++) {
			cout << sig->id(i) << ",";
		}
		cout << sig->time() << ",";
		cout << sig->state() << ",";
		cout << sig->specific_control_flags() << ",";
		cout << sig->event_count() << ",";
		cout << sig->count_down_stop_flag() << ",";
		for (int i = 0; i < sig->light_info_size(); i++) {
			const cool4_api_0_6_0::Light_info lig = sig->light_info(i);
			cout << lig.main() << ",";
			cout << lig.arrow() << ",";
			cout << lig.min_time_to_change() << ",";
			cout << lig.max_time_to_change() << ",";
		}
		cout << is_info->dm2_creator_id() << ",";
		cout << is_info->dm2_ip_addr() << ",";
		cout << is_info->dm2_hash() << ",";
		for (int i = 0; i < is_info->dm2_ts_group_size(); i++) {
			cout << is_info->dm2_ts_group(i) << ",";
		}
		cout << "" << endl;
	}
	/**
	* センサー情報のデバッグ出力 (API 仕様案 Ver.0.8.0)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	obj		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	*
	* @return	シリアライズされた文字列
	 */

	void ProtobufParser::debugSensorInfo(const cool4_api_0_8_0::Sensor_info_0_8_0 *sen, dm2_proto::Is_tuple_info *is_info)
	{
		const cool4_api_0_8_0::Position _sen_po = sen->position();
		const cool4_api_0_8_0::Position *sen_po = &_sen_po;
		cout << "[ts:" << is_info->dm2_create_ts() << "]";
		cout << sen->unit_id() << ",";
		cout << sen->sensor_id() << ",";
		cout << sen->sensor_type() << ",";
		cout << sen_po->geodetic_srid() << ",";
		cout << sen_po->latitude() << ",";
		cout << sen_po->longitude() << ",";
		cout << sen_po->altitude() << ",";
		cout << sen_po->projective_srid() << ",";
		cout << sen_po->prc_x() << ",";
		cout << sen_po->prc_y() << ",";
		cout << sen_po->prc_z() << ",";
		cout << sen_po->crp_id() << ",";
		cout << sen_po->dx_crp() << ",";
		cout << sen_po->dy_crp() << ",";
		cout << sen_po->dh_crp() << ",";
		cout << sen_po->lane_count() << ",";
		cout << sen_po->lane_position() << ",";
		cout << sen_po->lane_lateral_position() << ",";
		cout << sen_po->crp_id_begin() << ",";
		cout << sen_po->crp_id_end() << ",";
		cout << sen_po->lane_vertical_position() << ",";
		cout << sen_po->lane_id() << ",";
		cout << sen_po->dx_lane() << ",";
		cout << sen_po->dy_lane() << ",";
		cout << sen_po->dh_lane() << ",";
		cout << sen_po->semi_axis_length_major() << ",";
		cout << sen_po->semi_axis_length_minor() << ",";
		cout << sen_po->ellipse_orientation() << ",";
		cout << sen_po->altitude_accuracy() << ",";
		
		for (int i = 0; i < sen->detectable_capabilities_size(); i++) {
			const cool4_api_0_8_0::Detectable_capabilities dc = sen->detectable_capabilities(i);
			cout << dc.detectable_classes() << ",[";
			for (int i = 0; i < dc.detectable_domain_x_list_size(); i++) {
				if (i != 0)  cout << ",";
				cout << dc.detectable_domain_x_list(i);
			}
			cout << "],[";
			for (int i = 0; i < dc.detectable_domain_y_list_size(); i++) {
				if (i != 0)  cout << ",";
				cout << dc.detectable_domain_y_list(i);
			}
			cout << "]," << dc.confidence() << ",";
			cout << dc.minimal_detectable_size() << ",";
		}
		cout << sen->sensor_status() << ",";

		cout << is_info->dm2_creator_id() << ",";
		cout << is_info->dm2_ip_addr() << ",";
		cout << is_info->dm2_hash() << ",";
		for (int i = 0; i < is_info->dm2_ts_group_size(); i++) {
			cout << is_info->dm2_ts_group(i) << ",";
		}
		cout << "" << endl;
	}

	/**
	* センサー情報のデバッグ出力 (API 仕様案 Ver.0.8.1)
	*
	* @author	Shinichi Kusayama
	* @date		2024/10/7
	*
	* @param [in]	obj		API仕様の情報
	* @param [in]	is_info	ISのタプル情報
	*
	* @return	シリアライズされた文字列
	 */

	void ProtobufParser::debugSensorInfo(const cool4_api_0_8_0::Sensor_info_0_8_1 *sen, dm2_proto::Is_tuple_info *is_info)
	{
		const cool4_api_0_8_0::Position _sen_po = sen->position();
		const cool4_api_0_8_0::Position *sen_po = &_sen_po;
		cout << "[ts:" << is_info->dm2_create_ts() << "]";
		cout << sen->unit_id() << ",";
		cout << sen->sensor_id() << ",";
		cout << sen->sensor_type() << ",";
		cout << sen_po->geodetic_srid() << ",";
		cout << sen_po->latitude() << ",";
		cout << sen_po->longitude() << ",";
		cout << sen_po->altitude() << ",";
		cout << sen_po->projective_srid() << ",";
		cout << sen_po->prc_x() << ",";
		cout << sen_po->prc_y() << ",";
		cout << sen_po->prc_z() << ",";
		cout << sen_po->crp_id() << ",";
		cout << sen_po->dx_crp() << ",";
		cout << sen_po->dy_crp() << ",";
		cout << sen_po->dh_crp() << ",";
		cout << sen_po->lane_count() << ",";
		cout << sen_po->lane_position() << ",";
		cout << sen_po->lane_lateral_position() << ",";
		cout << sen_po->crp_id_begin() << ",";
		cout << sen_po->crp_id_end() << ",";
		cout << sen_po->lane_vertical_position() << ",";
		cout << sen_po->lane_id() << ",";
		cout << sen_po->dx_lane() << ",";
		cout << sen_po->dy_lane() << ",";
		cout << sen_po->dh_lane() << ",";
		cout << sen_po->semi_axis_length_major() << ",";
		cout << sen_po->semi_axis_length_minor() << ",";
		cout << sen_po->ellipse_orientation() << ",";
		cout << sen_po->altitude_accuracy() << ",";
		cout << sen->time() << ",";
		
		for (int i = 0; i < sen->detectable_capabilities_size(); i++) {
			const cool4_api_0_8_0::Detectable_capabilities dc = sen->detectable_capabilities(i);
			cout << dc.detectable_classes() << ",[";
			for (int i = 0; i < dc.detectable_domain_x_list_size(); i++) {
				if (i != 0)  cout << ",";
				cout << dc.detectable_domain_x_list(i);
			}
			cout << "],[";
			for (int i = 0; i < dc.detectable_domain_y_list_size(); i++) {
				if (i != 0)  cout << ",";
				cout << dc.detectable_domain_y_list(i);
			}
			cout << "]," << dc.confidence() << ",";
			cout << dc.minimal_detectable_size() << ",";
		}
		cout << sen->sensor_status() << ",";

		cout << is_info->dm2_creator_id() << ",";
		cout << is_info->dm2_ip_addr() << ",";
		cout << is_info->dm2_hash() << ",";
		for (int i = 0; i < is_info->dm2_ts_group_size(); i++) {
			cout << is_info->dm2_ts_group(i) << ",";
		}
		cout << "" << endl;
	}

	/**
	* 動的にデシリアライズ (不定形メッセージ対応)
	*
	* @author	Shinichi Kusayama
	* @date		2024/07/03
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	* @param [out]	fieldNameList	フィールド名リスト
	* @param [out]	fieldTypeList	データタイプリスト
	* @param [in]	fromApl			アプリからの電文フラグ
	*
	*/
	void ProtobufParser::DeserializeToTupleDynamically(string serializedStr, vector<Tuple> &tuples, const Schema &schema, vector<string> &fieldNameList, vector<string> &fieldTypeList, const bool &fromApl)
	{
		size_t delimit_pos = serializedStr.find(",");
		if (delimit_pos == std::string::npos) {
			loggerWarn(__func__, " MetaData Delimeter Missing");
			return;
		}
		string metadataLenStr = serializedStr.substr(0, delimit_pos);
		string metaAndMessage = serializedStr.substr(delimit_pos + 1);
		int metadataLen;
		try {
			metadataLen = stoi(metadataLenStr);
		} catch (...) {
			loggerWarn(__func__, " MetaData Length Missing:" + metadataLenStr);
			return;
		}
		string metadataStr = metaAndMessage.substr(0, metadataLen);
		string messageStr = metaAndMessage.substr(metadataLen);

		FileDescriptorProto fileDescriptor;
		fileDescriptor.ParseFromString(metadataStr);

		DescriptorPool descriptor_pool;
		const FileDescriptor* dynamic_fd = descriptor_pool.BuildFile(fileDescriptor);
		if (dynamic_fd == nullptr) {
			loggerWarn(__func__, "BuildFile failed.");
			return;
		}
		//メタデータの情報表示
		for (int i = 0; i < dynamic_fd->message_type_count(); ++i) {
			const Descriptor* desc = dynamic_fd->message_type(i);
			string message_name = desc->name();
			//cout << "message_type_count: " << dynamic_fd->message_type_count() << ", message_index: " << i << ",message_name: " << message_name << ",field_count : " << desc->field_count() << endl;
			for (int j = 0; j < desc->field_count(); ++j) {
				const FieldDescriptor* field = desc->field(j);
				string field_type = FieldTypeToString(field);
				//cout << "  FieldTypeToString: " << field_type << ", field_index:" << j << endl;
				if (message_name == "Tuple_set") {
					fieldNameList.push_back(field->name());
					fieldTypeList.push_back(field_type);
				}
			}
		}
		// メタデータの情報を元に、メッセージの中のtuple_setにDescriptorを割り当て
		// message DynamicMessage_dm2is {
		//	repeated Tuple_set tuple_set = 1;
		// }
		const Descriptor* top_level_des = dynamic_fd->FindMessageTypeByName("DynamicMessage_dm2is");
		DynamicMessageFactory factory;
		Message* top_level_mes = factory.GetPrototype(top_level_des)->New();
		top_level_mes->ParseFromString(messageStr);
		const FieldDescriptor* tuple_set_fd = top_level_des->FindFieldByName("tuple_set");
		const Descriptor* tuple_set_des = tuple_set_fd->message_type();

		// 以下は、下記のフィールドの処理
		// ex.) message Tuple_set {
		//	uint32 id = 1;
		//	string name = 2;
		// }
		int tuple_count = (int)top_level_mes->GetReflection()->FieldSize(*top_level_mes, tuple_set_fd);
		//cout << "[DeserializeToTupleDynamically] tuple_count:" << tuple_count << endl;
		
		vector<string> attrTypeList;
		int attr_size = schema.getAttributeSize();
		if (fromApl) {
			for (int i = 0; i < attr_size; i++) {
				attrTypeList.push_back(schema.getAttributeType(i));
			}
		}
		// 最後の項目「dm2_create_ts」の分を削除
		int field_count = (int)fieldNameList.size() - 1;
		int tuple_normal_attr_count = attr_size - schema.getAdminAttributeSize();
		// IS -> APL の場合、且つ、不定形の場合、スキーマが空になるため、attr_sizeは0になる
		if (attr_size == 0) {
			//届いたProtobuf電文のフィールド数をセットする。
			attr_size = (int)fieldNameList.size() - 1;
			tuple_normal_attr_count = attr_size;
		}
		//cout << "[DeserializeToTupleDynamically] tuple_normal_attr_count:" << tuple_normal_attr_count << ",attr_size:" << attr_size << ",field_count:" << field_count << ",fromApl:" << fromApl << endl;
		for (int i = 0; i < tuple_count; i++) {
			const Message& tuple_set_mes = top_level_mes->GetReflection()->GetRepeatedMessage(*top_level_mes, tuple_set_fd, i);
			const Reflection* reflection = tuple_set_mes.GetReflection();
			
			Tuple tuple(attr_size);

			const FieldDescriptor* field_descriptor = tuple_set_des->FindFieldByName("dm2_create_ts");
			long ts = tuple_set_mes.GetReflection()->GetInt64(tuple_set_mes, field_descriptor);

			for (int j = 0; j < field_count; j++) {
				// アプリからの電文は、管理者属性が付与されていないため、ユーザが付与した属性の数までで処理を終了する。
				if (fromApl && j >= tuple_normal_attr_count) continue;
				const FieldDescriptor* field_descriptor = tuple_set_des->FindFieldByName(fieldNameList.at(j));
				// protobufメッセージに該当するフィールドが無い場合はnull
				//bool isNull = !tuple_set_mes.GetReflection()->HasField(tuple_set_mes, field_descriptor);
				bool isNull = false;
				if (fromApl) {
					setTupleAttrValue(reflection, tuple_set_mes, field_descriptor, tuple, j, ts, fieldNameList.at(j), isNull);
					checkAndNormalizedTuple(field_descriptor, tuple, j, attrTypeList[j]);
				} else {
					setTupleAttrValue(reflection, tuple_set_mes, field_descriptor, tuple, j, ts, fieldNameList.at(j), isNull);
				}
			}
			tuples.push_back(tuple);
		}
		delete top_level_mes;
		// 返却用にdm2_create_tsの情報を削除
		fieldNameList.pop_back();
		fieldTypeList.pop_back();

	}

	/**
	* 動的にデシリアライズ (不定形メッセージ対応)
	*
	* @author	Shinichi Kusayama
	* @date		2024/07/03
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [in]	tuples			デシリアライズされたタプル
	* @param [in]	schema			スキーマ情報
	*
	*/
	void ProtobufParser::DeserializeToTupleDynamically(string serializedStr, vector<Tuple> &tuples, const Schema &schema, const bool &fromApl)
	{
		vector<string> field_name_list, field_type_list;
		DeserializeToTupleDynamically(serializedStr, tuples, schema, field_name_list, field_type_list, fromApl);
	}
	/**
	* タプルの項目の型が正しいかチェックし、正しくない場合は変換する
	*
	* @author	Shinichi Kusayama
	* @date		2026/01/05
	*
	* @param [in]		fd				FieldDescriptor
	* @param [in,out]	tuple			タプル
	* @param [in]		t_idx			タプルインデックス
	* @param [in]		attrType		正しい型
	*
	 */
	void ProtobufParser::checkAndNormalizedTuple(const FieldDescriptor* fd, Tuple &tuple, int t_idx, const string &attrType)
	{
		FieldDescriptor::Type field_type = fd->type();
		FieldDescriptor::Label field_label = fd->label();
		//cout << "[checkAndNormalizedTuple]No." << t_idx << ",protoTypeMap:" << protoTypeMap[field_type] << ",attrType:" << attrType << endl;
		bool equalAttr = false;
		if(stringUtil.contain(attrType, "vector")) {
			if (field_label == FieldDescriptor::LABEL_REPEATED) {
				if (protoTypeMap.find(field_type) != protoTypeMap.end()) {
					if (protoTypeMap[field_type] == attrType) equalAttr = true;
				}
			}
		} else {
			if (protoTypeMap.find(field_type) != protoTypeMap.end()) {
				if (protoTypeMap[field_type] == attrType) equalAttr = true;
			}
		}
		if (!equalAttr) {
			any val;
			long ts;
			tuple.getValue(t_idx, val, ts);
			any retVal = normalizeAny(val, attrType);
			tuple.setValue(t_idx, retVal, ts);
		}
	}
	/**
	* 属性の種別に合ったProtobufの形式でタプルに値をセット
	*
	* @author	Shinichi Kusayama
	* @date		2024/07/08
	*
	* @param [in]		reflection		Reflection
	* @param [in]		mes				Message
	* @param [in]		fd				FieldDescriptor
	* @param [in,out]	tuple			タプル
	* @param [in]		t_idx			タプルインデックス
	* @param [in]		ts				タイムスタンプ
	* @param [in]		field_name		フィールド名
	* @param [in]		isNull			nullフラグ
	*
	 */
	void ProtobufParser::setTupleAttrValue(const Reflection* reflection, const Message& mes, const FieldDescriptor* fd, Tuple &tuple, int t_idx, long ts, const string &field_name, bool isNull)
	{
		FieldDescriptor::Type field_type = fd->type();
		FieldDescriptor::Label field_label = fd->label();
		//cout << "field_name:" << field_name << ", field_label:" << field_label << ", field_type:" << field_type << ", t_idx:" << t_idx << endl;
		if (field_label == FieldDescriptor::LABEL_REPEATED) {
			//1次元配列用
			int size = reflection->FieldSize(mes, fd);
			if (field_type == FieldDescriptor::TYPE_INT32) {
				vector<int> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedInt32(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_INT64) {
				vector<long> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedInt64(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_DOUBLE) {
				vector<double> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedDouble(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_STRING) {
				vector<string> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedString(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_BOOL) {
				vector<bool> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedBool(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_UINT32) {
				vector<unsigned int> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedUInt32(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_UINT64) {
				vector<unsigned long long> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedUInt64(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_MESSAGE) {
				//2次元配列用
				vector<vector<int>> v2val_int;
				vector<vector<long>> v2val_long;
				vector<vector<double>> v2val_double;
				vector<vector<string>> v2val_string;
				vector<vector<bool>> v2val_bool;
				vector<vector<unsigned int>> v2val_uint;
				vector<vector<unsigned long long>> v2val_ulong;
				FieldDescriptor::Type v2field_type = FieldDescriptor::TYPE_MESSAGE;
				for (int i = 0; i < size; i++) {
					const Message& v2mes = reflection->GetRepeatedMessage(mes, fd, i);
					const Reflection* v2reflection = v2mes.GetReflection();
					const Descriptor* v2des = fd->message_type();
					const FieldDescriptor* v2fd = v2des->FindFieldByName(field_name + "_item");
					v2field_type = v2fd->type();
					int v2size = v2reflection->FieldSize(v2mes, v2fd);
					if (v2field_type == FieldDescriptor::TYPE_INT32) {
						vector<int> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedInt32(v2mes, v2fd, j));
						}
						v2val_int.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_INT64) {
						vector<long> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedInt64(v2mes, v2fd, j));
						}
						v2val_long.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_DOUBLE) {
						vector<double> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedDouble(v2mes, v2fd, j));
						}
						v2val_double.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_STRING) {
						vector<string> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedString(v2mes, v2fd, j));
						}
						v2val_string.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_BOOL) {
						vector<bool> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedBool(v2mes, v2fd, j));
						}
						v2val_bool.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_UINT32) {
						vector<unsigned int> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedUInt32(v2mes, v2fd, j));
						}
						v2val_uint.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_UINT64) {
						vector<unsigned long long> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedUInt64(v2mes, v2fd, j));
						}
						v2val_ulong.push_back(val);
					}
				}
				if (v2field_type == FieldDescriptor::TYPE_INT32) tuple.setValue(t_idx, v2val_int, ts);
				else if (v2field_type == FieldDescriptor::TYPE_INT64) tuple.setValue(t_idx, v2val_long, ts);
				else if (v2field_type == FieldDescriptor::TYPE_DOUBLE) tuple.setValue(t_idx, v2val_double, ts);
				else if (v2field_type == FieldDescriptor::TYPE_STRING) tuple.setValue(t_idx, v2val_string, ts);
				else if (v2field_type == FieldDescriptor::TYPE_BOOL) tuple.setValue(t_idx, v2val_bool, ts);
				else if (v2field_type == FieldDescriptor::TYPE_UINT32) tuple.setValue(t_idx, v2val_uint, ts);
				else if (v2field_type == FieldDescriptor::TYPE_UINT64) tuple.setValue(t_idx, v2val_ulong, ts);
				//else loggerWarn(__func__, "Unknown FieldType");
			}
		} else {
			// not 配列
			if (field_type == FieldDescriptor::TYPE_INT32) {
				int val = reflection->GetInt32(mes, fd);
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_INT64) {
				long val = reflection->GetInt64(mes, fd);
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_DOUBLE) {
				double val = reflection->GetDouble(mes, fd);
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_STRING) {
				string val = reflection->GetString(mes, fd);
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_BOOL) {
				bool val = reflection->GetBool(mes, fd);
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_UINT32) {
				unsigned int val = reflection->GetUInt32(mes, fd);
				tuple.setValue(t_idx, val, ts, isNull);
			} else if (field_type == FieldDescriptor::TYPE_UINT64) {
				unsigned long long val = reflection->GetUInt64(mes, fd);
				tuple.setValue(t_idx, val, ts, isNull);
			} else {
				loggerWarn(__func__, "unknown field_type");
			}
		}			
	}
	/**
	* 属性の種別に合ったProtobufの形式で値をセット
	*
	* @author	Shinichi Kusayama
	* @date		2024/07/08
	*
	* @param [in]	tuple_set_mes	Message
	* @param [in]	tuple_set_des	Descriptor
	* @param [in]	val				値
	* @param [in]	attrType		フィールド
	* @param [in]	attrName		フィールド
	*
	 */
	void ProtobufParser::setTupleAttrValue(const Reflection* reflection, const Message& mes, const FieldDescriptor* fd, Tuple &tuple, int t_idx, long ts, const string &field_name)
	{
		FieldDescriptor::Type field_type = fd->type();
		FieldDescriptor::Label field_label = fd->label();
		if (field_label == FieldDescriptor::LABEL_REPEATED) {
			int size = reflection->FieldSize(mes, fd);
			if (field_type == FieldDescriptor::TYPE_INT32) {
				vector<int> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedInt32(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_INT64) {
				vector<long> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedInt64(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_DOUBLE) {
				vector<double> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedDouble(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_STRING) {
				vector<string> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedString(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_BOOL) {
				vector<bool> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedBool(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_UINT32) {
				vector<unsigned int> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedUInt32(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_UINT64) {
				vector<unsigned long long> val;
				for (int i = 0; i < size; i++) {
					val.push_back(reflection->GetRepeatedUInt64(mes, fd, i));
				}
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_MESSAGE) {
				//cout << "Message" << endl;
				vector<vector<int>> v2val_int;
				vector<vector<long>> v2val_long;
				vector<vector<double>> v2val_double;
				vector<vector<string>> v2val_string;
				vector<vector<bool>> v2val_bool;
				vector<vector<unsigned int>> v2val_uint;
				vector<vector<unsigned long long>> v2val_ulong;
				FieldDescriptor::Type v2field_type = FieldDescriptor::TYPE_MESSAGE;
				for (int i = 0; i < size; i++) {
					const Message& v2mes = reflection->GetRepeatedMessage(mes, fd, i);
					const Reflection* v2reflection = v2mes.GetReflection();
					const Descriptor* v2des = fd->message_type();
					const FieldDescriptor* v2fd = v2des->FindFieldByName(field_name + "_item");
					v2field_type = v2fd->type();
					int v2size = v2reflection->FieldSize(v2mes, v2fd);
					if (v2field_type == FieldDescriptor::TYPE_INT32) {
						vector<int> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedInt32(v2mes, v2fd, j));
						}
						v2val_int.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_INT64) {
						vector<long> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedInt64(v2mes, v2fd, j));
						}
						v2val_long.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_DOUBLE) {
						vector<double> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedDouble(v2mes, v2fd, j));
						}
						v2val_double.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_STRING) {
						vector<string> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedString(v2mes, v2fd, j));
						}
						v2val_string.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_BOOL) {
						vector<bool> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedBool(v2mes, v2fd, j));
						}
						v2val_bool.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_UINT32) {
						vector<unsigned int> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedUInt32(v2mes, v2fd, j));
						}
						v2val_uint.push_back(val);
					} else if (v2field_type == FieldDescriptor::TYPE_UINT64) {
						vector<unsigned long long> val;
						for (int j = 0; j < v2size; j++) {
							val.push_back(v2reflection->GetRepeatedUInt64(v2mes, v2fd, j));
						}
						v2val_ulong.push_back(val);
					}
				}
				if (v2field_type == FieldDescriptor::TYPE_INT32) tuple.setValue(t_idx, v2val_int, ts);
				else if (v2field_type == FieldDescriptor::TYPE_INT64) tuple.setValue(t_idx, v2val_long, ts);
				else if (v2field_type == FieldDescriptor::TYPE_DOUBLE) tuple.setValue(t_idx, v2val_double, ts);
				else if (v2field_type == FieldDescriptor::TYPE_STRING) tuple.setValue(t_idx, v2val_string, ts);
				else if (v2field_type == FieldDescriptor::TYPE_BOOL) tuple.setValue(t_idx, v2val_bool, ts);
				else if (v2field_type == FieldDescriptor::TYPE_UINT32) tuple.setValue(t_idx, v2val_uint, ts);
				else if (v2field_type == FieldDescriptor::TYPE_UINT64) tuple.setValue(t_idx, v2val_ulong, ts);
				//else loggerWarn(__func__, " Unknown FieldType");
			}
		} else {
			if (field_type == FieldDescriptor::TYPE_INT32) {
				int val = reflection->GetInt32(mes, fd);
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_INT64) {
				long val = reflection->GetInt64(mes, fd);
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_DOUBLE) {
				double val = reflection->GetDouble(mes, fd);
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_STRING) {
				string val = reflection->GetString(mes, fd);
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_BOOL) {
				bool val = reflection->GetBool(mes, fd);
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_UINT32) {
				unsigned int val = reflection->GetUInt32(mes, fd);
				tuple.setValue(t_idx, val, ts);
			} else if (field_type == FieldDescriptor::TYPE_UINT64) {
				unsigned long long val = reflection->GetUInt64(mes, fd);
				tuple.setValue(t_idx, val, ts);
			} else {
				cout << "unknown field_type:" << field_type << endl;
			}
		}			
	}
	/**
	* 動的にシリアライズ (不定形メッセージ対応)
	*
	* @author	Shinichi Kusayama
	* @date		2024/07/03
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	*/
	string ProtobufParser::SerializeToStringDynamically(TupleSet &tupleset)
	{
		Schema schema = tupleset.getSchemaRef();
		return SerializeToStringDynamically(tupleset.getTuples(), &schema);
	}

	/**
	* 動的にシリアライズ (不定形メッセージ対応)
	*
	* @author	Shinichi Kusayama
	* @date		2024/07/03
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	シリアライズされた文字列
	*/
	string ProtobufParser::SerializeToStringDynamically(const vector<Tuple> &tuples, const Schema *schema)
	{
		string retStr = "";
		FileDescriptorProto fileDescriptor;

		// syntax, pacckage, message名の設定
		fileDescriptor.set_name("dynamic_proto");
		fileDescriptor.set_syntax("proto3");
		fileDescriptor.set_package("dm2_proto");
		DescriptorProto* messageDescriptor = fileDescriptor.add_message_type();
		messageDescriptor->set_name("DynamicMessage_dm2is");

		// 行(TupleSet)を構成するメッセージを定義
		// message DynamicMessage_dm2is {
		//	repeated Tuple_set tuple_set = 1;
		// }
		FieldDescriptorProto* repeated_field = messageDescriptor->add_field();
		repeated_field->set_type_name("Tuple_set");
		repeated_field->set_name("tuple_set");
		repeated_field->set_number(1);
		repeated_field->set_label(FieldDescriptorProto_Label_LABEL_REPEATED);
		repeated_field->set_type(FieldDescriptorProto_Type_TYPE_MESSAGE);

		// 列(Tuple)を構成するメッセージを定義。各列（下記でいうidのような名前やuint32のような型) はスキーマ情報から読み取る
		// ex.) message Tuple_set {
		//	uint32 id = 1;
		//	string name = 2;
		// }
		DescriptorProto* tuplesetDescriptor = fileDescriptor.add_message_type();
		tuplesetDescriptor->set_name("Tuple_set");

		int attr_size = 0;
		if (schema == nullptr) {
			attr_size = tuples.at(0).size();
		} else {
			if (tuples.size() > 0) attr_size = schema->getAttributeSize();
		}
		string attrTypeList[attr_size];
		string attrNameList[attr_size];
		
		// *** フィールド（列情報）の構築
		for (int i = 0; i < attr_size; i++) {
			int repeated = 0;
			FieldDescriptorProto_Type type;
			try {
				if (schema == nullptr) {
					// スキーマ未定義につき、col1, col2, ...と採番する
					attrNameList[i] = std::string("col") + to_string(i + 1);
					// スキーマ未定義につき、tupleに格納されたvalueの型を元に、typeとrepeatedをセットする
					any value;
					tuples.at(0).getValueByIdx(i, value);
					std::type_index ti(value.type());
					if (typeStringMap.find(ti) == typeStringMap.end()) {
						Tuple t = tuples.at(0);
						throw std::runtime_error(string(" unknown type - ") + t.getDumpAny(value));
					}
					attrTypeList[i] = typeStringMap[ti];
				} else {
					attrNameList[i] = schema->getAttributeName(i);
					//TODO:もしかしたらいらないかも　typeが存在しない場合はnameを入れる
					attrTypeList[i] = schema->getAttributeType(i) != "" ?  schema->getAttributeType(i) : schema->getAttributeName(i);
					if (attrNameList[i].find(".") != string::npos) {
						attrNameList[i] = attrNameList[i].substr(attrNameList[i].find(".") + 1);
					}
				}
				cout << "attr_size:" << attr_size << ", attrNameList[i]: " << attrNameList[i] << ",  attrTypeList[i]:" << attrTypeList[i] << endl;
				type = AttributeNameToFieldType(attrTypeList[i], repeated);
				
				FieldDescriptorProto* field = tuplesetDescriptor->add_field();
				field->set_name(attrNameList[i]);
				field->set_number(i + 1);
				if (repeated > 0) {
					field->set_label(FieldDescriptorProto_Label_LABEL_REPEATED);
					if (repeated == 2) {
						// 2次元配列用にメッセージを定義
						// ex.) message Tuple_set {
						//	repeated vector2array_ vector2array = 1;
						// }
						// message vector2array_ {
						//   repeated int32 vector2array_item = 1;
						// }
						field->set_type(FieldDescriptorProto_Type_TYPE_MESSAGE);
						field->set_type_name(attrNameList[i] + "_");
						DescriptorProto* vectorDescriptor = fileDescriptor.add_message_type();
						vectorDescriptor->set_name(attrNameList[i] + "_");
						FieldDescriptorProto* vector_field = vectorDescriptor->add_field();
						vector_field->set_name(attrNameList[i] + "_item");
						vector_field->set_number(1);
						vector_field->set_type(type);
						vector_field->set_label(FieldDescriptorProto_Label_LABEL_REPEATED);
					}
				}
				if (repeated < 2) {
					field->set_type(type);
				}
			} catch (const exception &e) {
				string what(e.what());
				loggerWarn(__func__, what);
				return "";
			}
		}

		FieldDescriptorProto* field = tuplesetDescriptor->add_field();
		field->set_name("dm2_create_ts");
		field->set_number(attr_size + 1);
		field->set_type(FieldDescriptorProto_Type_TYPE_INT64);

		// ファイルの構築
		DescriptorPool descriptor_pool;
		const FileDescriptor* dynamic_fd = descriptor_pool.BuildFile(fileDescriptor);
		if (dynamic_fd == nullptr) {
			loggerWarn(__func__, "BuildFile failed.");
			return "";
		}
		// ファイルから最上位となるメッセージ名を検索し、factoryクラスを使ってメッセージを実体化
		// message DynamicMessage_dm2is {
		//	repeated Tuple_set tuple_set = 1;
		// }
		const Descriptor* top_level_mes_des = dynamic_fd->FindMessageTypeByName("DynamicMessage_dm2is");
		const Descriptor* tuple_set_des = dynamic_fd->FindMessageTypeByName("Tuple_set");
		DynamicMessageFactory factory;
		Message* top_level_mes = factory.GetPrototype(top_level_mes_des)->New();

		// 以下は、下記のメッセージと各フィールドの生成処理
		// ex.) message Tuple_set {
		//	uint32 id = 1;
		//	string name = 2;
		// }
		for (Tuple tuple : tuples)
		{
			int tuple_size = tuple.size();
			long ts_first = 0;
			long ts_tmp = 0;
			// タプル数の数だけAddMessageで登録
			Message* tuple_set_mes = top_level_mes->GetReflection()->AddMessage(top_level_mes, top_level_mes_des->FindFieldByName("tuple_set"));
			const Reflection* reflection = tuple_set_mes->GetReflection();
			for (int i = 0; i < tuple_size; i++) {
				any val;
				bool isnull;
				tuple.getValue(i, val, ts_tmp, isnull);
				if (i == 0) ts_first = ts_tmp;
				//cout << attrTypeList[i] << " - " << tuple.getDumpAny(val) << endl;
				if (!isnull) {
					try {
						setAttrValue(reflection, tuple_set_mes, tuple_set_des, val, attrTypeList[i], attrNameList[i], dynamic_fd);
					} catch (const exception &e) {
						string what(e.what());
						loggerWarn(__func__, what + tuple.getDumpAny(val));
						return "";
					}
				} else {
					// nullの場合はフィールドを削除
					const Descriptor* tuple_set_desc = tuple_set_mes->GetDescriptor();
					tuple_set_mes->GetReflection()->ClearField(tuple_set_mes, tuple_set_desc->FindFieldByName(attrNameList[i]));
				}
			}
			tuple_set_mes->GetReflection()->SetInt64(tuple_set_mes, tuple_set_des->FindFieldByName("dm2_create_ts"), ts_first);
		}

		string serialized_data, dynamic_serialized_metadata;
		top_level_mes->SerializeToString(&serialized_data);
		fileDescriptor.SerializeToString(&dynamic_serialized_metadata);
		
		//データ結合("メタデータ"+"データ本体")、"本体データ"と"メタデータ"を結合して受信側に渡す。
		string combined_data = to_string(dynamic_serialized_metadata.length()) + "," + dynamic_serialized_metadata + serialized_data;
		delete top_level_mes;
		/* デバッグ用
		{
			vector<Tuple> tuples;
			Schema schema;
			DeserializeToTupleDynamically(combined_data, tuples, schema);
		}
		*/
		return combined_data;
	}
	/**
	* 属性の種別に合ったProtobufの形式で値をセット
	*
	* @author	Shinichi Kusayama
	* @date		2024/07/08
	*
	* @param [in]	tuple_set_mes	Message
	* @param [in]	tuple_set_des	Descriptor
	* @param [in]	val				値
	* @param [in]	attrType		フィールド
	* @param [in]	attrName		フィールド
	*
	 */
	void ProtobufParser::setAttrValue(const Reflection* reflection, Message *mes, const Descriptor* tuple_set_des, any val, const string &attrType, const string &attrName, const FileDescriptor* dynamic_fd)
	{
		if (attrType == "int" || attrType == "int4") {
			reflection->SetInt32(mes, tuple_set_des->FindFieldByName(attrName), any_cast<int>(val));
		} else if (attrType == "long") {
			reflection->SetInt64(mes, tuple_set_des->FindFieldByName(attrName), any_cast<long>(val));
		} else if (attrType == "double") {
			reflection->SetDouble(mes, tuple_set_des->FindFieldByName(attrName), any_cast<double>(val));
		} else if (attrType == "string" || attrType == "geometry" || attrType == "geography") {
			reflection->SetString(mes, tuple_set_des->FindFieldByName(attrName), any_cast<string>(val));
		} else if (attrType == "bool") {
			reflection->SetBool(mes, tuple_set_des->FindFieldByName(attrName), any_cast<bool>(val));
		} else if (attrType == "uint") {
			reflection->SetUInt32(mes, tuple_set_des->FindFieldByName(attrName), any_cast<uint>(val));
		} else if (attrType == "ulong") {
			reflection->SetUInt64(mes, tuple_set_des->FindFieldByName(attrName), any_cast<unsigned long long>(val));
		} else if (attrType == "vector(int)" || attrType == "vector(int4)") {
			try {
				vector<int> vany = any_cast<vector<int>>(val);
				for (int i = 0; i < (int)vany.size(); i++) {
					reflection->AddInt32(mes, tuple_set_des->FindFieldByName(attrName), vany[i]);
				}
			} catch (...) {
				try {
					vector<any> vany = any_cast<vector<any>>(val);
					for (int i = 0; i < (int)vany.size(); i++) {
						reflection->AddInt32(mes, tuple_set_des->FindFieldByName(attrName), any_cast<int>(vany[i]));
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(long)") {
			try {
				vector<long> vany = any_cast<vector<long>>(val);
				for (int i = 0; i < (int)vany.size(); i++) {
					reflection->AddInt64(mes, tuple_set_des->FindFieldByName(attrName), vany[i]);
				}
			} catch (...) {
				try {
					vector<any> vany = any_cast<vector<any>>(val);
					for (int i = 0; i < (int)vany.size(); i++) {
						reflection->AddInt64(mes, tuple_set_des->FindFieldByName(attrName), any_cast<long>(vany[i]));
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(double)") {
			try {
				vector<double> vany = any_cast<vector<double>>(val);
				for (int i = 0; i < (int)vany.size(); i++) {
					reflection->AddDouble(mes, tuple_set_des->FindFieldByName(attrName), vany[i]);
				}
			} catch (...) {
				try {
					vector<any> vany = any_cast<vector<any>>(val);
					for (int i = 0; i < (int)vany.size(); i++) {
						reflection->AddDouble(mes, tuple_set_des->FindFieldByName(attrName), any_cast<double>(vany[i]));
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(string)"  || attrType == "vector(geometry)" || attrType == "vector(geography)") {
			try {
				vector<string> vany = any_cast<vector<string>>(val);
				for (int i = 0; i < (int)vany.size(); i++) {
					reflection->AddString(mes, tuple_set_des->FindFieldByName(attrName), vany[i]);
				}
			} catch (...) {
				try {
					vector<any> vany = any_cast<vector<any>>(val);
					for (int i = 0; i < (int)vany.size(); i++) {
						reflection->AddString(mes, tuple_set_des->FindFieldByName(attrName), any_cast<string>(vany[i]));
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(bool)") {
			try {
				vector<bool> vany = any_cast<vector<bool>>(val);
				for (int i = 0; i < (int)vany.size(); i++) {
					reflection->AddBool(mes, tuple_set_des->FindFieldByName(attrName), vany[i]);
				}
			} catch (...) {
				try {
					vector<any> vany = any_cast<vector<any>>(val);
					for (int i = 0; i < (int)vany.size(); i++) {
						reflection->AddBool(mes, tuple_set_des->FindFieldByName(attrName), any_cast<bool>(vany[i]));
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(uint)") {
			try {
				vector<uint> vany = any_cast<vector<uint>>(val);
				for (int i = 0; i < (int)vany.size(); i++) {
					reflection->AddUInt32(mes, tuple_set_des->FindFieldByName(attrName), vany[i]);
				}
			} catch (...) {
				try {
					vector<any> vany = any_cast<vector<any>>(val);
					for (int i = 0; i < (int)vany.size(); i++) {
						reflection->AddUInt32(mes, tuple_set_des->FindFieldByName(attrName), any_cast<uint>(vany[i]));
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(ulong)") {
			try {
				vector<unsigned long long> vany = any_cast<vector<unsigned long long>>(val);
				for (int i = 0; i < (int)vany.size(); i++) {
					reflection->AddUInt64(mes, tuple_set_des->FindFieldByName(attrName), vany[i]);
				}
			} catch (...) {
				try {
					vector<any> vany = any_cast<vector<any>>(val);
					for (int i = 0; i < (int)vany.size(); i++) {
						reflection->AddUInt64(mes, tuple_set_des->FindFieldByName(attrName), any_cast<unsigned long long>(vany[i]));
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(vector(int))" || attrType == "vector(vector(int4))") {
			try {
				vector<vector<int>> v2any = any_cast<vector<vector<int>>>(val);
				const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
				for (int i = 0; i < (int)v2any.size(); i++) {
					Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
					const Reflection* reflection = v2mes->GetReflection();
					for (int j = 0; j < (int)v2any[i].size(); j++) {
						reflection->AddInt32(v2mes, attr_des->FindFieldByName(attrName + "_item"), v2any[i][j]);
					}
				}
			} catch (...) {
				try {
					vector<vector<any>> v2any = any_cast<vector<vector<any>>>(val);
					const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
					for (int i = 0; i < (int)v2any.size(); i++) {
						Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
						const Reflection* reflection = v2mes->GetReflection();
						for (int j = 0; j < (int)v2any[i].size(); j++) {
							reflection->AddInt32(v2mes, attr_des->FindFieldByName(attrName + "_item"), any_cast<int>(v2any[i][j]));
						}
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(vector(long))") {
			try {
				vector<vector<long>> v2any = any_cast<vector<vector<long>>>(val);
				const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
				for (int i = 0; i < (int)v2any.size(); i++) {
					Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
					const Reflection* reflection = v2mes->GetReflection();
					for (int j = 0; j < (int)v2any[i].size(); j++) {
						reflection->AddInt64(v2mes, attr_des->FindFieldByName(attrName + "_item"), v2any[i][j]);
					}
				}
			} catch (...) {
				try {
					vector<vector<any>> v2any = any_cast<vector<vector<any>>>(val);
					const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
					for (int i = 0; i < (int)v2any.size(); i++) {
						Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
						const Reflection* reflection = v2mes->GetReflection();
						for (int j = 0; j < (int)v2any[i].size(); j++) {
							reflection->AddInt64(v2mes, attr_des->FindFieldByName(attrName + "_item"), any_cast<long>(v2any[i][j]));
						}
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(vector(double))") {
			try {
				vector<vector<double>> v2any = any_cast<vector<vector<double>>>(val);
				const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
				for (int i = 0; i < (int)v2any.size(); i++) {
					Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
					const Reflection* reflection = v2mes->GetReflection();
					for (int j = 0; j < (int)v2any[i].size(); j++) {
						reflection->AddDouble(v2mes, attr_des->FindFieldByName(attrName + "_item"), v2any[i][j]);
					}
				}
			} catch (...) {
				try {
					vector<vector<any>> v2any = any_cast<vector<vector<any>>>(val);
					const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
					for (int i = 0; i < (int)v2any.size(); i++) {
						Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
						const Reflection* reflection = v2mes->GetReflection();
						for (int j = 0; j < (int)v2any[i].size(); j++) {
							reflection->AddDouble(v2mes, attr_des->FindFieldByName(attrName + "_item"), any_cast<double>(v2any[i][j]));
						}
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(vector(string))" || attrType == "vector(vector(geometry))" || attrType == "vector(vector(geography))") {
			try {
				vector<vector<string>> v2any = any_cast<vector<vector<string>>>(val);
				const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
				for (int i = 0; i < (int)v2any.size(); i++) {
					Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
					const Reflection* reflection = v2mes->GetReflection();
					for (int j = 0; j < (int)v2any[i].size(); j++) {
						reflection->AddString(v2mes, attr_des->FindFieldByName(attrName + "_item"), v2any[i][j]);
					}
				}
			} catch (...) {
				try {
					vector<vector<any>> v2any = any_cast<vector<vector<any>>>(val);
					const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
					for (int i = 0; i < (int)v2any.size(); i++) {
						Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
						const Reflection* reflection = v2mes->GetReflection();
						for (int j = 0; j < (int)v2any[i].size(); j++) {
							reflection->AddString(v2mes, attr_des->FindFieldByName(attrName + "_item"), any_cast<string>(v2any[i][j]));
						}
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(vector(bool))") {
			try {
				vector<vector<bool>> v2any = any_cast<vector<vector<bool>>>(val);
				const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
				for (int i = 0; i < (int)v2any.size(); i++) {
					Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
					const Reflection* reflection = v2mes->GetReflection();
					for (int j = 0; j < (int)v2any[i].size(); j++) {
						reflection->AddBool(v2mes, attr_des->FindFieldByName(attrName + "_item"), v2any[i][j]);
					}
				}
			} catch (...) {
				try {
					vector<vector<any>> v2any = any_cast<vector<vector<any>>>(val);
					const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
					for (int i = 0; i < (int)v2any.size(); i++) {
						Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
						const Reflection* reflection = v2mes->GetReflection();
						for (int j = 0; j < (int)v2any[i].size(); j++) {
							reflection->AddBool(v2mes, attr_des->FindFieldByName(attrName + "_item"), any_cast<bool>(v2any[i][j]));
						}
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(vector(uint))") {
			try {
				vector<vector<unsigned int>> v2any = any_cast<vector<vector<unsigned int>>>(val);
				const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
				for (int i = 0; i < (int)v2any.size(); i++) {
					Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
					const Reflection* reflection = v2mes->GetReflection();
					for (int j = 0; j < (int)v2any[i].size(); j++) {
						reflection->AddUInt32(v2mes, attr_des->FindFieldByName(attrName + "_item"), v2any[i][j]);
					}
				}
			} catch (...) {
				try {
					vector<vector<any>> v2any = any_cast<vector<vector<any>>>(val);
					const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
					for (int i = 0; i < (int)v2any.size(); i++) {
						Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
						const Reflection* reflection = v2mes->GetReflection();
						for (int j = 0; j < (int)v2any[i].size(); j++) {
							reflection->AddUInt32(v2mes, attr_des->FindFieldByName(attrName + "_item"), any_cast<unsigned int>(v2any[i][j]));
						}
					}
				} catch (...) {
				}
			}
		} else if (attrType == "vector(vector(ulong))") {
			try {
				vector<vector<unsigned long long>> v2any = any_cast<vector<vector<unsigned long long>>>(val);
				const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
				for (int i = 0; i < (int)v2any.size(); i++) {
					Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
					const Reflection* reflection = v2mes->GetReflection();
					for (int j = 0; j < (int)v2any[i].size(); j++) {
						reflection->AddUInt64(v2mes, attr_des->FindFieldByName(attrName + "_item"), v2any[i][j]);
					}
				}
			} catch (...) {
				try {
					vector<vector<any>> v2any = any_cast<vector<vector<any>>>(val);
					const Descriptor* attr_des = dynamic_fd->FindMessageTypeByName(attrName + "_");
					for (int i = 0; i < (int)v2any.size(); i++) {
						Message* v2mes = mes->GetReflection()->AddMessage(mes, tuple_set_des->FindFieldByName(attrName));
						const Reflection* reflection = v2mes->GetReflection();
						for (int j = 0; j < (int)v2any[i].size(); j++) {
							reflection->AddUInt64(v2mes, attr_des->FindFieldByName(attrName + "_item"), any_cast<unsigned long long>(v2any[i][j]));
						}
					}
				} catch (...) {
				}
			}
		}
		return;
	}
	/**
	* 属性名からProtobufの型に変換
	*
	* @author	Shinichi Kusayama
	* @date		2024/07/03
	*
	* @param [in]	attrType	フィールド
	* @param [out]	repeated	repeated数
	*
	* @return	文字列
	 */
	FieldDescriptorProto_Type ProtobufParser::AttributeNameToFieldType(const string& attrType, int &repeated)
	{
		if (attrType == "int" || attrType == "int4") {
			return FieldDescriptorProto_Type_TYPE_INT32;
		} else if (attrType == "long") {
			return FieldDescriptorProto_Type_TYPE_INT64;
		} else if (attrType == "double") {
			return FieldDescriptorProto_Type_TYPE_DOUBLE;
		} else if (attrType == "float" || attrType == "float8") {
			return FieldDescriptorProto_Type_TYPE_FLOAT;
		} else if (attrType == "string" || attrType == "geometry" || attrType == "geography") {
			return FieldDescriptorProto_Type_TYPE_STRING;
		} else if (attrType == "bool") {
			return FieldDescriptorProto_Type_TYPE_BOOL;
		} else if (attrType == "uint") {
			return FieldDescriptorProto_Type_TYPE_UINT32;
		} else if (attrType == "ulong") {
			return FieldDescriptorProto_Type_TYPE_UINT64;
		} else if (attrType == "vector(int)" || attrType == "vector(int4)") {
			repeated = 1;
			return FieldDescriptorProto_Type_TYPE_INT32;
		} else if (attrType == "vector(long)") {
			repeated = 1;
			return FieldDescriptorProto_Type_TYPE_INT64;
		} else if (attrType == "vector(double)") {
			repeated = 1;
			return FieldDescriptorProto_Type_TYPE_DOUBLE;
		} else if (attrType == "vector(string)" || attrType == "vector(geometry)" || attrType == "vector(geography)") {
			repeated = 1;
			return FieldDescriptorProto_Type_TYPE_STRING;
		} else if (attrType == "vector(bool)") {
			repeated = 1;
			return FieldDescriptorProto_Type_TYPE_BOOL;
		} else if (attrType == "vector(uint)") {
			repeated = 1;
			return FieldDescriptorProto_Type_TYPE_UINT32;
		} else if (attrType == "vector(ulong)") {
			repeated = 1;
			return FieldDescriptorProto_Type_TYPE_UINT64;
		} else if (attrType == "vector(vector(int))" || attrType == "vector(vector(int4))") {
			repeated = 2;
			return FieldDescriptorProto_Type_TYPE_INT32;
		} else if (attrType == "vector(vector(long))") {
			repeated = 2;
			return FieldDescriptorProto_Type_TYPE_INT64;
		} else if (attrType == "vector(vector(double))") {
			repeated = 2;
			return FieldDescriptorProto_Type_TYPE_DOUBLE;
		} else if (attrType == "vector(vector(string))" || attrType == "vector(vector(geometry))" || attrType == "vector(vector(geography))") {
			repeated = 2;
			return FieldDescriptorProto_Type_TYPE_STRING;
		} else if (attrType == "vector(vector(bool))") {
			repeated = 2;
			return FieldDescriptorProto_Type_TYPE_BOOL;
		} else if (attrType == "vector(vector(uint))") {
			repeated = 2;
			return FieldDescriptorProto_Type_TYPE_UINT32;
		} else if (attrType == "vector(vector(ulong))") {
			repeated = 2;
			return FieldDescriptorProto_Type_TYPE_UINT64;
		} else {
			string msg = "[AttributeNameToFieldType] Not matched attrType: "  + attrType;
			throw std::logic_error(msg);
		}
		return FieldDescriptorProto_Type_TYPE_STRING;
	}
	/**
	* タプルの各属性の型を正しい型に合わせる
	*
	* @author	Shinichi Kusayama
	* @date		2026/1/4
	*
	* @param [in]	tuples		タプル
	* @param [out]	typeList	型リスト
	*
	* @return	変換後のタプル
	 */
	vector<Tuple> ProtobufParser::normalizeTuple(const vector<Tuple> &tuples, const std::vector<std::string> &typeList)
	{
		vector<Tuple> retTuples;
		long ts = 0;
		for (Tuple tuple : tuples) {
			Tuple newTuple(tuple.size());
			for (int i = 0; i < tuple.size(); i++) {
				any val;
				tuple.getValue(i, val, ts);
				any retVal = normalizeAny(val, typeList[i]);
				newTuple.setValue(i, retVal, ts);
			}
			retTuples.push_back(newTuple);
		}
		return retTuples;
	}
	/**
	* anyに入っている型を正しい型に合わせる
	*
	* @author	Shinichi Kusayama
	* @date		2026/1/4
	*
	* @param [in]	value			any型の値
	* @param [out]	expectedType	正しい型
	*
	* @return	変換後のany型
	 */
	any ProtobufParser::normalizeAny(any& value, const std::string& expectedType)
	{
		auto it = typeMap.find(expectedType);
		if (it == typeMap.end())
			throw std::runtime_error("[ProtobufParser::normalizeAny]unknown expected type: " + expectedType);

		// すでに正しい型
		if (std::type_index(value.type()) == it->second)
			return value;

		string s = stringUtil.getAnyString(value);
		any retVal = stringUtil.getAnyValFromString(s, expectedType);

		/*
		const std::type_info& typeInfo = retVal.type();
		if (typeInfo == typeid(int)) {
			cout << "normalize Any-Trans[int]" << s << endl;
		}
		*/
		return retVal;
	}

	/**
	* 接続要求/クエリ登録/クエリキャンセルのデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	*
	* @return	受信メッセージ 
	*
	 */
	dm2_proto::System_request ProtobufParser::systemRequestDeserialize(const string &serializedStr)
	{
		dm2_proto::System_request_dm2is msg;
		msg.ParseFromString(serializedStr);

		return msg.system_request();
	}

	/**
	* 接続要求のデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [out]	username		ユーザ名
	* @param [out]	md5				ハッシュ値
	*
	 */
	void ProtobufParser::createSessionDeserialize(const string &serializedStr, string &username, string &md5)
	{
		dm2_proto::System_request request = systemRequestDeserialize(serializedStr);
		
		username = request.body();
		md5 = request.key();
	}

	/**
	* 接続要求/クエリ登録/クエリキャンセル 応答のデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	*
	* @return	受信メッセージ 
	*
	 */
	dm2_proto::System_response ProtobufParser::systemResponseDeserialize(const string &serializedStr)
	{
		dm2_proto::System_response_dm2is msg;
		msg.ParseFromString(serializedStr);

		return msg.system_response();
	}

	/**
	* 接続要求応答のデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [out]	sessionKey		セッションキー
	* @param [out]	errCode			エラーコード
	* @param [out]	errMessage		エラーメッセージ
	*
	 */
	void ProtobufParser::createSessionResponseDeserialize(const string &serializedStr, string &sessionKey, int &errCode, string &errMessage)
	{
		dm2_proto::System_response response = systemResponseDeserialize(serializedStr);

		string errCodeStr = response.error();

		if (errCodeStr.length() == 0)
		{
			sessionKey = response.value();
		} else {
			errCode = stoi(errCodeStr);
			errMessage = response.value();
		}
	}
	/**
	* クエリ登録エラー応答のデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [out]	errCode			エラーコード
	* @param [out]	errMessage		エラーメッセージ
	*
	 */
	void ProtobufParser::queryErrorReponseDeserialize(const string &serializedStr, int &errCode, string &errMessage)
	{
		dm2_proto::System_response response = systemResponseDeserialize(serializedStr);

		string errCodeStr = response.error();
		errCode = stoi(errCodeStr);
		errMessage = response.value();
	}
	/**
	* クエリキャンセル応答のデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [out]	mngId			クエリ管理番号
	* @param [out]	errCode			エラーコード
	* @param [out]	errMessage		エラーメッセージ
	*
	 */
	void ProtobufParser::cancelResponseDeserialize(const string &serializedStr, unsigned int &mngId, int &errCode, string &errMessage)
	{
		dm2_proto::System_response response = systemResponseDeserialize(serializedStr);

		string errCodeStr = response.error();
		if (errCodeStr.length() == 0) {
			mngId = stoi(response.value());
		}
		else {
			errCode = stoi(errCodeStr);
			errMessage = response.value();
		}
	}

	/**
	* クエリ登録のデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [out]	query			クエリ文
	* @param [out]	senderSID		実行元SID
	* @param [out]	destinationSID	宛先SID
	* @param [out]	receptionPort	受付ポート番号
	* @param [out]	getTcpSession	TCP Sessionをサーバへ要求
	* @param [out]	continuous		継続クエリか判定するフラグ
	*
	 */
	void ProtobufParser::queryDeserialize(const string &serializedStr, string &query, unsigned long long &senderSID, unsigned long long &destinationSID, int &receptionPort, bool &getTcpSession, bool &continuous)
	{
		dm2_proto::System_request request = systemRequestDeserialize(serializedStr);

		query = request.body();
		destinationSID = request.destination();
		senderSID = request.sender();
		receptionPort = request.port();
		getTcpSession = request.tcp_session();
		continuous = request.continuous();
	}

	/**
	* クエリ登録のデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [out]	query			クエリ文
	* @param [out]	destinationSID	宛先SID
	* @param [out]	receptionPort	受付ポート番号
	* @param [out]	continuous		継続クエリか判定するフラグ
	*
	 */
	void ProtobufParser::queryDeserialize(const string &serializedStr, string &query, unsigned long long &destinationSID, int &receptionPort, bool &continuous)
	{
		unsigned long long senderSID;
		bool getTcpSession;
		queryDeserialize(serializedStr, query, senderSID, destinationSID, receptionPort, getTcpSession, continuous);
	}

	/**
	* クエリ登録応答のデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [out]	mngId			クエリ文
	* @param [out]	receptionPort	受付ポート番号
	* @param [out]	errCode			エラーコード
	* @param [out]	errMessage		エラーメッセージ
	*
	 */
	void ProtobufParser::queryReponseDeserialize(const string &serializedStr, unsigned int &mngId, int &receptionPort, int &errCode, string &errMessage)
	{
		dm2_proto::System_response response = systemResponseDeserialize(serializedStr);

		string errCodeStr = response.error();
		if (errCodeStr.length() == 0) {
			mngId = stoi(response.value());
			receptionPort = response.port();
		}
		else {
			errCode = stoi(errCodeStr);
			errMessage = response.value();
		}
	}

	/**
	* クエリ結果のデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [out]	tableName		テーブル名
	* @param [out]	tuples			クエリ結果タプルリスト
	*
	 */
	void ProtobufParser::queryResultDeserialize(const string &serializedStr, const string &tableName, vector<Tuple> &tuples)
	{
		vector<string> nameList, typeList;
		queryResultDeserialize(serializedStr, tableName, tuples, nameList, typeList);
	}

	/**
	* クエリ結果のデシリアライズ
	*
	* @author	Nagoya University
	* @date	2025/10/31
	*
	* @param [in]	serializedStr	シリアライズされた文字列
	* @param [out]	tableName		テーブル名
	* @param [out]	tuples			クエリ結果タプルリスト
	* @param [out]	nameList		カラム名リスト
	* @param [out]	typeList		データタイプリスト
	*
	 */
	void ProtobufParser::queryResultDeserialize(const string &serializedStr, const string &tableName, vector<Tuple> &tuples, vector<string> &nameList, vector<string> &typeList)
	{
		Schema schema;
		if (tableName == "object_info" || tableName == "object_info_processed") {
			objectInfoDeserializeToTuple_0_6_0(serializedStr, tuples, schema);
			if (tuples.size() > 0)
				for(int i = 0; i < tuples.at(0).size(); i++ ) { nameList.push_back(objectInfoNameList_0_6_0[i]); typeList.push_back(objectInfoTypeList_0_6_0[i]);}
		} else if (tableName == "object_info_0_8_0" || tableName == "object_info_0_8_0_processed") {
			objectInfoDeserializeToTuple_0_8_0(serializedStr, tuples, schema);
			if (tuples.size() > 0)
				for(int i = 0; i < tuples.at(0).size(); i++ ) { nameList.push_back(objectInfoNameList_0_8_0[i]); typeList.push_back(objectInfoTypeList_0_8_0[i]);}
		} else if (tableName == "object_info_0_8_1" || tableName == "object_info_0_8_1_processed") {
			objectInfoDeserializeToTuple_0_8_1(serializedStr, tuples, schema);
			if (tuples.size() > 0)
				for(int i = 0; i < tuples.at(0).size(); i++ ) { nameList.push_back(objectInfoNameList_0_8_1[i]); typeList.push_back(objectInfoTypeList_0_8_1[i]);}
		} else if (tableName == "freespace_info") {
			freespaceInfoDeserializeToTuple_0_6_0(serializedStr, tuples, schema);
			if (tuples.size() > 0)
				for(int i = 0; i < tuples.at(0).size(); i++ ) { nameList.push_back(freespaceInfoNameList_0_6_0[i]); typeList.push_back(freespaceInfoTypeList_0_6_0[i]);}
		} else if (tableName == "freespace_info_0_8_0") {
			freespaceInfoDeserializeToTuple_0_8_0(serializedStr, tuples, schema);
			if (tuples.size() > 0)
				for(int i = 0; i < tuples.at(0).size(); i++ ) { nameList.push_back(freespaceInfoNameList_0_8_0[i]); typeList.push_back(freespaceInfoTypeList_0_8_0[i]);}
		} else if (tableName == "freespace_info_0_8_1") {
			freespaceInfoDeserializeToTuple_0_8_1(serializedStr, tuples, schema);
			if (tuples.size() > 0)
				for(int i = 0; i < tuples.at(0).size(); i++ ) { nameList.push_back(freespaceInfoNameList_0_8_1[i]); typeList.push_back(freespaceInfoTypeList_0_8_1[i]);}
		} else if (tableName == "signal_info") {
			signalInfoDeserializeToTuple_0_6_0(serializedStr, tuples, schema);
			if (tuples.size() > 0)
				for(int i = 0; i < tuples.at(0).size(); i++ ) { nameList.push_back(signalInfoNameList_0_6_0[i]); typeList.push_back(signalInfoTypeList_0_6_0[i]);}
		} else if (tableName == "sensor_info") {
			sensorInfoDeserializeToTuple_0_8_0(serializedStr, tuples, schema);
			if (tuples.size() > 0)
				for(int i = 0; i < tuples.at(0).size(); i++ ) { nameList.push_back(sensorInfoNameList_0_8_0[i]); typeList.push_back(sensorInfoTypeList_0_8_0[i]);}
		} else if (tableName == "sensor_info_0_8_1") {
			sensorInfoDeserializeToTuple_0_8_1(serializedStr, tuples, schema);
			if (tuples.size() > 0)
				for(int i = 0; i < tuples.at(0).size(); i++ ) { nameList.push_back(sensorInfoNameList_0_8_1[i]); typeList.push_back(sensorInfoTypeList_0_8_1[i]);}
		} else {
			DeserializeToTupleDynamically(serializedStr, tuples, schema, nameList, typeList, false);
		}
	}


}
