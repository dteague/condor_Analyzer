#!/bin/bash

CMSSW_RELEASE_BASE="/cvmfs/cms.cern.ch/slc6_amd64_gcc530/cms/cmssw/CMSSW_9_1_0_pre3"

source /cvmfs/cms.cern.ch/cmsset_default.sh
pushd $CMSSW_RELEASE_BASE
eval `scramv1 runtime -sh`
popd
export X509_USER_PROXY=userproxy

echo "$1 is input command"
echo "$2 is file name"

extraoption="-p"
if [ $2 == "WZ_EWK" ]; then
    extraoption=""
fi

ls
mkdir obj
make -j 10

echo ./Analyzer -i sample_list_$1.dat -o test_$1.root $extraoption
./Analyzer -i sample_list_$1.dat -o test_$1.root $extraoption
