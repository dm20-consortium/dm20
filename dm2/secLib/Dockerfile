arg IMAGE_TAG=20.04
from ubuntu:${IMAGE_TAG}

copy . /dm20/secLib/
workdir /dm20/secLib/

run apt-get update \
  && apt-get install -y \
    make \
    build-essential g++ \
    liblog4cxx-dev \
    libssl-dev \
  && rm -rf /var/lib/apt/lists/*

run make clean
run make all
run make install
run ldconfig

workdir /
