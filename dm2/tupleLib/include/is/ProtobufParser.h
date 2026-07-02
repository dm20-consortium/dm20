#ifndef PROTOBUFPARSER_H
#define PROTOBUFPARSER_H
#include "Tuple.h"
#include "TupleSet.h"
#include "StringUtil.h"
#include "Schema.h"
#include "DmUtil.h"

#include "object_info_0_6_0.pb.h"
#include "object_info_0_8_0.pb.h"
#include "freespace_info_0_6_0.pb.h"
#include "freespace_info_0_8_0.pb.h"
#include "signal_info_0_6_0.pb.h"
#include "sensor_info_0_8_0.pb.h"
#include "dm2is.pb.h"

#include <string>
#include <vector>
#include <typeindex>
//#include <log4cxx/logger.h>

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/message.h>

using namespace std;
//using namespace log4cxx;
using namespace google::protobuf;

// ヘッダ固定部分長　フラグ(1byte) + テーブル名長(2byte) + テーブル名(97byte) + protobufデータ長(4byte) + クエリ管理番号(4byte) + フラグメント番号(1byte) + 全体のフラグメント数(1byte) + キー長(4byte)
#define PROTOBUF_HEADER_FIXED_SIZE 114

struct ProtobufHeader {
	char flag;						// フラグ
	int table_name_length;			// テーブル名長
	string table_name;				// テーブル名
	int payload_size = 0;			// protobufデータ長
	unsigned int mngId = 0;			// クエリ管理番号
	uint8_t fragment_index = 0;		// フラグメント番号
	uint8_t total_fragments = 1;	// 全体のフラグメント数
	int key_length = 0;				// セッションキー長
	string key;						// セッションキー
};
struct ProtobufHeaderInfo {
	ProtobufHeader header;
	const char *payload_p;
	int headerSize = 0;
};

namespace IS {
	/**
	 * Protobufを解析するクラス
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/12/10
	 */

	class ProtobufParser
	{
	private:
		ProtobufParser() = default;
		~ProtobufParser() = default;

		StringUtil stringUtil;

		//LoggerPtr logger = Logger::getLogger("ProtobufParser");

