#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

mkdir -p binary
mkdir -p .pid
mkdir -p /data/log
cd binary
ln -sf ../service_loader global_all
cd -

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_tsan

start()
{
${DIR_file}/binary/global_all --worldid=0 --start --logpath=/data/log/global_all -d
}

stop()
{
    ${DIR_file}/binary/global_all --worldid=0 --stop --logpath=/data/log/global_all
}

if [ $1 ];
then
    $1;
else
    echo "$0 [start|stop]";    
    
fi