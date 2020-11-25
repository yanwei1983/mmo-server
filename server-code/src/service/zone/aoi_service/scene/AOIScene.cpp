
#include "AOIScene.h"

#include "AOIPhase.h"
#include "AOIService.h"
#include "GameMap.h"
#include "MapManager.h"
#include "NetMSGProcess.h"
#include "config/Cfg_Phase.pb.h"
#include "config/Cfg_Scene.pb.h"
CAOIScene::CAOIScene() {}

CAOIScene::~CAOIScene()
{
    m_pPhaseSetByIdx.clear();
    m_pPhaseSet.clear();
}

bool CAOIScene::Init(uint16_t idMap)
{
    m_idMap = idMap;
    return true;
}

CAOIPhase* CAOIScene::CreatePhase(const SceneIdx& idxScene, uint64_t idPhase)
{
    __ENTER_FUNCTION
    auto pMap = MapManager()->QueryMap(m_idMap);
    CHECKF(pMap);
    auto       pPhaseData = pMap->GetPhaseDataById(idPhase);
    CAOIPhase* pPhase     = CAOIPhase::CreateNew(this, idxScene, idPhase, pPhaseData);
    CHECKF(pPhase);
    m_pPhaseSet[idPhase].reset(pPhase);
    m_pPhaseSetByIdx[idxScene.GetPhaseIdx()] = pPhase;
    return pPhase;
    __LEAVE_FUNCTION
    return nullptr;
}

CAOIPhase* CAOIScene::CreatePhase(const SceneIdx& idxScene, uint64_t idPhase, const Cfg_Phase* pPhaseData)
{
    __ENTER_FUNCTION
    CAOIPhase* pPhase = CAOIPhase::CreateNew(this, idxScene, idPhase, pPhaseData);
    CHECKF(pPhase);
    m_pPhaseSet[idPhase].reset(pPhase);
    m_pPhaseSetByIdx[idxScene.GetPhaseIdx()] = pPhase;
    return pPhase;
    __LEAVE_FUNCTION
    return nullptr;
}

bool CAOIScene::DestoryPhase(uint64_t idPhase)
{
    __ENTER_FUNCTION
    CAOIPhase* pPhase = QueryPhaseByID(idPhase);
    CHECKF(pPhase);

    m_pPhaseSetByIdx.erase(pPhase->GetSceneIdx().GetPhaseIdx());
    m_pPhaseSet.erase(pPhase->GetPhaseID());
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CAOIScene::DestoryPhaseByIdx(uint32_t idxPhase)
{
    __ENTER_FUNCTION
    CAOIPhase* pPhase = QueryPhaseByIdx(idxPhase);
    CHECKF(pPhase);

    m_pPhaseSetByIdx.erase(pPhase->GetSceneIdx().GetPhaseIdx());
    m_pPhaseSet.erase(pPhase->GetPhaseID());
    return true;
    __LEAVE_FUNCTION
    return false;
}

CAOIPhase* CAOIScene::QueryPhaseByID(uint64_t idPhase) const
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

CAOIPhase* CAOIScene::QueryPhaseByIdx(uint32_t idxPhase) const
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