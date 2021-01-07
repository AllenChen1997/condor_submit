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
echo "using $inputdir as inputs"
echo "the result will be in $outputlog"
echo "" > $outputlog
files=`ls $1`
n=0
for ifile in $files;do
  n=$((${n}+1))
  echo "submit $n job, using $ifile " >> $outputlog
  fname=`echo ${ifile%.txt}`
  cp submit_multi.sub submit_multi_temp.sub
  sed -i "/transfer_input_files =/c transfer_input_files = runAnalysis.sh, readElement.C, dummy.txt, $inputdir/$ifile" submit_multi_temp.sub
  sed -i "/arguments =/c arguments = ${ifile} ${fname}.root \$(Proxy_path)" submit_multi_temp.sub
  condor_submit submit_multi_temp.sub |tee -a $outputlog
  echo "done" >> $outputlog
done
echo "total $n jobs " >> $outputlog
