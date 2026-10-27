ARG IMAGE_TAG=24.04
FROM ubuntu:${IMAGE_TAG}

ARG IMAGE_TAG

ENV DEBIAN_FRONTEND=noninteractive

# Common packages
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        make \
        build-essential \
        g++ \
        liblog4cxx-dev \
    && if [ "${IMAGE_TAG}" = "22.04" ]; then \
        apt-get install -y --no-install-recommends \
            libboost-thread-dev; \
    fi \
    && rm -rf /var/lib/apt/lists/*

# Ubuntu 22.04 only: log4cxx configuration
RUN if [ "${IMAGE_TAG}" = "22.04" ]; then \
        CONFIG=/usr/include/log4cxx/boost-std-configuration.h; \
        cp "${CONFIG}" "${CONFIG}.bak"; \
        sed -i \
            -e 's/^#define STD_SHARED_MUTEX_FOUND 1$/#define STD_SHARED_MUTEX_FOUND 0/' \
            -e 's/^#define Boost_SHARED_MUTEX_FOUND 0$/#define Boost_SHARED_MUTEX_FOUND 1/' \
            "${CONFIG}"; \
        grep -E '^#define (STD_SHARED_MUTEX_FOUND|Boost_SHARED_MUTEX_FOUND)' "${CONFIG}"; \
    fi