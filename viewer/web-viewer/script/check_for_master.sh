#!/bin/sh
CHECK_DAY=`date +"%Y%m%d"`
DIRNAME=/data/csv

CNT=150
SCHEMA=object_info_0_8_1
OBJ_FILENAME=${SCHEMA}_${CHECK_DAY}.csv

CSV_FILENAME=viewer_input.csv

echo "\n*** rec_obj: information_source_list***"

## 物標情報＋TTC
# 1:ID, 2:時刻, 4:種別, 5:信頼度, 6:サブ種別, 7:信頼度, 22:緯度, 23:経度, 50:速さ, 54:加速度, 56:向き, 58:長さ, 60:幅, 88:情報源リスト, 89:TTC
tail -n ${CNT} ${DIRNAME}/${OBJ_FILENAME} | grep -a , | sort -r | uniq -w 19 | sort -k 2 -t , -T ./ |cut -d , -f 1,2,4,5,6,7,22,23,50,54,56,58,60,88,89

# $1: 時刻, $2: 緯度, $3: 経度
tail -n ${CNT} ${DIRNAME}/${CSV_FILENAME} | awk -F, 'BEGIN {OFS=","} {print 1,$1,1,0,1,0,$2,$3,0,0,0,0,0,"[1]",-1}'

## for V2X E2E Simulator
# $2 (ITS時刻): 9時間現在 (JST -> UTC)
# $56: 向きが異常値のため、暫定的に90度で固定
# $58: 物標IDが入っているため、暫定的に科警研の情報源リストで固定
#tail -n ${CNT} ${DIRNAME}/${FILENAME} | grep -a , | sort -r | uniq -w 19 | sort -k 2 -t , -T ./ | awk -F',' 'BEGIN{OFS=","} {$2 -= 9 * 60 * 60 * 1000; $56 = 9000; $88 = "[302120962]"; print $1,$2,$4,$5,$6,$7,$22,$23,$50,$54,$56,$58,$60,$88}'

CNT=50
SCHEMA=signal_info
FILENAME=${SCHEMA}_${CHECK_DAY}.csv

echo "\n*** rec_sig: crp_id ***"
#tail -n ${CNT} ${DIRNAME}/${FILENAME}  | awk '{gsub(/\[[^]]*\]/, sprintf("[%s]", gensub(/,/, "|", "g", substr($0, match($0, /\[[^]]*\]/) + 1, RLENGTH - 2)))); print}' | sort -r | uniq -w 16 | cut -d , -f 1,2,3,8,10,11,12,14,15,16,18,19,20,22,23,24,26,27,57 | grep -a '|'

echo "\n*** rec_free: freespace ***"
#sh freespace.sh