		enum class OBJECT_INFO_POS_0_6_0 {
			id, time, object_class_id_0, object_class_confidence_0, object_class_subclass_type_0, object_class_subclass_confidence_0, object_class_id_1, object_class_confidence_1, object_class_subclass_type_1, object_class_subclass_confidence_1, object_class_id_2, object_class_confidence_2, object_class_subclass_type_2, object_class_subclass_confidence_2, object_class_id_3, object_class_confidence_3, object_class_subclass_type_3, object_class_subclass_confidence_3, existency, geodetic_system, latitude, longitude, altitude, crp_id, dx_crp, dy_crp, dh_crp, lane_count, lane_position, lane_lateral_position, crp_id_begin, crp_id_end, lane_vertical_position, lane_id, dx_lane, dy_lane, dh_lane, semi_axis_length_major, semi_axis_length_minor, ellipse_orientation, altitude_accuracy, reference_point, move_direction_value, move_direction_accuracy, speed_value, speed_accuracy, yaw_rate_value, yaw_rate_accuracy, acceleration_value, acceleration_accuracy, orientation_value, orientation_accuracy, length_value, length_accuracy, width_value, width_accuracy, height_value, height_accuracy, color, shift_position, steering_angle_front, steering_angle_rear, brake_state, auxiliary_brake_state, throttle_position, exterior_lights, adaptive_cruise_control_system, cooperative_adaptive_cruise_control_system, pre_crash_safety_system, antilock_brake_system, traction_control_system, electronic_stability_control_system, lane_keeping_assist_system, lane_departure_warning_system, vehicle_role, vehicle_extended_info, towing_vehicle, information_source_list
		};
		enum class OBJECT_INFO_POS_0_8_0 {
			object_id, time, object_class_id_0, object_class_confidence_0, object_class_subclass_type_0, object_class_subclass_confidence_0, object_class_id_1, object_class_confidence_1, object_class_subclass_type_1, object_class_subclass_confidence_1, object_class_id_2, object_class_confidence_2, object_class_subclass_type_2, object_class_subclass_confidence_2, object_class_id_3, object_class_confidence_3, object_class_subclass_type_3, object_class_subclass_confidence_3, existency, geodetic_srid, latitude, longitude, altitude, projective_srid, prc_x, prc_y, prc_z, crp_id, dx_crp, dy_crp, dh_crp, lane_count, lane_position, lane_lateral_position, crp_id_begin, crp_id_end, lane_vertical_position, lane_id, dx_lane, dy_lane, dh_lane, semi_axis_length_major, semi_axis_length_minor, ellipse_orientation, altitude_accuracy, reference_point, move_direction_value, move_direction_accuracy, speed_value, speed_accuracy, yaw_rate_value, yaw_rate_accuracy, acceleration_value, acceleration_accuracy, orientation_value, orientation_accuracy, length_value, length_accuracy, width_value, width_accuracy, height_value, height_accuracy, static_status, shift_position, steering_angle_front, steering_angle_rear, brake_state, auxiliary_brake_state, throttle_position, exterior_lights, adaptive_cruise_control_system, cooperative_adaptive_cruise_control_system, pre_crash_safety_system, antilock_brake_system, traction_control_system, electronic_stability_control_system, lane_keeping_assist_system, lane_departure_warning_system, vehicle_size_type, vehicle_role, vehicle_extended_info, towing_vehicle, tracking_status, detection_count, lost_count, object_age, information_source_list
		};
		enum class OBJECT_INFO_POS_0_8_1 {
			object_id, time, revision, object_class_id_0, object_class_confidence_0, object_class_subclass_type_0, object_class_subclass_confidence_0, object_class_id_1, object_class_confidence_1, object_class_subclass_type_1, object_class_subclass_confidence_1, object_class_id_2, object_class_confidence_2, object_class_subclass_type_2, object_class_subclass_confidence_2, object_class_id_3, object_class_confidence_3, object_class_subclass_type_3, object_class_subclass_confidence_3, existency, geodetic_srid, latitude, longitude, altitude, projective_srid, prc_x, prc_y, prc_z, crp_id, dx_crp, dy_crp, dh_crp, lane_count, lane_position, lane_lateral_position, crp_id_begin, crp_id_end, lane_vertical_position, lane_id, dx_lane, dy_lane, dh_lane, semi_axis_length_major, semi_axis_length_minor, ellipse_orientation, altitude_accuracy, reference_point, move_direction_value, move_direction_accuracy, speed_value, speed_accuracy, yaw_rate_value, yaw_rate_accuracy, acceleration_value, acceleration_accuracy, orientation_value, orientation_accuracy, length_value, length_accuracy, width_value, width_accuracy, height_value, height_accuracy, static_status, shift_position, steering_angle_front, steering_angle_rear, brake_state, auxiliary_brake_state, throttle_position, exterior_lights, adaptive_cruise_control_system, cooperative_adaptive_cruise_control_system, pre_crash_safety_system, antilock_brake_system, traction_control_system, electronic_stability_control_system, lane_keeping_assist_system, lane_departure_warning_system, vehicle_size_type, vehicle_role, vehicle_extended_info, towing_vehicle, tracking_status, detection_count, lost_count, object_age, information_source_list
		};
		enum class FREESPACE_INFO_POS_0_6_0 {
			id, time, existency, minimal_detectable_size, position_begin_geodetic_system, position_begin_latitude, position_begin_longitude, position_begin_altitude, position_begin_crp_id, position_begin_dx_crp, position_begin_dy_crp, position_begin_dh_crp, position_begin_lane_count, position_begin_lane_position, position_begin_lane_lateral_position, position_begin_crp_id_begin, position_begin_crp_id_end, position_begin_lane_vertical_position, position_begin_lane_id, position_begin_dx_lane, position_begin_dy_lane, position_begin_dh_lane, position_begin_semi_axis_length_major, position_begin_semi_axis_length_minor, position_begin_ellipse_orientation, position_begin_altitude_accuracy, position_end_geodetic_system, position_end_latitude, position_end_longitude, position_end_altitude, position_end_crp_id, position_end_dx_crp, position_end_dy_crp, position_end_dh_crp, position_end_lane_count, position_end_lane_position, position_end_lane_lateral_position, position_end_crp_id_begin, position_end_crp_id_end, position_end_lane_vertical_position, position_end_lane_id, position_end_dx_lane, position_end_dy_lane, position_end_dh_lane, position_end_semi_axis_length_major, position_end_semi_axis_length_minor, position_end_ellipse_orientation, position_end_altitude_accuracy, length_value, length_accuracy, id_begin, id_end, information_source_list
		};
		enum class FREESPACE_INFO_POS_0_8_0 {
			id, time, detection_method, detactable_classes, vertices_begin_geodetic_srid, vertices_begin_latitude, vertices_begin_longitude, vertices_begin_altitude, vertices_begin_projective_srid, vertices_begin_prc_x, vertices_begin_prc_y, vertices_begin_prc_z, vertices_begin_crp_id, vertices_begin_dx_crp, vertices_begin_dy_crp, vertices_begin_dh_crp, vertices_begin_lane_count, vertices_begin_lane_position, vertices_begin_lane_lateral_position, vertices_begin_crp_id_begin, vertices_begin_crp_id_end, vertices_begin_lane_vertical_position, vertices_begin_lane_id, vertices_begin_dx_lane, vertices_begin_dy_lane, vertices_begin_dh_lane, vertices_begin_semi_axis_length_major, vertices_begin_semi_axis_length_minor, vertices_begin_ellipse_orientation, vertices_begin_altitude_accuracy, vertices_dx_list, vertices_dy_list, position_begin_geodetic_srid, position_begin_latitude, position_begin_longitude, position_begin_altitude, position_begin_projective_srid, position_begin_prc_x, position_begin_prc_y, position_begin_prc_z, position_begin_crp_id, position_begin_dx_crp, position_begin_dy_crp, position_begin_dh_crp, position_begin_lane_count, position_begin_lane_position, position_begin_lane_lateral_position, position_begin_crp_id_begin, position_begin_crp_id_end, position_begin_lane_vertical_position, position_begin_lane_id, position_begin_dx_lane, position_begin_dy_lane, position_begin_dh_lane, position_begin_semi_axis_length_major, position_begin_semi_axis_length_minor, position_begin_ellipse_orientation, position_begin_altitude_accuracy, position_end_geodetic_srid, position_end_latitude, position_end_longitude, position_end_altitude, position_end_projective_srid, position_end_prc_x, position_end_prc_y, position_end_prc_z, position_end_crp_id, position_end_dx_crp, position_end_dy_crp, position_end_dh_crp, position_end_lane_count, position_end_lane_position, position_end_lane_lateral_position, position_end_crp_id_begin, position_end_crp_id_end, position_end_lane_vertical_position, position_end_lane_id, position_end_dx_lane, position_end_dy_lane, position_end_dh_lane, position_end_semi_axis_length_major, position_end_semi_axis_length_minor, position_end_ellipse_orientation, position_end_altitude_accuracy, length_value, length_accuracy, id_begin, id_end, existency, minimal_detectable_size, information_source_list
		};
		enum class FREESPACE_INFO_POS_0_8_1 {
			id, time, revision, detection_method, detactable_classes, vertices_begin_geodetic_srid, vertices_begin_latitude, vertices_begin_longitude, vertices_begin_altitude, vertices_begin_projective_srid, vertices_begin_prc_x, vertices_begin_prc_y, vertices_begin_prc_z, vertices_begin_crp_id, vertices_begin_dx_crp, vertices_begin_dy_crp, vertices_begin_dh_crp, vertices_begin_lane_count, vertices_begin_lane_position, vertices_begin_lane_lateral_position, vertices_begin_crp_id_begin, vertices_begin_crp_id_end, vertices_begin_lane_vertical_position, vertices_begin_lane_id, vertices_begin_dx_lane, vertices_begin_dy_lane, vertices_begin_dh_lane, vertices_begin_semi_axis_length_major, vertices_begin_semi_axis_length_minor, vertices_begin_ellipse_orientation, vertices_begin_altitude_accuracy, vertices_dx_list, vertices_dy_list, position_begin_geodetic_srid, position_begin_latitude, position_begin_longitude, position_begin_altitude, position_begin_projective_srid, position_begin_prc_x, position_begin_prc_y, position_begin_prc_z, position_begin_crp_id, position_begin_dx_crp, position_begin_dy_crp, position_begin_dh_crp, position_begin_lane_count, position_begin_lane_position, position_begin_lane_lateral_position, position_begin_crp_id_begin, position_begin_crp_id_end, position_begin_lane_vertical_position, position_begin_lane_id, position_begin_dx_lane, position_begin_dy_lane, position_begin_dh_lane, position_begin_semi_axis_length_major, position_begin_semi_axis_length_minor, position_begin_ellipse_orientation, position_begin_altitude_accuracy, position_end_geodetic_srid, position_end_latitude, position_end_longitude, position_end_altitude, position_end_projective_srid, position_end_prc_x, position_end_prc_y, position_end_prc_z, position_end_crp_id, position_end_dx_crp, position_end_dy_crp, position_end_dh_crp, position_end_lane_count, position_end_lane_position, position_end_lane_lateral_position, position_end_crp_id_begin, position_end_crp_id_end, position_end_lane_vertical_position, position_end_lane_id, position_end_dx_lane, position_end_dy_lane, position_end_dh_lane, position_end_semi_axis_length_major, position_end_semi_axis_length_minor, position_end_ellipse_orientation, position_end_altitude_accuracy, length_value, length_accuracy, id_begin, id_end, existency, minimal_detectable_size, information_source_list
		};
		enum class SIGNAL_INFO_POS_0_6_0 {
			crp_id, id, time, state, specific_control_flags, event_count, count_down_stop_flag, light_info_main_0, light_info_arrow_0, light_info_min_time_to_change_0, light_info_max_time_to_change_0, light_info_main_1, light_info_arrow_1, light_info_min_time_to_change_1, light_info_max_time_to_change_1, light_info_main_2, light_info_arrow_2, light_info_min_time_to_change_2, light_info_max_time_to_change_2, light_info_main_3, light_info_arrow_3, light_info_min_time_to_change_3, light_info_max_time_to_change_3, light_info_main_4, light_info_arrow_4, light_info_min_time_to_change_4, light_info_max_time_to_change_4, light_info_main_5, light_info_arrow_5, light_info_min_time_to_change_5, light_info_max_time_to_change_5, light_info_main_6, light_info_arrow_6, light_info_min_time_to_change_6, light_info_max_time_to_change_6, light_info_main_7, light_info_arrow_7, light_info_min_time_to_change_7, light_info_max_time_to_change_7, light_info_main_8, light_info_arrow_8, light_info_min_time_to_change_8, light_info_max_time_to_change_8, light_info_main_9, light_info_arrow_9, light_info_min_time_to_change_9, light_info_max_time_to_change_9, light_info_main_10, light_info_arrow_10, light_info_min_time_to_change_10, light_info_max_time_to_change_10, light_info_main_11, light_info_arrow_11, light_info_min_time_to_change_11, light_info_max_time_to_change_11
		};
		enum class SENSOR_INFO_POS_0_8_0 {
			unit_id, sensor_id, sensor_type, geodetic_srid, latitude, longitude, altitude, projective_srid, prc_x, prc_y, prc_z, crp_id, dx_crp, dy_crp, dh_crp, lane_count, lane_position, lane_lateral_position, crp_id_begin, crp_id_end, lane_vertical_position, lane_id, dx_lane, dy_lane, dh_lane, semi_axis_length_major, semi_axis_length_minor, ellipse_orientation, altitude_accuracy, detectable_classes_0, detectable_domain_x_list_0, detectable_domain_y_list_0, confidence_0, minimal_detectable_size_0, detectable_classes_1, detectable_domain_x_list_1, detectable_domain_y_list_1, confidence_1, minimal_detectable_size_1, detectable_classes_2, detectable_domain_x_list_2, detectable_domain_y_list_2, confidence_2, minimal_detectable_size_2, detectable_classes_3, detectable_domain_x_list_3, detectable_domain_y_list_3, confidence_3, minimal_detectable_size_3, detectable_classes_4, detectable_domain_x_list_4, detectable_domain_y_list_4, confidence_4, minimal_detectable_size_4, detectable_classes_5, detectable_domain_x_list_5, detectable_domain_y_list_5, confidence_5, minimal_detectable_size_5, detectable_classes_6, detectable_domain_x_list_6, detectable_domain_y_list_6, confidence_6, minimal_detectable_size_6, detectable_classes_7, detectable_domain_x_list_7, detectable_domain_y_list_7, confidence_7, minimal_detectable_size_7, sensor_status
		};
		enum class SENSOR_INFO_POS_0_8_1 {
			unit_id, sensor_id, sensor_type, geodetic_srid, latitude, longitude, altitude, projective_srid, prc_x, prc_y, prc_z, crp_id, dx_crp, dy_crp, dh_crp, lane_count, lane_position, lane_lateral_position, crp_id_begin, crp_id_end, lane_vertical_position, lane_id, dx_lane, dy_lane, dh_lane, semi_axis_length_major, semi_axis_length_minor, ellipse_orientation, altitude_accuracy, time, detectable_classes_0, detectable_domain_x_list_0, detectable_domain_y_list_0, confidence_0, minimal_detectable_size_0, detectable_classes_1, detectable_domain_x_list_1, detectable_domain_y_list_1, confidence_1, minimal_detectable_size_1, detectable_classes_2, detectable_domain_x_list_2, detectable_domain_y_list_2, confidence_2, minimal_detectable_size_2, detectable_classes_3, detectable_domain_x_list_3, detectable_domain_y_list_3, confidence_3, minimal_detectable_size_3, detectable_classes_4, detectable_domain_x_list_4, detectable_domain_y_list_4, confidence_4, minimal_detectable_size_4, detectable_classes_5, detectable_domain_x_list_5, detectable_domain_y_list_5, confidence_5, minimal_detectable_size_5, detectable_classes_6, detectable_domain_x_list_6, detectable_domain_y_list_6, confidence_6, minimal_detectable_size_6, detectable_classes_7, detectable_domain_x_list_7, detectable_domain_y_list_7, confidence_7, minimal_detectable_size_7, sensor_status
		};
		enum class IS_TUPLE_INFO_POS {
			dm2_creator_id, dm2_ip_addr, dm2_hash, dm2_ts_group
		};
		void setObjectInfo(cool4_api_0_6_0::Object_info* obj, dm2_proto::Is_tuple_info *is_info, Tuple tuple);
		void setObjectInfo(cool4_api_0_8_0::Object_info_0_8_0* obj, dm2_proto::Is_tuple_info *is_info, Tuple tuple);
		void setObjectInfo(cool4_api_0_8_0::Object_info_0_8_1* obj, dm2_proto::Is_tuple_info *is_info, Tuple tuple);
		void setFreespaceInfo(cool4_api_0_6_0::Freespace_info* fre, dm2_proto::Is_tuple_info *is_info, Tuple tuple);
		void setFreespaceInfo(cool4_api_0_8_0::Freespace_info_0_8_0* fre, dm2_proto::Is_tuple_info *is_info, Tuple tuple);
		void setFreespaceInfo(cool4_api_0_8_0::Freespace_info_0_8_1* fre, dm2_proto::Is_tuple_info *is_info, Tuple tuple);
		void setSignalInfo(cool4_api_0_6_0::Signal_info* sig, dm2_proto::Is_tuple_info *is_info, Tuple tuple);
		void setSensorInfo(cool4_api_0_8_0::Sensor_info_0_8_0* sig, dm2_proto::Is_tuple_info *is_info, Tuple tuple);
		void setSensorInfo(cool4_api_0_8_0::Sensor_info_0_8_1* sig, dm2_proto::Is_tuple_info *is_info, Tuple tuple);
		void setIsTupleInfo(dm2_proto::Is_tuple_info *is_info, vector<any> &vals, const int &curIdx);
		void isTupleInfoDeserializeToTuple(Tuple &tuple, const dm2_proto::Is_tuple_info &is_info, long &ts, const int &curIdx);

