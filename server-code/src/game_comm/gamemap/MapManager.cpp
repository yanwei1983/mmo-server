#include "MapManager.h"

#include "CallStackDumper.h"
#include "DataPack.h"
#include "GameMap.h"
#include "GameMapDef.h"
#include "MapData.h"
#include "ProtobuffUtil.h"
#include "config/Cfg_Phase.pb.h"
#include "config/Cfg_Scene.pb.h"
#include "config/Cfg_Scene_EnterPoint.pb.h"
#include "config/Cfg_Scene_LeavePoint.pb.h"
#include "config/Cfg_Scene_MonsterGenerator.pb.h"
#include "config/Cfg_Scene_Patrol.pb.h"
#include "config/Cfg_Scene_Reborn.pb.h"
#include "game_common_def.h"
CMapManager::CMapManager() {}

CMapManager::~CMapManager() {}

template<class T>
bool LoadCfg(const std::string& filename, CMapManager* pMapManager)
{
    std::vector<T> vecCfg;
    if(DataPack::LoadFromBinaryFile(GetCfgFilePath() + filename, vecCfg) == false)
    {
        return false;
    }

    for(const auto& iter: vecCfg)
    {
        CGameMap* pGameMap = pMapManager->_QueryMap(iter.idmap());
        if(pGameMap == nullptr)
            continue;

        pGameMap->_AddData(iter);
    }

    LOGDEBUG("{} LoadSucc.", filename);
    return true;
}

bool CMapManager::Init(uint16_t idZone)
{
    //读取配置文件
    {
        std::vector<Cfg_Scene> vecCfg;
        if(DataPack::LoadFromBinaryFile(GetCfgFilePath() + "Cfg_Scene.bytes", vecCfg) == false)
        {
            LOGERROR("InitFromFile Cfg_Scene.bytes Fail");
            return false;
        }

        for(const auto& iter: vecCfg)
        {
            CMapData* pMapData = nullptr;
            //只有本zone需要用到的map，才会加载mapdata
            if(idZone == 0 || iter.idzone() == 0 || iter.idzone() == idZone)
            {
                auto itFind = m_vecMapData.find(iter.idmapdata());
                if(itFind != m_vecMapData.end())
                {
                    pMapData = itFind->second.get();
                }
                else
                {
                    pMapData = CreateNew<CMapData>(iter.idmapdata());
                    CHECKF(pMapData);
                    m_vecMapData[pMapData->GetMapTemplateID()].reset(pMapData);
                }
            }

            CGameMap* pGameMap = CreateNew<CGameMap>(this, iter, pMapData);
            if(pGameMap == nullptr)
            {
                return false;
            }

            m_vecMap[pGameMap->GetMapID()].reset(pGameMap);
        }
    }

    LoadCfg<Cfg_Scene_EnterPoint>("Cfg_Scene_EnterPoint.bytes", this);
    LoadCfg<Cfg_Scene_LeavePoint>("Cfg_Scene_LeavePoint.bytes", this);
    LoadCfg<Cfg_Scene_MonsterGenerator>("Cfg_Scene_MonsterGenerator.bytes", this);
    LoadCfg<Cfg_Scene_Patrol>("Cfg_Scene_Patrol.bytes", this);
    LoadCfg<Cfg_Scene_Reborn>("Cfg_Scene_Reborn.bytes", this);
    LoadCfg<Cfg_Phase>("Cfg_Phase.bytes", this);

    LOGDEBUG("MapManager LoadSucc.");
    return true;
}

void CMapManager::ForEach(const std::function<void(CGameMap*)>& func) const
{
    for(const auto& v: m_vecMap)
    {
        func(v.second.get());
    }
}

const CGameMap* CMapManager::QueryMap(uint16_t idMap) const
{
    auto it_find = m_vecMap.find(idMap);
    if(it_find != m_vecMap.end())
        return it_find->second.get();
    return nullptr;
}

CGameMap* CMapManager::_QueryMap(uint16_t idMap) const
{
    auto it_find = m_vecMap.find(idMap);
    if(it_find != m_vecMap.end())
        return it_find->second.get();
    return nullptr;
}

const CMapData* CMapManager::QueryMapData(uint16_t idMapTemplate) const
{
    auto it_find = m_vecMapData.find(idMapTemplate);
    if(it_find != m_vecMapData.end())
        return it_find->second.get();
    return nullptr;
}
