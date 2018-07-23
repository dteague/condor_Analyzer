#!/bin/bash

for dir in $(find . -type d -not -path '*/\.*' -not -path "."); do
    if [ $dir == "." ]; then continue; fi
    if [ $(find $dir -name "*root" | wc -l ) -eq 0 ]; then continue; fi
	
    filename=${dir/.\//}
    echo $filename
    hadd -f -k ../Plotter/files/${filename}.root $dir/*root &>/dev/null
done
