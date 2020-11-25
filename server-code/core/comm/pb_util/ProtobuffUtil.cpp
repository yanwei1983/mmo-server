#include "ProtobuffUtil.h"

#include <cstdlib>
#include <fstream>

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>

#include "LoggingMgr.h"
#include "json.hpp"

namespace pb_util
{
    bool LoadFromTextualFile(const std::string& filename, google::protobuf::Message& pbm)
    {
        bool          rv = false;
        std::ifstream ifs(filename.data());
        if(ifs.is_open())
        {
            google::protobuf::io::IstreamInputStream zcis(&ifs);
            rv = google::protobuf::TextFormat::Parse(&zcis, &pbm);
            ifs.close();
        }
        return rv;
    }

    bool SaveToTextualFile(const google::protobuf::Message& pbm, const std::string& filename)
    {
        bool          rv = false;
        std::ofstream ofs(filename.data(), std::ios::out | std::ios::trunc);
        if(ofs.is_open())
        {
            std::string text;
            rv = google::protobuf::TextFormat::PrintToString(pbm, &text);
            ofs << text;

            ofs.close();
        }
        return rv;
    }

    bool SaveToBinaryFile(const google::protobuf::Message& pbm, const std::string& filename)
    {
        bool          rv = false;
        std::ofstream ofs(filename.data(), std::ios::out | std::ios::binary);
        if(ofs.is_open())
        {
            rv = pbm.SerializeToOstream(&ofs);
            ofs.close();
        }
        return rv;
    }

    bool LoadFromBinaryFile(const std::string& filename, google::protobuf::Message& pbm)
    {
        bool rv = false;

        std::ifstream ifs(filename.data());
        if(ifs.is_open())
        {
            rv = pbm.ParseFromIstream(&ifs);
            ifs.close();
        }
        else
        {
            return false;
        }
        return rv;
    }

