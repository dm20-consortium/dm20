#!/bin/bash
set -e

cd dm2

make all
sudo make install
sudo ldconfig
