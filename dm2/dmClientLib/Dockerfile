arg IMAGE_TAG=20.04
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

run make clean
run make all
run make install
run ldconfig

workdir /
run rm -rf /tmp/dmClientLib
