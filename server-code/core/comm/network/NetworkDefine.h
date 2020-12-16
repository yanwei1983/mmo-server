#ifndef NetworkDefine_h__
#define NetworkDefine_h__

#include <atomic>
#include <unordered_map>

#include "BaseType.h"
#include "CheckUtil.h"
#include "IntUtil.h"
#include "Misc.h"
#include "export_lua.h"

const size_t _MAX_MSGSIZE                  = 1024 * 4;
const size_t _DEFAULT_LOGWRITEHIGHWATEMARK = 1024; // packet*1024
using WorldID_t                            = uint16_t;

using ServiceType_t = uint8_t;
using ServiceIdx_t  = uint8_t;

using ServiceID_Data_t = uint16_t;
export_lua class ServiceID
{
public:
    export_lua ServiceID(ServiceID_Data_t nServerID = 0)
        : m_Data16(nServerID)
    {
    }

    export_lua ServiceID(ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
        : m_Data{idServiceType, idServiceIdx}
    {
    }

    export_lua ServiceID(const ServiceID& rht)
        : m_Data(rht.m_Data)
    {
    }

    export_lua bool IsVaild() const { return m_Data16 != 0; }

    operator ServiceID_Data_t() const { return m_Data16; }

    export_lua bool operator==(const ServiceID& rht) const { return m_Data16 == rht.m_Data16; }

    export_lua bool operator<(const ServiceID& rht) const { return m_Data16 < rht.m_Data16; }

    export_lua ServiceType_t    GetServiceType() const { return m_Data.idServiceType; }
    export_lua void             SetServiceType(ServiceType_t idServiceType) { m_Data.idServiceType = idServiceType; }
    export_lua ServiceIdx_t     GetServiceIdx() const { return m_Data.idServiceIdx; }
    export_lua void             SetServiceIdx(ServiceIdx_t idServiceIdx) { m_Data.idServiceIdx = idServiceIdx; }
    export_lua ServiceID_Data_t GetData() const { return m_Data16; }
    export_lua void             SetData(ServiceID_Data_t _Data16) { m_Data16 = _Data16; }

private:
    struct ST_SERVERID
    {
        ServiceType_t idServiceType = 0; //实际取值范围0~0xFF 255
        ServiceIdx_t  idServiceIdx  = 0; //实际取值范围0~0xFF 255
    };
    union {
        ST_SERVERID      m_Data;
        ServiceID_Data_t m_Data16 = 0;
    };
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    template<>
    struct hash<ServiceID>
    {
        typedef ServiceID   argument_type;
        typedef std::size_t result_type;
        result_type         operator()(argument_type const& s) const
        {
            std::hash<ServiceID_Data_t> hasher;
            return hasher(s.GetData());
        }
    };
} // namespace std

template<>
struct fmt::formatter<ServiceID> : public fmt::formatter<ServiceID_Data_t>
{
    template<typename FormatContext>
    auto format(const ServiceID& serviceid, FormatContext& ctx)
    {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "({}-{})", serviceid.GetServiceType(), serviceid.GetServiceIdx());
    }
};

