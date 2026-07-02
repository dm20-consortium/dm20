#!/bin/bash
sudo apt update

sudo apt install -y \
  g++ \
  build-essential \
  postgresql \
  postgresql-contrib \
  postgis \
  postgresql-16-postgis-3 \
  libproj-dev \
  libpq-dev \
  libpqxx-7.8t64 \
  liblog4cxx-dev \
  libssl-dev \
  zlib1g-dev \
  libzstd-dev \
  libyaml-cpp-dev \
  libgeos++-dev \
  libgeos-dev \
  libxerces-c-dev \
  protobuf-compiler \
  libprotobuf-dev

sudo ldconfig