#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

cd ../../..
root_dir=`pwd`
cd -

mysql_ver="8.0"
mysql_datadir="/opt/mysqldata"
mysql_root_pwd="test12345"
registry_addr=""

docker run -d --rm --privileged=true \
--network=network-mmo --ip=172.28.1.2 \
-e MYSQL_ROOT_PASSWORD="${mysql_root_pwd}" \
-e TZ=Asia/Shanghai \
-v /${root_dir}/data/mysql-conf:/etc/mysql/mysql.conf.d \
-v /${root_dir}/data/mysql-global:/var/lib/mysql \
--name mysql-global mysql:${mysql_ver} --default-time_zone='+8:00'

docker run -d --rm --privileged=true \
--network=network-mmo --ip=172.28.2.2 \
-e MYSQL_ROOT_PASSWORD="${mysql_root_pwd}" \
-e TZ=Asia/Shanghai \
-v /${root_dir}/data/mysql-conf:/etc/mysql/mysql.conf.d \
-v /${root_dir}/data/mysql-zone1:/var/lib/mysql \
--name mysql-zone-1 mysql:${mysql_ver} --default-time_zone='+8:00'