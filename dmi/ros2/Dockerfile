arg IMAGE_TAG=20.04

from dm2/proto:${IMAGE_TAG} as proto-image
from dm2/dmclientlib:${IMAGE_TAG} as dmclient-image
from ros:foxy

arg ROS_APT_SOURCE_VERSION=1.1.0

copy ./dm_msgs /opt/ws/src/dm_msgs
copy ./ros2_dmi /opt/ws/src/ros2_dmi
copy ./cmake /opt/ws/src/cmake

copy --from=dmclient-image /usr/local/include/is /usr/local/include/is
copy --from=dmclient-image /usr/local/lib/*.a /usr/local/lib/

run apt-get -o Acquire::AllowInsecureRepositories=true update && apt-get install -y curl
run curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo $VERSION_CODENAME)_all.deb"
run apt-get install /tmp/ros2-apt-source.deb
run rm /etc/apt/sources.list.d/ros2-snapshots.list

run apt-get update && \
  apt-get install -y \
    ros-foxy-cyclonedds \
    ros-foxy-rmw-cyclonedds-cpp \
    libboost-all-dev \
    libxerces-c-dev \
    libgeos++-dev

env RMW_IMPLEMENTATION=rmw_cyclonedds_cpp

copy --from=proto-image /usr/local/bin/protoc /usr/local/bin/protoc
copy --from=proto-image /usr/local/lib/libdm2proto* /usr/local/lib/
copy --from=proto-image /usr/local/lib/libprotobuf* /usr/local/lib/
copy --from=proto-image /usr/local/include/is /usr/local/include/is
copy --from=proto-image /usr/local/include/google /usr/local/include/google

workdir /opt/ws
run ldconfig
run bash -c "source /opt/ros/foxy/setup.bash && colcon build --symlink-install"

copy ./entrypoint.sh /
RUN chmod +x /entrypoint.sh
entrypoint ["/entrypoint.sh"]
