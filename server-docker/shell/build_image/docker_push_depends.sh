#!/bin/bash
cd `dirname $0`
cd ../../..
DIR_file=`pwd`

echo ${DOCKER_PASSWORD} | docker login --username=${DOCKER_USERNAME} --password-stdin ${DOCKER_REPO_BASE}

docker tag mmo-server-base:20.04 ${DOCKER_REPO_URL}/mmo-server-base:20.04
docker tag mmo-server-depends:base ${DOCKER_REPO_URL}/mmo-server-depends:base   
docker tag mmo-server-depends:latest ${DOCKER_REPO_URL}/mmo-server-depends:latest
docker push ${DOCKER_REPO_URL}/mmo-server-base
docker push ${DOCKER_REPO_URL}/mmo-server-depends
