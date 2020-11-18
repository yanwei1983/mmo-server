#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

tools_bin/pbbin2json --input=res/map/$1.scene --pbdir=../src/share-code/proto --pb=config/Cfg_MapData.proto --output_json --output=res/map/$1.scene.json $2