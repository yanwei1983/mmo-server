#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

docker pull ubuntu:20.04
docker pull mysql:8.0