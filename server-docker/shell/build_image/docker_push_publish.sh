#!/bin/bash
cd `dirname $0`
cd ../../..
DIR_file=`pwd`

echo ${DOCKER_PASSWORD} | docker login --username=${DOCKER_USERNAME} --password-stdin ${DOCKER_REPO_BASE}

docker tag mmo-server:latest ${DOCKER_REPO_URL}/mmo-server:latest
docker push ${DOCKER_REPO_URL}/mmo-server

