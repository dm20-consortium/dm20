arg IMAGE_TAG=20.04
# docker build -t dm2/commonlib:latest .
from dm2/cslib:${IMAGE_TAG}

run apt-get update && \
  apt-get install -y \
    build-essential \
    g++ \
    liblog4cxx-dev \
    libssl-dev

copy . /dm20/commonLib/
workdir /dm20/commonLib/

run make clean
run make all
run make install
run ldconfig
