#!/bin/bash

# https://github.com/ros-industrial/docker/blob/master/ros-core-nvidia/lunar/ros_entrypoint.sh
set -e

source "/opt/ws/install/setup.bash"

if [ ! "${SETUP}" == "" ]; then
  source ${SETUP}
fi

exec "$@"
