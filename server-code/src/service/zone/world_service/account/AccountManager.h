#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "BaseCode.h"
#include "NetworkDefine.h"

class CNetworkMessage;
class CMysqlConnection;
class CAccount;

class CAccountManager : public NoncopyableT<CAccountManager>
{
    CAccountManager();
    bool Init(class CWorldService* pWorld);

public:
    CreateNewImpl(CAccountManager);

public:
    ~CAccountManager();

    //将Account加入列表,通知前端登录成功,并下发角色信息
    void Login(const VirtualSocket& vs, const std::string& openid);
    void Logout(const VirtualSocket& vs);

    void OnTimer();

public:
    size_t    GetAccountSize() const;
    size_t    GetWaitAccountSize() const;
    void      Destroy();
    CAccount* QueryAccountBySocket(const VirtualSocket& vs) const;

private:
    //已经认证的Account
    std::unordered_map<std::string, CAccount*>   m_setAccount;
    std::unordered_map<VirtualSocket, CAccount*> m_setAccountBySocket;
    std::deque<CAccount*>                        m_setWaitAccount;
};

#endif /* ACCOUNTMANAGER_H */
