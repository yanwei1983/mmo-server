#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`


tools_bin/pbbin2txt --input=res/config/$1.bytes --pbdir=../src/game_comm/proto/ --pb=config/$1.proto --debug
