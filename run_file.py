#!/usr/bin/env python

from  bin_packing import pack, Bin
import os
from subprocess import Popen, PIPE, check_output
import operator
from ROOT import gSystem
import getpass
from argparse import ArgumentParser

try:
    from subprocess import DEVNULL # py3k
except ImportError:
    import os
    DEVNULL = open(os.devnull, 'wb')


Samplefile = open('Sample_list.txt', 'r')

xrootd_area = ""
working_files = []
PU_tag = "_200PU/"

bitsTo1000Events = 70000000


parser = ArgumentParser()
parser.add_argument("-n", action="store", help="Number of jobs (roughly) to send to Condor", default=100, type=int)

args = parser.parse_args()
spliting = args.n

''' open the sample file: it is of the format

xrootd area #only on first line

<Sample_name>  <number_of_files>   <spliting>

Need to fix commenting and total structure
'''
for line in Samplefile:
    working_line = line.split()
    if len(working_line) == 0: continue
    if '#' in working_line[0]: continue
    if xrootd_area == "" and len(working_line) == 1:
        xrootd_area = working_line[0]
    elif len(working_line) == 2:
        working_files.append(working_line)
        print working_line[0]


xrootdStatName = xrootd_area.split('//')

if ".samples" not in os.listdir('.'):
    os.mkdir(".samples")

''' This section does the finding of the files and getting their 
information.  If they have already been catalogued, they will be
in the directory 

.samples

'''
vomsString = "voms-proxy-info --timeleft"
timeleft= check_output(vomsString, shell=True)
if int(timeleft) < 10000 :
    print "We need your password: please enter password"
    password = getpass.getpass()
    p = Popen(['voms-proxy-init -voms cms -pwstdin'], stdin=PIPE, shell=True)
    p.communicate(input=password)
    if p.returncode != 0:
        print "Your voms wasn't set properly, try again"
        exit(p.returncode)
check_output("cp $(voms-proxy-info -path) userproxy", shell=True)
        
        
print "Sourcing files to run over:"

# for file_bundle in working_files:
#     fileMap = []
#     totalBitSize = 0
#     nameFiles = file_bundle[0]
#     num_files = int(file_bundle[1])

#     if nameFiles.replace('/','_')+'.txt' in os.listdir('.samples'): continue
    
#     f = open('.samples/' + nameFiles.replace('/','_')+'.txt','w')
#     for i in range(1, num_files+1):
#         statCommand = 'xrdfs root://' + xrootdStatName[1] + '/ stat '
#         fullFileName = '/' + xrootdStatName[2] + nameFiles + '_200PU/' + nameFiles + "_" + str(i) + "_0.root"
#         sizeOutput = check_output(statCommand + fullFileName + " | sed -n 's/Size:\s*\(.*\)/\\1/p'", shell=True).strip()
#         if sizeOutput == "": continue
#         totalBitSize += int(sizeOutput)
#         f.write("root://" + xrootdStatName[1] + '/' + fullFileName + ", " + sizeOutput + "\n")
#     f.close()

    
allFiles_bitsize = 0;
for file_bundle in working_files:
    f = open(".samples/" + file_bundle[0].replace('/','_') + ".txt", 'r')
    
    for line in f:
        allFiles_bitsize += int(line.split(',')[1])
    f.close()

    
splitSize = (allFiles_bitsize+1) / spliting
print splitSize / bitsTo1000Events / 60, "minutes to run (estimate)"

for file_bundle in working_files:
    fileMap = []
    totalBitSize = 0
    nameFiles = file_bundle[0]
    num_files = int(file_bundle[1])

    f = open(".samples/" + nameFiles.replace('/','_') + ".txt", 'r')
    for line in f:
        workingLine = line.split(',')
        totalBitSize += int(workingLine[1].strip())
        fileMap.append( (workingLine[0], int(workingLine[1].strip())))
    f.close()

    print "Binning sample :", nameFiles

    
    bins = pack(fileMap, splitSize)

    if nameFiles.replace('/','_') not in os.listdir('.'):
        os.mkdir(nameFiles.replace('/','_'))
    for count, bin in enumerate(bins):
        fbinning = open(nameFiles.replace('/','_') + "/sample_list_" + str(count)+".dat", 'w')
        for items in bin.get_items():
            fbinning.write(items[0] + "\n")
        fbinning.close()
    condorCmd = 'condor_submit submit.condor -append "Initialdir = ' + nameFiles.replace('/','_') + '" -append "Arguments = \$(Process) ' + nameFiles.replace('/','_') + '" -queue ' + str(len(bins))
    print condorCmd
    os.system(condorCmd)

#'" -append "Transfer_output_files = ' + nameFiles.replace('/','_')+ '_\$(Process).root"
