#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <unordered_map>

#include "BaseCode.h"

class CGameMap;
class CMapData;
export_lua class CMapManager : public NoncopyableT<CMapManager>
{
    CMapManager();

public:
    CreateNewImpl(CMapManager);

public:
    ~CMapManager();

    bool             Init(uint16_t idZone);
    export_lua void  ForEach(const std::function<void(CGameMap*)>& func) const;
    export_lua const CGameMap* QueryMap(uint16_t idMap) const;
    export_lua const CMapData* QueryMapData(uint16_t idMapTemplate) const;

    CGameMap* _QueryMap(uint16_t idMap) const;

private:
    std::unordered_map<uint16_t, std::unique_ptr<CGameMap>> m_vecMap;
    std::unordered_map<uint16_t, std::unique_ptr<CMapData>> m_vecMapData;
};

#endif /* MAPMANAGER_H */
