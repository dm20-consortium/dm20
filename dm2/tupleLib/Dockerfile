ARG IMAGE_TAG=24.04
FROM dm2/proto:${IMAGE_TAG} AS proto-image
FROM dm2/base:${IMAGE_TAG}

ARG CPLUS_VER=c++17

# avoid hunging up
ENV TZ=Asia/Tokyo
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
  && apt-get install -y --no-install-recommends \
    make \
    build-essential g++ \
    zlib1g-dev \
    libzstd-dev \
    pkg-config \
  && rm -rf /var/lib/apt/lists/*

# protobuf
COPY --from=proto-image /usr/bin/protoc /usr/bin/protoc
COPY --from=proto-image /usr/local/lib/libdm2proto* /usr/local/lib/
COPY --from=proto-image /usr/lib/x86_64-linux-gnu/libprotobuf* /usr/lib/x86_64-linux-gnu/
COPY --from=proto-image /usr/local/include/is /usr/local/include/is
COPY --from=proto-image /usr/include/google /usr/include/google
COPY --from=proto-image /usr/lib/x86_64-linux-gnu/pkgconfig/protobuf.pc /usr/lib/x86_64-linux-gnu/pkgconfig/

RUN ldconfig

COPY common.mk /tmp/common.mk
COPY tupleLib /tmp/tupleLib
WORKDIR /tmp/tupleLib/

RUN make clean
RUN make all CPLUS_VER=${CPLUS_VER}
RUN make install
RUN ldconfig

WORKDIR /
RUN rm -rf /tmp/tupleLib
