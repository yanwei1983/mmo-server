#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

ln -sf .dockerignore.depends .dockerignore

build_base_2004()
{
    docker build --rm -f "server-docker/mmo-server-base-2004.Dockerfile" -t mmo-server-base:20.04 server-docker
}

if [ $1 ];
then
    $1;
else
    build_base_2004;
    
fi
