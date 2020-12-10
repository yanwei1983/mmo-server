#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "BaseCode.h"
#include "MysqlConnection.h"
#include "NetworkDefine.h"
#include "Thread.h"



class CAuthService;

class CAuthManager : public NoncopyableT<CAuthManager>
{
    CAuthManager();
    bool Init(CAuthService* pService);

public:
    CreateNewImpl(CAuthManager);
    void Destory();

public:
    ~CAuthManager();

    //验证
    bool IsAuthing(const VirtualSocket& vs) const;
    bool Auth(const std::string& openid, const std::string& auth, const VirtualSocket& vs);
    void _OnAuthFail(const VirtualSocket& vs, const std::string& str_detail);
    void _OnAuthSucc(const VirtualSocket& vs);
    void CancleAuth(const VirtualSocket& vs);
    void ProcessResult();

    bool CheckProgVer(const std::string& prog_ver) const;

private:
    //等待认证列表
    struct ST_AUTH_DATA
    {
        std::string   open_id;
        VirtualSocket from;
    };
    std::unordered_map<VirtualSocket, ST_AUTH_DATA> m_AuthDataList;
    MPSCQueue<std::function<void()>>           m_ResultList;
    uint64_t                                   m_CallIdx = 0;

    std::unique_ptr<CWorkerThread> m_threadAuth;
};

#endif /* AUTHMANAGER_H */
