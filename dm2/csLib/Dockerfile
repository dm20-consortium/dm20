arg IMAGE_TAG=20.04
from dm2/seclib:${IMAGE_TAG}

run apt-get update && \
  apt-get install -y \
    build-essential \
    g++ \
    liblog4cxx-dev \
    libssl-dev \
    libev-dev

copy . /dm20/csLib/
workdir /dm20/csLib/

run make clean
run make all
run make install
run ldconfig
