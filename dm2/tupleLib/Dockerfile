arg IMAGE_TAG=20.04
from dm2/proto:${IMAGE_TAG} as proto-image
FROM ubuntu:${IMAGE_TAG}

copy . /tmp/tupleLib/
workdir /tmp/tupleLib/

run apt-get update \
  && apt-get install -y \
    make \
    build-essential g++ \
    zlib1g-dev \
    libzstd-dev \
  && rm -rf /var/lib/apt/lists/*

copy --from=proto-image /usr/local/bin/protoc /usr/local/bin/protoc
copy --from=proto-image /usr/local/lib/libdm2proto* /usr/local/lib/
copy --from=proto-image /usr/local/lib/libprotobuf* /usr/local/lib/
copy --from=proto-image /usr/local/include/is/proto /usr/local/include/is/proto
copy --from=proto-image /usr/local/include/google /usr/local/include/google

run ldconfig

run make clean
run make all
run make install
run ldconfig

workdir /
run rm -rf /tmp/tupleLib
