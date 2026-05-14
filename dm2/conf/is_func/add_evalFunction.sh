#!/bin/bash
if [ $# -ne 1 ]; then
    echo "sh add_evalFunction func.so" 1>&2
    exit 1
fi
filename=$1
if [ ! -f ${filename} ]; then
    echo "sh add_evalFunction func.so" 1>&2
    exit 1
fi
funcname=`echo ${filename} | sed 's/\.so//g' | tr a-z A-Z`
sourcename=`echo ${filename} | sed 's/\.so/\.cpp/g'`
for kata in `grep argList\.at ${sourcename} | awk '{print $1}'`; do
  if [ ${kata} = "bool" ] || [ ${kata} = "string" ] || [ ${kata} = "std::string" ] || [ ${kata} = "int" ] || [ ${kata} = "double" ] || [ ${kata} = "long" ]; then
    suggest=`grep argList\.at ${sourcename} | awk '{print $1}' | sed -z 's/\n/,/g' | awk '{print substr($0, 1, length($0)-1)}'`
  fi
done
idx=0
echo "(1) 引数リストを設定します。"
while true
do
  if [ ${idx} -eq 0 ]; then
    if [ "${suggest}" != "" ]; then
      echo "以下の候補がありますが使用しますか？[y/y以外]"
      echo ${suggest}
      read answer
      if [ "${answer}" = "y" ]; then
        argList=${suggest}
        break
      fi
    fi
  fi
  idx=`expr ${idx} + 1`
  echo "第"${idx}"引数を入力して下さい。なければEnterを押下して下さい。"
  read str
  if [ "${str}" = "" ]; then
    break
  fi
  if [ "${argList}" != "" ]; then
    argList=${argList},
  fi
  argList=${argList}${str}
done
echo "引数リストを設定しました。\n\n (2) 戻り値リストを設定します"
suggest=""
if [ -f ${sourcename} ]; then
  #retList.push_back(ret)からretを抽出
  rowval=`grep retList.push ${sourcename} | head -n 1 | cut -d'(' -f2 | cut -d')' -f1`
  if [ "${rowval}" != "" ]; then
    #ret.push_back(std::to_string(retBool))からretBoolを抽出
    colval=`grep ${rowval}.push ${sourcename} | cut -d'(' -f3 | cut -d')' -f1`
    if [ "${colval}" != "" ]; then
      for kata in `grep ${colval} ${sourcename} | awk '{print $1}'`; do
          if [ ${kata} = "bool" ] || [ ${kata} = "string" ] || [ ${kata} = "std::string" ] || [ ${kata} = "int" ] || [ ${kata} = "double" ] || [ ${kata} = "long" ]; then
            suggest=${kata}
          fi
      done
    fi
  fi
fi
idx=0
while true
do
  if [ ${idx} -eq 0 ]; then
    if [ "${suggest}" != "" ]; then
      echo "以下の候補がありますが使用しますか？[y/y以外]"
      echo ${suggest}
      read answer
      if [ "${answer}" = "y" ]; then
        retList=${suggest}
        break
      fi
    fi
  fi
  idx=`expr ${idx} + 1`
  echo ""${idx}"番目の戻り値を入力して下さい。なければEnterを押下して下さい。"
  read str
  if [ "${str}" = "" ]; then
    break
  fi
  if [ "${retList}" != "" ]; then
    retList=${retList},
  fi
  retList=${retList}${str}
done
echo "戻り値リストを設定しました。\n\n "

outfile="../../conf/evalFunction.conf"
line1="# ---------- ${funcname}の定義 ---------- #"
line2="${funcname}_LIB=${filename}"
line3="${funcname}_ARG_TYPE=${argList}"
line4="${funcname}_RET_TYPE=${retList}"

echo "(3) 下記の内容で、${out_file}の末尾に書き込みます。よろしいですか？[y/y以外]"
echo "\n${line1}\n${line2}\n${line3}\n${line4}"

read writeFlg
if [ "${writeFlg}" = "y" ]; then
  echo "\n${line1}\n${line2}\n${line3}\n${line4}" >> ${outfile}
  echo "${out_file}の末尾に書き込みました。末尾７行を出力します"
  tail -n 7 ${outfile}
fi


