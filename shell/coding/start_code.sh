#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

docker run -it --network=network-mmo --ip=172.28.1.254 --name mmo-server-build --volumes-from mmo-server-code --privileged --ulimit core=-1 --security-opt seccomp=unconfined mmo-server-depends bash