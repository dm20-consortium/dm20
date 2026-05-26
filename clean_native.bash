#!/bin/bash

# DMI ToDo: protobuf
readonly dirs=(
  ros2
  udptcp
)
cd dmi
for dir in ${dirs[@]}
do
  cd $dir
  bash clean.bash
  cd -
done
cd ..

# DM Lib
cd dm2
make clean
