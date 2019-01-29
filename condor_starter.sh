#!/bin/bash

# setting up cmssw stuff
CMSSW_RELEASE_BASE="/cvmfs/cms.cern.ch/slc6_amd64_gcc530/cms/cmssw/CMSSW_9_1_0_pre3"
source /cvmfs/cms.cern.ch/cmsset_default.sh
pushd $CMSSW_RELEASE_BASE
eval `scramv1 runtime -sh`
popd
export X509_USER_PROXY=userproxy

echo "$1 is input command"
echo "$2 is file name"


#code goes here

ls
mkdir obj
make -j 10

lister=$(cat sample_list_$1.dat)

echo ./Analyzer -in $lister -out test_$1.root
./Analyzer -in $lister -out test_$1.root 

