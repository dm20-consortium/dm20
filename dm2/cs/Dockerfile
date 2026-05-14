arg IMAGE_TAG=20.04
# docker build -t cs:latest .
from dm2/cslib:${IMAGE_TAG}

run apt-get update && \
  apt-get install -y \
    build-essential \
    g++ \
    liblog4cxx-dev \
    libssl-dev

copy . /dm20/cs/
workdir /dm20/cs/

run make clean
run make all
run make install
run ldconfig

workdir /dm20/cs/bin
