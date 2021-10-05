#!bin/bash

cp submit_multi_getR_quever.sub submit_multi_getR_quever_temp.sub
inputdir="../sample_from_Eiko/"
filelist=`ls $inputdir`
for afile in $filelist;do
	outfilename=`echo $afile|cut -d '.' -f 1`
	sed -i '/outputname =/c outputname = '${outfilename}'' submit_multi_getR_quever_temp.sub
	sed -i '/listFile =/c listFile = '${inputdir}/${afile}'' submit_multi_getR_quever_temp.sub
	condor_submit submit_multi_getR_quever_temp.sub |tee ${outfilename}_submit.log
done

