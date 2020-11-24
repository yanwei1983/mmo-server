#ifndef SERVICEDEFINE_H
#define SERVICEDEFINE_H

#include <atomic>
#include <unordered_map>

#include "BaseType.h"
#include "CheckUtil.h"
#include "Misc.h"
#include "NetworkDefine.h"
#include "export_lua.h"
#include "magic_enum.h"

export_lua enum SERVICE_TYPE {
    UNKNOW_SERVICE = 0,
    WORLD_SERVICE  = 1, //单服单个, 重要节点,奔溃需重启整组服务, 登录服务, 单服内全局事件, 排行榜, 公会, 组队
    SCENE_SERVICE  = 2, //单服多个, 游戏服务,负载场景
    AI_SERVICE     = 3, //单服多个, 与scene联动, ai服务,每个ai服务对应一个scene
    AOI_SERVICE    = 4, //单服多个, 与scene联动, aoi服务,每个aoi服务对应一个scene

    MARKET_SERVICE = 5, //单服单个, 拍卖行
    GM_SERVICE     = 6, //单服单个, 无状态, 后台调用, 充值调用

    SOCKET_SERVICE = 7, //单服多个, 连接服务, 网关, 广播, 组播
    AUTH_SERVICE   = 8, //单服多个/单个, 无状态, 验证登录, 与socket联动

    GUILD_SERVICE = 9,  //单服单个
    ROUTE_SERVICE = 10, //单服单个, 无状态, 用来转发连接多个zone

    GM_PROXY_SERVICE = 11, //全局多个, 无状态, 充值回调, 后台回调, http服务, 收到后进行验证,验证后,转发给对应服的GM_SERVICE
};

enum SERVICE_UID_TYPE
{
    WORLD_SERVICE_UID       = 1,
    MARKET_SERVICE_UID      = 2,
    GM_SERVICE_UID          = 3,
    GUILD_SERIVE_UID        = 4,
    SCENE_SERVICE_UID_START = 100,
    SCENE_SERVICE_UID_END   = 200,
};

inline std::string GetServiceName(const ServiceID& service_id)
{
    auto service_type = service_id.GetServiceType();
    if(magic_enum::enum_contains<SERVICE_TYPE>(service_type) == false)
    {
        return "UNKNOWN";
    }
    auto service_type_strview = magic_enum::enum_name<SERVICE_TYPE>(static_cast<SERVICE_TYPE>(service_type));

    std::string service_type_str{service_type_strview.data(), service_type_strview.size()};

    if(service_id.GetServiceIdx() == 0)
    {

        return service_type_str;
    }
    else
    {
        return service_type_str + std::string("_") + std::to_string(service_id.GetServiceIdx());
    }
}

inline SERVICE_TYPE GetServiceTypeFromName(const std::string& name)
{
    auto service_type = magic_enum::enum_cast<SERVICE_TYPE>(name);
    return service_type.value_or(UNKNOW_SERVICE);
}

#endif /* SERVICEDEFINE_H */
