#include "UserManager.h"

#include "Account.h"
#include "AccountInfo.h"
#include "User.h"
#include "WorldService.h"
CUserManager::CUserManager() {}

CUserManager::~CUserManager()
{
    Destroy();
}

bool CUserManager::Init()
{
    return true;
}

void CUserManager::Destroy()
{
    LOGDEBUG("CUserManager::Destory");
    for(auto& v: m_setUser)
    {
        SAFE_DELETE(v.second);
    }
    m_setUser.clear();
    m_setUserBySocket.clear();
}

CUser* CUserManager::QueryUser(OBJID uid)
{
    auto itFind = m_setUser.find(uid);
    if(itFind == m_setUser.end())
        return nullptr;
    return itFind->second;
}

CUser* CUserManager::QueryUserBySocket(const VirtualSocket& vs)
{
    auto itFind = m_setUserBySocket.find(vs);
    if(itFind == m_setUserBySocket.end())
        return nullptr;
    return itFind->second;
}

CUser* CUserManager::CreateUser(CAccount* pAccount, ST_ROLE_INFO* pInfo)
{
    __ENTER_FUNCTION

    CUser* pUser = QueryUser(pInfo->GetID());
    if(pUser)
    {
        LOGERROR(" CUserManager::CreateUser  {} twice !!!!!!", pInfo->GetID());
        return pUser;
    }
    pUser = CreateNew<CUser>(pAccount, pInfo);
    if(pUser == nullptr)
    {
        LOGERROR(" CUserManager::CreateUser  {} fail !!!!!!", pInfo->GetID());
        return nullptr;
    }
    m_setUser[pUser->GetID()]             = pUser;
    m_setUserBySocket[pUser->GetSocket()] = pUser;
    return pUser;
    __LEAVE_FUNCTION
    return nullptr;
}

void CUserManager::RemoveUser(CUser*& pRemoveUser)
{
    __ENTER_FUNCTION

    if(pRemoveUser == nullptr)
        return;
    auto itFind = m_setUser.find(pRemoveUser->GetID());
    if(itFind == m_setUser.end())
        return;

    CUser* pUser = itFind->second;
    if(pUser != pRemoveUser)
    {
        LOGERROR(" CUserManager::RemoveUser  {} fail !!!!!!", pRemoveUser->GetID());
        return;
    }
    m_setUser.erase(itFind);
    m_setUserBySocket.erase(pRemoveUser->GetSocket());

    SAFE_DELETE(pUser);
    pRemoveUser = nullptr;
    __LEAVE_FUNCTION
}
