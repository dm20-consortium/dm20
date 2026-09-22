#!/bin/bash

#
# replay_csv_realtime.sh
#
# 元CSVの時間差を維持してリプレイする。
# ITS時刻は現在時刻へ平行移動して出力する。
#
# Usage:
#   ./replay_csv_realtime.sh input.csv time_column
#

if [ $# -ne 2 ]; then
    echo "Usage: $0 input.csv time_column"
    exit 1
fi

INPUT="$1"
TIME_COLUMN="$2"

ITS_OFFSET=1072915195000

TODAY=$(date +%Y%m%d)
BASENAME=$(basename "$INPUT")
OUTPUT=$(echo "$BASENAME" | sed -E "s/[0-9]{8}/${TODAY}/")

> "$OUTPUT"

########################################
# 最初の行
########################################

FIRST_LINE=$(head -n1 "$INPUT")

if [ -z "$FIRST_LINE" ]; then
    echo "Empty file."
    exit 1
fi

FIRST_ITS=$(echo "$FIRST_LINE" | awk -F',' -v c="$TIME_COLUMN" '{print $c}')

NOW_UNIX_MS=$(date +%s%3N)
NOW_ITS=$((NOW_UNIX_MS - ITS_OFFSET))

OFFSET=$((NOW_ITS - FIRST_ITS))

PREV_ITS=$FIRST_ITS

########################################
# リプレイ開始
########################################

while IFS= read -r line
do

    CUR_ITS=$(echo "$line" | awk -F',' -v c="$TIME_COLUMN" '{print $c}')

    #
    # 前レコードとの差分
    #
    DIFF=$((CUR_ITS - PREV_ITS))

    #
    # 元データの時間差だけ待つ
    #
    if [ "$DIFF" -gt 0 ]; then
        SLEEP_SEC=$(awk "BEGIN{printf \"%.3f\", $DIFF/1000}")
        sleep "$SLEEP_SEC"
    fi

    #
    # ITS時刻を現在へ平行移動
    #
    NEW_ITS=$((CUR_ITS + OFFSET))

    echo "$line" | \
    awk -F',' \
        -v OFS=',' \
        -v c="$TIME_COLUMN" \
        -v t="$NEW_ITS" '
    {
        $c=t;
        print
    }' >> "$OUTPUT"

    PREV_ITS=$CUR_ITS

done < "$INPUT"

echo "Replay finished."