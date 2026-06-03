for dir in tcp_dmi udp_dmi dmi_utils cool4_api_dataclass; do 
  cd ${dir}
  echo "try to clean ${dir}" 
  rm -rf build
  cd ..
done