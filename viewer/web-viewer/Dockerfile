FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV PYTHONUNBUFFERED=1

WORKDIR /data

RUN apt-get update && \
    apt-get install -y \
        python3 \
        python3-pip \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

COPY requirement.txt .

RUN python3 -m pip install --no-cache-dir -r requirement.txt

COPY . .

RUN chmod +x start_viewer.bash

EXPOSE 33013
EXPOSE 8765
EXPOSE 33333/udp

ENTRYPOINT ["./start_viewer.bash"]