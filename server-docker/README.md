# 快捷shell
|构建||
|:-|:-|
| shell/build_image/install_docker.sh           |linux安装docker|
| shell/build_image/pull_depends_image.sh       |拉取依赖docker|
| shell/build_image/build_base2004_image.sh     |构建ubuntu20.04基础docker|
| shell/build_image/build_mmo-server-depends.sh |构建外部依赖库|
| shell/build_image/build_publish_image.sh      |对外发布|
***

|设置代码开发环境||
|:-|:-|
| shell/coding/link_codedir.sh             |将代码目录映射给docker数据卷|
| shell/coding/start_code.sh               |启动编译用docker|
***
|设置代码dev调试环境||
|:-|:-|
| shell/setup_dev/create_network.sh           |创建docker实验网络|
| shell/setup_dev/run_mysql.sh              |启动global，zone的数据库docker|
| shell/setup_dev/init_globaldb.sh  254_debug.env          |初始化global数据库|
| shell/setup_dev/init_newzonedb.sh 254_debug.env  |初始化zone数据库|
***

|运行dev服务器||
|:-|:-|
| shell/run_dev/show_serverdetail.sh        |显示数据库里的服务路由表|
| shell/run_dev/start_all_global.sh         |分离式启动所有global服务|
| shell/run_dev/run_global_dev.sh           |单进程启动global服务|
| shell/run_dev/run_zone_dev.sh             |单进程启动zone服务|