		void loggerWarn(const char *func, const string &msg);
		void debugObjectInfo(const cool4_api_0_6_0::Object_info* obj, dm2_proto::Is_tuple_info *is_info);
		void debugObjectInfo(const cool4_api_0_8_0::Object_info_0_8_0* obj, dm2_proto::Is_tuple_info *is_info);
		void debugObjectInfo(const cool4_api_0_8_0::Object_info_0_8_1* obj, dm2_proto::Is_tuple_info *is_info);
		void debugFreespaceInfo(const cool4_api_0_6_0::Freespace_info* fre, dm2_proto::Is_tuple_info *is_info);
		void debugFreespaceInfo(const cool4_api_0_8_0::Freespace_info_0_8_0* fre, dm2_proto::Is_tuple_info *is_info);
		void debugFreespaceInfo(const cool4_api_0_8_0::Freespace_info_0_8_1* fre, dm2_proto::Is_tuple_info *is_info);
		void debugSignalInfo(const cool4_api_0_6_0::Signal_info* sig, dm2_proto::Is_tuple_info *is_info);
		void debugSensorInfo(const cool4_api_0_8_0::Sensor_info_0_8_0* sig, dm2_proto::Is_tuple_info *is_info);
		void debugSensorInfo(const cool4_api_0_8_0::Sensor_info_0_8_1* sig, dm2_proto::Is_tuple_info *is_info);

