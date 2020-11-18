#!/bin/bash
cd `dirname $0`
cd ../..
DIR_file=`pwd`

docker pull ${DOCKER_REPO_URL}/mmo-server-depends
docker tag ${DOCKER_REPO_URL}/mmo-server-depends mmo-server-depends