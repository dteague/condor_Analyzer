#!/bin/bash
IFS=$'\n'

for line in $(cat Sample_list.txt); do
    newline=$(echo $line | awk '{print $1}')
    if [ -d $newline ] && [ ! -z "$newline" ]; then
#	echo $newline/*
    	rm -f $newline/*
    fi
done
