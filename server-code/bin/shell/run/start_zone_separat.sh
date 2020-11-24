#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

serverid=$1
if [ ! -n "$1" ] ;then  
    serverid=1
fi 

mkdir -p binary
mkdir -p .pid
mkdir -p /data/log
cd binary
ln -sf ../service_loader z${serverid}_world
ln -sf ../service_loader z${serverid}_route
ln -sf ../service_loader z${serverid}_scene1
ln -sf ../service_loader z${serverid}_scene2
ln -sf ../service_loader z${serverid}_scene3
ln -sf ../service_loader z${serverid}_scene4
ln -sf ../service_loader z${serverid}_scene5
ln -sf ../service_loader z${serverid}_socket1
ln -sf ../service_loader z${serverid}_socket2
ln -sf ../service_loader z${serverid}_socket3
ln -sf ../service_loader z${serverid}_socket4
ln -sf ../service_loader z${serverid}_socket5
cd -

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_tsan

world()
{
${DIR_file}/binary/z${serverid}_world --worldid=${serverid} --start=WORLD_SERVICE-0,MARKET_SERVICE-0,GM_SERVICE-0 --logpath=/data/log/zone_${serverid}/world -d
}

route()
{
${DIR_file}/binary/z${serverid}_route --worldid=${serverid} --start=ROUTE_SERVICE-0 --logpath=/data/log/zone_${serverid}/route -d
}

scene1()
{
    ${DIR_file}/binary/z${serverid}_scene1 --worldid=${serverid} --start=SCENE_SERVICE-1,AI_SERVICE-1,AOI_SERVICE-1 --logpath=/data/log/zone_${serverid}/scene1 -d
}

scene2()
{
    ${DIR_file}/binary/z${serverid}_scene2 --worldid=${serverid} --start=SCENE_SERVICE-2,AI_SERVICE-2,AOI_SERVICE-2 --logpath=/data/log/zone_${serverid}/scene2 -d
}

scene3()
{
    ${DIR_file}/binary/z${serverid}_scene3 --worldid=${serverid} --start=SCENE_SERVICE-3,AI_SERVICE-3,AOI_SERVICE-3 --logpath=/data/log/zone_${serverid}/scene3 -d
}

scene4()
{
    ${DIR_file}/binary/z${serverid}_scene4 --worldid=${serverid} --start=SCENE_SERVICE-4,AI_SERVICE-4,AOI_SERVICE-4 --logpath=/data/log/zone_${serverid}/scene4 -d
}

scene5()
{
    ${DIR_file}/binary/z${serverid}_scene5 --worldid=${serverid} --start=SCENE_SERVICE-5,AI_SERVICE-5,AOI_SERVICE-5 --logpath=/data/log/zone_${serverid}/scene5 -d
}

socket1()
{
    ${DIR_file}/binary/z${serverid}_socket1 --worldid=${serverid} --start=SOCKET_SERVICE-1,AUTH_SERVICE-1 --logpath=/data/log/zone_${serverid}/socket1 -d
}

socket2()
{
    ${DIR_file}/binary/z${serverid}_socket2 --worldid=${serverid} --start=SOCKET_SERVICE-2,AUTH_SERVICE-2 --logpath=/data/log/zone_${serverid}/socket2 -d
}

socket3()
{
    ${DIR_file}/binary/z${serverid}_socket3 --worldid=${serverid} --start=SOCKET_SERVICE-3,AUTH_SERVICE-3 --logpath=/data/log/zone_${serverid}/socket3 -d
}

socket4()
{
    ${DIR_file}/binary/z${serverid}_socket4 --worldid=${serverid} --start=SOCKET_SERVICE-4,AUTH_SERVICE-4 --logpath=/data/log/zone_${serverid}/socket4 -d
}

socket5()
{
    ${DIR_file}/binary/z${serverid}_socket5 --worldid=${serverid} --start=SOCKET_SERVICE-5,AUTH_SERVICE-5 --logpath=/data/log/zone_${serverid}/socket5 -d
}


scene_all()
{
    scene1;
    scene2;
    scene3;
    scene4;
    scene5;
}


socket_all()
{
    socket1;
    socket2;
    #socket3;
    #socket4;
    #socket5;
}

if [ $2 ];
then
    $2;
else
    world;
    route;
    scene_all;
    socket_all;
    
fi