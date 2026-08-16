arg IMAGE_TAG=20.04
from dm2/proto:${IMAGE_TAG} as proto-image
FROM ubuntu:${IMAGE_TAG}

# avoid hunging up
env TZ=Asia/Tokyo
run ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

run apt-get update \
  && apt-get install -y \
    make \
    build-essential g++ \
    zlib1g-dev \
    libzstd-dev \
    pkg-config \
  && rm -rf /var/lib/apt/lists/*

copy --from=proto-image /usr/local/bin/protoc /usr/local/bin/protoc
copy --from=proto-image /usr/local/lib/libdm2proto* /usr/local/lib/
copy --from=proto-image /usr/local/lib/libprotobuf* /usr/local/lib/
copy --from=proto-image /usr/local/include/is /usr/local/include/is
copy --from=proto-image /usr/local/include/google /usr/local/include/google
COPY --from=proto-image /usr/local/lib/pkgconfig/protobuf.pc /usr/local/lib/pkgconfig/

run ldconfig

copy common.mk /tmp/common.mk
copy tupleLib /tmp/tupleLib
workdir /tmp/tupleLib/

run make clean
run make all
run make install
run ldconfig

workdir /
run rm -rf /tmp/tupleLib
