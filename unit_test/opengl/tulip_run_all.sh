#!/bin/sh

# This script allows the run of all previously recorded tests

TYPE=$1

# clean up result_*.jpg files
rm result_*.jpg > /dev/null 2>&1

# clean up log file
if [ -f all_tests_run.log ]; then
    rm all_tests_run.log
fi

touch all_tests_run.log

echo "Check ComparePicture application..."
if [ ! -f ComparePicture ]; then
    echo 'ComparePicture executable not found';
    exit
fi

# launch Tulip for nothing
# just to reduce the future time of shared libs loading
# expecting they will be already in memory
echo "Check Tulip launch ..."
TULIP=`which tulip`
if [ $? -eq 1 ]; then
    echo 'tulip executable not found';
    exit
fi
sh  ./launch_tulip.sh $TULIP > /dev/null 2>&1 &
sleep 10
. ./stop_tulip.sh
 
# run tests loop
for TEST in `ls run_*${TYPE}_test.sh`
do
    echo "Running $TEST ..."
    sh $TEST >> all_tests_run.log 2>&1
done

# check results
./ComparePicture
