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

start_world()
{
${DIR_file}/binary/z${serverid}_world --worldid=${serverid} --start=WORLD_SERVICE-0,MARKET_SERVICE-0,GM_SERVICE-0 --logpath=/data/log/zone_${serverid}/world -d --loglev=0
}

start_route()
{
${DIR_file}/binary/z${serverid}_route --worldid=${serverid} --start=ROUTE_SERVICE-0 --logpath=/data/log/zone_${serverid}/route -d --loglev=0
}

start_scene1()
{
    ${DIR_file}/binary/z${serverid}_scene1 --worldid=${serverid} --start=SCENE_SERVICE-1,AI_SERVICE-1,AOI_SERVICE-1 --logpath=/data/log/zone_${serverid}/scene1 -d --loglev=0
}

start_scene2()
{
    ${DIR_file}/binary/z${serverid}_scene2 --worldid=${serverid} --start=SCENE_SERVICE-2,AI_SERVICE-2,AOI_SERVICE-2 --logpath=/data/log/zone_${serverid}/scene2 -d --loglev=0
}

start_scene3()
{
    ${DIR_file}/binary/z${serverid}_scene3 --worldid=${serverid} --start=SCENE_SERVICE-3,AI_SERVICE-3,AOI_SERVICE-3 --logpath=/data/log/zone_${serverid}/scene3 -d --loglev=0
}

start_scene4()
{
    ${DIR_file}/binary/z${serverid}_scene4 --worldid=${serverid} --start=SCENE_SERVICE-4,AI_SERVICE-4,AOI_SERVICE-4 --logpath=/data/log/zone_${serverid}/scene4 -d --loglev=0
}

start_scene5()
{
    ${DIR_file}/binary/z${serverid}_scene5 --worldid=${serverid} --start=SCENE_SERVICE-5,AI_SERVICE-5,AOI_SERVICE-5 --logpath=/data/log/zone_${serverid}/scene5 -d --loglev=0
}

start_socket1()
{
    ${DIR_file}/binary/z${serverid}_socket1 --worldid=${serverid} --start=SOCKET_SERVICE-1,AUTH_SERVICE-1 --logpath=/data/log/zone_${serverid}/socket1 -d --loglev=0
}

start_socket2()
{
    ${DIR_file}/binary/z${serverid}_socket2 --worldid=${serverid} --start=SOCKET_SERVICE-2,AUTH_SERVICE-2 --logpath=/data/log/zone_${serverid}/socket2 -d --loglev=0
}

start_socket3()
{
    ${DIR_file}/binary/z${serverid}_socket3 --worldid=${serverid} --start=SOCKET_SERVICE-3,AUTH_SERVICE-3 --logpath=/data/log/zone_${serverid}/socket3 -d --loglev=0
}

start_socket4()
{
    ${DIR_file}/binary/z${serverid}_socket4 --worldid=${serverid} --start=SOCKET_SERVICE-4,AUTH_SERVICE-4 --logpath=/data/log/zone_${serverid}/socket4 -d --loglev=0
}

start_socket5()
{
    ${DIR_file}/binary/z${serverid}_socket5 --worldid=${serverid} --start=SOCKET_SERVICE-5,AUTH_SERVICE-5 --logpath=/data/log/zone_${serverid}/socket5 -d --loglev=0
}


start_scene_all()
{
    start_scene1;
    start_scene2;
    start_scene3;
    start_scene4;
    start_scene5;
}


start_socket_all()
{
    start_socket1;
    start_socket2;
    #socket3;
    #socket4;
    #socket5;
}



stop_world()
{
${DIR_file}/binary/z${serverid}_world --worldid=${serverid} --stop=WORLD_SERVICE-0,MARKET_SERVICE-0,GM_SERVICE-0
}

stop_route()
{
${DIR_file}/binary/z${serverid}_route --worldid=${serverid} --stop=ROUTE_SERVICE-0
}

stop_scene1()
{
    ${DIR_file}/binary/z${serverid}_scene1 --worldid=${serverid} --stop=SCENE_SERVICE-1,AI_SERVICE-1,AOI_SERVICE-1
}

stop_scene2()
{
    ${DIR_file}/binary/z${serverid}_scene2 --worldid=${serverid} --stop=SCENE_SERVICE-2,AI_SERVICE-2,AOI_SERVICE-2
}

stop_scene3()
{
    ${DIR_file}/binary/z${serverid}_scene3 --worldid=${serverid} --stop=SCENE_SERVICE-3,AI_SERVICE-3,AOI_SERVICE-3
}

stop_scene4()
{
    ${DIR_file}/binary/z${serverid}_scene4 --worldid=${serverid} --stop=SCENE_SERVICE-4,AI_SERVICE-4,AOI_SERVICE-4
}

stop_scene5()
{
    ${DIR_file}/binary/z${serverid}_scene5 --worldid=${serverid} --stop=SCENE_SERVICE-5,AI_SERVICE-5,AOI_SERVICE-5
}

stop_socket1()
{
    ${DIR_file}/binary/z${serverid}_socket1 --worldid=${serverid} --stop=SOCKET_SERVICE-1,AUTH_SERVICE-1
}

stop_socket2()
{
    ${DIR_file}/binary/z${serverid}_socket2 --worldid=${serverid} --stop=SOCKET_SERVICE-2,AUTH_SERVICE-2
}

stop_socket3()
{
    ${DIR_file}/binary/z${serverid}_socket3 --worldid=${serverid} --stop=SOCKET_SERVICE-3,AUTH_SERVICE-3
}

stop_socket4()
{
    ${DIR_file}/binary/z${serverid}_socket4 --worldid=${serverid} --stop=SOCKET_SERVICE-4,AUTH_SERVICE-4
}

stop_socket5()
{
    ${DIR_file}/binary/z${serverid}_socket5 --worldid=${serverid} --stop=SOCKET_SERVICE-5,AUTH_SERVICE-5
}


stop_scene_all()
{
    stop_scene1;
    stop_scene2;
    stop_scene3;
    stop_scene4;
    stop_scene5;
}


stop_socket_all()
{
    stop_socket1;
    stop_socket2;
    stop_socket3;
    stop_socket4;
    stop_socket5;
}

start()
{
    start_world;
    start_route;
    start_scene_all;
    start_socket_all;
}

stop()
{
    stop_world;
    stop_route;
    stop_scene_all;
    stop_socket_all;
}

if [ $1 ];
then
    $1;
else
    echo "$0 [start|stop] [zone_id]";    
fi