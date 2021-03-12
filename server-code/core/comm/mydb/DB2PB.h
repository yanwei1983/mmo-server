#ifndef DBORM_H
#define DBORM_H
#include <memory>
#include <vector>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "DBField.h"
#include "DBRecord.h"

#include "ProtobuffUtil.h"

namespace DB2PB
{
    template<class PROTO_T, class DBRecord_T>
    void DBField2PB(DBRecord_T* pDBRecord, PROTO_T* pMsg)
    {
        using namespace google::protobuf;
        const Descriptor* msg_desc = PROTO_T::GetDescriptor();
        const Reflection* reflect  = PROTO_T::GetReflection();
        for(int32_t i = 0; i < msg_desc->field_count(); i++)
        {
            const auto& field      = pDBRecord->Field(i);
            auto        field_desc = msg_desc->field(i);
            if(field_desc == nullptr)
                continue;

            switch(field_desc->type())
            {
                case FieldDescriptor::TYPE_DOUBLE:
                    reflect->SetDouble(pMsg, field_desc, field);
                    break;
                case FieldDescriptor::TYPE_FLOAT:
                    reflect->SetFloat(pMsg, field_desc, field);
                    break;
                case FieldDescriptor::TYPE_INT64:
                    reflect->SetInt64(pMsg, field_desc, field);
                    break;
                case FieldDescriptor::TYPE_UINT64:
                    reflect->SetUInt64(pMsg, field_desc, field);
                    break;
                case FieldDescriptor::TYPE_INT32:
                    reflect->SetInt32(pMsg, field_desc, field);
                    break;
                case FieldDescriptor::TYPE_UINT32:
                    reflect->SetUInt32(pMsg, field_desc, field);
                    break;
                case FieldDescriptor::TYPE_STRING:
                    reflect->SetString(pMsg, field_desc, field);
                    break;
                case FieldDescriptor::TYPE_BYTES:
                    reflect->SetString(pMsg, field_desc, field);
                    break;
                default:
                    break;
            }
        }
    }

