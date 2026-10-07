#!/bin/bash

#
# replay_csv_realtime.sh
#
# 元CSVの時間差を維持してリプレイする。
# ITS時刻は現在時刻へ平行移動して出力する。
#
# Usage:
#   ./replay_csv_realtime.sh input.csv time_column time_type[its,date] (mode[file/stdout] skip_sleep_threshold)
#

if [ $# -le 1 ]; then
    echo "Usage: $0 input.csv time_column time_type (mode skip_sleep_threshold)"
    echo "  time_type: its | date"
    echo "  mode: file | stdout"
    exit 1
fi

INPUT="$1"
TIME_COLUMN="$2"
TIME_TYPE="$3"

OUTPUT_MODE="file"

if [ $# -ge 4 ]; then
    OUTPUT_MODE="$4"
fi

SKIP_SLEEP_THRESHOLD=""

if [ $# -ge 5 ]; then
    SKIP_SLEEP_THRESHOLD="$4"
fi

case "$TIME_TYPE" in
    its|date)
        ;;
    *)
        echo "Error: invalid time_type: $TIME_TYPE" >&2
        echo "time_type must be its, or date." >&2
        exit 1
        ;;
esac

ITS_OFFSET=1072915195000

## 時刻をミリ秒変換
parse_time_ms()
{
    local value="$1"

    case "$TIME_TYPE" in
        its)
            echo "$((value + ITS_OFFSET))"
            ;;

        date)
            local main="$value"
            local fraction=""

            if [[ "$value" == *.* ]]; then
                main="${value%%.*}"
                fraction="${value##*.}"
            fi

            local sec
            sec=$(date -d "$main" +%s) || {
                echo "Error: Cannot parse date: $value" >&2
                return 1
            }

            local ms=0

            if [ -n "$fraction" ]; then
                fraction="${fraction}000"
                fraction="${fraction:0:3}"
                ms=$((10#$fraction))
            fi

            echo $((sec * 1000 + ms))
            ;;
    esac
}
format_its()
{
    local unix_ms="$1"

    echo "$((unix_ms - ITS_OFFSET))"
}

TODAY=$(date +%Y%m%d)
BASENAME=$(basename "$INPUT")

OUTPUT=$(echo "$BASENAME" | sed -E "s/_[0-9]{8}.*\.csv$/_${TODAY}.csv/")
TMP_OUTPUT="${OUTPUT%.csv}_tmp.csv"

#
# TIME_COLUMN は1始まり
# Bash配列は0始まりなので変換
#
TIME_INDEX=$((TIME_COLUMN - 1))

#
# OUTPUTが既に存在する場合はエラー
#
if [ "$OUTPUT_MODE" == "file" ]; then
    if [ -e "$OUTPUT" ]; then
        echo "Error: OUTPUT file already exists."
        echo "OUTPUT: $OUTPUT"
        exit 1
    fi
fi

#
# 終了時にOUTPUTを_tmpへ退避
#
cleanup()
{
    if [ "$OUTPUT_MODE" == "file" ]; then
        if [ -f "$OUTPUT" ]; then
            mv "$OUTPUT" "$TMP_OUTPUT"
            echo
            echo "Output file moved to:"
            echo "$TMP_OUTPUT"
        fi
    fi
}

trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

#
# 出力先FD
#
if [ "$OUTPUT_MODE" == "file" ]; then
    exec 3>>"$OUTPUT"
else
    exec 3>&1
fi

########################################
# 最初の行
########################################

IFS= read -r FIRST_LINE < "$INPUT"

if [ -z "$FIRST_LINE" ]; then
    echo "Empty file." >&2
    exit 1
fi

#
# CSVを分解
#

IFS=',' read -r -a FIRST_FIELDS <<< "$FIRST_LINE"

FIRST_TIME="${FIRST_FIELDS[$TIME_INDEX]}"
FIRST_MS=$(parse_time_ms "$FIRST_TIME")

NOW_UNIX_MS=$(date +%s%3N)

OFFSET=$((NOW_UNIX_MS - FIRST_MS))
PREV_MS=$FIRST_MS
#
# SKIP_SLEEP_THRESHOLDをmsへ変換
#
if [ -n "$SKIP_SLEEP_THRESHOLD" ]; then
    SKIP_SLEEP_THRESHOLD_MS=$(awk \
        -v threshold="$SKIP_SLEEP_THRESHOLD" \
        'BEGIN { printf "%.0f", threshold * 1000 }')
fi

########################################
# リプレイ開始
########################################

while IFS= read -r line
do

    #
    # CSVを分解
    #
    IFS=',' read -r -a FIELDS <<< "$line"

    CUR_TIME="${FIELDS[$TIME_INDEX]}"
    CUR_MS=$(parse_time_ms "$CUR_TIME")

    #
    # 前レコードとの差分
    #
    DIFF=$((CUR_MS - PREV_MS))

    #
    # 元データの時間差だけ待つ
    #
    if [ "$DIFF" -gt 0 ]; then

        if [ -n "$SKIP_SLEEP_THRESHOLD" ] &&
           [ "$DIFF" -ge "$SKIP_SLEEP_THRESHOLD_MS" ]; then

            echo "Skip sleep: ${DIFF}ms >= ${SKIP_SLEEP_THRESHOLD_MS}ms" >&2

        else
            #
            # ミリ秒を秒表記へ変換
            #
            SEC=$((DIFF / 1000))
            MS=$((DIFF % 1000))

            if [ "$MS" -eq 0 ]; then
                SLEEP_SEC="$SEC"
            else
                SLEEP_SEC="${SEC}.$(printf '%03d' "$MS")"
            fi

            sleep "$SLEEP_SEC"
        fi
    fi

    #
    # ITS時刻を現在へ平行移動
    #
    NEW_MS=$((CUR_MS + OFFSET))
    # 常にITS時刻で出力
    FIELDS[$TIME_INDEX]=$(format_its "$NEW_MS")

    #
    # CSVとして再構築
    #
    (
        IFS=','
        printf '%s\n' "${FIELDS[*]}"
    ) >&3

    PREV_MS=$CUR_MS

done < "$INPUT"
