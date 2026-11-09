#!/bin/sh
CHECK_DAY=`date +"%Y%m%d"`
DIRNAME=/tmp

CNT=150
SCHEMA=object_info_0_8_1
OBJ_FILENAME=${SCHEMA}_${CHECK_DAY}.csv

CSV_FILENAME=viewer_input.csv

echo "\n*** rec_obj: information_source_list***"

## 物標情報＋TTC
# 1:ID, 2:時刻, 4:種別, 5:信頼度, 6:サブ種別, 7:信頼度, 22:緯度, 23:経度, 50:速さ, 54:加速度, 56:向き, 58:長さ, 60:幅, 88:情報源リスト, 89:TTC
tail -n ${CNT} ${DIRNAME}/${OBJ_FILENAME} | grep -a , | sort -r | uniq -w 19 | sort -k 2 -t , -T ./ |cut -d , -f 1,2,4,5,6,7,22,23,50,54,56,58,60,88

# $1: 時刻, $2: 緯度, $3: 経度
tail -n ${CNT} ${DIRNAME}/${CSV_FILENAME} | awk -F, 'BEGIN {OFS=","} {print 1,$1,1,0,1,0,$2,$3,0,0,0,0,0,"[1]",-1}'

CNT=50
SCHEMA=signal_info
FILENAME=${SCHEMA}_${CHECK_DAY}.csv

echo "\n*** rec_sig: crp_id ***"
tail -n ${CNT} ${DIRNAME}/${FILENAME}  | awk '{gsub(/\[[^]]*\]/, sprintf("[%s]", gensub(/,/, "|", "g", substr($0, match($0, /\[[^]]*\]/) + 1, RLENGTH - 2)))); print}' | sort -r | uniq -w 16 | cut -d , -f 1,2,3,8,10,11,12,14,15,16,18,19,20,22,23,24,26,27,57 | grep -a '|'

echo "\n*** rec_free: freespace ***"
#sh freespace.sh