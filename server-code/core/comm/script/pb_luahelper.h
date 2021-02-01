#ifndef PB_LUAHELPER_H
#define PB_LUAHELPER_H

#include "ProtobuffUtil.h"
#include "lua_tinker.h"

namespace pb_luahelper
{
    struct ConstRepeatedProtobufMessageWarp;
    struct ConstProtobufMessageWarp;
    struct ProtobufMessageWarp;

    inline int32_t PushMessageDataToLua(lua_State* L, const google::protobuf::Message* pPBMessage, const std::string& field_name);
    struct ProtobufMessageWarp
    {
        static std::atomic<uint32_t> s_nCount;
        ProtobufMessageWarp(const std::string& strMessageType)
            : m_pMessage(pb_util::NewProtoMessage(strMessageType), pb_util::DelProtoMessage)
        {
            s_nCount++;
        }
        ProtobufMessageWarp(const ProtobufMessageWarp& rht)
            : m_pMessage(rht.m_pMessage)
        {
            s_nCount++;
        }
        ~ProtobufMessageWarp()
        {
            s_nCount--;
        }

        void set(const std::string& key, const std::string& val) { pb_util::SetMessageData(m_pMessage.get(), key, val); }

        static int32_t meta_get(lua_State* L)
        {
            using namespace lua_tinker;
            using namespace lua_tinker::detail;
            stack_obj class_obj(L, 1);
            stack_obj key_obj(L, 2);
            stack_obj class_meta = class_obj.get_metatable();
            stack_obj val_obj    = class_meta.rawget(key_obj);
            int32_t   result     = 0;
            if(val_obj.is_nil())
            {
                val_obj.remove();
                result = index_ProtobufMessageWarp(L);
            }
            else
            {
                result = 1;
            }
            class_meta.remove();
            return result;
        }

        static int32_t index_ProtobufMessageWarp(lua_State* L)
        {
            ProtobufMessageWarp* pWarp = lua_tinker::detail::read<ProtobufMessageWarp*>(L, 1);
            const std::string&   key   = lua_tinker::detail::read<const std::string&>(L, 2);
            if(pWarp)
            {
                return PushMessageDataToLua(L, pWarp->m_pMessage.get(), key);
            }

            return 0;
        }

        static google::protobuf::Message* GetProtobufMessagePtr(ProtobufMessageWarp* pWarp) { return pWarp->m_pMessage.get(); }

        static size_t GetProtobufFieldSize(ProtobufMessageWarp* pWarp, const std::string& key)
        {
            google::protobuf::Message*               pThisRow   = pWarp->m_pMessage.get();
            const google::protobuf::FieldDescriptor* pFieldDesc = nullptr;
            if(pb_util::FindFieldInMessage(key, pThisRow, pFieldDesc) == false)
                return 0;

            return pThisRow->GetReflection()->FieldSize(*pThisRow, pFieldDesc);
        }

        std::shared_ptr<google::protobuf::Message> m_pMessage;
    };

    struct ConstProtobufMessageWarp
    {
        static std::atomic<uint32_t> s_nCount;
        ConstProtobufMessageWarp(const google::protobuf::Message& msg)
            : m_MsgRef(msg)
        {
            s_nCount++;
        }
        ConstProtobufMessageWarp(const ConstProtobufMessageWarp& rht)
            :m_MsgRef(rht.m_MsgRef)
        {
            s_nCount++;
        }
        ~ConstProtobufMessageWarp() { s_nCount--; }

        static int32_t meta_get(lua_State* L)
        {
            using namespace lua_tinker;
            using namespace lua_tinker::detail;
            stack_obj class_obj(L, 1);
            stack_obj key_obj(L, 2);
            stack_obj class_meta = class_obj.get_metatable();
            stack_obj val_obj    = class_meta.rawget(key_obj);
            int32_t   result     = 0;
            if(val_obj.is_nil())
            {
                val_obj.remove();
                result = index_ProtobufMessageWarp(L);
            }
            else
            {
                result = 1;
            }
            class_meta.remove();
            return result;
        }

