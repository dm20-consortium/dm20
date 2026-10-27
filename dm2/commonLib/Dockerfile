ARG IMAGE_TAG=24.04
FROM dm2/cslib:${IMAGE_TAG}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
  apt-get install -y --no-install-recommends \
    build-essential \
    g++ \
    liblog4cxx-dev \
    libssl-dev \
  && rm -rf /var/lib/apt/lists/*

COPY . /dm20/commonLib/
WORKDIR /dm20/commonLib/

RUN make clean
RUN make all
RUN make install
RUN ldconfig
