arg IMAGE_TAG=20.04

from dm2/proto:${IMAGE_TAG} as proto-image

run apt-get update && \
  apt-get install -y \
    cmake \
    build-essential \
    libboost-all-dev \
    libgflags-dev \
    libgoogle-glog-dev

copy . /tmp/ccam_cool4_sensor_io
run rm -rf /tmp/ccam_cool4_sensor_io/build
workdir /tmp/ccam_cool4_sensor_io/build
run cmake ..
run make install
workdir /
run rm -rf /tmp/ccam_cool4_sensor_io
run ldconfig
