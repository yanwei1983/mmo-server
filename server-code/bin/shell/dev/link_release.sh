#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

mkdir -p tools_bin
cd tools_bin
rm -f *
ln -sfv ../../build/bin/core/* .
ln -sfv ../../build/bin/release/* .

cd -
rm -f service_loader
ln -sfv ../build/bin/core/service_loader .

mkdir -p libs
cd libs
rm -f *
ln -sfv ../../build/lib/core/*.so .
ln -sfv ../../build/lib/release/*.so .
cd ..
