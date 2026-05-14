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


def create_downloaders():
    return [
        ComposableNode(
            package='ros2_dmi',
            plugin='ros2_dmi::Downloader',
            namespace='/object_info',
            name='downloader',
            parameters=[{
                'station_id': LC('station_id'),
                'ip_addr': LC('ip_addr'),
                'port': LC('port_down'),
                'dm_user': LC('dm_user'),
                'pass': LC('pass'),
                'log_interval_sec': LC('downloader_log_interval_sec'),
                'query': LC('query_object_info'),
                'topic_name': '/object_info/dm'}]),
        ComposableNode(
            package='ros2_dmi',
            plugin='ros2_dmi::Downloader',
            namespace='/freespace_info',
            name='downloader',
            parameters=[{
                'station_id': LC('station_id'),
                'ip_addr': LC('ip_addr'),
                'port': LC('port_down'),
                'dm_user': LC('dm_user'),
                'pass': LC('pass'),
                'log_interval_sec': LC('downloader_log_interval_sec'),
                'query': LC('query_freespace_info'),
                'topic_name': '/freespace_info/dm'}]),
        ComposableNode(
            package='ros2_dmi',
            plugin='ros2_dmi::Downloader',
            namespace='/signal_info',
            name='downloader',
            parameters=[{
                'station_id': LC('station_id'),
                'ip_addr': LC('ip_addr'),
                'port': LC('port_down'),
                'dm_user': LC('dm_user'),
                'pass': LC('pass'),
                'log_interval_sec': LC('downloader_log_interval_sec'),
                'query': LC('query_signal_info'),
                'topic_name': '/signal_info/dm'}]),
        ComposableNode(
            package='ros2_dmi',
            plugin='ros2_dmi::Downloader',
            namespace='/sensor_info',
            name='downloader',
            parameters=[{
                'station_id': LC('station_id'),
                'ip_addr': LC('ip_addr'),
                'port': LC('port_down'),
                'dm_user': LC('dm_user'),
                'pass': LC('pass'),
                'log_interval_sec': LC('downloader_log_interval_sec'),
                'query': LC('query_sensor_info'),
                'topic_name': '/sensor_info/dm'}]),
            ]


def generate_launch_description():
    # 起動時に設定できるパラメータ ---------------------------
    # 設定例)
    # ros2 launch ros2_dmi all.launch.py ip_addr:="127.0.0.1" dm_pass:=hogehoge"

    # 機器ID（現在未使用．削除予定．）
    station_id = Arg('station_id', default_value=TS(text='0'))

    # DM2.0が動作しているホストのIPアドレス．
    # DM2.0との導通が確認できない場合，プログラムは起動しない
    ip_addr = Arg('ip_addr', default_value=TS(text='127.0.0.1'))

    # DBにアクセスする際に用いるユーザ名
    dm_user = Arg('dm_user', default_value=TS(text='dm2sampleuser'))

    # DBにアクセスする際に用いるパスワード
    dm_pass = Arg('pass')

    # 情報のダウンロード時に用いるポート番号
    # 基本的に，デフォルト値で問題ない
    port_down = Arg('port_down', default_value=TS(text='9001'))

    # Downloader（DM2.0PF->ROS2)で行う変換処理において，必須パラメータの不正検知・警告出力処理の周期を定めるパラメータ[sec]
    downloader_log_interval_sec = Arg('downloader_log_interval_sec', default_value=TS(text='300'))

    query_object_info = Arg(
        'query_object_info', default_value=TS(
            text='master object_info_0_8_1 select * from object_info_0_8_1 [range 300 msec]'))
    query_freespace_info = Arg(
        'query_freespace_info', default_value=TS(
            text='master freespace_info_0_8_1 select * from freespace_info_0_8_1 [range 300 msec]'))
    query_signal_info = Arg(
        'query_signal_info', default_value=TS(
            text='master signal_info select * from signal_info [range 300 msec]'))
    query_sensor_info = Arg(
        'query_sensor_info', default_value=TS(
            text='master sensor_info_0_8_1 select * from sensor_info_0_8_1 [range 300 msec]'))

    # 以上 ---------------------------

    downloaders = create_downloaders()

    container = ComposableNodeContainer(
        name='ros2_dmi_downloaders',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=downloaders,
        output='log')

    return launch.LaunchDescription([
        station_id, ip_addr, dm_user, dm_pass,
        port_down,
        downloader_log_interval_sec,
        query_object_info,
        query_freespace_info,
        query_signal_info,
        query_sensor_info,
        container])
