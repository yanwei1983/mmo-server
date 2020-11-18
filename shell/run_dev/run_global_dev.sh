#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

docker run -t --rm --network=network-mmo --ip=172.28.1.3 --name mmo-server-global-dev --volumes-from mmo-server-code --privileged --ulimit core=-1 --security-opt seccomp=unconfined mmo-server-depends sh -c "cd bin&&./start_global.sh&&bash"