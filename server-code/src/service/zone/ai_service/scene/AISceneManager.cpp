#include "AIPhase.h"
#include "AIScene.h"
#include "AISceneManagr.h"
#include "AIService.h"
#include "MapManager.h"

CAISceneManager::CAISceneManager() {}

CAISceneManager::~CAISceneManager()
{
    Destroy();
}

bool CAISceneManager::Init(uint32_t idZone)
{
    return true;
}

void CAISceneManager::Destroy()
{
    __ENTER_FUNCTION
    for(auto& [k, v]: m_mapScene)
    {
        SAFE_DELETE(v);
        LOGDEBUG("AIScene {} Destroy", k);
    }
    m_mapScene.clear();
    __LEAVE_FUNCTION
}

CAIScene* CAISceneManager::CreateScene(uint16_t idMap)
{
    __ENTER_FUNCTION
    auto pMap = MapManager()->QueryMap(idMap);
    CHECKF(pMap);

    CAIScene* pScene = CAIScene::CreateNew(idMap);
    CHECKF(pScene);

    m_mapScene[idMap] = pScene;

    LOGDEBUG("AIScene {} Created", idMap);
    m_nStaticScene++;
    return pScene;
    __LEAVE_FUNCTION
    return nullptr;
}

CAIScene* CAISceneManager::QueryScene(const SceneIdx& idxScene)
{
    __ENTER_FUNCTION
    auto itFind = m_mapScene.find(idxScene.GetMapID());
    if(itFind == m_mapScene.end())
        return nullptr;

    CAIScene* pScene = itFind->second;
    return pScene;
    __LEAVE_FUNCTION
    return nullptr;
}

CAIPhase* CAISceneManager::QueryPhase(const SceneIdx& idxScene)
{
    __ENTER_FUNCTION
    CAIScene* pScene = QueryScene(idxScene);
    if(pScene == nullptr)
    {
        return nullptr;
    }

    return pScene->QueryPhaseByIdx(idxScene.GetPhaseIdx());
    __LEAVE_FUNCTION
    return nullptr;
}

void CAISceneManager::OnTimer() {}