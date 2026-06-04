# docker build -t dm2mes:latest .
arg IMAGE_TAG=20.04
from dm2/proto:${IMAGE_TAG} as proto-image
from dm2/tuplelib:${IMAGE_TAG} as tuplelib-image
from dm2/seclib:${IMAGE_TAG} as seclib-image
from dm2/dmclientlib:${IMAGE_TAG} as dmclientlib-image
from dm2/commonlib:${IMAGE_TAG} as commonlib-image

copy --from=seclib-image /dm20/secLib /dm20/secLib
copy --from=seclib-image /usr/local/lib/libdm2security.a /usr/local/lib/

# avoid hunging up
env TZ=Asia/Tokyo
run ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# required libraries
run apt-get update && \
  apt-get install -y \
    wget \
    zlib1g-dev \
    libzstd-dev \
    libgeos++-dev \
    libgeos-c1v5 \
    libxerces-c-dev \
    libssl-dev \
    libpqxx-6.4 \
    libpq-dev \
    libpqxx-dev \
    liblog4cxx-dev \
  && rm -rf /var/lib/apt/lists/*

copy . /dm20/apl/dm2mes
workdir /dm20/apl/dm2mes

copy --from=tuplelib-image /usr/local/include/is /usr/local/include/is
copy --from=tuplelib-image /usr/local/lib/libtuple.a /usr/local/lib/
copy --from=proto-image /usr/local/bin/protoc /usr/local/bin/protoc
copy --from=proto-image /usr/local/lib/libdm2proto* /usr/local/lib/
copy --from=proto-image /usr/local/lib/libprotobuf* /usr/local/lib/
copy --from=proto-image /usr/local/include/is/proto /usr/local/include/is/proto
copy --from=proto-image /usr/local/include/google /usr/local/include/google
copy --from=dmclientlib-image /usr/local/include/is /usr/local/include/is
copy --from=dmclientlib-image /usr/local/lib/libtuple.a /usr/local/lib/libtuple.a
copy --from=dmclientlib-image /usr/local/lib/libdmclient.a /usr/local/lib/libdmclient.a
run ldconfig
run make clean
run make all
run make install
run ldconfig

workdir /
run rm -rf /dm20/apl
