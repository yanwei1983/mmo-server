#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

rm -f tools_bin/*
rm -f service_loader

mkdir -p tools_bin
cp -vrdf ../build/bin/core/* tools_bin/
cp -vrdf ../build/bin/release/* tools_bin/
cp -vrdf ../build/bin/core/service_loader .

mkdir -p libs
cd libs
rm -rf *
cp -vrdf ../../build/lib/core/*.so .
cp -vrdf ../../build/lib/release/*.so .
cd -
