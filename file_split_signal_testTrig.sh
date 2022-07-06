#!bin/bash

# this is used to split lines into many pieces
linesPerFile=100
splited_listdir="signalList"

inputF=$1
Nmax=`wc -l $inputF|cut -d ' ' -f 1`
c1=1  # this will be used as line counter
c2=$linesPerFile
i=0
prefix=`echo $inputF| rev |cut -d '/' -f 1 |cut -d '.' -f 2 |rev`
echo "adding new directory $splited_listdir/$prefix"
mkdir -p $splited_listdir/$prefix
rm $splited_listdir/$prefix/*
echo "start to do the split"
while [ $c1 -le $Nmax ];do
  if [ $c2 -gt $Nmax ];then
    c2=$Nmax
  fi
  newFlist="${prefix}_${i}" 
  sed -n "${c1},${c2}p" $inputF > $splited_listdir/$prefix/${newFlist}.txt
  (( i = i + 1 ))
  (( c1 = c1 + $linesPerFile ))
  (( c2 = c2 + $linesPerFile ))
done
echo "make the splited list ${prefix}_for_submit.txt"
ls $splited_listdir/$prefix/* > ${prefix}_for_submit.txt
cp submit_testTrig.sub submit_testTrig_temp.sub
sed -i "/listFile = /c listFile = ${prefix}_for_submit.txt" submit_multi_temp.sub
sed -i "/outputname = /c outputname = ${prefix}" submit_multi_temp.sub
condor_submit submit_testTrig_temp.sub |tee ${prefix}.log
