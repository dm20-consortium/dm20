set -e 
for dir in cool4_api_dataclass dmi_utils tcp_dmi; do 
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