#!/bin/bash

# DM Lib
cd dm2
make libs -f makefile_docker
cd - > /dev/null

# DMI
readonly dirs=(
  ros2
  udptcp
  protobuf
)
cd dmi
for dir in ${dirs[@]}
do
  cd $dir
  make -f makefile_docker
  cd -
done
cd .. > /dev/null

# IS/CS, messanger
cd dm2
make -f makefile_docker
cd - > /dev/null
