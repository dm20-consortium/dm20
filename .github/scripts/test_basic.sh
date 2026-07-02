#!/bin/bash
set -e

cd dm2

echo "=== Start dm2is ==="

dm2is -d conf >is.log 2>&1 &
IS_PID=$!

sleep 2

echo "=== Start receiver ==="

dm2mes -r >recv.log 2>&1 &
RECV_PID=$!

sleep 2

echo "=== Send message ==="

echo "hello,dm2" | dm2mes

sleep 2

echo "=== Receiver log ==="

cat recv.log

grep "hello,dm2" recv.log

echo "=== Stop ==="

kill $RECV_PID || true
kill $IS_PID || true