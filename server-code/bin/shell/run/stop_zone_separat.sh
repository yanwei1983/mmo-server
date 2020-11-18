#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`



serverid=$1
if [ ! -n "$1" ] ;then  
    serverid=1
fi 


export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_tsan


world()
{
${DIR_file}/binary/z${serverid}_world --worldid=${serverid} --stop=WORLD_SERVICE-0,MARKET_SERVICE-0,GM_SERVICE-0
}

route()
{
${DIR_file}/binary/z${serverid}_route --worldid=${serverid} --stop=ROUTE_SERVICE-0
}

scene1()
{
    ${DIR_file}/binary/z${serverid}_scene1 --worldid=${serverid} --stop=SCENE_SERVICE-1,AI_SERVICE-1
}

scene2()
{
    ${DIR_file}/binary/z${serverid}_scene2 --worldid=${serverid} --stop=SCENE_SERVICE-2,AI_SERVICE-2
}

scene3()
{
    ${DIR_file}/binary/z${serverid}_scene3 --worldid=${serverid} --stop=SCENE_SERVICE-3,AI_SERVICE-3
}

scene4()
{
    ${DIR_file}/binary/z${serverid}_scene4 --worldid=${serverid} --stop=SCENE_SERVICE-4,AI_SERVICE-4
}

scene5()
{
    ${DIR_file}/binary/z${serverid}_scene5 --worldid=${serverid} --stop=SCENE_SERVICE-5,AI_SERVICE-5
}

socket1()
{
    ${DIR_file}/binary/z${serverid}_socket1 --worldid=${serverid} --stop=SOCKET_SERVICE-1,AUTH_SERVICE-1
}

socket2()
{
    ${DIR_file}/binary/z${serverid}_socket2 --worldid=${serverid} --stop=SOCKET_SERVICE-2,AUTH_SERVICE-2
}

socket3()
{
    ${DIR_file}/binary/z${serverid}_socket3 --worldid=${serverid} --stop=SOCKET_SERVICE-3,AUTH_SERVICE-3
}

socket4()
{
    ${DIR_file}/binary/z${serverid}_socket4 --worldid=${serverid} --stop=SOCKET_SERVICE-4,AUTH_SERVICE-4
}

socket5()
{
    ${DIR_file}/binary/z${serverid}_socket5 --worldid=${serverid} --stop=SOCKET_SERVICE-5,AUTH_SERVICE-5
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
    socket3;
    socket4;
    socket5;
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