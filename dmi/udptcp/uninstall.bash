for dir in tcp_dmi udp_dmi dmi_utils cool4_api_dataclass; do 
  cd ${dir}
  echo "try to uninstall ${dir}" 
  cd build && sudo make uninstall && cd ..
  sudo ldconfig
  cd ..
done