#include "GMManager.h"

#include "MysqlConnection.h"
#include "WorldService.h"
#include "globaldb.h"

CGMManager::CGMManager() {}

CGMManager::~CGMManager() {}

bool CGMManager::Init()
{
    auto pGlobalDB = WorldService()->ConnectGlobalDB();
    return Init(pGlobalDB.get());
}

bool CGMManager::Init(CMysqlConnection* pGlobalDB)
{
    __ENTER_FUNCTION
    CHECKF(pGlobalDB);
    auto result = pGlobalDB->QueryAll(TBLD_GMLIST::table_name());
    if(result)
    {
        for(size_t i = 0; i < result->get_num_row(); i++)
        {
            auto row = result->fetch_row(false);
            if(row)
            {
                std::string openid = row->Field(TBLD_GMLIST::OPENID);
                uint32_t    lev    = row->Field(TBLD_GMLIST::GMLEV);
                m_GMList[openid]   = lev;
            }
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CGMManager::Destroy()
{
    m_GMList.clear();
}

void CGMManager::Reload()
{
    Destroy();
    Init();
}

uint32_t CGMManager::GetGMLevel(const std::string& openid) const
{
    auto it = m_GMList.find(openid);
    if(it != m_GMList.end())
    {
        return it->second;
    }
    else
    {
        struct stat _stat_data;
        if(stat("GM_TAG", &_stat_data) == 0)
            return 1;
        return 0;
    }
}
