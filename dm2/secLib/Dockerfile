ARG IMAGE_TAG=24.04
FROM dm2/base:${IMAGE_TAG}

ENV DEBIAN_FRONTEND=noninteractive

COPY . /dm20/secLib/
WORKDIR /dm20/secLib/

RUN apt-get update \
  && apt-get install -y --no-install-recommends \
    libssl-dev \
  && rm -rf /var/lib/apt/lists/*

RUN make clean
RUN make all
RUN make install
RUN ldconfig

WORKDIR /
