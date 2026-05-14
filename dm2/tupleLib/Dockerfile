arg IMAGE_TAG=20.04
from ubuntu:${IMAGE_TAG}

copy . /tmp/tupleLib/
workdir /tmp/tupleLib/

run apt-get update \
  && apt-get install -y \
    make \
    build-essential g++ \
    zlib1g-dev \
    libzstd-dev \
  && rm -rf /var/lib/apt/lists/*

run make clean
run make all
run make install
run ldconfig

workdir /
run rm -rf /tmp/tupleLib
