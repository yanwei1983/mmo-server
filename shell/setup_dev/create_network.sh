#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

docker network create --driver=bridge --subnet=172.28.0.0/16 --ip-range=172.28.0.0/16 --gateway=172.28.1.1 --attachable network-mmo
