ARG IMAGE_TAG=24.04

FROM dm2/dmclientlib:${IMAGE_TAG} AS dmlib-image
FROM dm2/proto:${IMAGE_TAG} AS base

ARG CPLUS_VER=17

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
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

COPY --from=dmlib-image /usr/local/lib/* /usr/local/lib/
COPY --from=dmlib-image /usr/local/include/is /usr/local/include/is

# ---- build stage ----
FROM base AS builder

COPY ./ccam_cool4_sensor_io /tmp/dmi/ccam_cool4_sensor_io
COPY ./protobuf_dmi /tmp/dmi/protobuf_dmi
COPY ./cmake /tmp/dmi/cmake

WORKDIR /tmp/dmi/
RUN set -e && \
  for dir in ccam_cool4_sensor_io protobuf_dmi; do \
    cd /tmp/dmi/${dir} && \
    echo "try to build ${dir}" && \
    rm -rf build && \
    mkdir build && \
    cd build && \
    cmake -DCPLUS_VER=${CPLUS_VER} .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig; \
  done

# ---- runtime stage ----
FROM base AS runtime

COPY --from=builder /usr/local/lib/lib*.so /usr/local/lib/
COPY --from=builder /usr/local/include/ccam_cool4_sensor_io /usr/local/include/ccam_cool4_sensor_io
COPY --from=builder /usr/local/include/protobuf_dmi /usr/local/include/protobuf_dmi
COPY --from=builder /usr/local/bin/* /usr/local/bin/
RUN ldconfig
