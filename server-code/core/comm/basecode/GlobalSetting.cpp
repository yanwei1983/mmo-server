#include "GlobalSetting.h"
#include "LoggingMgr.h"
#include <fstream>

static CGlobalSetting* g_pGlobalSetting = nullptr;
CGlobalSetting*        GetGlobalSetting()
{
    return g_pGlobalSetting;
}

void CreateGlobalSetting()
{
    if(g_pGlobalSetting == nullptr)
        g_pGlobalSetting = CGlobalSetting::CreateNew();
}

void ReleaseGlobalSetting()
{
    if(g_pGlobalSetting)
    {
        g_pGlobalSetting->Destory();
        SAFE_DELETE(g_pGlobalSetting);
    }
}


CGlobalSetting::CGlobalSetting(){}
CGlobalSetting::~CGlobalSetting(){}

bool CGlobalSetting::Init()
{
    return true;
}
bool CGlobalSetting::Destory()
{
    return true;
}

bool CGlobalSetting::LoadSetting(const std::string& filename)
{
    if(filename.empty() == false)
    {
        std::ifstream infile(filename);
        infile >> m_setDataMap;
        
        if(m_setDataMap.is_discarded())
        {
            LOGFATAL("CGlobalSetting::LoadSetting {} is not a json");
            return false;
        }

    }
    else
    {
        m_setDataMap["ServerInfoMYSQL"]["url"] = std::getenv("ServerInfoMYSQL_URL");
    }


    BaseCode::g_log_aidebug = m_setDataMap["debug"]["log_aidebug"];
    BaseCode::g_log_actordebug = m_setDataMap["debug"]["log_actordebug"];
    BaseCode::g_log_skilldebug = m_setDataMap["debug"]["log_skilldebug"];

    return true;
}