#!/bin/bash
# this code is used to clean the log which produced by condor 
# effect to the folder including log, error, and output
echo "removing old_log"
rm -r old_log 2> /dev/null
echo "removing old_error"
rm -r old_error 2> /dev/null
echo "removing old_output"
rm -r old_output 2> /dev/null
echo "moving news to olds"
mv -i log old_log
mv -i output old_output
mv -i error old_error
echo "make folder error log output again"
mkdir error log output