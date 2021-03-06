#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

serverid=$2
if [ ! -n "$2" ] ;then  
    serverid=1
fi 

mkdir -p binary
mkdir -p .pid
mkdir -p /data/log
cd binary
ln -sf ../service_loader zone_${serverid}
cd -

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_tsan
export JE_MALLOC_CONF="metadata_thp:auto,thp:default"
start()
{
    ${DIR_file}/binary/zone_${serverid} --worldid=$serverid --start --logpath=/data/log/zone_$serverid -d
}

stop()
{
    ${DIR_file}/binary/zone_${serverid} --worldid=$serverid --stop
}


if [ $1 ];
then
    $1;
else
    echo "$0 [start|stop] [zone_id]";    
fi