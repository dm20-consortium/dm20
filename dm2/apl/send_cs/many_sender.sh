#!/bin/bash

# Usage:

if [ $# -lt 1 ]; then
    echo "Usage: $0 <PROC_NUM> (<LOOP_NUM> <LOOP_RANGE> <COMPRESS_FLG> <TUPLE_NUM>)"
    exit 1
fi

PROC_NUM=$1
LOOP_NUM=1
LOOP_RANGE=1000
COMPRESS_FLG=0
TUPLE_NUM=1
if [ $# -ge 2 ]; then
    LOOP_NUM=$2
fi
if [ $# -ge 3 ]; then
    LOOP_RANGE=$3
fi
if [ $# -ge 4 ]; then
    COMPRESS_FLG=$4
fi
if [ $# -ge 5 ]; then
    TUPLE_NUM=$5
fi

echo "PROC_COUNT: $PROC_NUM"
for ((i=1; i<=PROC_NUM; i++)); do
    ./send_cs.out -i 192.168.39.242 -n $TUPLE_NUM -c $COMPRESS_FLG -o $((i*1000000)) -l $LOOP_NUM -I $LOOP_RANGE &
done

wait
