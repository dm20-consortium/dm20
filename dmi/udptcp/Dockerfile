ARG IMAGE_TAG=24.04

FROM dm2/proto:${IMAGE_TAG} AS proto-image
FROM dm2/dmclientlib:${IMAGE_TAG} AS base

ARG CPLUS_VER=17

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
  && apt-get install -y --no-install-recommends \
    libgeos++-dev \
    libxerces-c-dev \
    libssl-dev \
    libboost-all-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    cmake \
  && rm -rf /var/lib/apt/lists/*

# protobuf
COPY --from=proto-image /usr/bin/protoc /usr/bin/protoc
COPY --from=proto-image /usr/local/lib/libdm2proto* /usr/local/lib/
COPY --from=proto-image /usr/lib/x86_64-linux-gnu/libprotobuf* /usr/lib/x86_64-linux-gnu/
COPY --from=proto-image /usr/local/include/is /usr/local/include/is
COPY --from=proto-image /usr/include/google /usr/include/google

# ---- build stabe ----
FROM base AS builder

COPY ./cool4_api_dataclass /tmp/dmi/cool4_api_dataclass
COPY ./dmi_utils /tmp/dmi/dmi_utils
COPY ./udp_dmi /tmp/dmi/udpdmi
COPY ./tcp_dmi /tmp/dmi/tcpdmi
COPY ./cmake /tmp/dmi/cmake

WORKDIR /tmp/dmi/
RUN set -e && \
  for dir in cool4_api_dataclass dmi_utils udpdmi tcpdmi; do \
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
COPY --from=builder /usr/local/include/cool4_api_dataclass /usr/local/include/cool4_api_dataclass
COPY --from=builder /usr/local/include/dmi_utils /usr/local/include/dmi_utils
COPY --from=builder /usr/local/include/udp_dmi /usr/local/include/udp_dmi
COPY --from=builder /usr/local/include/tcp_dmi /usr/local/include/tcp_dmi
COPY --from=builder /usr/local/bin/* /usr/local/bin/
RUN ldconfig