        static int32_t index_ProtobufMessageWarp(lua_State* L)
        {
            ConstProtobufMessageWarp* pWarp = lua_tinker::detail::read<ConstProtobufMessageWarp*>(L, 1);
            const std::string&        key   = lua_tinker::detail::read<const std::string&>(L, 2);
            if(pWarp)
            {
                return PushMessageDataToLua(L, &(pWarp->m_MsgRef), key);
            }

            return 0;
        }

        const google::protobuf::Message& m_MsgRef;
    };

    struct ConstRepeatedProtobufMessageWarp
    {
        static std::atomic<uint32_t> s_nCount;
        ConstRepeatedProtobufMessageWarp(const google::protobuf::Message* pMessage, const google::protobuf::FieldDescriptor* pFieldDesc)
            : m_pMessage(pMessage)
            , m_pFieldDesc(pFieldDesc)
        {
            s_nCount++;
        }
        ConstRepeatedProtobufMessageWarp(const ConstRepeatedProtobufMessageWarp& rht)
            : m_pMessage(rht.m_pMessage)
            , m_pFieldDesc(rht.m_pFieldDesc)
        {
            s_nCount++;
        }
        ~ConstRepeatedProtobufMessageWarp() { s_nCount--; }
        size_t size() const { return m_pMessage->GetReflection()->FieldSize(*m_pMessage, m_pFieldDesc); }

        static int32_t meta_get(lua_State* L)
        {
            using namespace lua_tinker;
            using namespace lua_tinker::detail;
            stack_obj class_obj(L, 1);
            stack_obj key_obj(L, 2);
            stack_obj class_meta = class_obj.get_metatable();
            stack_obj val_obj    = class_meta.rawget(key_obj);
            int32_t   result     = 0;
            if(val_obj.is_nil())
            {
                val_obj.remove();
                result = index_ProtobufMessageWarp(L);
            }
            else
            {
                result = 1;
            }
            class_meta.remove();
            return result;
        }
        static int32_t index_ProtobufMessageWarp(lua_State* L)
        {
            using namespace google::protobuf;

            ConstRepeatedProtobufMessageWarp* pWarp = lua_tinker::detail::read<ConstRepeatedProtobufMessageWarp*>(L, 1);
            int32_t                           idx   = lua_tinker::detail::read<int32_t>(L, 2);
            if(pWarp)
            {
                if(idx >= (int32_t)pWarp->size())
                {
                    luaL_error(L, "idx {} is out of size {}", idx, pWarp->size());
                    return 0;
                }
                switch(pWarp->m_pFieldDesc->type())
                {
                    case FieldDescriptor::TYPE_DOUBLE:
                    {
                        lua_tinker::detail::push(
                            L,
                            pWarp->m_pMessage->GetReflection()->GetRepeatedDouble(*(pWarp->m_pMessage), pWarp->m_pFieldDesc, idx));
                    }
                    break;
                    case FieldDescriptor::TYPE_FLOAT:
                    {
                        lua_tinker::detail::push(
                            L,
                            pWarp->m_pMessage->GetReflection()->GetRepeatedFloat(*(pWarp->m_pMessage), pWarp->m_pFieldDesc, idx));
                    }
                    break;
                    case FieldDescriptor::TYPE_INT32:
                    case FieldDescriptor::TYPE_SINT32:
                    case FieldDescriptor::TYPE_SFIXED32:
                    {
                        lua_tinker::detail::push(
                            L,
                            pWarp->m_pMessage->GetReflection()->GetRepeatedInt32(*(pWarp->m_pMessage), pWarp->m_pFieldDesc, idx));
                    }
                    break;
                    case FieldDescriptor::TYPE_UINT32:
                    case FieldDescriptor::TYPE_FIXED32:
                    {
                        lua_tinker::detail::push(
                            L,
                            pWarp->m_pMessage->GetReflection()->GetRepeatedUInt32(*(pWarp->m_pMessage), pWarp->m_pFieldDesc, idx));
                    }
                    break;
                    case FieldDescriptor::TYPE_INT64:
                    case FieldDescriptor::TYPE_SINT64:
                    case FieldDescriptor::TYPE_SFIXED64:
                    {
                        lua_tinker::detail::push(
                            L,
                            pWarp->m_pMessage->GetReflection()->GetRepeatedInt64(*(pWarp->m_pMessage), pWarp->m_pFieldDesc, idx));
                    }
                    break;
                    case FieldDescriptor::TYPE_UINT64:
                    case FieldDescriptor::TYPE_FIXED64:
                    {
                        lua_tinker::detail::push(
                            L,
                            pWarp->m_pMessage->GetReflection()->GetRepeatedUInt64(*(pWarp->m_pMessage), pWarp->m_pFieldDesc, idx));
                    }
                    break;
                    case FieldDescriptor::TYPE_ENUM:
                    {
                        lua_tinker::detail::push(
                            L,
                            pWarp->m_pMessage->GetReflection()->GetRepeatedEnumValue(*(pWarp->m_pMessage), pWarp->m_pFieldDesc, idx));
                    }
                    break;
                    case FieldDescriptor::TYPE_BOOL:
                    {
                        lua_tinker::detail::push(L,
                                                 pWarp->m_pMessage->GetReflection()->GetRepeatedBool(*(pWarp->m_pMessage), pWarp->m_pFieldDesc, idx));
                    }
                    break;
                    case FieldDescriptor::TYPE_STRING:
                    case FieldDescriptor::TYPE_BYTES:
                    {
                        lua_tinker::detail::push(
                            L,
                            pWarp->m_pMessage->GetReflection()->GetRepeatedString(*(pWarp->m_pMessage), pWarp->m_pFieldDesc, idx));
                    }
                    break;
                    case FieldDescriptor::TYPE_MESSAGE:
                    {
                        const Message& sub_message =
                            pWarp->m_pMessage->GetReflection()->GetRepeatedMessage(*(pWarp->m_pMessage), pWarp->m_pFieldDesc, idx);
                        lua_tinker::detail::push(L, ConstProtobufMessageWarp(sub_message));
                    }
                    break;
                    default:
                        break;
                }
                return 1;
            }

            return 0;
        }

