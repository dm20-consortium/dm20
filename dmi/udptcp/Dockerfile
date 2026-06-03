from dm2/dmclientlib:20.04 as base

run apt-get update \
  && apt-get install -y \
    libgeos++-dev \
    libxerces-c-dev \
    libssl-dev \
    libboost-all-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    cmake \
  && rm -rf /var/lib/apt/lists/*

# ---- build stabe ----
from base as builder

copy ./cool4_api_dataclass /tmp/dmi/cool4_api_dataclass
copy ./dmi_utils /tmp/dmi/dmi_utils
copy ./udp_dmi /tmp/dmi/udpdmi
copy ./tcp_dmi /tmp/dmi/tcpdmi
copy ./cmake /tmp/dmi/cmake

workdir /tmp/dmi/
run set -e && \
  for dir in cool4_api_dataclass dmi_utils udpdmi tcpdmi; do \
    cd /tmp/dmi/${dir} && \
    echo "try to build ${dir}" && \
    rm -rf build && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig; \
  done

# ---- runtime stage ----
from base as runtime

copy --from=builder /usr/local/lib/lib*.so /usr/local/lib/
copy --from=builder /usr/local/include/cool4_api_dataclass /usr/local/include/cool4_api_dataclass
copy --from=builder /usr/local/include/dmi_utils /usr/local/include/dmi_utils
copy --from=builder /usr/local/include/udp_dmi /usr/local/include/udp_dmi
copy --from=builder /usr/local/include/tcp_dmi /usr/local/include/tcp_dmi
copy --from=builder /usr/local/bin/* /usr/local/bin/
run ldconfig
