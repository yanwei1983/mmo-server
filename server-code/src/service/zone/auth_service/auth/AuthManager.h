#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "BaseCode.h"
#include "MysqlConnection.h"
#include "NetworkDefine.h"
#include "Thread.h"

namespace brpc
{
    class Channel;
};

constexpr const char* AUTH_SERVER_SIGNATURE = "test";
constexpr int32_t     AUTH_KEY_CANUSE_SECS  = 180;
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
    bool IsAuthing(const std::string& openid) const;
    bool Auth(const std::string& openid, const std::string& auth, const VirtualSocket& vs);
    void _OnAuthFail(uint64_t call_id, const std::string& str_detail);
    void _OnAuthSucc(uint64_t call_id);
    void _AddResult(std::function<void()>&& result_func);
    void CancleAuth(const VirtualSocket& vs);
    void PorcessResult();

    void OnAuthThreadCreate();
    void OnAuthThreadFinish();

    bool CheckProgVer(const std::string& prog_ver) const;

private:
    //等待认证列表
    std::unordered_map<std::string, uint64_t>   m_AuthList;
    std::unordered_map<VirtualSocket, uint64_t> m_AuthVSList;
    struct ST_AUTH_DATA
    {
        std::string   open_id;
        VirtualSocket from;
    };
    std::unordered_map<uint64_t, ST_AUTH_DATA> m_AuthDataList;
    MPSCQueue<std::function<void()>>           m_ResultList;
    uint64_t                                   m_CallIdx = 0;

    std::unique_ptr<brpc::Channel> m_pAuthChannel;
};

#endif /* AUTHMANAGER_H */