using ServerPort_Data_t = uint32_t;
export_lua class ServerPort
{
public:
    export_lua ServerPort(ServerPort_Data_t nServerPort = 0)
        : m_Data32(nServerPort)
    {
    }

    export_lua ServerPort(WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
        : m_Data{idWorld, ServiceID{idServiceType, idServiceIdx}}
    {
    }

    export_lua ServerPort(WorldID_t idWorld, const ServiceID& idService)
        : m_Data{idWorld, idService}
    {
    }

    export_lua ServerPort(const ServerPort& rht)
        : m_Data(rht.m_Data)
    {
    }

    export_lua bool IsVaild() const { return m_Data32 != 0; }

    operator ServerPort_Data_t() const { return m_Data32; }

    export_lua bool operator==(const ServerPort& rht) const { return m_Data32 == rht.m_Data32; }

    export_lua bool operator<(const ServerPort& rht) const { return m_Data32 < rht.m_Data32; }

    export_lua WorldID_t GetWorldID() const { return m_Data.idWorld; }
    export_lua void      SetWorldID(WorldID_t idWorld) { m_Data.idWorld = idWorld; }

    export_lua const ServiceID& GetServiceID() const { return m_Data.idService; }
    export_lua void             SetServiceID(const ServiceID& idService) { m_Data.idService = idService; }

    export_lua ServiceType_t     GetServiceType() const { return m_Data.idService.GetServiceType(); }
    export_lua void              SetServiceType(ServiceType_t idServiceType) { m_Data.idService.SetServiceType(idServiceType); }
    export_lua ServiceIdx_t      GetServiceIdx() const { return m_Data.idService.GetServiceIdx(); }
    export_lua void              SetServiceIdx(ServiceIdx_t idServiceIdx) { m_Data.idService.SetServiceIdx(idServiceIdx); }
    export_lua ServerPort_Data_t GetData() const { return m_Data32; }
    export_lua void              SetData(ServerPort_Data_t _Data32) { m_Data32 = _Data32; }

private:
    struct ST_SERVERPORT
    {
        WorldID_t idWorld = 0; //实际取值范围0~0xFFFF 65535
        ServiceID idService;
    };
    union {
        ST_SERVERPORT     m_Data;
        ServerPort_Data_t m_Data32 = 0;
    };
};
using ServerPortList = std::vector<ServerPort>;
// custom specialization of std::hash can be injected in namespace std
namespace std
{
    template<>
    struct hash<ServerPort>
    {
        typedef ServerPort  argument_type;
        typedef std::size_t result_type;
        result_type         operator()(argument_type const& s) const
        {
            std::hash<ServerPort_Data_t> hasher;
            return hasher(s.GetData());
        }
    };
} // namespace std

template<>
struct fmt::formatter<ServerPort> : public fmt::formatter<ServerPort_Data_t>
{
    template<typename FormatContext>
    auto format(const ServerPort& port, FormatContext& ctx)
    {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "({}-{}-{})", port.GetWorldID(), port.GetServiceType(), port.GetServiceIdx());
    }
};

using VirtualSocket_Data_t = uint64_t;
using SocketIdx_t          = uint16_t;
export_lua class VirtualSocket
{
public:
    export_lua VirtualSocket(VirtualSocket_Data_t nVirtualSocket = 0)
        : m_Data64(nVirtualSocket)
    {
    }

    export_lua VirtualSocket(const ServerPort& nServerPort)
        : m_Data{nServerPort, 0, 0}
    {
    }

    export_lua VirtualSocket(const ServerPort& nServerPort, SocketIdx_t nSocketIdx)
        : m_Data{nServerPort, nSocketIdx, 0}
    {
    }
    export_lua VirtualSocket(const VirtualSocket& rht)
        : m_Data64(rht.m_Data64)
    {
    }

    export_lua static VirtualSocket CreateVirtualSocket(const ServerPort& nServerPort, SocketIdx_t nSocketIdx)
    {
        return VirtualSocket{nServerPort, nSocketIdx};
    }

    export_lua bool IsVaild() const { return m_Data64 != 0; }

    operator VirtualSocket_Data_t() const { return m_Data64; }

    export_lua bool operator==(const VirtualSocket& rht) const { return m_Data64 == rht.m_Data64; }

    export_lua bool operator<(const VirtualSocket& rht) const { return m_Data64 < rht.m_Data64; }

    export_lua const ServerPort& GetServerPort() const { return m_Data.stServerPort; }
    export_lua void              SetServerPort(const ServerPort& val) { m_Data.stServerPort = val; }

    export_lua WorldID_t GetWorldID() const { return m_Data.stServerPort.GetWorldID(); }
    export_lua const ServiceID& GetServiceID() const { return m_Data.stServerPort.GetServiceID(); }
    export_lua ServiceType_t    GetServiceType() const { return m_Data.stServerPort.GetServiceID().GetServiceType(); }
    export_lua ServiceIdx_t     GetServiceIdx() const { return m_Data.stServerPort.GetServiceID().GetServiceIdx(); }

    export_lua SocketIdx_t          GetSocketIdx() const { return m_Data.nSocketIdx; }
    export_lua void                 SetSocketIdx(SocketIdx_t val) { m_Data.nSocketIdx = val; }
    export_lua VirtualSocket_Data_t GetData64() const { return m_Data64; }

private:
    struct ST_VIRTUALSOCKET
    {
        ServerPort  stServerPort;
        SocketIdx_t nSocketIdx = 0; //实际取值范围1~0xFFFF 65535
        uint16_t    nRevert    = 0;
    };
    union {
        ST_VIRTUALSOCKET     m_Data;
        VirtualSocket_Data_t m_Data64 = 0;
    };
};

