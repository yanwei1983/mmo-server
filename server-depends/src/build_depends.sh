#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

set -e
CC_COMPILER=clang-9
CXX_COMPILER=clang++-9
CC_FLAGS="-fPIC -g -Wno-inconsistent-missing-override -Wno-defaulted-function-deleted -Wno-deprecated-declarations -Wno-extra-semi-stmt -Wno-cast-qual"
CXX_FLAGS="-fPIC -g -Wno-inconsistent-missing-override -Wno-defaulted-function-deleted -Wno-deprecated-declarations -Wno-extra-semi-stmt -Wno-cast-qual"
LINK_FLAGS="-fuse-ld=lld-9"

base()
{
mkdir -p ../lib
mkdir -p ../bin

#sudo autoreconf -ivf 
find -name "*.sh" -type f -exec chmod +x {} \;
}



jemalloc()
{
cd jemalloc
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} LD_FLAG=${LINK_FLAGS} ./autogen.sh  --with-jemalloc-prefix="je_" --with-mangling --enable-stats --enable-prof
make -j4
cp lib/lib* ../../lib/ -rp
cd ..
}

protobuf()
{
cd protobuf/
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake ../cmake -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -j4
cp protoc ../../../bin/ -rp
cp protoc-* ../../../bin/ -rp
cp *.a ../../../lib/ -rp
cd ../../
}

cpp_redis()
{
cd cpp_redis
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -j4
cp lib/libcpp_redis.a ../../../lib/ -rp
cp lib/libtacopie.a ../../../lib/ -rp
cd ../..
}

gflags()
{
cd gflags
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_SHARED_LIBS=1 -DBUILD_STATIC_LIBS=1 -DCMAKE_BUILD_RPATH=FALSE -DCMAKE_SKIP_INSTALL_RPATH=FALSE -DCMAKE_BUILD_WITH_INSTALL_RPATH=TRUE -DCMAKE_INSTALL_RPATH="." -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=FALSE
cmake --build . -j4
cp lib/* ../../../lib/ -rp
cp include .. -rp
cd ../..
}

snappy()
{
cd snappy
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_SHARED_LIBS=OFF -DSNAPPY_BUILD_TESTS=OFF -DCMAKE_CXX_FLAGS="-fPIC -g"
cmake --build . -j4
cp libsnappy.* ../../../lib/ -rp
cp snappy-stubs-public.h ..
cd ../..
}

leveldb()
{
cd leveldb
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_INCLUDE_PATH="../snappy/" -DCMAKE_LIBRARY_PATH="../../lib/" -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLEVELDB_BUILD_TESTS=OFF -DLEVELDB_BUILD_BENCHMARKS=OFF -DLEVELDB_INSTALL=OFF -DCMAKE_BUILD_RPATH=FALSE -DCMAKE_SKIP_INSTALL_RPATH=FALSE -DCMAKE_BUILD_WITH_INSTALL_RPATH=TRUE -DCMAKE_INSTALL_RPATH="." -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=FALSE -DHAVE_SNAPPY=ON
cmake --build . -j4
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_INCLUDE_PATH="../snappy/" -DCMAKE_LIBRARY_PATH="../../lib/" -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLEVELDB_BUILD_TESTS=OFF -DLEVELDB_BUILD_BENCHMARKS=OFF -DLEVELDB_INSTALL=OFF -DCMAKE_BUILD_RPATH=FALSE -DCMAKE_SKIP_INSTALL_RPATH=FALSE -DCMAKE_BUILD_WITH_INSTALL_RPATH=TRUE -DCMAKE_INSTALL_RPATH="." -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=FALSE -DHAVE_SNAPPY=ON
cmake --build . -j4
cp include .. -rp
cp libleveldb.* ../../../lib/ -rp
cd ../..
}

brpc()
{
cd brpc
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INCLUDE_PATH="../gflags/include;../protobuf/src;../leveldb/include" -DCMAKE_LIBRARY_PATH="../../lib/" -DPROTOBUF_PROTOC_EXECUTABLE="../../../bin/protoc" -DProtobuf_PROTOC_EXECUTABLE="../../../bin/protoc" -DCMAKE_BUILD_RPATH=FALSE -DCMAKE_SKIP_INSTALL_RPATH=FALSE -DCMAKE_BUILD_WITH_INSTALL_RPATH=TRUE -DCMAKE_INSTALL_RPATH="./:./libs:../libs:./depends:../depends" -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=FALSE
cmake --build . -j4
cp output/include .. -rp
cp output/lib/libbrpc.* ../../../lib/ -rp
cd ../..
}

libevent()
{
cd libevent
mkdir -p cmake_build
cd cmake_build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DEVENT__DISABLE_BENCHMARK=ON -DEVENT__DISABLE_TESTS=ON -DEVENT__DISABLE_REGRESS=ON -DEVENT__DISABLE_SAMPLES=ON
cmake --build . -j4
cp include .. -rp
cp lib/*.so* ../../../lib/ -rp
cd ../..
}

tinyxml2()
{
cd tinyxml2/
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -j4
cp libtinyxml2.* ../../../lib/ -rp
cd ../..
}

lua()
{
cd lua
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} LD_FLAG=${LINK_FLAGS} make linux -j4
cp src/liblua.a ../../lib/
cp src/lua ../../bin/
cp src/luac ../../bin/
cd ..
}

curlpp()
{
cd curlpp
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -j4
cp libcurlpp.* ../../../lib/ -rp
cd ../..
}

xlnt()
{
cd xlnt
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -j4
cp source/libxlnt.so* ../../../lib/ -rp
cd ../..
}

fmt()
{
cd fmt
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DFMT_TEST=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -j4
cp libfmt.a ../../../lib/ -rp
#cp libfmt.so* ../../../lib/ -rp
cd ../..
}


recastnavigation()
{
cd recastnavigation
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -j4
cp Recast/libRecast.so* ../../../lib/ -rp
cp Detour/libDetour.so* ../../../lib/ -rp
cp DetourCrowd/libDetourCrowd.so* ../../../lib/ -rp
cp DetourTileCache/libDetourTileCache.so* ../../../lib/ -rp
cd ../..
}

box2d()
{
cd box2d
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -j4
cp src/libbox2d.a ../../../lib/ -rp
cd ../..
}


spy()
{
cd spy
mkdir -p build
cd build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cd ../..
}

cryptopp()
{
cd cryptopp
mkdir -p cmake_build
cd cmake_build
CC=${CC_COMPILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} BUILD_OS=linux BUILD_MODE=native cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING=OFF -DDISABLE_ASM=ON
cmake --build . -j4
cp libcryptopp.* ../../../lib/ -rp
cd ../..
}


lz4()
{
cd lz4/build/cmake/
mkdir -p build
cd build
CC=${CC_COMILER} CXX=${CXX_COMPILER} CFLAGS=${CC_FLAGS} CXXFLAGS=${CXX_FLAGS} LD_FLAG=${LINK_FLAGS} cmake .. -DLZ4_BUILD_CLI=OFF -DLZ4_BUILD_LEGACY_LZ4C=OFF -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . -j4
cp liblz4.a ../../../../../lib/ -rp
cd ../../../..
}

$1;
 