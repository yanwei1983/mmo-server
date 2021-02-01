
#include "AIScene.h"

#include "AIPhase.h"
#include "AIService.h"
#include "GameMap.h"
#include "MapManager.h"
#include "NetMSGProcess.h"
#include "config/Cfg_Phase.pb.h"
#include "config/Cfg_Scene.pb.h"
CAIScene::CAIScene() {}

CAIScene::~CAIScene()
{
    m_pPhaseSetByIdx.clear();
    m_pPhaseSet.clear();
}

bool CAIScene::Init(uint16_t idMap)
{
    m_idMap = idMap;
    return true;
}

CAIPhase* CAIScene::CreatePhase(const SceneIdx& idxScene, uint64_t idPhase)
{
    __ENTER_FUNCTION
    auto pMap = MapManager()->QueryMap(m_idMap);
    CHECKF(pMap);
    auto      pPhaseData = pMap->GetPhaseDataById(idPhase);
    CAIPhase* pPhase     = CreateNew<CAIPhase>(this, idxScene, idPhase, pPhaseData);
    CHECKF(pPhase);
    m_pPhaseSet[idPhase].reset(pPhase);
    m_pPhaseSetByIdx[idxScene.GetPhaseIdx()] = pPhase;
    return pPhase;
    __LEAVE_FUNCTION
    return nullptr;
}

CAIPhase* CAIScene::CreatePhase(const SceneIdx& idxScene, uint64_t idPhase, const Cfg_Phase* pPhaseData)
{
    __ENTER_FUNCTION
    CAIPhase* pPhase = CreateNew<CAIPhase>(this, idxScene, idPhase, pPhaseData);
    CHECKF(pPhase);
    m_pPhaseSet[idPhase].reset(pPhase);
    m_pPhaseSetByIdx[idxScene.GetPhaseIdx()] = pPhase;
    return pPhase;
    __LEAVE_FUNCTION
    return nullptr;
}

bool CAIScene::DestoryPhase(uint64_t idPhase)
{
    __ENTER_FUNCTION
    CAIPhase* pPhase = QueryPhaseByID(idPhase);
    CHECKF(pPhase);

    m_pPhaseSetByIdx.erase(pPhase->GetSceneIdx().GetPhaseIdx());
    m_pPhaseSet.erase(pPhase->GetPhaseID());
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CAIScene::DestoryPhaseByIdx(uint32_t idxPhase)
{
    __ENTER_FUNCTION
    CAIPhase* pPhase = QueryPhaseByIdx(idxPhase);
    CHECKF(pPhase);

    m_pPhaseSetByIdx.erase(pPhase->GetSceneIdx().GetPhaseIdx());
    m_pPhaseSet.erase(pPhase->GetPhaseID());
    return true;
    __LEAVE_FUNCTION
    return false;
}

CAIPhase* CAIScene::QueryPhaseByID(uint64_t idPhase) const
{
    __ENTER_FUNCTION
    auto it = m_pPhaseSet.find(idPhase);
    if(it != m_pPhaseSet.end())
    {
        return it->second.get();
    }
    __LEAVE_FUNCTION
    return nullptr;
}

CAIPhase* CAIScene::QueryPhaseByIdx(uint32_t idxPhase) const
{
    __ENTER_FUNCTION
    auto it = m_pPhaseSetByIdx.find(idxPhase);
    if(it != m_pPhaseSetByIdx.end())
    {
        return it->second;
    }
    __LEAVE_FUNCTION
    return nullptr;
}