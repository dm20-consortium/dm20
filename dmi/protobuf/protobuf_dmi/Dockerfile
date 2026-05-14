arg IMAGE_TAG=20.04

from ccam_cool4_sensor_io:${IMAGE_TAG} as ccsi-image
from dm2/dmclientlib:${IMAGE_TAG}


copy --from=ccsi-image /usr/local/include/ccam_cool4_sensor_io /usr/local/include/ccam_cool4_sensor_io
copy --from=ccsi-image /usr/local/include/google /usr/local/include/google
copy --from=ccsi-image /usr/local/bin/protoc /usr/local/bin/protoc
copy --from=ccsi-image /usr/local/lib/libccam_cool4_sensor_io_* /usr/local/lib/
copy --from=ccsi-image /usr/local/lib/libprotobuf* /usr/local/lib/
copy --from=ccsi-image /usr/local/share/cmake/ccam_cool4_sensor_io /usr/local/share/cmake/

run apt-get update \
  && apt-get install -y \
    libgeos++-dev \
    libxerces-c-dev \
    libssl-dev \
    libboost-all-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    cmake \
  && rm -rf /var/lib/apt/lists/*

copy . /tmp/pkg/

workdir /tmp/pkg
run rm -rf build
run mkdir build

workdir /tmp/pkg/build
run cmake .. \
  && make -j4 install
run ldconfig

workdir /
run rm -rf /tmp/pkg
