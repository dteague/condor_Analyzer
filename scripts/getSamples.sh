#!/bin/bash

file_area='/hdfs/store/user/dteague/delphes_WZ_YR/skimmed/'

for dir in $(find $file_area -maxdepth 1 ! -path $file_area); do
    sample=$(echo $dir | sed "s@${file_area}@@")
    #$(echo $dir | sed -e "s@$file_area@@" -e 's/_pruned//')
    #fullpath=$dir/
    fullpath=root://cmsxrootd.hep.wisc.edu/$(echo $dir | sed 's@/hdfs@@')/
    ls -l $dir | awk '{if($9 != "") print "'$fullpath'"$9", "$5}' > .samples/${sample}.txt

done
	    
