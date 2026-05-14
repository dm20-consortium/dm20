arg IMAGE_TAG=20.04
from dm2/udpdmi:${IMAGE_TAG}

copy . /tmp/pkg/
workdir /tmp/pkg

run rm -rf build
run mkdir build

workdir /tmp/pkg/build
run cmake .. \
  && make -j4 install
run ldconfig

workdir /
run rm -rf /tmp/pkg
