#ifndef PROTOBUFFUTIL_H
#define PROTOBUFFUTIL_H

#include <string>

#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>

#include "BaseCode.h"

namespace pb_util
{
    bool LoadFromTextualFile(const std::string& filename, google::protobuf::Message& pbm);
    bool SaveToTextualFile(const google::protobuf::Message& pbm, const std::string& filename);
    bool LoadFromBinaryFile(const std::string& filename, google::protobuf::Message& pbm);
    bool SaveToBinaryFile(const google::protobuf::Message& pbm, const std::string& filename);

    bool LoadFromJsonFile(const std::string& filename, google::protobuf::Message& pbm);
    bool SaveToJsonFile(const google::protobuf::Message& pbm, const std::string& filename);
    bool LoadFromJsonTxt(const std::string& jsonTxt, google::protobuf::Message& pbm);
    bool SaveToJsonTxt(const google::protobuf::Message& pbm, std::string& jsonTxt);


    bool                       SetMessageData(google::protobuf::Message* pPBMessage, const std::string& field_name, const std::string& data);
    bool                       AddMessageData(google::protobuf::Message* pPBMessage, const std::string& field_name, const std::string& data);
    bool                       JoinMessageData(google::protobuf::Message* pPBMessage, const std::string& field_name, const std::string& data);
    google::protobuf::Message* AddMessageSubMessage(google::protobuf::Message* pPBMessage, const std::string& field_name);

    google::protobuf::Message* NewProtoMessage(const std::string& MsgType);
    void                       DelProtoMessage(google::protobuf::Message* pMessage);

    bool FindFieldInMessage(const std::string&                        field_name,
                            google::protobuf::Message*&               pThisRow,
                            const google::protobuf::FieldDescriptor*& pFieldDesc);

    // Copy all the fields tagged with '[reconfigurable]' from |src| to |dst|.
    // NOTE: |src| and |dst| must be of the same type.
    void CopyReconfigurableFields(const google::protobuf::Message& src, google::protobuf::Message& dst);
} /*namespace pb_util*/

#endif /* PROTOBUFFUTIL_H */
