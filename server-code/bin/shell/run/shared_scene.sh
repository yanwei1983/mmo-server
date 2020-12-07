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
ln -sf ../service_loader shared_scene_1
ln -sf ../service_loader shared_scene_2
ln -sf ../service_loader shared_scene_3
ln -sf ../service_loader shared_scene_4
ln -sf ../service_loader shared_scene_5
cd -

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/asan_cfg/options_tsan

start_scene1()
{
${DIR_file}/binary/shared_scene_1 --worldid=0 --start=SCENE_SERVICE-51,AI_SERVICE-51,AOI_SERVICE-51 --logpath=/data/log/global/shared_scene_1 -d $ext_param
}

start_scene2()
{
${DIR_file}/binary/shared_scene_2 --worldid=0 --start=SCENE_SERVICE-52,AI_SERVICE-52,AOI_SERVICE-52 --logpath=/data/log/global/shared_scene_2 -d $ext_param
}

start_scene3()
{
${DIR_file}/binary/shared_scene_3 --worldid=0 --start=SCENE_SERVICE-53,AI_SERVICE-53,AOI_SERVICE-53 --logpath=/data/log/global/shared_scene_3 -d $ext_param
}

start_scene4()
{
${DIR_file}/binary/shared_scene_4 --worldid=0 --start=SCENE_SERVICE-54,AI_SERVICE-54,AOI_SERVICE-54 --logpath=/data/log/global/shared_scene_4 -d $ext_param
}

start_scene5()
{
${DIR_file}/binary/shared_scene_5 --worldid=0 --start=SCENE_SERVICE-55,AI_SERVICE-55,AOI_SERVICE-55 --logpath=/data/log/global/shared_scene_5 -d $ext_param
}


stop_scene1()
{
${DIR_file}/service_loader --worldid=0 --stop=SCENE_SERVICE-51,AI_SERVICE-51,AOI_SERVICE-51
}

stop_scene2()
{
${DIR_file}/service_loader --worldid=0 --stop=SCENE_SERVICE-52,AI_SERVICE-52,AOI_SERVICE-52
}

stop_scene3()
{
${DIR_file}/service_loader --worldid=0 --stop=SCENE_SERVICE-53,AI_SERVICE-53,AOI_SERVICE-53
}

stop_scene4()
{
${DIR_file}/service_loader --worldid=0 --stop=SCENE_SERVICE-54,AI_SERVICE-54,AOI_SERVICE-54
}

stop_scene5()
{
${DIR_file}/service_loader --worldid=0 --stop=SCENE_SERVICE-55,AI_SERVICE-55,AOI_SERVICE-55
}

start()
{
    start_scene1;
    start_scene2;
    start_scene3;
    start_scene4;
    start_scene5;
}

stop()
{
    stop_scene1;
    stop_scene2;
    stop_scene3;
    stop_scene4;
    stop_scene5;
}

if [ $1 ];
then
    $1;
else
    echo "$0 [start|stop]";   
fi