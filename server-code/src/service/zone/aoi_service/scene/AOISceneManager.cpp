#include "AOIPhase.h"
#include "AOIScene.h"
#include "AOISceneManagr.h"
#include "AOIService.h"
#include "MapManager.h"

CAOISceneManager::CAOISceneManager() {}

CAOISceneManager::~CAOISceneManager()
{
    Destory();
}

bool CAOISceneManager::Init(uint32_t idZone)
{
    return true;
}

void CAOISceneManager::Destory()
{
    __ENTER_FUNCTION
    for(auto& [k, v]: m_mapScene)
    {
        SAFE_DELETE(v);
        LOGDEBUG("AOIScene {} Destroy", k);
    }
    m_mapScene.clear();
    __LEAVE_FUNCTION
}

CAOIScene* CAOISceneManager::CreateScene(uint16_t idMap)
{
    __ENTER_FUNCTION
    auto pMap = MapManager()->QueryMap(idMap);
    CHECKF(pMap);

    CAOIScene* pScene = CAOIScene::CreateNew(idMap);
    CHECKF(pScene);

    m_mapScene[idMap] = pScene;

    LOGDEBUG("AOIScene {} Created", idMap);
    m_nStaticScene++;
    return pScene;
    __LEAVE_FUNCTION
    return nullptr;
}

CAOIScene* CAOISceneManager::QueryScene(const SceneIdx& idxScene)
{
    __ENTER_FUNCTION
    auto itFind = m_mapScene.find(idxScene.GetMapID());
    if(itFind == m_mapScene.end())
        return nullptr;

    CAOIScene* pScene = itFind->second;
    return pScene;
    __LEAVE_FUNCTION
    return nullptr;
}

CAOIPhase* CAOISceneManager::QueryPhase(const SceneIdx& idxScene)
{
    __ENTER_FUNCTION
    CAOIScene* pScene = QueryScene(idxScene);
    if(pScene == nullptr)
    {
        return nullptr;
    }

    return pScene->QueryPhaseByIdx(idxScene.GetPhaseIdx());
    __LEAVE_FUNCTION
    return nullptr;
}

void CAOISceneManager::OnTimer() {}