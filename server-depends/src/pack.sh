#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

set -e

filelist=`ls -F | grep '/$'`
for file in $filelist
do
    filename=$(basename "$file") 
    tar -cvf ../pack/${filename}.tar.gz ${filename}
done