		string FieldTypeToString(const FieldDescriptor *field);
		FieldDescriptorProto_Type AttributeNameToFieldType(const string& attrName, int &repeated);
		void setAttrValue(const Reflection* reflection, Message *mes, const Descriptor* tuple_set_des, any val, const string &attrType, const string &attrName, const FileDescriptor* dynamic_fd);
		void setAttrValue(const Reflection* reflection, Message *mes, const Descriptor* tuple_set_des, any val, const string &attrName, const FileDescriptor* dynamic_fd);
		void setTupleAttrValue(const Reflection* reflection, const Message& mes, const FieldDescriptor* field_descriptor, Tuple &tuple, int j, long ts, const string &field_name);
		void setTupleAttrValue(const Reflection* reflection, const Message& mes, const FieldDescriptor* fd, Tuple &tuple, int t_idx, long ts, const string &field_name, bool isNull);
		void checkAndNormalizedTuple(const FieldDescriptor* fd, Tuple &tuple, int t_idx, const string &attrType);

		FieldDescriptorProto_Type ValueToFieldType(const any value, int &repeated);

		string createSessionSerializeToString(const string &username, const string &md5);
		string systemResponseSerializeToString(const string type, const string &value, const int tcpPortconst = -1, const string &errCode = "");
		string querySerializeToString(const string &query, const string &key, const bool &continuous, const int* receptionPort_p = nullptr,
			const unsigned long long* senderSID_p = nullptr, const unsigned long long* destionationSID_p = nullptr, const bool* getTcpSession_p = nullptr);
		string queryResultSerializeToString(const string &tableName, TupleSet& ts, const bool isDynamicColumn);
		string createCancelSerializeToString(const unsigned int mngId, const string &key);
		string streamSerializeToString(const string &tableName, const vector<Tuple> &tuples);

