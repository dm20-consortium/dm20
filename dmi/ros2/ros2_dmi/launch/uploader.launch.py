# -*- coding: utf-8 -*-
import os

import launch
from launch_ros.actions import (
    ComposableNodeContainer,
)
from launch.actions import DeclareLaunchArgument as Arg
from launch_ros.descriptions import ComposableNode
from launch.substitutions import (
    TextSubstitution as TS,
    LaunchConfiguration as LC)


def create_uploaders():
    return [
        ComposableNode(
            package='ros2_dmi',
            plugin='ros2_dmi::Uploader',
            namespace='/object_info',
            name='uploader',
            parameters=[{
                'station_id': LC('station_id'),
                'ip_addr': LC('ip_addr'),
                'port': LC('port_up'),
                'dm_user': LC('dm_user'),
                'pass': LC('pass'),
                'diag_interval_sec': LC('uploader_diag_interval_sec'),
                'target_table': 'object_info_0_8_1',
                'topic_name': '/object_info/local',
                'topic_type': 'ObjectInfoArray'}]),
        ComposableNode(
            package='ros2_dmi',
            plugin='ros2_dmi::Uploader',
            namespace='/freespace_info',
            name='uploader',
            parameters=[{
                'station_id': LC('station_id'),
                'ip_addr': LC('ip_addr'),
                'port': LC('port_up'),
                'dm_user': LC('dm_user'),
                'pass': LC('pass'),
                'diag_interval_sec': LC('uploader_diag_interval_sec'),
                'target_table': 'freespace_info_0_8_1',
                'topic_name': '/freespace_info/local',
                'topic_type': 'FreespaceInfoArray'}]),
        ComposableNode(
            package='ros2_dmi',
            plugin='ros2_dmi::Uploader',
            namespace='/signal_info',
            name='uploader',
            parameters=[{
                'station_id': LC('station_id'),
                'ip_addr': LC('ip_addr'),
                'port': LC('port_up'),
                'dm_user': LC('dm_user'),
                'pass': LC('pass'),
                'diag_interval_sec': LC('uploader_diag_interval_sec'),
                'target_table': 'signal_info',
                'topic_name': '/signal_info/local',
                'topic_type': 'SignalInfoArray'}]),
        ComposableNode(
            package='ros2_dmi',
            plugin='ros2_dmi::Uploader',
            namespace='/sensor_info',
            name='uploader',
            parameters=[{
                'station_id': LC('station_id'),
                'ip_addr': LC('ip_addr'),
                'port': LC('port_up'),
                'dm_user': LC('dm_user'),
                'pass': LC('pass'),
                'diag_interval_sec': LC('uploader_diag_interval_sec'),
                'target_table': 'sensor_info_0_8_1',
                'topic_name': '/sensor_info/local',
                'topic_type': 'SensorInfoArray'}]),
            ]


def generate_launch_description():
    # 起動時に設定できるパラメータ ---------------------------
    # 設定例)
    # ros2 launch ros2_dmi all.launch.py ip_addr:="127.0.0.1" dm_pass:=hogehoge query_window_object_info:="rows 30"

    # 機器ID（現在未使用．削除予定．）
    station_id = Arg('station_id', default_value=TS(text='0'))

    # DM2.0が動作しているホストのIPアドレス．
    # DM2.0との導通が確認できない場合，プログラムは起動しない
    ip_addr = Arg('ip_addr', default_value=TS(text='127.0.0.1'))

    # DBにアクセスする際に用いるユーザ名
    dm_user = Arg('dm_user', default_value=TS(text='dm2sampleuser'))

    # DBにアクセスする際に用いるパスワード
    dm_pass = Arg('pass')

    # 情報のアップロード時に用いるポート番号
    # 基本的に，デフォルト値で問題ない
    port_up = Arg('port_up', default_value=TS(text='9002'))

    # Uploader（ROS2->DM2.0PF)で行う変換処理において，必須パラメータの不正検知・警告出力処理の周期を定めるパラメータ[sec]
    uploader_diag_interval_sec = Arg('uploader_diag_interval_sec', default_value=TS(text='300'))
    # 以上 ---------------------------

    uploaders = create_uploaders()

    container = ComposableNodeContainer(
        name='ros2_dmi_manager_uploader',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=uploaders,
        output='log')

    return launch.LaunchDescription([
        station_id, ip_addr, dm_user, dm_pass,
        port_up,
        uploader_diag_interval_sec,
        container])
