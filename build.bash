#!/bin/bash

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
