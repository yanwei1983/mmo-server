#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`


set -e

filelist=`ls res/map/*.scene.json`
for file in $filelist
do
	filename=$(basename "$file") 
	echo "process: "$filename
	shell/cfg/mapjson_to_bin.sh ${filename%%\.*}
done
