arg IMAGE_TAG=20.04

from dm2/dmclientlib:${IMAGE_TAG} as dmlib-image
from dm2/proto:${IMAGE_TAG} as base

run apt-get update && \
  apt-get install -y \
    cmake \
    build-essential \
    libboost-all-dev \
    libgflags-dev \
    libgoogle-glog-dev \
    libgeos++-dev \
    libxerces-c-dev \
    libssl-dev \
    zlib1g-dev \
    libzstd-dev \
  && rm -rf /var/lib/apt/lists/*

copy --from=dmlib-image /usr/local/lib/* /usr/local/lib/
copy --from=dmlib-image /usr/local/include/is /usr/local/include/is

# ---- build stage ----
from base as builder

copy ./ccam_cool4_sensor_io /tmp/dmi/ccam_cool4_sensor_io
copy ./protobuf_dmi /tmp/dmi/protobuf_dmi
copy ./cmake /opt/ws/src/cmake

workdir /tmp/dmi/
run set -e && \
  for dir in ccam_cool4_sensor_io protobuf_dmi; do \
    cd /tmp/dmi/${dir} && \
    echo "try to build ${dir}" && \
    rm -rf build && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig; \
  done

# ---- runtime stage ----
from base as runtime

copy --from=builder /usr/local/lib/lib*.so /usr/local/lib/
copy --from=builder /usr/local/include/google /usr/local/include/google
copy --from=builder /usr/local/include/ccam_cool4_sensor_io /usr/local/include/ccam_cool4_sensor_io
copy --from=builder /usr/local/include/protobuf_dmi /usr/local/include/protobuf_dmi
copy --from=builder /usr/local/bin/* /usr/local/bin/
run ldconfig
