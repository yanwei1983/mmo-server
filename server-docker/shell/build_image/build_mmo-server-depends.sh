#!/bin/bash
cd `dirname $0`
cd ../../..
DIR_file=`pwd`

DOCKER_BUILDKIT=1 docker build --rm -f "server-docker/mmo-server-depends-base.Dockerfile" -t mmo-server-depends:base "."
DOCKER_BUILDKIT=1 docker build --rm -f "server-docker/mmo-server-depends.Dockerfile" -t mmo-server-depends "server-docker"