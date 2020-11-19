#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

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

route()
{
   ${DIR_file}/binary/route --worldid=0 --start=ROUTE_SERVICE-0 --logpath=/data/log/global/route -d 
}

gmproxy_1()
{
    ${DIR_file}/binary/gmproxy_1 --worldid=0 --start=GM_PROXY_SERVICE-1 --logpath=/data/log/global/gmproxy1 -d 
}

gmproxy_2()
{
    ${DIR_file}/binary/gmproxy_2 --worldid=0 --start=GM_PROXY_SERVICE-2 --logpath=/data/log/global/gmproxy2 -d 
}

gmproxy_3()
{
${DIR_file}/binary/gmproxy_3 --worldid=0 --start=GM_PROXY_SERVICE-3 --logpath=/data/log/global/gmproxy3 -d 
}

gmproxy_4()
{
${DIR_file}/binary/gmproxy_4 --worldid=0 --start=GM_PROXY_SERVICE-4 --logpath=/data/log/global/gmproxy4 -d 
}

gmproxy_5()
{
${DIR_file}/binary/gmproxy_5 --worldid=0 --start=GM_PROXY_SERVICE-5 --logpath=/data/log/global/gmproxy5 -d 
}


gmproxy_all()
{
    gmproxy_1;
    #gmproxy_2;
    #gmproxy_3;
    #gmproxy_4;
    #gmproxy_5;
}

if [ $1 ];
then
    $1;
else
    route;
    gmproxy_all;
    
fi