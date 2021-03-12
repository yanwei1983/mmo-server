
#ifndef gamedb_H
#define gamedb_H
#include <string>

#include "BaseCode.h"
#include "DBField.h"

struct TBLD_COMMONDATA
{
    static constexpr const char* table_name() { return "tbld_commondata"; }
    enum FIELD_ENUMS
    {
        ID,       //'id'
        PLAYERID, //'玩家id'
        KEYTYPE,  //'数据类型'
        KEYIDX,   //'数据编号'
        DATA0,    //'数据1'
        DATA1,    //'数据2'
        DATA2,    //'数据3'
        DATA3,    //'数据4'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("playerid", "  `playerid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("keytype", "  `keytype` int unsigned NOT NULL DEFAULT '0' COMMENT '数据类型'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("keyidx", "  `keyidx` int unsigned NOT NULL DEFAULT '0' COMMENT '数据编号'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("data0", "  `data0` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据1'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("data1", "  `data1` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据2'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("data2", "  `data2` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据3'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("data3", "  `data3` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据4'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, uint64_t, uint64_t>;

    static constexpr size_t field_count() { return 8; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("idx_player", "playerid"), std::make_tuple("PRIMARY", "id")); }

    static constexpr size_t keys_size() { return 2; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_commondata` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `playerid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `keytype` int unsigned NOT NULL DEFAULT '0' COMMENT '数据类型',
  `keyidx` int unsigned NOT NULL DEFAULT '0' COMMENT '数据编号',
  `data0` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据1',
  `data1` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据2',
  `data2` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据3',
  `data3` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据4',
  PRIMARY KEY (`id`),
  KEY `idx_player` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_COOLDOWN
{
    static constexpr const char* table_name() { return "tbld_cooldown"; }
    enum FIELD_ENUMS
    {
        ID,          //'id'
        PLAYERID,    //'玩家id'
        KEYTYPE,     //'CD类型'
        KEYIDX,      //'CD编号'
        EXPIRE_TIME, //'CD结束时间戳'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("playerid", "  `playerid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("keytype", "  `keytype` int unsigned NOT NULL DEFAULT '0' COMMENT 'CD类型'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("keyidx", "  `keyidx` int unsigned NOT NULL DEFAULT '0' COMMENT 'CD编号'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("expire_time",
                            "  `expire_time` int unsigned NOT NULL DEFAULT '0' COMMENT 'CD结束时间戳'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 5; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_player", "playerid"),
                               std::make_tuple("idx_player_idx", "playerid,keytype,keyidx"),
                               std::make_tuple("PRIMARY", "id"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_cooldown` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `playerid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `keytype` int unsigned NOT NULL DEFAULT '0' COMMENT 'CD类型',
  `keyidx` int unsigned NOT NULL DEFAULT '0' COMMENT 'CD编号',
  `expire_time` int unsigned NOT NULL DEFAULT '0' COMMENT 'CD结束时间戳',
  PRIMARY KEY (`id`),
  KEY `idx_player_idx` (`playerid`,`keytype`,`keyidx`),
  KEY `idx_player` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_DATACOUNT
{
    static constexpr const char* table_name() { return "tbld_datacount"; }
    enum FIELD_ENUMS
    {
        ID,              //'id'
        PLAYERID,        //'玩家id'
        KEYTYPE,         //'data类型'
        KEYIDX,          //'data编号'
        DATA_NUM,        //'累计计数'
        NEXT_RESET_TIME, //'下一次重置的时间戳'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("playerid", "  `playerid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("keytype", "  `keytype` int unsigned NOT NULL DEFAULT '0' COMMENT 'data类型'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("keyidx", "  `keyidx` int unsigned NOT NULL DEFAULT '0' COMMENT 'data编号'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("data_num",
                            "  `data_num` bigint unsigned NOT NULL DEFAULT '0' COMMENT '累计计数'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false),
            std::make_tuple("next_reset_time",
                            "  `next_reset_time` int unsigned NOT NULL DEFAULT '0' COMMENT '下一次重置的时间戳'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint64_t, uint32_t>;

    static constexpr size_t field_count() { return 6; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_player", "playerid"),
                               std::make_tuple("idx_player_idx", "playerid,keytype,keyidx"),
                               std::make_tuple("PRIMARY", "id"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_datacount` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `playerid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `keytype` int unsigned NOT NULL DEFAULT '0' COMMENT 'data类型',
  `keyidx` int unsigned NOT NULL DEFAULT '0' COMMENT 'data编号',
  `data_num` bigint unsigned NOT NULL DEFAULT '0' COMMENT '累计计数',
  `next_reset_time` int unsigned NOT NULL DEFAULT '0' COMMENT '下一次重置的时间戳',
  PRIMARY KEY (`id`),
  KEY `idx_player_idx` (`playerid`,`keytype`,`keyidx`),
  KEY `idx_player` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_ITEM
{
    static constexpr const char* table_name() { return "tbld_item"; }
    enum FIELD_ENUMS
    {
        ID,           //'id'
        OWNER_ID,     //'所有者id'
        ITEMTYPE,     //'道具类型编号'
        POSITION,     //'所属包裹类型0:包裹 1:装备槽 100:仓库 101:帮派仓库 102:回购列表 103:邮箱 104:拍卖行'
        GRID,         //'所属包裹位置'
        FLAG,         //'物品掩码'
        PILENUM,      //'堆叠数量'
        EXPIRE_TIME,  //'过期时间戳秒'
        DURA,         //'当前耐久'
        DURA_LIMIT,   //'耐久上限'
        ADDITION_LEV, //'强化等级'
        EXTRA,        //'额外信息'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("owner_id",
                            "  `owner_id` bigint unsigned NOT NULL DEFAULT '0' COMMENT '所有者id'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false),
            std::make_tuple("itemtype", "  `itemtype` int unsigned NOT NULL DEFAULT '0' COMMENT '道具类型编号'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("position",
                            "  `position` int unsigned NOT NULL DEFAULT '0' COMMENT '所属包裹类型0:包裹 1:装备槽 100:仓库 101:帮派仓库 102:回购列表 "
                            "103:邮箱 104:拍卖行'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("grid", "  `grid` int unsigned NOT NULL DEFAULT '0' COMMENT '所属包裹位置'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("flag", "  `flag` int unsigned NOT NULL DEFAULT '0' COMMENT '物品掩码'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("pilenum", "  `pilenum` int unsigned NOT NULL DEFAULT '0' COMMENT '堆叠数量'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("expire_time",
                            "  `expire_time` int unsigned NOT NULL DEFAULT '0' COMMENT '过期时间戳秒'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("dura", "  `dura` int unsigned NOT NULL DEFAULT '0' COMMENT '当前耐久'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("dura_limit", "  `dura_limit` int unsigned NOT NULL DEFAULT '0' COMMENT '耐久上限'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("addition_lev",
                            "  `addition_lev` int unsigned NOT NULL DEFAULT '0' COMMENT '强化等级'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("extra", "  `extra` blob COMMENT '额外信息'", DB_FIELD_TYPE_BLOB, false));
    }

    using field_type_t =
        type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, std::string>;

    static constexpr size_t field_count() { return 12; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_owner_pack", "owner_id,position"),
                               std::make_tuple("idx_itemtype", "itemtype"),
                               std::make_tuple("idx_owner", "owner_id"),
                               std::make_tuple("PRIMARY", "id"));
    }

    static constexpr size_t keys_size() { return 4; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_item` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `owner_id` bigint unsigned NOT NULL DEFAULT '0' COMMENT '所有者id',
  `itemtype` int unsigned NOT NULL DEFAULT '0' COMMENT '道具类型编号',
  `position` int unsigned NOT NULL DEFAULT '0' COMMENT '所属包裹类型0:包裹 1:装备槽 100:仓库 101:帮派仓库 102:回购列表 103:邮箱 104:拍卖行',
  `grid` int unsigned NOT NULL DEFAULT '0' COMMENT '所属包裹位置',
  `flag` int unsigned NOT NULL DEFAULT '0' COMMENT '物品掩码',
  `pilenum` int unsigned NOT NULL DEFAULT '0' COMMENT '堆叠数量',
  `expire_time` int unsigned NOT NULL DEFAULT '0' COMMENT '过期时间戳秒',
  `dura` int unsigned NOT NULL DEFAULT '0' COMMENT '当前耐久',
  `dura_limit` int unsigned NOT NULL DEFAULT '0' COMMENT '耐久上限',
  `addition_lev` int unsigned NOT NULL DEFAULT '0' COMMENT '强化等级',
  `extra` blob COMMENT '额外信息',
  PRIMARY KEY (`id`),
  KEY `idx_itemtype` (`itemtype`),
  KEY `idx_owner_pack` (`owner_id`,`position`),
  KEY `idx_owner` (`owner_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_MAIL
{
    static constexpr const char* table_name() { return "tbld_mail"; }
    enum FIELD_ENUMS
    {
        ID,          //'id'
        OWNERID,     //'拥有者id'
        SENDERID,    //'拥有者id'
        SENDERNAME,  //'发送者名字'
        CREATE_TIME, //'发送时间戳'
        FLAG,        //'邮件标志'
        SUBJECT,     //'邮件标题'
        CONTENT,     //'邮件内容'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("ownerid", "  `ownerid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '拥有者id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("senderid",
                            "  `senderid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '拥有者id'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false),
            std::make_tuple("sendername",
                            "  `sendername` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '发送者名字'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("create_time",
                            "  `create_time` int unsigned NOT NULL DEFAULT '0' COMMENT '发送时间戳'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("flag", "  `flag` bigint unsigned NOT NULL DEFAULT '0' COMMENT '邮件标志'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("subject",
                            "  `subject` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '邮件标题'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("content",
                            "  `content` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '邮件内容'",
                            DB_FIELD_TYPE_VARCHAR,
                            false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint64_t, char[255], uint32_t, uint64_t, char[255], char[1024]>;

    static constexpr size_t field_count() { return 8; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_ownerid", "ownerid"),
                               std::make_tuple("idx_sendid", "senderid"),
                               std::make_tuple("idx_create_time", "create_time"),
                               std::make_tuple("PRIMARY", "id"));
    }

    static constexpr size_t keys_size() { return 4; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_mail` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `ownerid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '拥有者id',
  `senderid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '拥有者id',
  `sendername` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '发送者名字',
  `create_time` int unsigned NOT NULL DEFAULT '0' COMMENT '发送时间戳',
  `flag` bigint unsigned NOT NULL DEFAULT '0' COMMENT '邮件标志',
  `subject` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '邮件标题',
  `content` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '邮件内容',
  PRIMARY KEY (`id`),
  KEY `idx_create_time` (`create_time`),
  KEY `idx_sendid` (`senderid`),
  KEY `idx_ownerid` (`ownerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_MAIL_ATTACHMENT
{
    static constexpr const char* table_name() { return "tbld_mail_attachment"; }
    enum FIELD_ENUMS
    {
        ID,          //'id'
        MAILID,      //'邮件id'
        ATTACH_TYPE, //'附件类型'
        ATTACH_DATA, //'数量/具体道具实例ID'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("mailid", "  `mailid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '邮件id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("attach_type",
                            "  `attach_type` int unsigned NOT NULL DEFAULT '0' COMMENT '附件类型'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("attach_data",
                            "  `attach_data` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数量/具体道具实例ID'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint64_t>;

    static constexpr size_t field_count() { return 4; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("idx_mailid", "mailid"), std::make_tuple("PRIMARY", "id")); }

    static constexpr size_t keys_size() { return 2; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_mail_attachment` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `mailid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '邮件id',
  `attach_type` int unsigned NOT NULL DEFAULT '0' COMMENT '附件类型',
  `attach_data` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数量/具体道具实例ID',
  PRIMARY KEY (`id`),
  KEY `idx_mailid` (`mailid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_PET
{
    static constexpr const char* table_name() { return "tbld_pet"; }
    enum FIELD_ENUMS
    {
        ID,       //'id'
        OWNERID,  //'所有者玩家id'
        PET_TYPE, //'宠物类型'
        HP,       //'当前血量'
        EXP,      //'当前经验'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("ownerid",
                            "  `ownerid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '所有者玩家id'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false),
            std::make_tuple("pet_type", "  `pet_type` int unsigned NOT NULL DEFAULT '0' COMMENT '宠物类型'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("hp", "  `hp` int unsigned NOT NULL DEFAULT '0' COMMENT '当前血量'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("exp", "  `exp` bigint unsigned NOT NULL DEFAULT '0' COMMENT '当前经验'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint64_t>;

    static constexpr size_t field_count() { return 5; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_ownerid", "ownerid"),
                               std::make_tuple("idx_pet_type", "pet_type"),
                               std::make_tuple("PRIMARY", "id"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_pet` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `ownerid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '所有者玩家id',
  `pet_type` int unsigned NOT NULL DEFAULT '0' COMMENT '宠物类型',
  `hp` int unsigned NOT NULL DEFAULT '0' COMMENT '当前血量',
  `exp` bigint unsigned NOT NULL DEFAULT '0' COMMENT '当前经验',
  PRIMARY KEY (`id`),
  KEY `idx_pet_type` (`pet_type`),
  KEY `idx_ownerid` (`ownerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_PLAYER
{
    static constexpr const char* table_name() { return "tbld_player"; }
    enum FIELD_ENUMS
    {
        ID,              //'id'
        WORLDID,         //'服务器编号'
        ORIWORLDID,      //'创建角色时的服务器编号'
        OPENID,          //'玩家账号'
        NAME,            //'玩家名字'
        PROF,            //'玩家职业'
        BASELOOK,        //'玩家基本外观'
        CREATE_TIME,     //'创建时间戳'
        LAST_LOGINTIME,  //'最后登录时间戳'
        LAST_LOGOUTTIME, //'最后登出时间戳'
        DEL_TIME,        //'删除时间戳'
        LEV,             //'玩家等级'
        VIPLEV,          //'vip等级'
        RECORD_SCENEID,  //'玩家当前所在的地图编号'
        RECORD_X,        //'玩家当前所在地图x'
        RECORD_Y,        //'玩家当前所在地图y'
        RECORD_FACE,     //'玩家当前所在地图face'
        HOME_SCENEID,    //'玩家回城点对应的地图编号'
        HOME_X,          //'玩家回城点对应的地图x'
        HOME_Y,          //'玩家回城点对应的地图y'
        HOME_FACE,       //'玩家回城点对应的地图face'
        MATE_ID,         //'伴侣ID'
        MATE_NAME,       //'伴侣名字'
        TEAMID,          //'队伍id'
        GUILDID,         //'公会id'
        EXP,             //'玩家经验'
        MONEY,           //'金币'
        MONEY_BIND,      //'绑定金币'
        GOLD,            //'元宝'
        GOLD_BIND,       //'绑定元宝'
        HP,              //'hp'
        MP,              //'mp'
        FP,              //'fp'
        NP,              //'np'
        PKVAL,           //'pk值'
        HONOR,           //'荣誉值'
        ACHIPOINT,       //'成就值'
        BAG_SIZE,        //'背包大小'
        STROGE_SIZE,     //'仓库大小'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("worldid", "  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '服务器编号'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("oriworldid",
                            "  `oriworldid` int unsigned NOT NULL DEFAULT '0' COMMENT '创建角色时的服务器编号'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("openid",
                            "  `openid` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '玩家账号'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("name",
                            "  `name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '玩家名字'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("prof", "  `prof` int unsigned NOT NULL DEFAULT '0' COMMENT '玩家职业'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("baselook", "  `baselook` int unsigned NOT NULL DEFAULT '0' COMMENT '玩家基本外观'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("create_time",
                            "  `create_time` int unsigned NOT NULL DEFAULT '0' COMMENT '创建时间戳'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("last_logintime",
                            "  `last_logintime` int unsigned NOT NULL DEFAULT '0' COMMENT '最后登录时间戳'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("last_logouttime",
                            "  `last_logouttime` int unsigned NOT NULL DEFAULT '0' COMMENT '最后登出时间戳'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("del_time", "  `del_time` int unsigned NOT NULL DEFAULT '0' COMMENT '删除时间戳'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("lev", "  `lev` int unsigned NOT NULL DEFAULT '0' COMMENT '玩家等级'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("viplev", "  `viplev` int unsigned NOT NULL DEFAULT '0' COMMENT 'vip等级'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("record_sceneid",
                            "  `record_sceneid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家当前所在的地图编号'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false),
            std::make_tuple("record_x",
                            "  `record_x` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图x'",
                            DB_FIELD_TYPE_FLOAT,
                            false),
            std::make_tuple("record_y",
                            "  `record_y` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图y'",
                            DB_FIELD_TYPE_FLOAT,
                            false),
            std::make_tuple("record_face",
                            "  `record_face` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图face'",
                            DB_FIELD_TYPE_FLOAT,
                            false),
            std::make_tuple("home_sceneid",
                            "  `home_sceneid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家回城点对应的地图编号'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false),
            std::make_tuple("home_x",
                            "  `home_x` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图x'",
                            DB_FIELD_TYPE_FLOAT,
                            false),
            std::make_tuple("home_y",
                            "  `home_y` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图y'",
                            DB_FIELD_TYPE_FLOAT,
                            false),
            std::make_tuple("home_face",
                            "  `home_face` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图face'",
                            DB_FIELD_TYPE_FLOAT,
                            false),
            std::make_tuple("mate_id", "  `mate_id` bigint unsigned NOT NULL DEFAULT '0' COMMENT '伴侣ID'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("mate_name",
                            "  `mate_name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '伴侣名字'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("teamid", "  `teamid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '队伍id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("guildid", "  `guildid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '公会id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("exp", "  `exp` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家经验'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("money", "  `money` bigint unsigned NOT NULL DEFAULT '0' COMMENT '金币'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("money_bind",
                            "  `money_bind` bigint unsigned NOT NULL DEFAULT '0' COMMENT '绑定金币'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false),
            std::make_tuple("gold", "  `gold` bigint unsigned NOT NULL DEFAULT '0' COMMENT '元宝'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("gold_bind",
                            "  `gold_bind` bigint unsigned NOT NULL DEFAULT '0' COMMENT '绑定元宝'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false),
            std::make_tuple("hp", "  `hp` int unsigned NOT NULL DEFAULT '0' COMMENT 'hp'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("mp", "  `mp` int unsigned NOT NULL DEFAULT '0' COMMENT 'mp'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("fp", "  `fp` int unsigned NOT NULL DEFAULT '0' COMMENT 'fp'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("np", "  `np` int unsigned NOT NULL DEFAULT '0' COMMENT 'np'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("pkval", "  `pkval` int unsigned NOT NULL DEFAULT '0' COMMENT 'pk值'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("honor", "  `honor` int unsigned NOT NULL DEFAULT '0' COMMENT '荣誉值'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("achipoint", "  `achipoint` int unsigned NOT NULL DEFAULT '0' COMMENT '成就值'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("bag_size", "  `bag_size` int unsigned NOT NULL DEFAULT '0' COMMENT '背包大小'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("stroge_size",
                            "  `stroge_size` int unsigned NOT NULL DEFAULT '0' COMMENT '仓库大小'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false));
    }

    using field_type_t = type_list<uint64_t,
                                   uint32_t,
                                   uint32_t,
                                   char[255],
                                   char[32],
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint64_t,
                                   float,
                                   float,
                                   float,
                                   uint64_t,
                                   float,
                                   float,
                                   float,
                                   uint64_t,
                                   char[32],
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t>;

    static constexpr size_t field_count() { return 39; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_worldid", "worldid"),
                               std::make_tuple("idx_last_logouttime", "last_logouttime"),
                               std::make_tuple("idx_prof", "prof"),
                               std::make_tuple("idx_create_time", "create_time"),
                               std::make_tuple("idx_last_logintime", "last_logintime"),
                               std::make_tuple("idx_del_time", "del_time"),
                               std::make_tuple("idx_lev", "lev"),
                               std::make_tuple("idx_openid", "openid"),
                               std::make_tuple("idx_name", "name"),
                               std::make_tuple("PRIMARY", "id"));
    }

    static constexpr size_t keys_size() { return 10; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_player` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '服务器编号',
  `oriworldid` int unsigned NOT NULL DEFAULT '0' COMMENT '创建角色时的服务器编号',
  `openid` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '玩家账号',
  `name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '玩家名字',
  `prof` int unsigned NOT NULL DEFAULT '0' COMMENT '玩家职业',
  `baselook` int unsigned NOT NULL DEFAULT '0' COMMENT '玩家基本外观',
  `create_time` int unsigned NOT NULL DEFAULT '0' COMMENT '创建时间戳',
  `last_logintime` int unsigned NOT NULL DEFAULT '0' COMMENT '最后登录时间戳',
  `last_logouttime` int unsigned NOT NULL DEFAULT '0' COMMENT '最后登出时间戳',
  `del_time` int unsigned NOT NULL DEFAULT '0' COMMENT '删除时间戳',
  `lev` int unsigned NOT NULL DEFAULT '0' COMMENT '玩家等级',
  `viplev` int unsigned NOT NULL DEFAULT '0' COMMENT 'vip等级',
  `record_sceneid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家当前所在的地图编号',
  `record_x` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图x',
  `record_y` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图y',
  `record_face` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图face',
  `home_sceneid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家回城点对应的地图编号',
  `home_x` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图x',
  `home_y` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图y',
  `home_face` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图face',
  `mate_id` bigint unsigned NOT NULL DEFAULT '0' COMMENT '伴侣ID',
  `mate_name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '伴侣名字',
  `teamid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '队伍id',
  `guildid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '公会id',
  `exp` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家经验',
  `money` bigint unsigned NOT NULL DEFAULT '0' COMMENT '金币',
  `money_bind` bigint unsigned NOT NULL DEFAULT '0' COMMENT '绑定金币',
  `gold` bigint unsigned NOT NULL DEFAULT '0' COMMENT '元宝',
  `gold_bind` bigint unsigned NOT NULL DEFAULT '0' COMMENT '绑定元宝',
  `hp` int unsigned NOT NULL DEFAULT '0' COMMENT 'hp',
  `mp` int unsigned NOT NULL DEFAULT '0' COMMENT 'mp',
  `fp` int unsigned NOT NULL DEFAULT '0' COMMENT 'fp',
  `np` int unsigned NOT NULL DEFAULT '0' COMMENT 'np',
  `pkval` int unsigned NOT NULL DEFAULT '0' COMMENT 'pk值',
  `honor` int unsigned NOT NULL DEFAULT '0' COMMENT '荣誉值',
  `achipoint` int unsigned NOT NULL DEFAULT '0' COMMENT '成就值',
  `bag_size` int unsigned NOT NULL DEFAULT '0' COMMENT '背包大小',
  `stroge_size` int unsigned NOT NULL DEFAULT '0' COMMENT '仓库大小',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_name` (`name`),
  KEY `idx_lev` (`lev`),
  KEY `idx_del_time` (`del_time`),
  KEY `idx_last_logintime` (`last_logintime`),
  KEY `idx_create_time` (`create_time`),
  KEY `idx_prof` (`prof`),
  KEY `idx_openid` (`openid`),
  KEY `idx_last_logouttime` (`last_logouttime`),
  KEY `idx_worldid` (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_SKILL
{
    static constexpr const char* table_name() { return "tbld_skill"; }
    enum FIELD_ENUMS
    {
        ID,        //'id'
        USERID,    //'玩家id'
        SKILLTYPE, //'技能编号'
        LEV,       //'技能等级'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("userid", "  `userid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("skilltype", "  `skilltype` int unsigned NOT NULL DEFAULT '0' COMMENT '技能编号'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("lev", "  `lev` int unsigned NOT NULL DEFAULT '0' COMMENT '技能等级'", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 4; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("idx_userid", "userid"), std::make_tuple("PRIMARY", "id")); }

    static constexpr size_t keys_size() { return 2; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_skill` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `skilltype` int unsigned NOT NULL DEFAULT '0' COMMENT '技能编号',
  `lev` int unsigned NOT NULL DEFAULT '0' COMMENT '技能等级',
  PRIMARY KEY (`id`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_TASK
{
    static constexpr const char* table_name() { return "tbld_task"; }
    enum FIELD_ENUMS
    {
        ID,             //'id'
        USERID,         //'玩家id'
        TASKID,         //'任务id'
        NUM0,           //'任务计数1'
        NUM1,           //'任务计数2'
        NUM2,           //'任务计数3'
        NUM3,           //'任务计数4'
        ACCEPT_USERLEV, //'接受任务时的等级'
        ACCEPT_TIME,    //'接受任务时间戳'
        FINISH_TIME,    //'完成任务时间戳'
        EXPIRE_TIME,    //'任务过期时间戳'
        DAYCOUNT,       //'本日完成次数'
        DAYCOUNT_MAX,   //'本日最大可完成次数'
        STATE,          //'任务状态'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("userid", "  `userid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("taskid", "  `taskid` int unsigned NOT NULL DEFAULT '0' COMMENT '任务id'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("num0", "  `num0` int unsigned NOT NULL DEFAULT '0' COMMENT '任务计数1'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("num1", "  `num1` int unsigned NOT NULL DEFAULT '0' COMMENT '任务计数2'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("num2", "  `num2` int unsigned NOT NULL DEFAULT '0' COMMENT '任务计数3'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("num3", "  `num3` int unsigned NOT NULL DEFAULT '0' COMMENT '任务计数4'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("accept_userlev",
                            "  `accept_userlev` int unsigned NOT NULL DEFAULT '0' COMMENT '接受任务时的等级'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("accept_time",
                            "  `accept_time` int unsigned NOT NULL DEFAULT '0' COMMENT '接受任务时间戳'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("finish_time",
                            "  `finish_time` int unsigned NOT NULL DEFAULT '0' COMMENT '完成任务时间戳'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("expire_time",
                            "  `expire_time` int unsigned NOT NULL DEFAULT '0' COMMENT '任务过期时间戳'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("daycount", "  `daycount` int unsigned NOT NULL DEFAULT '0' COMMENT '本日完成次数'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("daycount_max",
                            "  `daycount_max` int unsigned NOT NULL DEFAULT '0' COMMENT '本日最大可完成次数'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("state", "  `state` int unsigned NOT NULL DEFAULT '0' COMMENT '任务状态'", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t,
                                   uint64_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t>;

    static constexpr size_t field_count() { return 14; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_userid", "userid"), std::make_tuple("idx_taskid", "taskid"), std::make_tuple("PRIMARY", "id"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_task` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `taskid` int unsigned NOT NULL DEFAULT '0' COMMENT '任务id',
  `num0` int unsigned NOT NULL DEFAULT '0' COMMENT '任务计数1',
  `num1` int unsigned NOT NULL DEFAULT '0' COMMENT '任务计数2',
  `num2` int unsigned NOT NULL DEFAULT '0' COMMENT '任务计数3',
  `num3` int unsigned NOT NULL DEFAULT '0' COMMENT '任务计数4',
  `accept_userlev` int unsigned NOT NULL DEFAULT '0' COMMENT '接受任务时的等级',
  `accept_time` int unsigned NOT NULL DEFAULT '0' COMMENT '接受任务时间戳',
  `finish_time` int unsigned NOT NULL DEFAULT '0' COMMENT '完成任务时间戳',
  `expire_time` int unsigned NOT NULL DEFAULT '0' COMMENT '任务过期时间戳',
  `daycount` int unsigned NOT NULL DEFAULT '0' COMMENT '本日完成次数',
  `daycount_max` int unsigned NOT NULL DEFAULT '0' COMMENT '本日最大可完成次数',
  `state` int unsigned NOT NULL DEFAULT '0' COMMENT '任务状态',
  PRIMARY KEY (`id`),
  KEY `idx_taskid` (`taskid`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_ACHIEVEMENT
{
    static constexpr const char* table_name() { return "tbld_achievement"; }
    enum FIELD_ENUMS
    {
        ID,     //'id'
        USERID, //'玩家id'
        ACHIID, //'成就id'
        TAKE,   //'成就id'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("userid", "  `userid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("achiid", "  `achiid` int unsigned NOT NULL DEFAULT '0' COMMENT '成就id'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("take", "  `take` int unsigned NOT NULL DEFAULT '0' COMMENT '成就id'", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 4; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_userid", "userid"), std::make_tuple("idx_achiid", "achiid"), std::make_tuple("PRIMARY", "id"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_achievement` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `achiid` int unsigned NOT NULL DEFAULT '0' COMMENT '成就id',
  `take` int unsigned NOT NULL DEFAULT '0' COMMENT '成就id',
  PRIMARY KEY (`id`),
  KEY `idx_achiid` (`achiid`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_STATUS
{
    static constexpr const char* table_name() { return "tbld_status"; }
    enum FIELD_ENUMS
    {
        ID,        //'id'
        USERID,    //'玩家id'
        TYPEID,    //'状态类型编号'
        LEV,       //'状态等级'
        POWER,     //'数据'
        SECS,      //'持续时间'
        TIMES,     //'作用次数'
        LASTSTAMP, //'最后一次作用的时间戳'
        CASTERID,  //'施加此状态的角色id'
        PAUSE,     //'是否暂停'
        STATUSID,  //'状态等级'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", "  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("userid", "  `userid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("typeid", "  `typeid` int unsigned NOT NULL DEFAULT '0' COMMENT '状态类型编号'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("lev", "  `lev` int unsigned NOT NULL DEFAULT '0' COMMENT '状态等级'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("power", "  `power` int unsigned NOT NULL DEFAULT '0' COMMENT '数据'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("secs", "  `secs` int unsigned NOT NULL DEFAULT '0' COMMENT '持续时间'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("times", "  `times` int unsigned NOT NULL DEFAULT '0' COMMENT '作用次数'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("laststamp",
                            "  `laststamp` int unsigned NOT NULL DEFAULT '0' COMMENT '最后一次作用的时间戳'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("casterid",
                            "  `casterid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '施加此状态的角色id'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false),
            std::make_tuple("pause", "  `pause` int unsigned NOT NULL DEFAULT '0' COMMENT '是否暂停'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("statusid", "  `statusid` int unsigned NOT NULL DEFAULT '0' COMMENT '状态等级'", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint64_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 11; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_userid", "userid"),
                               std::make_tuple("idx_statusid", "typeid,lev"),
                               std::make_tuple("PRIMARY", "id"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_status` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `typeid` int unsigned NOT NULL DEFAULT '0' COMMENT '状态类型编号',
  `lev` int unsigned NOT NULL DEFAULT '0' COMMENT '状态等级',
  `power` int unsigned NOT NULL DEFAULT '0' COMMENT '数据',
  `secs` int unsigned NOT NULL DEFAULT '0' COMMENT '持续时间',
  `times` int unsigned NOT NULL DEFAULT '0' COMMENT '作用次数',
  `laststamp` int unsigned NOT NULL DEFAULT '0' COMMENT '最后一次作用的时间戳',
  `casterid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '施加此状态的角色id',
  `pause` int unsigned NOT NULL DEFAULT '0' COMMENT '是否暂停',
  `statusid` int unsigned NOT NULL DEFAULT '0' COMMENT '状态等级',
  PRIMARY KEY (`id`),
  KEY `idx_statusid` (`typeid`,`lev`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_SYSTEMVAR
{
    static constexpr const char* table_name() { return "tbld_systemvar"; }
    enum FIELD_ENUMS
    {
        KEYIDX, //'id'
        NAME,   //'系统变量名'
        DATA0,  //'数据1'
        DATA1,  //'数据2'
        DATA2,  //'数据3'
        DATA3,  //'数据4'
        STR0,   //'文字1'
        STR1,   //'文字2'
        STR2,   //'文字3'
        STR3,   //'文字4'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("keyidx", "  `keyidx` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
            std::make_tuple("name",
                            "  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '系统变量名'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("data0", "  `data0` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据1'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("data1", "  `data1` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据2'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("data2", "  `data2` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据3'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("data3", "  `data3` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据4'", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
            std::make_tuple("str0",
                            "  `str0` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '文字1'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("str1",
                            "  `str1` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '文字2'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("str2",
                            "  `str2` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '文字3'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("str3",
                            "  `str3` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '文字4'",
                            DB_FIELD_TYPE_VARCHAR,
                            false));
    }

    using field_type_t = type_list<uint64_t, char[255], uint64_t, uint64_t, uint64_t, uint64_t, char[255], char[255], char[255], char[255]>;

    static constexpr size_t field_count() { return 10; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "keyidx")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_systemvar` (
  `keyidx` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '系统变量名',
  `data0` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据1',
  `data1` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据2',
  `data2` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据3',
  `data3` bigint unsigned NOT NULL DEFAULT '0' COMMENT '数据4',
  `str0` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '文字1',
  `str1` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '文字2',
  `str2` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '文字3',
  `str3` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '文字4',
  PRIMARY KEY (`keyidx`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

using GAMEDB_TABLE_LIST = type_list<TBLD_COMMONDATA,
                                    TBLD_COOLDOWN,
                                    TBLD_DATACOUNT,
                                    TBLD_ITEM,
                                    TBLD_MAIL,
                                    TBLD_MAIL_ATTACHMENT,
                                    TBLD_PET,
                                    TBLD_PLAYER,
                                    TBLD_SKILL,
                                    TBLD_TASK,
                                    TBLD_ACHIEVEMENT,
                                    TBLD_STATUS,
                                    TBLD_SYSTEMVAR>;

#endif
