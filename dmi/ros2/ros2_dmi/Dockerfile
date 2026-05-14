from dm2/dmclientlib:20.04 as dmclient-image
from dm2/dm_msgs:20.04


copy --from=dmclient-image /usr/local/include/is /usr/local/include/is
copy --from=dmclient-image /usr/local/lib/*.a /usr/local/lib/

run apt-get update \
  && apt-get install -y \
    libboost-all-dev \
    libxerces-c-dev \
    libgeos++-dev


copy . /opt/ws/src/ros2_dmi/

workdir /opt/ws
run ldconfig
run bash -c "source /opt/ros/foxy/setup.bash && colcon build --symlink-install"
