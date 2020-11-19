# mmo-server
这是一个实验性质的mmo-server,出于开发阶段，请勿使用于生产环境


# 目录
|目录||
|:-|:-|
|./sercer-code | 代码|
|./server-depends |外部依赖库|
|./server-docker |docker配置文件|
|./server-res |资源目录|
|./share-code |client-server共享文件|

  
***

可以通过docker来编译运行， 具体请看[server-docker](server-docker/README.md)
也可以直接编译
. 执行server-depends/src/aptget_install.sh 安装依赖
. 执行server-depends/src/build_depends.sh 编译第三方库
. 执行server-code/build_and_check.sh 编译服务器  [server-code](server-code/README.md) 


