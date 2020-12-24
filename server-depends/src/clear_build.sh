#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

cd jemalloc
make clean
cd ..

cd lua
make clean
cd ..

rm libevent/cmake_build/* -rf
rm protobuf/build/* -rf
rm cpp_redis/build/* -rf
rm gflags/build/* -rf
rm snappy/build/* -rf
rm leveldb/build/* -rf
rm brpc/build/* -rf

rm tinyxml2/build/* -rf
rm curlpp/build/* -rf
rm xlnt/build/* -rf
rm fmt/build/* -rf
rm recastnavigation/build/* -rf
rm box2d/build/* -rf
rm cryptopp/cmake_build/* -rf
rm lz4/build/cmake/build/* -rf

