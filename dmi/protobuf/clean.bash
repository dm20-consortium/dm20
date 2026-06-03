for dir in ccam_cool4_sensor_io protobuf_dmi; do 
  cd ${dir}
  echo "try to clean ${dir}" 
  rm -rf build
  cd ..
done