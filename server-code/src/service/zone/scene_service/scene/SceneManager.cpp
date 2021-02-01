#include "SceneManager.h"

#include "GameMap.h"
#include "MapManager.h"
#include "Phase.h"
#include "Scene.h"
#include "SceneService.h"
CSceneManager::CSceneManager()
    : m_nStaticScene(0)
{
}

CSceneManager::~CSceneManager()
{
    Destroy();
}

bool CSceneManager::Init(uint32_t idZone)
{
    __ENTER_FUNCTION
    //将所有归属于本Zone的非副本地图进行加载
    MapManager()->ForEach([pThis = this, idZone](CGameMap* pMap) {
        //如果不是本Zone的Map, 不会读取MapData,  副本地图也不需要创建
        if(pMap->IsZoneMap(idZone) == false || pMap->IsDynaMap() == true)
            return;

        pThis->_CreateStaticScene(pMap->GetMapID());
    });

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CSceneManager::Destroy()
{
    __ENTER_FUNCTION
    LOGDEBUG("CSceneManager::Destory");
    for(auto& [k, v]: m_mapScene)
    {
        v->Destroy();
        SAFE_DELETE(v);
    }
    m_mapScene.clear();
    __LEAVE_FUNCTION
}

CPhase* CSceneManager::CreatePhase(uint16_t idMap, uint16_t idPhaseType, uint64_t idPhase)
{
    __ENTER_FUNCTION
    const CGameMap* pMap = MapManager()->QueryMap(idMap);
    CHECKF(pMap && pMap->IsZoneMap(SceneService()->GetZoneID()));
    CScene* pScene = QueryScene(idMap);
    if(pScene)
    {
        return pScene->CreatePhase(idPhaseType, idPhase);
    }
    else
    {
        SceneIdx idxScene(SceneService()->GetZoneID(), idMap, 0);
        CScene*  pScene = CreateNew<CScene>(idxScene, idPhaseType, idPhase);

        m_mapScene[idMap] = pScene;

        LOGMESSAGE("DynaScene {} Created", idMap);

        return pScene->QueryPhase(idPhase);
    }
    __LEAVE_FUNCTION
    return nullptr;
}

CScene* CSceneManager::_CreateStaticScene(uint16_t idMap)
{
    __ENTER_FUNCTION
    const CGameMap* pMap = MapManager()->QueryMap(idMap);
    CHECKF(pMap && pMap->IsDynaMap() == false);

    SceneIdx idxScene(SceneService()->GetZoneID(), idMap, 0);
    CHECKF(QueryScene(idMap) == nullptr);
    CScene* pScene = CreateNew<CScene>(idMap, 0, 0);

    m_mapScene[idMap] = pScene;

    m_nStaticScene++;
    return pScene;
    __LEAVE_FUNCTION
    return nullptr;
}

CPhase* CSceneManager::QueryPhase(const SceneIdx& idxScene)
{
    __ENTER_FUNCTION
    CScene* pScene = QueryScene(idxScene.GetMapID());
    if(pScene == nullptr)
    {
        return nullptr;
    }
    auto pPhase = pScene->QueryPhaseByIdx(idxScene.GetPhaseIdx());
    return pPhase;
    __LEAVE_FUNCTION
    return nullptr;
}

CScene* CSceneManager::QueryScene(uint16_t idMap)
{
    __ENTER_FUNCTION
    auto itFind = m_mapScene.find(idMap);
    if(itFind != m_mapScene.end())
        return itFind->second;
    __LEAVE_FUNCTION
    return nullptr;
}

size_t CSceneManager::GetSceneCount()
{
    return m_nStaticScene;
}

size_t CSceneManager::GetDynaSceneCount()
{
    return m_mapScene.size() - m_nStaticScene;
}

void CSceneManager::ForEach(const std::function<void(CScene*)>& func)
{
    for(const auto& [k, v]: m_mapScene)
    {
        func(v);
    }
}
