#!/bin/bash

# DMI ToDo: ros2, protobuf
readonly dirs=(
  udptcp
)
cd dmi
for dir in ${dirs[@]}
do
  cd $dir
  bash uninstall.bash
  cd -
done
cd ..

# DM Lib
cd dm2
sudo make uninstall
