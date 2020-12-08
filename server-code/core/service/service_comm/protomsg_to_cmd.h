#ifndef PROTOMSG_TO_CMD_H
#define PROTOMSG_TO_CMD_H

#include "BaseType.h"
namespace google
{
    namespace protobuf
    {
        class Message;
    }
}
using proto_msg_t = google::protobuf::Message;
uint32_t msg_to_cmd(const proto_msg_t& msg);
std::string sc_cmd_to_name(uint32_t cmd);
std::string cs_cmd_to_name(uint32_t cmd);


//这几个函数的实现在myproto.so中

#endif /* PROTOMSG_TO_CMD_H */
