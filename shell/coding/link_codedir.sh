#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

cd ../..
root_dir=`pwd`
cd -

docker run --network=network-mmo --ip=172.28.1.3 \
--name mmo-server-code  \
--ulimit core=-1 --security-opt seccomp=unconfined \
-v /${root_dir}/server-code:/data/mmorpg/server-code \
-v /${root_dir}/server-res:/data/mmorpg/server-res \
-v /${root_dir}/share-code:/data/mmorpg/share-code \
-v /${root_dir}/data/log:/data/log  \
--user=ubuntu mmo-server-depends