# server-code

## depends第三方库

|目录|说明|
|:-|:-|
|depends/src/box2d|2d物理
|depends/src/brpc| baidu-rpc, rpc-server/redis-client/内置访问网页
|depends/src/cpp_redis|redis的client
|depends/src/cpp-taskflow|并发库
|depends/src/curlpp|curl-c++封装
|depends/src/fmt|format
|depends/src/gflags|命令行参数解析,brpc depends
|depends/src/jemalloc|内存分配
|depends/src/leveldb|分层k-v，brpc depends
|depends/src/libevent|网络
|depends/src/libwebsockets|websocket封装
|depends/src/lua|脚本
|depends/src/protobuf|协议库
|depends/src/recastnavigation|navmesh库
|depends/src/rapidjson|json读取库
|depends/src/tinyxml2|xml读取库
|depends/src/xlnt|xlsx读取库

## core目录
|目录|说明|
|:-|:-|
|CMake|cmake额外依赖
|comm|全局公共库
|proto|公共库内的proto
|service|服务公共库
|service_loader|服务加载器
|test|测试项目
|tools|工具项目


### core/comm目录
|目录|说明|
|:-|:-|
|basecode|基础代码库
|mydb|封装mysql访问
|network|网络库封装
|message_route|服务间通信组件
|script|脚本库


### core/service目录
|目录|说明|
|:-|:-|
|comm|公共头文件
|service_comm|service公共库
|rpcservice_comm|rpc服务封装

### core/service_loader目录
|目录|说明|
|:-|:-|
|service_loader|服务器装载器, 读取service.xml加载配置, 使用service_loader -d --start=SERVICE_NAME-X,SERVICE_NAME-Y 启动服务 service_loader --stop=SERVICE_NAME-X,SERVICE_NAME-Y 关闭服务


### core/tools目录
|目录|说明|
|:-|:-|
|cxx2lua|解析项目中头文件上的export_lua标记，自动生成lua binding代码
|excel2pb|解析xlsx文件，转换为proto文件 
|pbbin2txt|解析proto文件，转换为文本
|sql2cpp|解析sql文件，转换为struct
|robotclient|压测机器人工具 使用lua编写实例

## src目录
|目录|说明|
|:-|:-|
|game_comm|游戏公共库
|server_share|服务间共享目录
|service|具体服务
|test|测试项目
|tools|工具项目


### src/game_comm目录
|目录|说明|
|:-|:-|
|gamemap|地图数据封装
|mydbtable|数据库 结构定义
|proto|消息结构和配置文件结构


### src/service目录

#### 具体service
|服务|说明|
|:-|:-|
|socket_service   |网关(玩家前端接入)|
|auth_service     |登录验证服务，一个socket对应一个auth服务|
|world_service    |账号登录,角色创建,角色选择, 帮派,组队,排行榜等全局唯一功能|
|scene_service    |游戏核心服务，各种场景|
|ai_service       |一个scene对应一个ai服务|
|market_serice    |拍卖行服务|
|gm_serivce       |接入运营控制(一个区一个)
|route_service    |消息转发|
|dlog             |数据日志上报


|全局服务|说明|
|:-|:-|
|gmproxy_service  |全局外部回调模块(根据外部http端口回调地址, 发送内部消息给对应区分的gm_service)|
|guild_serice     |公会服务|
|toplist_service  |排行榜服务|

### src/tools目录
|目录|说明|
|:-|:-|
|merge_server|合服工具
|robotclient|压测机器人工具 使用lua编写实例


## 建立vcproj文件
* 在windows上执行make_proj.bat

## 执行步骤

```
#编译第三方库
cd depends/src
./build_depends.sh

#编译项目
cd ../..
./build.sh

#挂接文件
cd bin
shell/dev/link_debug.sh 或者 lshell/dev/ink_release.sh
shell/dev/link_depends_lib.sh


#启动global
shell/run/global_one.sh start
#分离式启动global
shell/run/all_global.sh start

#启动zone
shell/run/all_zone.sh start 1001
#分离式启动zone进程
shell/run/zone_one.sh start 1001


也可使用
./service_loader --worldid=1001 --start -d
```


# 注意事项

所有的serivce使用动态库的方式实现而不是可执行文件
这样方便使用service_loader进程来分离或合并加载不同的service

为防止因为同名类/同名变量导致代码覆盖的问题, 
world_service,socket_service,zone_service,gm_serivce,makret_serice
均只导出一个函数ServiceCreate
其他符号均为隐藏
