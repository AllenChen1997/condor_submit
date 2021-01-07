#!/bin/bash
if [ -z $1 ]; then
  echo "usage: bash multisub.sh <yourinputlist>"
  exit 0
fi
if [ -z $2 ]; then
  outputlog="submitlog.txt"
else outputlog=$2
fi

inputdir=$1
echo "using $inputdir as input directory"
echo "the result will be in $outputlog"
echo "" > $outputlog
files=`ls $1`
n=0
for ifile in $files;do
  n=$((${n}+1))
  echo "submit $n job, using $ifile " >> $outputlog
  fname=`echo ${ifile%.txt}`
  cp submit_multi_quever.sub submit_multi_quever_temp.sub
  sed -i "/listFile = /c listFile = $inputdir/$ifile" submit_multi_quever_temp.sub
  sed -i "/outputname = /c outputname = $fname" submit_multi_quever_temp.sub
  condor_submit submit_multi_quever_temp.sub |tee -a $outputlog
  echo "done" >> $outputlog
done
echo "total $n jobs " >> $outputlog
