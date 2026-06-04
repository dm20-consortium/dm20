arg IMAGE_TAG=20.04
from dm2/proto:${IMAGE_TAG} as proto-image
from dm2/tuplelib:${IMAGE_TAG}

# avoid hunging up
env TZ=Asia/Tokyo
run ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

copy . /tmp/dmClientLib/
workdir /tmp/dmClientLib/

run apt-get update && apt-get install -y \
  libgeos++-dev libxerces-c-dev \
  libssl-dev \
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
run rm -rf /tmp/dmClientLib
