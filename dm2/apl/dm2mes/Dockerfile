ARG IMAGE_TAG=24.04
FROM dm2/proto:${IMAGE_TAG} AS proto-image
FROM dm2/tuplelib:${IMAGE_TAG} AS tuplelib-image
FROM dm2/seclib:${IMAGE_TAG} AS seclib-image
FROM dm2/commonlib:${IMAGE_TAG} AS commonlib-image
FROM dm2/dmclientlib:${IMAGE_TAG} AS dmclientlib-image
FROM dm2/dmclientlib:${IMAGE_TAG} AS dmclientlib-image

ARG CPLUS_VER=c++17

COPY --from=seclib-image /dm20/secLib /dm20/secLib
COPY --from=seclib-image /usr/local/lib/libdm2security.a /usr/local/lib/

# avoid hunging up
ENV TZ=Asia/Tokyo
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

ENV DEBIAN_FRONTEND=noninteractive

# required libraries
RUN apt-get update && \
  apt-get install -y --no-install-recommends \
    wget \
    zlib1g-dev \
    libzstd-dev \
    libgeos++-dev \
    libxerces-c-dev \
    libssl-dev \
    libpq-dev \
    libpqxx-dev \
    liblog4cxx-dev \
    pkg-config \
  && rm -rf /var/lib/apt/lists/*

COPY --from=tuplelib-image /usr/local/include/is /usr/local/include/is
COPY --from=tuplelib-image /usr/local/lib/libtuple.a /usr/local/lib/

# protobuf
COPY --from=proto-image /usr/bin/protoc /usr/bin/protoc
COPY --from=proto-image /usr/local/lib/libdm2proto* /usr/local/lib/
COPY --from=proto-image /usr/lib/x86_64-linux-gnu/libprotobuf* /usr/lib/x86_64-linux-gnu
COPY --from=proto-image /usr/local/include/is /usr/local/include/is
COPY --from=proto-image /usr/include/google /usr/include/google

COPY --from=commonlib-image /dm20/commonLib /tmp/commonLib
COPY --from=commonlib-image /dm20/csLib /tmp/csLib
COPY --from=commonlib-image /usr/local/lib/libdm2SocketInterface.so /usr/local/lib/libdm2SocketInterface.so
#COPY --from=dmclientlib-image /usr/local/include/is /usr/local/include/is
#COPY --from=dmclientlib-image /usr/local/lib/libtuple.a /usr/local/lib/libtuple.a
#COPY --from=dmclientlib-image /usr/local/lib/libdmclient.a /usr/local/lib/libdmclient.a
RUN ldconfig

COPY common.mk /tmp/common.mk
COPY apl/dm2mes /tmp/apl/dm2mes
workdir /tmp/apl/dm2mes/


RUN make clean
RUN make all CPLUS_VER=${CPLUS_VER}
RUN make install
RUN ldconfig

workdir /
RUN rm -rf /dm20/apl
