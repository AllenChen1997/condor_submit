#!/bin/bash
#### FRAMEWORK SANDBOX SETUP ####
# Load cmssw_setup function
#export SCRAM_ARCH=slc7_amd64_gcc700
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
#Proxy
export X509_USER_PROXY=$3
voms-proxy-info -all
voms-proxy-info -all -file $3

echo "1 is $1"
echo "2 is $2"
root -b -q ./readElement.C++\(\"$1\",\"$2\"\)
echo "finish the code"
if [ -e "$2" ]; then
  until xrdcp -f "$2" /eos/user/k/kuchen/myOutput/queueOut/"$2"; do
    sleep 60
    echo "Retrying"  
  done
fi

if [ ! -e "$2" ]; then
  echo "Error: The python script failed, could not create the output file."
  
fi

