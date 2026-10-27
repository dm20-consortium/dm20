# docker build -t dm2/is:latest .
ARG IMAGE_TAG=24.04
FROM dm2/protoc:${IMAGE_TAG} AS protoc-image

# avoid hunging up
ENV TZ=Asia/Tokyo
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update \
  && apt-get install -y \
    build-essential pkg-config cmake

# build proto files
COPY . /dm20/proto
RUN rm -rf /dm20/proto/build
WORKDIR /dm20/proto/build
RUN cmake .. && make -j4 install

WORKDIR /
