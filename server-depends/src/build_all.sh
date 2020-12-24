set -e
cd `dirname $0`
DIR_file=`pwd`


bash build_depends.sh base;
bash build_depends.sh xlnt;
bash build_depends.sh jemalloc;
bash build_depends.sh protobuf;
bash build_depends.sh gflags;
bash build_depends.sh snappy;
bash build_depends.sh leveldb;
bash build_depends.sh brpc;
bash build_depends.sh curlpp;
bash build_depends.sh cpp_redis;
bash build_depends.sh fmt;
bash build_depends.sh libevent;
bash build_depends.sh tinyxml2;
bash build_depends.sh lua;
bash build_depends.sh recastnavigation;
bash build_depends.sh box2d;
bash build_depends.sh spy;
bash build_depends.sh cryptopp;
bash build_depends.sh lz4;