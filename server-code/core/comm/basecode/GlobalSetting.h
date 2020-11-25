#ifndef GLOBALSETTING_H
#define GLOBALSETTING_H

#include "BaseCode.h"
#include "Noncopyable.h"
#include "json.hpp"

class CGlobalSetting : public NoncopyableT<CGlobalSetting>
{
    CGlobalSetting();
    bool Init();

public:
    virtual ~CGlobalSetting();
    bool Destory();
    CreateNewImpl(CGlobalSetting);

public:
    bool LoadSetting(const std::string& filename);

    const nlohmann::json& GetData() const { return m_setDataMap; }

private:
    nlohmann::json m_setDataMap;
};

CGlobalSetting* GetGlobalSetting();
void            CreateGlobalSetting();
void            ReleaseGlobalSetting();

#endif /* GLOBALSETTING_H */
