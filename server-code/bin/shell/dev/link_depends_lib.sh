#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

mkdir -p depends
cd depends
rm -f *
ln -sfv ../../depends/lib/*.so* .

cp -v /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.24 .
ln -sfv libstdc++.so.6.0.24 libstdc++.so.6

cd ..
#ln -s ../depends/bin/* .
