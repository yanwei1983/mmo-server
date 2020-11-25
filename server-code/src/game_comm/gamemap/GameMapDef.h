#ifndef GAMEMAPDEF_H
#define GAMEMAPDEF_H

#include "BaseCode.h"
export_lua enum MAP_TYPE {
    MAPTYPE_NORMAL = 0, //普通地图
};

export_lua enum MAP_FLAG {
    MAPFLAG_DYNAMAP = 0x0001, //副本

    MAPFLAG_COLLISION_ENABLE = 0x0100, // 此地图上需要进行碰撞检查

    MAPFLAG_DISABLE_RANDTRANS = 0x00001000, // 禁止随机传送（包括使用随机传送道具，随机传送技能）
    MAPFLAG_DISABLE_FLYMAP    = 0x00002000, // 禁止飞地图（包括使用：飞行道具，定位技能）
    MAPFLAG_DISABLE_ATTACK    = 0x00004000, // 禁止玩家使用任何技能同时不允许玩家攻击
    MAPFLAG_DISABLE_MOUNT     = 0x00008000, // 禁止玩家使用坐骑

    MAPFLAG_DISABLE_HELP        = 0x00010000, // 禁止求救
    MAPFLAG_DISABLE_CHANGEPK    = 0x00020000, // 禁止手动切换PK模式
    MAPFLAG_DISABLE_PKPROTECTED = 0x00040000, // 禁止PK保护
    MAPFLAG_DISABLE_PK          = 0x00080000, // 禁止PK

    MAPFLAG_DISABLE_LEAVE           = 0x00100000, // 禁止使用直接离开副本的按钮
    MAPFLAG_LEVAE_TO_HOME           = 0x00200000, // 点击离开副本按钮会回到安全区而不是回来进入副本前最后的记录点
    MAPFLAG_DISABLE_REBORN_MAPPOS   = 0x00400000, // 禁止在副本复活点复活
    MAPFLAG_DISABLE_REBORN_STANDPOS = 0x00800000, // 禁止原地复活

    MAPFLAG_RECORD_DISABLE = 0x01000000, // 禁止记录下线坐标
    MAPFLAG_HOME_DISABLE   = 0x02000000, // 禁止记录回城点
    MAPFLAG_DISABLE_STALL  = 0x04000000, // 禁止摆摊
    MAPFLAG_DISABLE_PLACE  = 0x08000000, // 禁止放置地面物品

};

export_lua enum PARTOL_TYPE { PARTOL_ONCE, PARTOL_RING, PARTOL_BACK };

export_lua enum MonsterGeneratorShapeType {
    MONSTERGENERATOR_CIRCLE = 0, // x,z为中心range的圆形
    MONSTERGENERATOR_RECT   = 1, // x,z为左上角,width宽,range高的矩形
};

export_lua enum REBORN_TYPE {
    REBORN_HOME,
    REBORN_MAPPOS,
    REBORN_STANDPOS,
};

#endif /* GAMEMAPDEF_H */
