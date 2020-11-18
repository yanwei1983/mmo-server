#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

${DIR_file}/stop_global_route.sh
${DIR_file}/stop_shared_scene.sh