namespace std
{
    template<>
    struct hash<VirtualSocket>
    {
        typedef VirtualSocket argument_type;
        typedef std::size_t   result_type;
        result_type           operator()(argument_type const& s) const
        {
            std::hash<VirtualSocket_Data_t> hasher;
            return hasher(s.operator uint64_t());
        }
    };
} // namespace std

template<>
struct fmt::formatter<VirtualSocket> : public fmt::formatter<VirtualSocket_Data_t>
{
    template<typename FormatContext>
    auto format(const VirtualSocket& vs, FormatContext& ctx)
    {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "({}-{}-{}-{})", vs.GetWorldID(), vs.GetServiceType(), vs.GetServiceIdx(), vs.GetSocketIdx());
    }
};

using VirtualSocketMap_t = std::unordered_map<ServerPort, std::vector<VirtualSocket>>;
namespace google
{
    namespace protobuf
    {
        class Message;
    }
} // namespace google
using proto_msg_t = google::protobuf::Message;

struct ServerAddrInfo
{
    ServerAddrInfo()                      = default;
    ServerAddrInfo(const ServerAddrInfo&) = default;
    ServerAddrInfo& operator=(const ServerAddrInfo&) = default;

    uint32_t    idWorld;
    uint32_t    idServiceType;
    uint32_t    idServiceIdx;
    std::string lib_name;
    std::string bind_addr;
    std::string route_addr;
    uint32_t    route_port;
    std::string publish_addr;
    uint32_t    publish_port;
    uint32_t    debug_port;
};

const uint16_t CLIENT_MSG_ID_BEGIN = 1;
const uint16_t CLIENT_MSG_ID_END   = 10000;

//常用内部消息通信及消息结构
enum MSGTYPE_INTERNAL
{
    COMMON_CMD_BEGIN     = 0,
    COMMON_CMD_SC_KEY    = 1,
    COMMON_CMD_PING      = 2,
    COMMON_CMD_PONG      = 3,
    COMMON_CMD_INTERRUPT = 9999,

};

#pragma pack(push) //保存对齐状态
#pragma pack(1)    // 1 bytes对齐

struct MSG_HEAD
{
    uint32_t msg_size = 0;
    uint16_t msg_cmd  = 0;
    uint8_t  is_ciper = 0;
    uint8_t  reserved = 0;
};

struct MSG_PING : public MSG_HEAD
{
    MSG_PING()
    {
        msg_cmd  = COMMON_CMD_PING;
        msg_size = sizeof(MSG_PING);
    }
};

struct MSG_PONG : public MSG_HEAD
{
    MSG_PONG()
    {
        msg_cmd  = COMMON_CMD_PONG;
        msg_size = sizeof(MSG_PONG);
    }
};

#pragma pack(pop) //恢复对齐状态

#endif // NetworkDefine_h__