		using TypeMap = std::map<std::string, std::type_index>;

		TypeMap typeMap = {
			{"int",            typeid(int)},
			{"uint",           typeid(unsigned int)},
			{"long",           typeid(long)},
			{"ulong",          typeid(unsigned long long)},
			{"double",         typeid(double)},
			{"string",         typeid(std::string)},
			{"bool",           typeid(bool)},
			{"vector(int)",    typeid(std::vector<int>)},
			{"vector(uint)",    typeid(std::vector<unsigned int>)},
			{"vector(long)",   typeid(std::vector<long>)},
			{"vector(ulong)",  typeid(std::vector<unsigned long long>)},
			{"vector(double)", typeid(std::vector<double>)},
			{"vector(string)", typeid(std::vector<std::string>)},
			{"vector(bool)",   typeid(std::vector<bool>)},
			{"vector(vector(int))",    typeid(std::vector<std::vector<int>>)},
			{"vector(vector(uint))",    typeid(std::vector<std::vector<unsigned int>>)},
			{"vector(vector(long))",   typeid(std::vector<std::vector<long>>)},
			{"vector(vector(ulong))",  typeid(std::vector<std::vector<unsigned long long>>)},
			{"vector(vector(double))", typeid(std::vector<std::vector<double>>)},
			{"vector(vector(string))", typeid(std::vector<std::vector<std::string>>)},
			{"vector(vector(bool))",   typeid(std::vector<std::vector<bool>>)}
		};
		map<FieldDescriptor::Type, string> protoTypeMap = {
			{FieldDescriptor::TYPE_INT32, "int"}, 
			{FieldDescriptor::TYPE_INT64, "long"}, 
			{FieldDescriptor::TYPE_DOUBLE, "double"}, 
			{FieldDescriptor::TYPE_STRING, "string"}, 
			{FieldDescriptor::TYPE_BOOL,   "bool"}, 
			{FieldDescriptor::TYPE_UINT32, "ulong"}, 
			{FieldDescriptor::TYPE_UINT64, "ulong"}
		};
		vector<Tuple> normalizeTuple(const vector<Tuple> &tuples, const std::vector<std::string> &typeList);
		any normalizeAny(any& value, const std::string& expectedType);
	public:
		ProtobufParser(const ProtobufParser&) = delete;
		ProtobufParser& operator=(const ProtobufParser&) = delete;
		ProtobufParser(ProtobufParser&&) = delete;
		ProtobufParser& operator=(ProtobufParser&&) = delete;

