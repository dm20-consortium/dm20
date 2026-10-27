ARG IMAGE_TAG=24.04
FROM dm2/seclib:${IMAGE_TAG}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
  apt-get install -y --no-install-recommends \
    libev-dev \
  && rm -rf /var/lib/apt/lists/*

COPY . /dm20/csLib/
WORKDIR /dm20/csLib/

RUN make clean
RUN make all
RUN make install
RUN ldconfig
