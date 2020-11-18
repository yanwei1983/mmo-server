#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_tsan

route()
{
    ${DIR_file}/binary/route --worldid=0 --stop=ROUTE_SERVICE-0
}

gmproxy_1()
{
    ${DIR_file}/binary/gmproxy_1 --worldid=0 --stop=GM_PROXY_SERVICE-1
}

gmproxy_2()
{
    ${DIR_file}/binary/gmproxy_2 --worldid=0 --stop=GM_PROXY_SERVICE-2
}

gmproxy_3()
{
${DIR_file}/binary/gmproxy_3 --worldid=0 --stop=GM_PROXY_SERVICE-3
}

gmproxy_4()
{
${DIR_file}/binary/gmproxy_4 --worldid=0 --stop=GM_PROXY_SERVICE-4
}

gmproxy_5()
{
${DIR_file}/binary/gmproxy_5 --worldid=0 --stop=GM_PROXY_SERVICE-5
}


gmproxy_all()
{
    gmproxy_1;
    gmproxy_2;
    gmproxy_3;
    gmproxy_4;
    gmproxy_5;
}

if [ $1 ];
then
    $1;
else
    route;
    gmproxy_all;
    
fi