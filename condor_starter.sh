#!/bin/bash

CMSSW_RELEASE_BASE="/cvmfs/cms.cern.ch/slc6_amd64_gcc530/cms/cmssw/CMSSW_9_1_0_pre3"

source /cvmfs/cms.cern.ch/cmsset_default.sh
pushd $CMSSW_RELEASE_BASE
eval `scramv1 runtime -sh`
popd
export X509_USER_PROXY=userproxy

echo "$1 is input command"

echo .L Analyzer.C+ | root.exe -b 2>/dev/null

python condor_runfile.py $1 test
