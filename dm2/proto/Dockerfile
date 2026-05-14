# docker build -t dm2/is:latest .
arg IMAGE_TAG=20.04
from dm2/protoc:${IMAGE_TAG} as protoc-image

# avoid hunging up
env TZ=Asia/Tokyo
run ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

run apt-get update \
  && apt-get install -y \
    pkg-config

# build proto files
copy . /dm20/proto
run rm -rf /dm20/proto/build
workdir /dm20/proto/build
run cmake .. && make -j4 install

workdir /