		static ProtobufParser& get_instance() {
			static ProtobufParser inst;
			return inst;
		}

		void init();
		void finalize();

		string createHeader(const unsigned int mngId, const string &tableName, const string &serializedStr);
		string createHeader(const unsigned int mngId, const string &tableName, const string &serializedStr, size_t strLength, short fragmentIndex = 0, short totalFragments = 1, const string &key = "");

		void getProtobufHeaderInfo(const string &receiveData, ProtobufHeaderInfo &info);
		string createCreateSession(const string &username, const string &md5);
		string createCreateSessionResponse(const string &sessionKey);
		string createCreateSessionErrorResponse(const string &errCode, const string &errMessage);
		string createQuery(const string &query, const string &key, const int receptionPort, const bool &getTcpSession, const bool &continuous);
		string createQuery(const string &query, const string &key, unsigned long long destinationSID, const bool &continuous);
		string createQueryResponse(const unsigned int mngId, const int tcpPort);
		string createQueryErrorResponse(const string &errCode, const string &errMessage);
		string createCancel(const unsigned int mngId, const string &key);
		string createCancelResponse(const unsigned int mngId);
		string createCancelErrorResponse(const string &errCode, const string &errMessage);

		string createQueryResult(const unsigned int mngId, const string &tableName, TupleSet& ts, const bool isDynamicColumn);
		vector<string> createQueryResult(const unsigned int mngId, const string &tableName, TupleSet& ts, const bool isDynamicColumn, const int sepSize);
		vector<string> createStreamList(const string &tableName, const vector<Tuple> &tuples, const string &key, const int sepSize);

