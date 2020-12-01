#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

echo "$1 $2"
ext_param=$2

mkdir -p binary
mkdir -p .pid
mkdir -p /data/log
cd binary

ln -sf ../service_loader gmproxy_1
ln -sf ../service_loader gmproxy_2
ln -sf ../service_loader gmproxy_3
ln -sf ../service_loader gmproxy_4
ln -sf ../service_loader gmproxy_5
ln -sf ../service_loader route
cd -

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_tsan

start_route()
{
   ${DIR_file}/binary/route --worldid=0 --start=ROUTE_SERVICE-0 --logpath=/data/log/global/route -d $ext_param
}

start_gmproxy_1()
{
    ${DIR_file}/binary/gmproxy_1 --worldid=0 --start=GM_PROXY_SERVICE-1 --logpath=/data/log/global/gmproxy1 -d $ext_param
}

start_gmproxy_2()
{
    ${DIR_file}/binary/gmproxy_2 --worldid=0 --start=GM_PROXY_SERVICE-2 --logpath=/data/log/global/gmproxy2 -d $ext_param
}

start_gmproxy_3()
{
${DIR_file}/binary/gmproxy_3 --worldid=0 --start=GM_PROXY_SERVICE-3 --logpath=/data/log/global/gmproxy3 -d $ext_param
}

start_gmproxy_4()
{
${DIR_file}/binary/gmproxy_4 --worldid=0 --start=GM_PROXY_SERVICE-4 --logpath=/data/log/global/gmproxy4 -d $ext_param
}

start_gmproxy_5()
{
${DIR_file}/binary/gmproxy_5 --worldid=0 --start=GM_PROXY_SERVICE-5 --logpath=/data/log/global/gmproxy5 -d $ext_param
}


stop_route()
{
    ${DIR_file}/binary/route --worldid=0 --stop=ROUTE_SERVICE-0
}

stop_gmproxy_1()
{
    ${DIR_file}/binary/gmproxy_1 --worldid=0 --stop=GM_PROXY_SERVICE-1
}

stop_gmproxy_2()
{
    ${DIR_file}/binary/gmproxy_2 --worldid=0 --stop=GM_PROXY_SERVICE-2
}

stop_gmproxy_3()
{
${DIR_file}/binary/gmproxy_3 --worldid=0 --stop=GM_PROXY_SERVICE-3
}

stop_gmproxy_4()
{
${DIR_file}/binary/gmproxy_4 --worldid=0 --stop=GM_PROXY_SERVICE-4
}

stop_gmproxy_5()
{
${DIR_file}/binary/gmproxy_5 --worldid=0 --stop=GM_PROXY_SERVICE-5
}


stop_gmproxy_all()
{
    stop_gmproxy_1;
    stop_gmproxy_2;
    stop_gmproxy_3;
    stop_gmproxy_4;
    stop_gmproxy_5;
}

start_gmproxy_all()
{
    start_gmproxy_1;
    #gmproxy_2;
    #gmproxy_3;
    #gmproxy_4;
    #gmproxy_5;
}

start()
{
    start_route;
    start_gmproxy_all;
}

stop()
{
    stop_route;
    stop_gmproxy_all;
}


if [ $1 ];
then
    $1;
else
    echo "$0 [start|stop]";   
    
fi