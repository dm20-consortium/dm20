# docker build -t dmi/dm_msgs:foxy .
from ros:foxy

arg ROS_APT_SOURCE_VERSION=1.1.0

copy ./dm_object_info_msgs /opt/ws/src/dm_object_info_msgs
copy ./dm_freespace_info_msgs /opt/ws/src/dm_freespace_info_msgs
copy ./dm_sensor_info_msgs /opt/ws/src/dm_sensor_info_msgs
copy ./dm_signal_info_msgs /opt/ws/src/dm_signal_info_msgs

run apt-get -o Acquire::AllowInsecureRepositories=true update && apt-get install -y curl
run curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo $VERSION_CODENAME)_all.deb"
run apt-get install /tmp/ros2-apt-source.deb
run rm /etc/apt/sources.list.d/ros2-snapshots.list

run apt-get update && \
  apt-get install -y \
    ros-foxy-cyclonedds \
    ros-foxy-rmw-cyclonedds-cpp

env RMW_IMPLEMENTATION=rmw_cyclonedds_cpp

workdir /opt/ws
run bash -c "source /opt/ros/foxy/setup.bash && colcon build --symlink-install"

copy ./entrypoint.sh /
entrypoint ["/entrypoint.sh"]
