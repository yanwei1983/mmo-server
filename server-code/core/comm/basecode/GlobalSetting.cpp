#include "GlobalSetting.h"

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
        g_pGlobalSetting->Destroy();
        SAFE_DELETE(g_pGlobalSetting);
    }
}

CGlobalSetting::CGlobalSetting() {}
CGlobalSetting::~CGlobalSetting() {}

bool CGlobalSetting::Init()
{
    return true;
}
bool CGlobalSetting::Destroy()
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

    return true;

    __LEAVE_FUNCTION
    return false;
}