#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

mkdir -p build
cd build/
set -e

#CC=gcc-7 CXX=g++-7 cmake .. -DDEBUG=ON
use_make()
{
    CCACHE_SLOPPINESS=pch_defines,time_macros CC=clang-9 CXX=clang++-9 cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON -DUSE_UBSAN=ON .. 
    make -j3
}

use_ninja()
{
    CCACHE_SLOPPINESS=pch_defines,time_macros CC=clang-9 CXX=clang++-9 cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON -DUSE_UBSAN=ON -GNinja .. 
    NINJA_STATUS="[%p/%f/%t %e] " ninja
}

if [ $1 ];
then
    $1;
else
    use_ninja;
fi