    bool LoadFromJsonFile(const std::string& filename, google::protobuf::Message& pbm)
    {
        using namespace google::protobuf::util;
        JsonParseOptions options;
        options.ignore_unknown_fields = true;

        std::ifstream ifs(filename.data());
        if(ifs.is_open())
        {
            std::istreambuf_iterator<char> beg(ifs);
            std::istreambuf_iterator<char> end;
            std::string                    str(beg, end);
            ifs.close();
            if(LoadFromJsonTxt(str, pbm) != true)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        return true;
    }

    bool SaveToJsonFile(const google::protobuf::Message& pbm, const std::string& filename)
    {
        std::string json_txt;
        SaveToJsonTxt(pbm, json_txt);

        std::ofstream ofs(filename.data(), std::ios::out | std::ios::trunc);
        if(ofs.is_open())
        {
            ofs << json_txt;
            ofs.close();
            return true;
        }
        return false;
    }

    bool LoadFromJsonTxt(const std::string& jsonTxt, google::protobuf::Message& pbm)
    {
        using namespace google::protobuf::util;
        JsonParseOptions options;
        options.ignore_unknown_fields = true;
        return JsonStringToMessage(jsonTxt, &pbm, options).ok();
    }

    bool SaveToJsonTxt(const google::protobuf::Message& pbm, std::string& jsonTxt)
    {
        using namespace google::protobuf::util;
        JsonPrintOptions options;
        options.add_whitespace             = true;
        options.always_print_enums_as_ints = true;
        return MessageToJsonString(pbm, &jsonTxt, options).ok();
    }

    bool FindFieldInMessage(const std::string& field_name, google::protobuf::Message*& pThisRow, const google::protobuf::FieldDescriptor*& pFieldDesc)
    {
        auto vecName = split_string(field_name, ".");
        while(vecName.empty() == false)
        {
            int32_t array_idx = -1;
            auto    name      = std::move(vecName.front());
            vecName.erase(vecName.begin());
            if(name.empty())
                continue;
            // xxx[1].xxx
            if(name[name.size() - 1] == ']')
            {
                auto pos = name.find_first_of('[', 0);
                if(pos != 0)
                {
                    std::string array_str{name.data() + pos + 1, name.size() - 2 - pos};
                    std::string real_name{name.data(), pos};
                    pFieldDesc = pThisRow->GetDescriptor()->FindFieldByName(real_name.data());
                    if(pFieldDesc == nullptr)
                    {
                        throw std::runtime_error{fmt::format("realname:{} not find in {}", real_name, pThisRow->GetDescriptor()->name())};
                        return false;
                    }
                    if(pFieldDesc->is_repeated() && pFieldDesc->type() != google::protobuf::FieldDescriptor::TYPE_MESSAGE)
                    {
                        throw std::runtime_error{
                            fmt::format("realname:{} in {} is repeated and not a message\n", real_name, pThisRow->GetDescriptor()->name())};
                        return false;
                    }
                    auto array_idx = atoi(array_str.data());
                    auto curSize   = pThisRow->GetReflection()->FieldSize(*pThisRow, pFieldDesc);
                    for(int32_t i = curSize; i < array_idx + 1; i++)
                    {
                        pThisRow->GetReflection()->AddMessage(pThisRow, pFieldDesc, nullptr);
                    }
                    pThisRow = pThisRow->GetReflection()->MutableRepeatedMessage(pThisRow, pFieldDesc, array_idx);
                    continue;
                }
                continue;
            }

            pFieldDesc = pThisRow->GetDescriptor()->FindFieldByName(name.data());
            if(pFieldDesc == nullptr)
            {
                throw std::runtime_error{fmt::format("{} not find in {}\n", name, pThisRow->GetDescriptor()->name())};
                return false;
            }

            if(pFieldDesc->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
            {
                if(pFieldDesc->is_repeated() == true)
                {
                    return true;
                }
                pThisRow = pThisRow->GetReflection()->MutableMessage(pThisRow, pFieldDesc);
                continue;
            }
        }

        if(pFieldDesc == nullptr)
        {
            return false;
        }
        return true;
    }

    void AddFieldData(google::protobuf::Message* pThisRow, const google::protobuf::FieldDescriptor* pFieldDesc, const std::string& data)
    {
        using namespace google::protobuf;
        switch(pFieldDesc->type())
        {
            case FieldDescriptor::TYPE_DOUBLE:
            {
                pThisRow->GetReflection()->AddDouble(pThisRow, pFieldDesc, stod(data));
            }
            break;
            case FieldDescriptor::TYPE_FLOAT:
            {
                pThisRow->GetReflection()->AddFloat(pThisRow, pFieldDesc, stof(data));
            }
            break;
            case FieldDescriptor::TYPE_INT32:
            case FieldDescriptor::TYPE_SINT32:
            case FieldDescriptor::TYPE_SFIXED32:
            {
                pThisRow->GetReflection()->AddInt32(pThisRow, pFieldDesc, stol(data));
            }
            break;
            case FieldDescriptor::TYPE_UINT32:
            case FieldDescriptor::TYPE_FIXED32:
            {
                if(data.substr(0, 2) == "0x" || data.substr(0, 2) == "0X")
                    pThisRow->GetReflection()->SetUInt32(pThisRow, pFieldDesc, stoul(data, 0, 16));
                else
                    pThisRow->GetReflection()->AddUInt32(pThisRow, pFieldDesc, stoul(data));
            }
            break;
            case FieldDescriptor::TYPE_INT64:
            case FieldDescriptor::TYPE_SINT64:
            case FieldDescriptor::TYPE_SFIXED64:
            {
                pThisRow->GetReflection()->AddInt64(pThisRow, pFieldDesc, stoll(data));
            }
            break;
            case FieldDescriptor::TYPE_UINT64:
            case FieldDescriptor::TYPE_FIXED64:
            {
                if(data.substr(0, 2) == "0x" || data.substr(0, 2) == "0X")
                    pThisRow->GetReflection()->AddUInt64(pThisRow, pFieldDesc, stoull(data, 0, 16));
                else
                    pThisRow->GetReflection()->AddUInt64(pThisRow, pFieldDesc, stoull(data));
            }
            break;
            case FieldDescriptor::TYPE_BOOL:
            {
                pThisRow->GetReflection()->AddBool(pThisRow, pFieldDesc, (data == "true" || data == "1"));
            }
            break;
            case FieldDescriptor::TYPE_STRING:
            case FieldDescriptor::TYPE_BYTES:
            {
                pThisRow->GetReflection()->AddString(pThisRow, pFieldDesc, data);
            }
            break;
            default:
                break;
        }
    }

    bool JoinFieldData(google::protobuf::Message* pThisRow, const google::protobuf::FieldDescriptor* pFieldDesc, const std::string& data)
    {
        using namespace google::protobuf;
        switch(pFieldDesc->type())
        {
            case FieldDescriptor::TYPE_UINT32:
            case FieldDescriptor::TYPE_FIXED32:
            {
                uint32 value = 0;
                if(data.substr(0, 2) == "0x" || data.substr(0, 2) == "0X")
                    value = stoul(data, 0, 16);
                else
                    value = stoul(data);
                uint32 old_value = pThisRow->GetReflection()->GetUInt32(*pThisRow, pFieldDesc);
                pThisRow->GetReflection()->SetUInt32(pThisRow, pFieldDesc, old_value | value);
            }
            break;
            case FieldDescriptor::TYPE_UINT64:
            case FieldDescriptor::TYPE_FIXED64:
            {
                uint64 value = 0;
                if(data.substr(0, 2) == "0x" || data.substr(0, 2) == "0X")
                    value = stoul(data, 0, 16);
                else
                    value = stoul(data);
                uint64 old_value = pThisRow->GetReflection()->GetUInt64(*pThisRow, pFieldDesc);
                pThisRow->GetReflection()->SetUInt64(pThisRow, pFieldDesc, old_value | value);
            }
            break;
            case FieldDescriptor::TYPE_STRING:
            case FieldDescriptor::TYPE_BYTES:
            {
                const auto& str = pThisRow->GetReflection()->GetString(*pThisRow, pFieldDesc);
                pThisRow->GetReflection()->SetString(pThisRow, pFieldDesc, str + data);
            }
            break;
            default:
                break;
        }
        return true;
    }

    bool SetFieldData(google::protobuf::Message*               pThisRow,
                      const google::protobuf::FieldDescriptor* pFieldDesc,
                      const std::string&                       field_name,
                      const std::string&                       data)
    {
        using namespace google::protobuf;
        switch(pFieldDesc->type())
        {
            case FieldDescriptor::TYPE_ENUM:
            {
                try
                {
                    int32_t val = stol(data);
                    pThisRow->GetReflection()->SetEnumValue(pThisRow, pFieldDesc, val);
                }
                catch(...)
                {
                    auto pEnumVal = pFieldDesc->enum_type()->FindValueByName(data);
                    if(pEnumVal == nullptr)
                    {
                        throw std::runtime_error{fmt::format("can't convert {} to {}\n", data, field_name)};
                        return false;
                    }
                    pThisRow->GetReflection()->SetEnum(pThisRow, pFieldDesc, pEnumVal);
                }
            }
            break;
            case FieldDescriptor::TYPE_DOUBLE:
            {
                pThisRow->GetReflection()->SetDouble(pThisRow, pFieldDesc, stod(data));
            }
            break;
            case FieldDescriptor::TYPE_FLOAT:
            {
                pThisRow->GetReflection()->SetFloat(pThisRow, pFieldDesc, stof(data));
            }
            break;
            case FieldDescriptor::TYPE_INT32:
            case FieldDescriptor::TYPE_SINT32:
            case FieldDescriptor::TYPE_SFIXED32:
            {
                pThisRow->GetReflection()->SetInt32(pThisRow, pFieldDesc, stol(data));
            }
            break;
            case FieldDescriptor::TYPE_UINT32:
            case FieldDescriptor::TYPE_FIXED32:
            {
                if(data.substr(0, 2) == "0x" || data.substr(0, 2) == "0X")
                    pThisRow->GetReflection()->SetUInt32(pThisRow, pFieldDesc, stoul(data, 0, 16));
                else
                    pThisRow->GetReflection()->SetUInt32(pThisRow, pFieldDesc, stoul(data));
            }
            break;
            case FieldDescriptor::TYPE_INT64:
            case FieldDescriptor::TYPE_SINT64:
            case FieldDescriptor::TYPE_SFIXED64:
            {
                pThisRow->GetReflection()->SetInt64(pThisRow, pFieldDesc, stoll(data));
            }
            break;
            case FieldDescriptor::TYPE_UINT64:
            case FieldDescriptor::TYPE_FIXED64:
            {
                if(data.substr(0, 2) == "0x" || data.substr(0, 2) == "0X")
                    pThisRow->GetReflection()->SetUInt64(pThisRow, pFieldDesc, stoull(data, 0, 16));
                else
                    pThisRow->GetReflection()->SetUInt64(pThisRow, pFieldDesc, stoull(data));
            }
            break;
            case FieldDescriptor::TYPE_BOOL:
            {
                bool bVal = (data == "true" || data == "TRUE" || data == "1");
                pThisRow->GetReflection()->SetBool(pThisRow, pFieldDesc, bVal);
            }
            break;
            case FieldDescriptor::TYPE_STRING:
            case FieldDescriptor::TYPE_BYTES:
            {
                pThisRow->GetReflection()->SetString(pThisRow, pFieldDesc, data);
            }
            break;
            default:
                break;
        }
        return true;
    }

    bool SetMessageData(google::protobuf::Message* pPBMessage, const std::string& field_name, const std::string& data)
    {
        using namespace google::protobuf;
        using json = nlohmann::json;

        Message*               pThisRow   = pPBMessage;
        const FieldDescriptor* pFieldDesc = nullptr;
        if(FindFieldInMessage(field_name, pThisRow, pFieldDesc) == false)
            return false;

        if(pFieldDesc->is_repeated())
        {

            auto js_doc = json::parse(data.c_str(), nullptr, false, true);
            if(js_doc.is_discarded())
            {
                // the input is invalid JSON, try split string
                auto vecData = split_string(data, ",");
                for(const std::string& str: vecData)
                {
                    AddFieldData(pThisRow, pFieldDesc, str);
                }
            }
            else
            {
                if(js_doc.is_array() == false)
                {
                    if(pFieldDesc->type() != FieldDescriptor::TYPE_MESSAGE)
                    {
                        AddFieldData(pThisRow, pFieldDesc, data);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }

                if(pFieldDesc->type() == FieldDescriptor::TYPE_MESSAGE)
                {
                    for(const auto& array_v: js_doc)
                    {
                        auto pSubMessage = pThisRow->GetReflection()->AddMessage(pThisRow, pFieldDesc, nullptr);

                        for(const auto& [key, v]: array_v.items())
                        {
                            if(v.is_string())
                            {
                                SetMessageData(pSubMessage, key, v.get<std::string>());
                            }
                            else
                            {

                                SetMessageData(pSubMessage, key, v.dump());
                            }
                        }
                    }
                }
                else
                {
                    for(auto& array_v: js_doc)
                    {
                        AddFieldData(pThisRow, pFieldDesc, array_v.dump());
                    }
                }
            }

            return true;
        }
        else if(pFieldDesc->type() == FieldDescriptor::TYPE_MESSAGE)
        {
            auto pSubMessage = pThisRow->GetReflection()->MutableMessage(pThisRow, pFieldDesc, nullptr);
            return pb_util::LoadFromJsonTxt(data, *pSubMessage);
        }
        else
        {
            return SetFieldData(pThisRow, pFieldDesc, field_name, data);
        }
    }

    bool AddMessageData(google::protobuf::Message* pPBMessage, const std::string& field_name, const std::string& data)
    {
        using namespace google::protobuf;
        Message*               pThisRow   = pPBMessage;
        const FieldDescriptor* pFieldDesc = nullptr;
        if(FindFieldInMessage(field_name, pThisRow, pFieldDesc) == false)
            return false;

        AddFieldData(pThisRow, pFieldDesc, data);

        return true;
    }

    bool JoinMessageData(google::protobuf::Message* pPBMessage, const std::string& field_name, const std::string& data)
    {
        using namespace google::protobuf;
        Message*               pThisRow   = pPBMessage;
        const FieldDescriptor* pFieldDesc = nullptr;
        if(FindFieldInMessage(field_name, pThisRow, pFieldDesc) == false)
            return false;

        JoinFieldData(pThisRow, pFieldDesc, data);

        return true;
    }

    google::protobuf::Message* AddMessageSubMessage(google::protobuf::Message* pPBMessage, const std::string& field_name)
    {
        using namespace google::protobuf;
        Message*               pThisRow   = pPBMessage;
        const FieldDescriptor* pFieldDesc = nullptr;
        if(FindFieldInMessage(field_name, pThisRow, pFieldDesc) == false)
            return nullptr;

        return pThisRow->GetReflection()->AddMessage(pThisRow, pFieldDesc);
    }

    google::protobuf::Message* NewProtoMessage(const std::string& MsgType)
    {
        using namespace google::protobuf;
        Message*          pMessage = nullptr;
        const Descriptor* pDesc    = DescriptorPool::generated_pool()->FindMessageTypeByName(MsgType);
        if(pDesc)
        {
            const Message* pPrototype = MessageFactory::generated_factory()->GetPrototype(pDesc);
            if(pPrototype)
            {
                pMessage = pPrototype->New();
            }
        }
        return pMessage;
    }

    void DelProtoMessage(google::protobuf::Message* pMessage)
    {
        if(pMessage)
        {
            pMessage->Clear();
            delete pMessage;
        }
    }

} // namespace pb_util