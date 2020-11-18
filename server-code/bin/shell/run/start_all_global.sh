#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

bash ${DIR_file}/start_global_route.sh
bash ${DIR_file}/start_shared_scene.sh
