#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

ln -sf .dockerignore.depends .dockerignore

build_base_1804()
{
    docker build --rm -f "server-docker/mmo-server-base-1804.Dockerfile" -t mmo-server-base:18.04 server-docker
}

build_base_1804_github_action()
{
    docker build --rm -f "server-docker/mmo-server-base-1804-github_action.Dockerfile" -t mmo-server-base:18.04 server-docker
}

if [ $1 ];
then
    $1;
else
    build_base_1804;
    
fi
