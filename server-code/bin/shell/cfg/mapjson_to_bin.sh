#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

tools_bin/pbbin2json --input=res/map/$1.scene.json --pbdir=../src/share-code/proto --pb=config/Cfg_MapData.proto --input_json --output=res/map/$1.scene $2