		dm2_proto::System_request systemRequestDeserialize(const string &serializedStr);
		void createSessionDeserialize(const string &serializedStr, string &username, string &md5);
		dm2_proto::System_response systemResponseDeserialize(const string &serializedStr);
		void createSessionResponseDeserialize(const string &serializedStr, string &sessionKey, int &errCode, string &errMessage);
		void queryDeserialize(const string &serializedStr, string &query, unsigned long long &senderSID, unsigned long long &destinationSID, int &receptionPort, bool &getTcpSession, bool &continuous);
		void queryDeserialize(const string &serializedStr, string &query, unsigned long long &destinationSID, int &receptionPort, bool &continuous);
		void queryReponseDeserialize(const string &serializedStr, unsigned int &mngId, int &receptionPort, int &errCode, string &errMessage);
		void queryErrorReponseDeserialize(const string &serializedStr, int &errCode, string &errMessage);
		void cancelResponseDeserialize(const string &serializedStr, unsigned int &mngId, int &errCode, string &errMessage);
		void queryResultDeserialize(const string &serializedStr, const string &tableName, vector<Tuple> &tuples);
		void queryResultDeserialize(const string &serializedStr, const string &tableName, vector<Tuple> &tuples, vector<string> &nameList, vector<string> &typeList);

		string SerializeToStringDynamically(TupleSet &tupleset);
		string SerializeToStringDynamically(const vector<Tuple> &tuples, const Schema *schema = nullptr);
		void DeserializeToTupleDynamically(string serializedStr, vector<Tuple> &tuples, const Schema &schema, const bool &payloadCheck);
		void DeserializeToTupleDynamically(string serializedStr, vector<Tuple> &tuples, const Schema &schema, vector<string> &fieldNameList, vector<string> &fieldTypeList, const bool &payloadCheck);

