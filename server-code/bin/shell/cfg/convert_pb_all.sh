#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

set -e
rm res/config/*.bytes -f
filelist=`ls ../../server-res/excel/*.xlsx`
for file in $filelist
do
	filename=$(basename "$file") 
	echo "process: "$filename
	shell/cfg/convert_pb.sh ${filename%\.*}
done
