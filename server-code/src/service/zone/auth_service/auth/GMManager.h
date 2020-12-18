#ifndef GMMANAGER_H
#define GMMANAGER_H
#include "BaseCode.h"

class CMysqlConnection;
class CGMManager : public NoncopyableT<CGMManager>
{
    CGMManager();
    bool Init();
    bool Init(CMysqlConnection* pGlobalDB);

public:
    CreateNewImpl(CGMManager);

public:
    ~CGMManager();

    void Destroy();
    void Reload();

    uint32_t GetGMLevel(const std::string& openid) const;

private:
    std::map<std::string, uint32_t> m_GMList;
};
#endif /* GMMANAGER_H */
