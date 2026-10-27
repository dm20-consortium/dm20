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

COPY conf /dm20/conf
COPY cs /dm20/cs
WORKDIR /dm20/cs/

RUN make clean
RUN make all
RUN make install
RUN ldconfig

WORKDIR /dm20/cs/bin
