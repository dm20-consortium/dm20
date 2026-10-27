ARG IMAGE_TAG=24.04

FROM dm2/udptcpdmi:${IMAGE_TAG} AS udptcpdmi-image
FROM dm2/protobufdmi:${IMAGE_TAG} AS protobufdmi-image

FROM dm2/proto:${IMAGE_TAG} AS proto-image
FROM dm2/seclib:${IMAGE_TAG} AS seclib-image
FROM dm2/commonlib:${IMAGE_TAG} AS commonlib-image
FROM dm2/tuplelib:${IMAGE_TAG} AS tuplelib-image

FROM dm2/ros2dmi:${IMAGE_TAG}

ENV DEBIAN_FRONTEND=noninteractive

ARG CPLUS_VER=c++17

# avoid hunging up
ENV TZ=Asia/Tokyo
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# required libraries
RUN apt-get update && \
  apt-get install -y --no-install-recommends \
    cmake \
    zlib1g-dev \
    libzstd-dev \
    libgeos++-dev \
    libxerces-c-dev \
    libssl-dev \
    libpq-dev \
    libpqxx-dev \
    liblog4cxx-dev \
    libyaml-cpp-dev \
    libboost-all-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    pkg-config \
  && rm -rf /var/lib/apt/lists/*


# build is
COPY --from=seclib-image /dm20/secLib /dm20/secLib
COPY --from=seclib-image /usr/local/lib/libdm2security.a /usr/local/lib/
COPY --from=commonlib-image /dm20/commonLib /dm20/commonLib
COPY --from=commonlib-image /dm20/csLib /dm20/csLib
COPY --from=commonlib-image /usr/local/lib/*.so /usr/local/lib/

# protobuf
COPY --from=proto-image /usr/bin/protoc /usr/bin/protoc
COPY --from=proto-image /usr/local/lib/libdm2proto* /usr/local/lib/
COPY --from=proto-image /usr/lib/x86_64-linux-gnu/libprotobuf* /usr/lib/x86_64-linux-gnu
COPY --from=proto-image /usr/local/include/is /usr/local/include/is
COPY --from=proto-image /usr/include/google /usr/include/google
#COPY --from=proto-image /usr/lib/x86_64-linux-gnu/pkgconfig/protobuf.pc /usr/lib/x86_64-linux-gnu/pkgconfig/

COPY --from=udptcpdmi-image /usr/local/include/cool4_api_dataclass /usr/local/include/cool4_api_dataclass
COPY --from=udptcpdmi-image /usr/local/include/dmi_utils /usr/local/include/dmi_utils
COPY --from=udptcpdmi-image /usr/local/include/udp_dmi /usr/local/include/udp_dmi
COPY --from=udptcpdmi-image /usr/local/include/tcp_dmi /usr/local/include/tcp_dmi
COPY --from=udptcpdmi-image /usr/local/include/is/*.h /usr/local/include/is/
COPY --from=udptcpdmi-image /usr/local/lib/lib*.so /usr/local/lib/
COPY --from=udptcpdmi-image /usr/local/bin/* /usr/local/bin/
COPY --from=protobufdmi-image /usr/local/include/protobuf_dmi /usr/local/include/protobuf_dmi
COPY --from=protobufdmi-image /usr/local/lib/lib* /usr/local/lib/
# tuplelib
COPY --from=tuplelib-image \
    /usr/local/lib/libtuple.a \
    /usr/local/lib/libtuple.a
    
RUN ldconfig
COPY conf /dm20/conf
COPY is /dm20/is
RUN rm -rf /dm20/is/build
WORKDIR /dm20/is/build
RUN cmake -DROS2_WORKSPACE_DIR=/opt/ws ..
RUN make -j4 install
RUN ldconfig

WORKDIR /
