#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

set -e
cmd="./releasebuild.sh && cd bin && ./cp_depends_lib.sh && ./cp_release.sh"

ln -sf .dockerignore.publish .dockerignore

docker run --rm --name mmo-server-publish --privileged --ulimit core=-1 --security-opt seccomp=unconfined -v /$(pwd)/server-code:/data/mmorpg/server-code -v /$(pwd)/server-res:/data/mmorpg/server-res -v /$(pwd)/share-code:/data/mmorpg/share-code --user=ubuntu mmo-server-depends sh -c "${cmd}"

docker build --rm -f "server-docker/mmo-server-publish.Dockerfile" -t mmo-server:$1 .
docker tag mmo-server:$1 mmo-server:latest
