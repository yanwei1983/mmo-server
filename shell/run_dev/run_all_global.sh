#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

docker run --rm -it --network=network-mmo --ip=172.28.1.3 --name mmo-server-publish-global --volumes-from mmo-server-code --ulimit core=-1 --security-opt seccomp=unconfined mmo-server sh -c "./start_all_global.sh&&bash"