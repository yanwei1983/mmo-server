#include "protomsg_to_cmd.h"
#include "BaseCode.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

using proto_msg_t = google::protobuf::Message;

#include "msg/ts_cmd.pb.h"
#include "server_msg/server_side.pb.h"


uint32_t msg_to_cmd(const proto_msg_t& msg)
{
    __ENTER_FUNCTION
    auto msg_desc = msg.GetDescriptor();
    const auto& options   = msg_desc->options();
    
    {
        const auto& cmd_extension = options.GetExtension(cmd);
        if(cmd_extension.ts_cmd() != 0)
        {
            CHECK_DEBUG( "CMD_" + msg_desc->name() == TS_CMD_Name(cmd_extension.ts_cmd()) );
            return cmd_extension.ts_cmd();
        }
        if(cmd_extension.sc_cmd() != 0)
        {
            CHECK_DEBUG( "CMD_" + msg_desc->name() ==  SC_CMD_Name(cmd_extension.sc_cmd()) );
            return cmd_extension.sc_cmd();
        }   
        if(cmd_extension.cs_cmd() != 0)
        {
            CHECK_DEBUG( "CMD_" + msg_desc->name() == CS_CMD_Name(cmd_extension.cs_cmd())  );
            return cmd_extension.cs_cmd();
        } 
    }
   
    {
        const auto& server_cmd_extension = options.GetExtension(ServerMSG::server_cmd);
        if(server_cmd_extension.id() != 0)
        {
            CHECK_DEBUG( "MsgID_" + msg_desc->name() == ServerMSG::OMsgID_Name(server_cmd_extension.id()) );
            return server_cmd_extension.id();
        }
    }

    __LEAVE_FUNCTION
    return 0;
}




std::string cs_cmd_to_name(uint32_t cmd)
{
    if(cmd < CS_CMD_MAX)
    {
        return CS_CMD_Name(cmd);
    }
    else
    {
        return ServerMSG::OMsgID_Name(cmd);
    }
}

std::string sc_cmd_to_name(uint32_t cmd)
{
    if(cmd < SC_CMD_MAX)
    {
        return SC_CMD_Name(cmd);
    }
    else
    {
        return ServerMSG::OMsgID_Name(cmd);
    }
}