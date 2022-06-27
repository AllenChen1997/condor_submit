# condor_submit
How to run with condor

you can follow the step below to test this frame work in lxplus7

# step 1: setup env.
```
	git clone https://github.com/AllenChen1997/condor_submit.git
	cd condor_submit
	mkdir log error output
	voms-proxy-init --voms cms --valid 168:00
```
after init voms you may see the line: Created proxy in /tmp/x509up_xxxxxxx

copy that to the certain place. e.g yourusername="k/kuchen" (for me)
```
	cp /tmp/x509up_xxxxxxx /afs/cern.ch/user/yourusername/private/x509up
```	
# step 2: edit configuration file and executable file
there are some important variables in both files needed to be edited.

in "submit_multi.sub", change the name in Proxy_path

in "runAnalysis.sh", change the directory to yours(the exist one) in line:

until xrdcp -f "$2" /eos/user/k/kuchen/condor_test/"$2"; do

# step 3: submit the condor job
```
	bash file_split.sh JetHT-Run2017B-31Mar2018-v1_forTest.txt
```
you need to prepare the filelist which contains the list of input root files
	
if you want to see the status:
```
	condor_q
```
####  extra contains ####

cleanlog.sh is used to clean the submit log for condor jobs, the old logs will move to old_*

##################################################################
there are more details in ./Condor_job_submit.pdf
