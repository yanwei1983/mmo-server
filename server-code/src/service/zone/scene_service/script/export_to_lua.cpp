#include "Common.pb.h"
#include "ScriptManager.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"
#include "pb_luahelper.h"
extern void scene2lua(lua_State* L);

std::atomic<uint32_t> pb_luahelper::ProtobufMessageWarp::s_nCount{0};
std::atomic<uint32_t> pb_luahelper::ConstProtobufMessageWarp::s_nCount{0};
std::atomic<uint32_t> pb_luahelper::ConstRepeatedProtobufMessageWarp::s_nCount{0};

void export_to_lua(lua_State* L, void*)
{
    //向lua注册c++类型
    pb_luahelper::init_lua(L);
    pb_luahelper::export_protobuf_enum_to_lua(L, ACTOR_ATTRIB_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, ACTOR_PROPERTY_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, MONEY_TYPE_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, PK_MODE_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, TalkChannel_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, TaskState_descriptor());

    pb_luahelper::export_protobuf_enum_to_lua(L, CS_CMD_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, SC_CMD_descriptor());

    scene2lua(L);

    //注册各种辅助函数
}
