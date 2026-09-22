#!/bin/bash

if [ $# -ne 4 ]; then
    echo "Usage: $0 input.csv lines_per_cycle interval_sec time_column"
    exit 1
fi

INPUT="$1"
LINES_PER_CYCLE="$2"
INTERVAL="$3"
TIME_COLUMN="$4"

ITS_OFFSET=1072915195000

TODAY=$(date +%Y%m%d)
BASENAME=$(basename "$INPUT")
OUTPUT=$(echo "$BASENAME" | sed -E "s/[0-9]{8}/${TODAY}/")

> "$OUTPUT"

LINE_NO=1

TOTAL=$(wc -l < "$INPUT")

while [ $LINE_NO -le $TOTAL ]
do
    END=$((LINE_NO + LINES_PER_CYCLE - 1))

    sed -n "${LINE_NO},${END}p" "$INPUT" | \
    while IFS= read -r line
    do
        UNIX_MS=$(date +%s%3N)
        ITS_MS=$((UNIX_MS - ITS_OFFSET))

        awk -F',' -v OFS=',' -v its="$ITS_MS" -v time="$TIME_COLUMN" '
        {
            $time=its;
            print
        }' <<< "$line" >> "$OUTPUT"
    done

    LINE_NO=$((END + 1))

    if [ $LINE_NO -le $TOTAL ]; then
        sleep "$INTERVAL"
    fi
done

echo "Finished."