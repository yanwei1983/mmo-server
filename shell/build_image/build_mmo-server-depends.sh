#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

ln -sf .dockerignore.depends .dockerignore
docker build --rm -f "server-docker/mmo-server-depends-base.Dockerfile" -t mmo-server-depends:base "."
docker build --rm -f "server-docker/mmo-server-depends.Dockerfile" -t mmo-server-depends "server-docker"