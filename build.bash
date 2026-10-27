#!/bin/bash
set -e

IMAGE_TAG="${1:-24.04}"

echo "IMAGE_TAG=${IMAGE_TAG}"

# DM Lib
cd dm2
make libs -f makefile_docker IMAGE_TAG="${IMAGE_TAG}"
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
  make -f makefile_docker IMAGE_TAG="${IMAGE_TAG}"
  cd -
done
cd .. > /dev/null

# IS/CS, messanger
cd dm2
make -f makefile_docker IMAGE_TAG="${IMAGE_TAG}"
cd - > /dev/null

echo "Build completed: IMAGE_TAG=${IMAGE_TAG}"