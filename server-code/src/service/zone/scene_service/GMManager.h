#ifndef GMMANAGER_H
#define GMMANAGER_H

#include "BaseCode.h"

class CPlayer;
class CMysqlConnection;
class CGMManager : public NoncopyableT<CGMManager>
{
    CGMManager();
    bool Init(CMysqlConnection* pGlobalDB);
    bool Init();

public:
    CreateNewImpl(CGMManager);

public:
    ~CGMManager();

    void Destroy();
    void Reload();

    uint32_t GetGMLevel(const std::string& openid) const;

    typedef std::function<void(CPlayer*, const std::vector<std::string>&)> GMCmdHandle;

    void ProcessGMCmd(CPlayer* pPlayer, const std::string& cmd);
    void RegisterGMCmd(const std::string& cmd, GMCmdHandle&& handle);
    void GMCmdHandlerRegister();

private:
    std::map<std::string, uint32_t>    m_GMList;
    std::map<std::string, GMCmdHandle> m_GMCmdHandle;
};

#endif /* GMMANAGER_H */
