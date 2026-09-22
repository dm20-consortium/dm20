set -e 
for dir in ccam_cool4_sensor_io protobuf_dmi; do 
  cd ${dir} 
  echo "try to build ${dir}" 
  rm -rf build 
  mkdir build 
  cd build 
  cmake .. 
  make -j$(nproc) 
  sudo make install 
  sudo ldconfig
  cd ../..
done
