#!/bin/bash/
# the default is to check if there exist "Error" in output/*
# the result will be in resubmitlist.txt

outputlog="*"
outputfilelist="resubmitlist.txt"
if [ ! -z $1 ]; then
  outputfilelist=$1
fi
if [ ! -z $2 ]; then
  outputlog=$2
fi
if [ -e $outputfilelist ];then
  echo "${outputfilelist} is already exist"
  read -p "type y for delete it and continue: " ans
  if [ $(echo $ans |tr -d ' '|tr 'y' 'Y') != "Y" ]; then
    echo "process canceled"
    exit 1
  fi 
fi
> $outputfilelist #clean all contents in the outputlist file
logs=`grep Error output/$outputlog|cut -d ':' -f 1`
for alog in $logs; do
  files=`grep "1 is" $alog|cut -d ' ' -f 3`
  echo $files >> $outputfilelist
#  echo $alog
done
