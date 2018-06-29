#!/usr/bin/env python
import os 
import sys
import subprocess
import time

# time.sleep(30)

# CMSSW_RELEASE_BASE="/cvmfs/cms.cern.ch/slc6_amd64_gcc530/cms/cmssw/CMSSW_9_1_0_pre3"
# currentDir = os.getcwd()

# os.popen("source /cvmfs/cms.cern.ch/cmsset_default.sh")
# os.chdir(CMSSW_RELEASE_BASE)
# os.popen("eval `scramv1 runtime -sh` ")
# os.chdir(currentDir)
# os.popen("export X509_USER_PROXY=userproxy ")

print "before import"
import imp
try:
    imp.find_module('ROOT')
    found = True
except ImportError:
    found = False

if not found:
    print "Can't find"
    print os.popen("type root").read()

from ROOT import gSystem

import time
print "before load"
gSystem.Load('Analyzer_C.so')
print "before import 2"
from ROOT import Analyzer

timer = time.clock()

processNum = sys.argv[1]
sampleName = sys.argv[2]
print len(sys.argv), sys.argv
f = open("sample_list_" + str(processNum) + ".dat", 'r')

test = Analyzer()
for itemList in f:
    test.add_run_file(itemList.strip())


while test.next():
    test.selection()
    test.fill_histo()
    if test.event_number() % 1000 == 0: print str(test.event_number()) + " Events" 
test.write_out(sampleName+"_" + processNum + ".root")

print time.clock()-timer