    template<class PROTO_T, class DBRecord_T>
    void PB2DBField(PROTO_T* pMsg, DBRecord_T* pDBRecord)
    {
        using namespace google::protobuf;
        const Descriptor* msg_desc = PROTO_T::GetDescriptor();
        const Reflection* reflect  = PROTO_T::GetReflection();
        for(int32_t i = 0; i < msg_desc->field_count(); i++)
        {
            auto& field      = pDBRecord->Field(i);
            auto  field_desc = msg_desc->field(i);
            if(field_desc == nullptr)
                continue;

            switch(field_desc->type())
            {
                case FieldDescriptor::TYPE_DOUBLE:
                    field = reflect->GetDouble(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_FLOAT:
                    field = reflect->GetFloat(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_INT64:
                    field = reflect->GetInt64(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_UINT64:
                    field = reflect->GetUInt64(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_INT32:
                    field = reflect->GetInt32(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_UINT32:
                    field = reflect->GetUInt32(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_STRING:
                    field = reflect->GetString(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_BYTES:
                    field = reflect->GetString(pMsg, field_desc);
                    break;
                default:
                    break;
            }
        }
    }

    template<class DBField_t>
    void LinkProtoWithJsonTxt(DBField_t& field, google::protobuf::Message& proto)
    {
        std::string json_txt = field;
        if(json_txt.empty() == false)
            pb_util::LoadFromJsonTxt(json_txt, proto);
            
        field.BindGetValString([&proto]()
        {
            std::string jsonTxt;
            pb_util::SaveToJsonTxt(proto, jsonTxt);
            return jsonTxt;
        });
    }

    template<class DBField_t>
    void LinkProtoWithBinary(DBField_t& field, google::protobuf::Message& proto)
    {
        std::string binary_txt = field;
        if(binary_txt.empty() == false)
            proto.ParseFromString(binary_txt);
            
        field.BindGetValString([&proto]()
        {
            std::string binary_txt;
            proto.SerializeToString(&binary_txt);
            return binary_txt;
        });
    }

}; // namespace DB2PB

template<class PROTO_T>
class TDBObj
{
public:
    TDBObj(CDBRecordPtr&& pDBRecordPtr)
        : m_pDBRecordPtr(std::move(pDBRecordPtr))
        , m_pPBMsg(PROTO_T::default_instance().New())
    {
        DB2PB();
    }

    TDBObj(CDBRecord* pDBRecordPtr, PROTO_T* pPBMsg)
        : m_pDBRecordPtr(pDBRecordPtr)
        , m_pPBMsg(pPBMsg)
    {
        DB2PB();
    }

    void DB2PB() { DB2PB::DBField2PB(m_pDBRecordPtr.get(), m_pPBMsg.get()); }

    void PB2DB() { DB2PB::PB2DBField(m_pPBMsg.get(), m_pDBRecordPtr.get()); }

    CDBRecordPtr             m_pDBRecordPtr;
    std::unique_ptr<PROTO_T> m_pPBMsg;
};

template<class PROTO_T>
using TDBObjPtr = std::unique_ptr<TDBObj<PROTO_T>>;

namespace DB2PB
{
    template<class TABLE_T, class PROTO_T, uint32_t nKeyID, class DB_T, class KEY_T>
    std::unique_ptr<PROTO_T> QueryOneConst(DB_T* pDB, KEY_T key)
    {
        auto result_ptr = pDB->template QueryKeyLimit<TABLE_T, nKeyID>(key, 1);
        if(result_ptr)
        {
            auto pDBRecord = result_ptr->fetch_row(false);
            if(pDBRecord)
            {
                std::unique_ptr<PROTO_T> proto_obj{PROTO_T::default_instance().New()};
                DBField2PB(pDBRecord.get(), proto_obj.get());
                return proto_obj;
            }
        }

        return nullptr;
    }

    template<class TABLE_T, class PROTO_T, uint32_t nKeyID, class DB_T, class KEY_T>
    TDBObjPtr<PROTO_T> QueryOne(DB_T* pDB, KEY_T key)
    {
        auto result_ptr = pDB->template QueryKeyLimit<TABLE_T, nKeyID>(key, 1);
        if(result_ptr)
        {
            auto pDBRecord = result_ptr->fetch_row(false);
            if(pDBRecord)
            {
                return std::make_unique<TDBObj<PROTO_T>>(pDBRecord);
            }
        }

        return nullptr;
    }

    template<class TABLE_T, class PROTO_T, uint32_t nKeyID, class DB_T, class KEY_T>
    std::vector<std::unique_ptr<PROTO_T>> QueryVectorConst(DB_T* pDB, KEY_T key)
    {
        std::vector<std::unique_ptr<PROTO_T>> result;

        auto result_ptr = pDB->template QueryKey<TABLE_T, nKeyID>(key);
        if(result_ptr)
        {
            for(int32_t i = 0; i < result_ptr->get_num_row(); i++)
            {
                auto pDBRecord = result_ptr->fetch_row(false);
                if(pDBRecord)
                {
                    std::unique_ptr<PROTO_T> proto_obj{PROTO_T::default_instance().New()};
                    DBField2PB(pDBRecord.get(), proto_obj.get());
                    result.emplace(std::move(proto_obj));
                }
            }
        }

        return result;
    }

    template<class TABLE_T, class PROTO_T, uint32_t nKeyID, class DB_T, class KEY_T>
    std::vector<TDBObjPtr<PROTO_T>> QueryVector(DB_T* pDB, KEY_T key)
    {
        std::vector<TDBObjPtr<PROTO_T>> result;

        auto result_ptr = pDB->template QueryKey<TABLE_T, nKeyID>(key);
        if(result_ptr)
        {
            for(int32_t i = 0; i < result_ptr->get_num_row(); i++)
            {
                auto pDBRecord = result_ptr->fetch_row(false);
                if(pDBRecord)
                {
                    auto db_obj = std::make_unique<TDBObj<PROTO_T>>(pDBRecord);
                    result.emplace(std::move(db_obj));
                }
            }
        }

        return result;
    }
} // namespace DB2PB

#endif /* DBORM_H */