        const google::protobuf::Message*         m_pMessage   = nullptr;
        const google::protobuf::FieldDescriptor* m_pFieldDesc = nullptr;
    };

    inline int32_t PushMessageDataToLua(lua_State* L, const google::protobuf::Message* pPBMessage, const std::string& field_name)
    {
        using namespace google::protobuf;
        const FieldDescriptor* pFieldDesc = pPBMessage->GetDescriptor()->FindFieldByName(field_name);
        if(pFieldDesc == nullptr)
        {
            luaL_error(L, "FindFieldInPBMessage: {} fail", field_name.c_str());
            return 0;
        }

        if(pFieldDesc->is_repeated())
        {
            lua_tinker::detail::push(L, ConstRepeatedProtobufMessageWarp(pPBMessage, pFieldDesc));
            return 1;
        }

        switch(pFieldDesc->type())
        {
            case FieldDescriptor::TYPE_DOUBLE:
            {
                lua_tinker::detail::push(L, pPBMessage->GetReflection()->GetDouble(*pPBMessage, pFieldDesc));
            }
            break;
            case FieldDescriptor::TYPE_FLOAT:
            {
                lua_tinker::detail::push(L, pPBMessage->GetReflection()->GetFloat(*pPBMessage, pFieldDesc));
            }
            break;
            case FieldDescriptor::TYPE_INT32:
            case FieldDescriptor::TYPE_SINT32:
            case FieldDescriptor::TYPE_SFIXED32:
            {
                lua_tinker::detail::push(L, pPBMessage->GetReflection()->GetInt32(*pPBMessage, pFieldDesc));
            }
            break;
            case FieldDescriptor::TYPE_UINT32:
            case FieldDescriptor::TYPE_FIXED32:
            {
                lua_tinker::detail::push(L, pPBMessage->GetReflection()->GetUInt32(*pPBMessage, pFieldDesc));
            }
            break;
            case FieldDescriptor::TYPE_INT64:
            case FieldDescriptor::TYPE_SINT64:
            case FieldDescriptor::TYPE_SFIXED64:
            {
                lua_tinker::detail::push(L, pPBMessage->GetReflection()->GetInt64(*pPBMessage, pFieldDesc));
            }
            break;
            case FieldDescriptor::TYPE_UINT64:
            case FieldDescriptor::TYPE_FIXED64:
            {
                lua_tinker::detail::push(L, pPBMessage->GetReflection()->GetUInt64(*pPBMessage, pFieldDesc));
            }
            break;
            case FieldDescriptor::TYPE_ENUM:
            {
                lua_tinker::detail::push(L, pPBMessage->GetReflection()->GetEnumValue(*pPBMessage, pFieldDesc));
            }
            break;
            case FieldDescriptor::TYPE_BOOL:
            {
                lua_tinker::detail::push(L, pPBMessage->GetReflection()->GetBool(*pPBMessage, pFieldDesc));
            }
            break;
            case FieldDescriptor::TYPE_STRING:
            case FieldDescriptor::TYPE_BYTES:
            {
                lua_tinker::detail::push(L, pPBMessage->GetReflection()->GetString(*pPBMessage, pFieldDesc));
            }
            break;
            case FieldDescriptor::TYPE_MESSAGE:
            {
                const Message& sub_message = pPBMessage->GetReflection()->GetMessage(*pPBMessage, pFieldDesc);
                lua_tinker::detail::push(L, ConstProtobufMessageWarp(sub_message));
            }
            break;
            default:
                break;
        }
        return 1;
    }

