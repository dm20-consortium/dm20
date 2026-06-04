# docker build -t dm2/is:latest .
arg IMAGE_TAG=20.04

from dm2/udptcpdmi:${IMAGE_TAG} as udptcpdmi-image
from dm2/protobufdmi:${IMAGE_TAG} as protobufdmi-image

from dm2/proto:${IMAGE_TAG} as proto-image
from dm2/seclib:${IMAGE_TAG} as seclib-image
from dm2/commonlib:${IMAGE_TAG} as commonlib-image
from dm2/tuplelib:${IMAGE_TAG} as tuplelib-image

from dm2/ros2dmi:${IMAGE_TAG}

# avoid hunging up
env TZ=Asia/Tokyo
run ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# required libraries
run apt-get update && \
  apt-get install -y \
    cmake \
    zlib1g-dev \
    libzstd-dev \
    libgeos++-dev \
    libxerces-c-dev \
    libssl-dev \
    libpqxx-6.4 \
    libpq-dev \
    libpqxx-dev \
    liblog4cxx-dev \
    libyaml-cpp-dev \
    libboost-all-dev \
    libgoogle-glog-dev \
    libgflags-dev \
  && rm -rf /var/lib/apt/lists/*


# build is
copy --from=seclib-image /dm20/secLib /dm20/secLib
copy --from=seclib-image /usr/local/lib/libdm2security.a /usr/local/lib/
copy --from=commonlib-image /dm20/commonLib /dm20/commonLib
copy --from=commonlib-image /dm20/csLib /dm20/csLib
copy --from=commonlib-image /usr/local/lib/*.so /usr/local/lib/
copy --from=proto-image /usr/local/bin/protoc /usr/local/bin/protoc
copy --from=proto-image /usr/local/lib/libdm2proto* /usr/local/lib/
copy --from=proto-image /usr/local/lib/libprotobuf* /usr/local/lib/
copy --from=proto-image /usr/local/include/is/proto /usr/local/include/is/proto
copy --from=proto-image /usr/local/include/google /usr/local/include/google
copy --from=udptcpdmi-image /usr/local/include/cool4_api_dataclass /usr/local/include/cool4_api_dataclass
copy --from=udptcpdmi-image /usr/local/include/dmi_utils /usr/local/include/dmi_utils
copy --from=udptcpdmi-image /usr/local/include/udp_dmi /usr/local/include/udp_dmi
copy --from=udptcpdmi-image /usr/local/include/tcp_dmi /usr/local/include/tcp_dmi
copy --from=udptcpdmi-image /usr/local/include/is/*.h /usr/local/include/is/
copy --from=udptcpdmi-image /usr/local/lib/lib*.so /usr/local/lib/
copy --from=udptcpdmi-image /usr/local/bin/* /usr/local/bin/
copy --from=protobufdmi-image /usr/local/include/protobuf_dmi /usr/local/include/protobuf_dmi
copy --from=protobufdmi-image /usr/local/lib/lib* /usr/local/lib/

run ldconfig
copy conf /dm20/conf
copy is /dm20/is
run rm -rf /dm20/is/build
workdir /dm20/is/build
run cmake -DROS2_WORKSPACE_DIR=/opt/ws ..
run make -j4 install
run ldconfig

workdir /
