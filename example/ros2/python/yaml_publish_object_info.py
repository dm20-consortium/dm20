import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy

import yaml
import time

from dm_object_info_msgs.msg import ObjectInfoArray
from dm_object_info_msgs.msg import ObjectClass
from dm_object_info_msgs.msg import ObjectId


class YamlPublisher(Node):

    def __init__(self):
        super().__init__('yaml_publisher')

        # QoS（echoと確実に合わせる）
        qos = QoSProfile(depth=10)
        qos.reliability = ReliabilityPolicy.RELIABLE

        self.publisher_ = self.create_publisher(
            ObjectInfoArray,
            '/object_info/local',
            qos
        )

        # YAML読み込み
        with open('object_info_data.yml', 'r') as f:
            self.data = yaml.safe_load(f)

        # メッセージを一度だけ生成（毎回作らない）
        self.msg = ObjectInfoArray()
        for obj_yaml in self.data['array']:
            obj = self.convert_object(obj_yaml)
            self.msg.array.append(obj)

        # タイマー（1秒ごとにpublish）
        self.timer = self.create_timer(1.0, self.timer_callback)

    def timer_callback(self):
        self.publisher_.publish(self.msg)
        self.get_logger().info('Publishing ObjectInfoArray')

    def convert_object(self, y):
        from dm_object_info_msgs.msg import ObjectInfo

        obj = ObjectInfo()

        # --- 基本 ---
        obj.id.value = y['id']['value']
        obj.time.value = y['time']['value']
        obj.revision.value = y['revision']['value']

        # --- 配列系 ---
        for oc_yaml in y.get('object_class', []):
            oc = ObjectClass()
            oc.id.value = oc_yaml['id']['value']
            oc.confidence.value = oc_yaml['confidence']['value']
            oc.subclass_type.value = oc_yaml['subclass_type']['value']
            oc.subclass_confidence.value = oc_yaml['subclass_confidence']['value']
            obj.object_class.append(oc)

        obj.existency.value = y['existency']['value']

        # --- location ---
        obj.object_location.latitude.value = y['object_location']['latitude']['value']
        obj.object_location.longitude.value = y['object_location']['longitude']['value']
        obj.object_location.altitude.value = y['object_location']['altitude']['value']

        obj.ref_point.value = y['ref_point']['value']

        # --- dynamics ---
        obj.direction.value.value = y['direction']['value']['value']
        obj.direction.accuracy.value = y['direction']['accuracy']['value']

        obj.speed.value.value = y['speed']['value']['value']
        obj.speed.accuracy.value = y['speed']['accuracy']['value']

        obj.yaw_rate.value.value = y['yaw_rate']['value']['value']
        obj.yaw_rate.accuracy.value = y['yaw_rate']['accuracy']['value']

        obj.acceleration.value.value = y['acceleration']['value']['value']
        obj.acceleration.accuracy.value = y['acceleration']['accuracy']['value']

        obj.orientation.value.value = y['orientation']['value']['value']
        obj.orientation.accuracy.value = y['orientation']['accuracy']['value']

        # --- size ---
        obj.size.length.value.value = y['size']['length']['value']['value']
        obj.size.length.accuracy.value = y['size']['length']['accuracy']['value']

        obj.size.width.value.value = y['size']['width']['value']['value']
        obj.size.width.accuracy.value = y['size']['width']['accuracy']['value']

        obj.size.height.value.value = y['size']['height']['value']['value']
        obj.size.height.accuracy.value = y['size']['height']['accuracy']['value']

        # --- vehicle states ---
        obj.static_status.value = y['static_status']['value']
        obj.shift_position.value = y['shift_position']['value']
        obj.steering_angle_front.value = y['steering_angle_front']['value']
        obj.steering_angle_rear.value = y['steering_angle_rear']['value']
        obj.brake_state.value = y['brake_state']['value']
        obj.auxiliary_brake_state.value = y['auxiliary_brake_state']['value']
        obj.throttle_position.value = y['throttle_position']['value']
        obj.exterior_lights.value = y['exterior_lights']['value']

        obj.control_system_states.adaptive_cruise_control_system.value = y['control_system_states']['adaptive_cruise_control_system']['value']
        obj.control_system_states.cooperative_adaptive_cruise_control_system.value = y['control_system_states']['cooperative_adaptive_cruise_control_system']['value']
        obj.control_system_states.pre_crash_safety_system.value = y['control_system_states']['pre_crash_safety_system']['value']
        obj.control_system_states.antilock_brake_system.value = y['control_system_states']['antilock_brake_system']['value']
        obj.control_system_states.traction_control_system.value = y['control_system_states']['traction_control_system']['value']
        obj.control_system_states.electronic_stability_control_system.value = y['control_system_states']['electronic_stability_control_system']['value']
        obj.control_system_states.lane_keeping_assist_system.value = y['control_system_states']['lane_keeping_assist_system']['value']
        obj.control_system_states.lane_departure_warning_system.value = y['control_system_states']['lane_departure_warning_system']['value']

        # --- その他 ---
        obj.vehicle_size_type.value = y['vehicle_size_type']['value']
        obj.vehicle_role.value = y['vehicle_role']['value']
        obj.vehicle_extended_info.value = y['vehicle_extended_info']['value']

        obj.towing_vehicle.value = y['towing_vehicle']['value']
        obj.tracking_status.value = y['tracking_status']['value']
        obj.detection_count.value = y['detection_count']['value']
        obj.lost_count.value = y['lost_count']['value']
        obj.object_age.value = y['object_age']['value']

        # --- 配列系 ---
        for src_yaml in y.get('information_source_list', []):
            src = ObjectId()
            src.value = src_yaml['value']
            obj.information_source_list.append(src)

        return obj


def main():
    rclpy.init()
    node = YamlPublisher()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()