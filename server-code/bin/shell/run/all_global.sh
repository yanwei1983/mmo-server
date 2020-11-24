#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

if [ $1 ];
then
    bash ${DIR_file}/global_route.sh $1
    bash ${DIR_file}/shared_scene.sh $1
else
    echo "$0 [start|stop]";
fi


