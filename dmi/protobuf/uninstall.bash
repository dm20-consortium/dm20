for dir in ccam_cool4_sensor_io protobuf_dmi; do 
  cd ${dir}
  echo "try to uninstall ${dir}" 
  cd build && sudo make uninstall && cd ..
  sudo ldconfig
  cd ..
done