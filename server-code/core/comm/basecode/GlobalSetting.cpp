#include "GlobalSetting.h"

#include <fstream>

#include "LoggingMgr.h"
#include "log4z.h"
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

CGlobalSetting::CGlobalSetting() {}
CGlobalSetting::~CGlobalSetting() {}

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
    __ENTER_FUNCTION
    if(filename.empty() == false)
    {
        
        std::ifstream infile(filename);
        m_setDataMap = nlohmann::json::parse(infile, nullptr, false, true);
        
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

    {
        std::string value       = m_setDataMap["debug"]["log_aidebug"];
        auto log_aidebug = magic_enum::enum_cast<ENUM_LOG_LEVEL>(value);
        if(log_aidebug)
            BaseCode::g_log_aidebug = enum_to(log_aidebug.value());
    }

    {
        std::string value       = m_setDataMap["debug"]["log_actordebug"];
        auto log_actordebug = magic_enum::enum_cast<ENUM_LOG_LEVEL>(value);
        if(log_actordebug)
            BaseCode::g_log_actordebug = enum_to(log_actordebug.value());
    }

    {
        std::string value = m_setDataMap["debug"]["log_skilldebug"];
        auto log_skilldebug = magic_enum::enum_cast<ENUM_LOG_LEVEL>(value);
        if(log_skilldebug)
            BaseCode::g_log_skilldebug = enum_to(log_skilldebug.value());
    }

    {
        std::string value = m_setDataMap["debug"]["log_lev"];
        auto log_lev = magic_enum::enum_cast<ENUM_LOG_LEVEL>(value);
        if(log_lev)
            BaseCode::SetLogLev(log_lev.value());
    }
    return true;

    __LEAVE_FUNCTION
    return false;
}