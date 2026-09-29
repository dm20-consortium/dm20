#!/bin/bash

set -e

TILE_URL=""
ATTRIBUTION=""
GRIDMAP_URL=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --tile-url)
            TILE_URL="$2"
            shift 2
            ;;
        --attribution)
            ATTRIBUTION="$2"
            shift 2
            ;;
        --gridmap-url)
            GRIDMAP_URL="$2"
            shift 2
            ;;
        --tile-max-zoom)
            TILE_MAX_ZOOM="$2"
            shift 2
            ;;
        *)
            echo "Error: unknown option: $1"
            exit 1
            ;;
    esac
done
if [ -z "$TILE_URL" ]; then
    echo "Error: --tile-url is required"
    exit 1
fi

if [ -z "$ATTRIBUTION" ]; then
    echo "Error: --attribution is required"
    exit 1
fi

mkdir -p logs

echo "Starting ws_server.py..."
python3 ws_server.py > logs/ws_server.log 2>&1 &
WS_SERVER_PID=$!
echo "ws_server PID: $WS_SERVER_PID"

echo "Starting send_udp.py..."
python3 send_udp.py > logs/send_udp.log 2>&1 &
SEND_UDP_PID=$!
echo "send_udp PID: $SEND_UDP_PID"

# 終了時に子プロセスも終了
cleanup() {
    echo "Stopping viewer services... ${WS_SERVER_PID} ${SEND_UDP_PID}"
    pkill -f ws_server
    pkill -f send_udp
}
trap cleanup EXIT

echo "Starting Streamlit..."
streamlit run ws_client.py \
    --server.address 0.0.0.0 \
    --server.port 33013 \
    -- \
    --tile-url "$TILE_URL" \
    --attribution "$ATTRIBUTION" \
    ${TILE_MAX_ZOOM:+--tile-max-zoom "$TILE_MAX_ZOOM"} \
    ${GRIDMAP_URL:+--gridmap-url "$GRIDMAP_URL"}