    inline void export_protobuf_enum_to_lua(lua_State* L, const ::google::protobuf::EnumDescriptor* pEnumDesc)
    {
        if(pEnumDesc == nullptr)
            return;

        for(int32_t i = 0; i < pEnumDesc->value_count(); i++)
        {
            auto pEnumValDesc = pEnumDesc->value(i);

            lua_tinker::set<int32_t>(L, pEnumValDesc->name().c_str(), pEnumValDesc->number());
        }
    }

    inline void init_lua(lua_State* L)
    {
        lua_tinker::class_add<google::protobuf::Message>(L, "google::protobuf::Message");
        lua_tinker::class_def<google::protobuf::Message>(L, "ParseFromArray", &google::protobuf::Message::ParseFromArray);

        lua_tinker::class_add<ProtobufMessageWarp>(L, "ProtobufMessageWarp");
        lua_tinker::class_con<ProtobufMessageWarp>(L, lua_tinker::constructor<ProtobufMessageWarp, const std::string&>::invoke);
        lua_tinker::class_def<ProtobufMessageWarp>(L, "__index", &ProtobufMessageWarp::meta_get);
        lua_tinker::class_def<ProtobufMessageWarp>(L, "__newindex", &ProtobufMessageWarp::set);
        lua_tinker::def(L, "GetProtobufMessagePtr", &ProtobufMessageWarp::GetProtobufMessagePtr);
        lua_tinker::def(L, "GetProtobufFieldSize", &ProtobufMessageWarp::GetProtobufFieldSize);

        lua_tinker::class_add<ConstProtobufMessageWarp>(L, "ConstProtobufMessageWarp", false);
        lua_tinker::class_def<ConstProtobufMessageWarp>(L, "__index", &ConstProtobufMessageWarp::meta_get);
        // lua_tinker::class_def<ConstProtobufMessageWarp>(L, "__newindex", &ProtobufMessageWarp::set);

        lua_tinker::class_add<ConstRepeatedProtobufMessageWarp>(L, "ConstRepeatedProtobufMessageWarp", false);
        lua_tinker::class_def<ConstRepeatedProtobufMessageWarp>(L, "size", &ConstRepeatedProtobufMessageWarp::size);
        lua_tinker::class_def<ConstRepeatedProtobufMessageWarp>(L, "__index", &ConstRepeatedProtobufMessageWarp::meta_get);

        lua_tinker::namespace_add(L, "pb_util");
        lua_tinker::namespace_def(L, "pb_util", "NewProtoMessage", pb_util::NewProtoMessage);
        lua_tinker::namespace_def(L, "pb_util", "DelProtoMessage", pb_util::DelProtoMessage);
        lua_tinker::namespace_def(L, "pb_util", "SetMessageData", pb_util::SetMessageData);
        lua_tinker::namespace_def(L, "pb_util", "AddMessageData", pb_util::AddMessageData);
        lua_tinker::namespace_def(L, "pb_util", "AddMessageSubMessage", pb_util::AddMessageSubMessage);
    }
};     // namespace pb_luahelper
#endif /* PB_LUAHELPER_H */
