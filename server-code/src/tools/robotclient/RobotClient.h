#ifndef ROBOTCLIENT_H
#define ROBOTCLIENT_H

#include <deque>
#include <functional>
#include <vector>

#include "EventEntry.h"
#include "NetEventHandler.h"
#include "NetSocket.h"
class RobotClientManager;
class CNetSocket;

class RobotClient : public CNetEventHandler
{
public:
    RobotClient(RobotClientManager* pManager);
    ~RobotClient();

    static void initInLua(struct lua_State* L);

public:
    virtual size_t GetRecvPacketSizeMax() const override { return _MAX_MSGSIZE * 10; }
    virtual void OnConnected(const CNetSocketSharedPtr& pSocket) override;
    virtual void OnConnectFailed(const CNetSocketSharedPtr&) override;
    virtual void OnDisconnected(const CNetSocketSharedPtr&) override;
    virtual void OnAccepted(const CNetSocketSharedPtr&) override;
    virtual void OnRecvData(const CNetSocketSharedPtr&, CNetworkMessage&& recv_msg) override;
    virtual void OnRecvTimeout(const CNetSocketSharedPtr&) override;

    void AddEventCallBack(uint32_t nWaitMs, const std::string& func_name, bool bPersist);
    bool IsConnectServer();
    void DisconnectServer();

public:
    uint32_t GetClientID() const { return m_idClient; }
    void     SetClientID(uint32_t val) { m_idClient = val; }

    void SendProtobufToServer(proto_msg_t* pMsg);

private:
    void SendToServer(const CNetworkMessage& msg);

private:
    RobotClientManager* m_pManager;
    CNetSocketWeakPtr   m_pServerSocket;
    uint32_t            m_idClient = 0;
    CEventEntryPtr      m_Event;
};
using RobotClientPtr = std::shared_ptr<RobotClient>;

#endif /* ROBOTCLIENT_H */
