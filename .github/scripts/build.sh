#!/bin/bash
set -eu

cd dm2

make all
sudo make install
sudo ldconfig
