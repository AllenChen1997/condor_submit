# condor_submit
unit test

you can follow the step below to test this frame work in lxplus7

# step 1: setup env.
	git clone https://github.com/AllenChen1997/condor_submit.git
	cd condor_submit
	voms-proxy-init --voms cms --valid 168:00
	
after init voms you may see the line: Created proxy in /tmp/x509up_xxxxxxx

copy that to the certain place. e.g yourusername="k/kuchen" (for me)

	cp /tmp/x509up_xxxxxxx /afs/cern.ch/user/yourusername/private/x509up
	
# step 2: edit configuration file and executable file
there are some important variables in both files needed to be edited.

in "submit_multi.sub", change the name in Proxy_path

in "runAnalysis.sh", change the directory to yours(the exist one) in line:

until xrdcp -f "$2" /eos/user/k/kuchen/condor_test/"$2"; do

# step 3: submit the condor job

	condor_submit submit_multi.sub
	
if you want to see the status:

	condor_q
	
##################################################################
there are more details in ./Condor_job_submit.pdf