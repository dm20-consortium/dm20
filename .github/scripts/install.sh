#!/bin/bash
sudo apt update

sudo apt install -y \
  g++ \
  build-essential \
  postgresql postgresql-contrib \
  postgis \
  libproj-dev \
  libpq-dev libpqxx-dev \
  liblog4cxx-dev liblog4cxx-doc \
  libssl-dev \
  zlib1g-dev libzstd-dev \
  libyaml-cpp-dev \
  postgresql-16-postgis-3 \
  libpqxx-7.8t64 \
  libgeos++-dev libgeos-c1t64 libgeos-dev \
  libxerces-c3.2t64 libxerces-c-dev \
  protobuf-compiler libprotobuf-dev

sudo ldconfig