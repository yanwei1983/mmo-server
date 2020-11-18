#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

${DIR_file}/tools_bin/excel2pb --excel=../../server-res/excel/$1.xlsx --inputpbdir=../src/game_comm/proto --inputpb=config/$1.proto --out=res/config/$1.bytes --debug