		string objectInfoSerializeToString_0_6_0(const vector<Tuple> &tuples);
		string objectInfoSerializeToString_0_6_0(TupleSet &tupleset);
		string objectInfoSerializeToString_0_8_0(const vector<Tuple> &tuples);
		string objectInfoSerializeToString_0_8_0(TupleSet &tupleset);
		string objectInfoSerializeToString_0_8_1(const vector<Tuple> &tuples);
		string objectInfoSerializeToString_0_8_1(TupleSet &tupleset);
		string freespaceInfoSerializeToString_0_6_0(const vector<Tuple> &tuples);
		string freespaceInfoSerializeToString_0_6_0(TupleSet &tupleset);
		string freespaceInfoSerializeToString_0_8_0(const vector<Tuple> &tuples);
		string freespaceInfoSerializeToString_0_8_0(TupleSet &tupleset);
		string freespaceInfoSerializeToString_0_8_1(const vector<Tuple> &tuples);
		string freespaceInfoSerializeToString_0_8_1(TupleSet &tupleset);
		string signalInfoSerializeToString_0_6_0(const vector<Tuple> &tuples);
		string signalInfoSerializeToString_0_6_0(TupleSet &tupleset);
		string sensorInfoSerializeToString_0_8_0(const vector<Tuple> &tuples);
		string sensorInfoSerializeToString_0_8_0(TupleSet &tupleset);
		string sensorInfoSerializeToString_0_8_1(const vector<Tuple> &tuples);
		string sensorInfoSerializeToString_0_8_1(TupleSet &tupleset);
		void objectInfoDeserializeToTuple_0_6_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema);
		void objectInfoDeserializeToTuple_0_8_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema);
		void objectInfoDeserializeToTuple_0_8_1(string serializedStr, vector<Tuple> &tuples, const Schema &schema);
		void freespaceInfoDeserializeToTuple_0_6_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema);
		void freespaceInfoDeserializeToTuple_0_8_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema);
		void freespaceInfoDeserializeToTuple_0_8_1(string serializedStr, vector<Tuple> &tuples, const Schema &schema);
		void signalInfoDeserializeToTuple_0_6_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema);
		void sensorInfoDeserializeToTuple_0_8_0(string serializedStr, vector<Tuple> &tuples, const Schema &schema);
		void sensorInfoDeserializeToTuple_0_8_1(string serializedStr, vector<Tuple> &tuples, const Schema &schema);
	};
}
#endif  // PROTOBUFPARSER_H
