ARG IMAGE_TAG=24.04
ARG ROS_DISTRO=jazzy

FROM dm2/proto:${IMAGE_TAG} AS proto-image
FROM dm2/dmclientlib:${IMAGE_TAG} AS dmclient-image

FROM ros:${ROS_DISTRO}

ARG IMAGE_TAG
ARG ROS_DISTRO

ENV DEBIAN_FRONTEND=noninteractive

ARG ROS_APT_SOURCE_VERSION=1.1.0

COPY ./dm_msgs /opt/ws/src/dm_msgs
COPY ./ros2_dmi /opt/ws/src/ros2_dmi
COPY ./cmake /opt/ws/src/cmake

COPY --from=dmclient-image /usr/local/include/is /usr/local/include/is
COPY --from=dmclient-image /usr/local/lib/*.a /usr/local/lib/

# protobuf
COPY --from=proto-image /usr/bin/protoc /usr/bin/protoc
COPY --from=proto-image /usr/local/lib/libdm2proto* /usr/local/lib/
COPY --from=proto-image /usr/lib/x86_64-linux-gnu/libprotobuf* /usr/lib/x86_64-linux-gnu/
COPY --from=proto-image /usr/local/include/is /usr/local/include/is
COPY --from=proto-image /usr/include/google /usr/include/google

RUN ldconfig

# ROS2 build
WORKDIR /opt/ws
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        libboost-dev \
        libboost-system-dev \
        libboost-thread-dev \
        libboost-regex-dev \
        libxerces-c-dev \
        libgeos++-dev \
        ros-${ROS_DISTRO}-rmw-cyclonedds-cpp \
        ros-${ROS_DISTRO}-topic-tools \
    && rm -rf /var/lib/apt/lists/*
    
RUN bash -c "source /opt/ros/${ROS_DISTRO}/setup.bash && colcon build --symlink-install"

COPY ./entrypoint.sh /
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
