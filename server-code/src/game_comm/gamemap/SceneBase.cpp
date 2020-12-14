#include "SceneBase.h"

#include "DynaRegion.h"
#include "GameMap.h"
#include "GameMapDef.h"
#include "MapData.h"
#include "MapManager.h"
#include "SceneObject.h"
#include "SceneTree.h"

CSceneBase::CSceneBase() {}

CSceneBase::~CSceneBase() {}

bool CSceneBase::Init(const SceneIdx& idxScene, CMapManager* pMapManager)
{
    __ENTER_FUNCTION
    m_idxScene = idxScene;
    m_pMap     = pMapManager->QueryMap(idxScene.GetMapID());
    CHECKF_FMT(m_pMap, "FIND Map Fail:{}", idxScene.GetMapID());
    CHECKF_FMT(m_pMap->GetMapData(), "FIND MapData Fail:{}", idxScene.GetMapID());

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CSceneBase::InitSceneTree(const CPos2D& vBasePos, float fWidth, float fHeight, uint32_t nTileGridRange, bool bDynamicSetLev)
{
    __ENTER_FUNCTION
    if(m_pSceneTree)
    {
        return false;
    }
    CSceneTree* pSceneTree = CSceneTree::CreateNew(m_pMap, vBasePos, fWidth, fHeight, nTileGridRange, bDynamicSetLev);
    CHECKF(pSceneTree);
    m_pSceneTree.reset(pSceneTree);

    LOGDEBUG("CSceneBase::InitSceneTree {} {:p} Tree:{:p}", GetID(), (void*)this, (void*)pSceneTree);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CSceneBase::LinkSceneTree(CSceneBase* pLinkScene)
{
    __ENTER_FUNCTION
    if(m_pSceneTree)
    {
        return false;
    }

    m_pSceneTree = pLinkScene->m_pSceneTree;
    LOGDEBUG("CSceneBase::InitSceneTree {} {:p} Tree:{:p}", GetID(), (void*)this, (void*)m_pSceneTree.get());
    return true;
    __LEAVE_FUNCTION
    return false;
}

uint64_t CSceneBase::GetScriptID() const
{
    return GetMap() ? GetMap()->GetScriptID() : 0;
}

uint16_t CSceneBase::GetMapID() const
{
    return m_pMap->GetMapID();
}

CSceneObject* CSceneBase::QueryPlayer(OBJID idObj) const
{
    __ENTER_FUNCTION
    auto it = m_setPlayer.find(idObj);
    if(it != m_setPlayer.end())
    {
        return it->second;
    }
    __LEAVE_FUNCTION

    return nullptr;
}

CSceneObject* CSceneBase::QuerySceneObj(OBJID idObj) const
{
    __ENTER_FUNCTION
    auto it = m_setActor.find(idObj);
    if(it != m_setActor.end())
    {
        return it->second;
    }
    __LEAVE_FUNCTION

    return nullptr;
}

bool CSceneBase::EnterMap(CSceneObject* pActor, float fPosX, float fPosY, float fFace)
{
    __ENTER_FUNCTION
    if(m_pSceneTree->IsInsideScene(fPosX, fPosY) == false)
    {
        LOGERROR("EnterMap Pos Fail id:{} pos:{},{} scene:{} map:{}", pActor->GetID(), fPosX, fPosY, GetID(), GetMapID());
        LOGTRACE("CallStack: {}", GetStackTraceString(CallFrameMap(2, 7)));
        return false;
    }

    if(pActor->IsPlayer())
        m_setPlayer[pActor->GetID()] = pActor;
    m_setActor[pActor->GetID()] = pActor;
    pActor->_SetPos(Vector2(fPosX, fPosY));
    pActor->SetFace(fFace);

    pActor->OnEnterMap(this);
    pActor->UpdateViewList(true);
    m_pSceneTree->CheckNeedResizeSceneTile(m_setPlayer.size());
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CSceneBase::LeaveMap(CSceneObject* pActor, uint16_t idTargetMap /*= 0*/)
{
    _LeaveMap(pActor, idTargetMap);
}

void CSceneBase::_LeaveMap(CSceneObject* pActor, uint16_t idTargetMap /*= 0*/)
{
    __ENTER_FUNCTION
    if(pActor->IsPlayer())
        m_setPlayer.erase(pActor->GetID());

    m_setActor.erase(pActor->GetID());

    //将玩家从场景树移除
    pActor->ClearViewList(true);
    pActor->OnLeaveMap(idTargetMap);

    m_pSceneTree->CheckNeedResizeSceneTile(m_setPlayer.size());
    __LEAVE_FUNCTION
}

std::optional<Vector2> CSceneBase::FindPosNearby(const Vector2& pos, float range) const
{
    __ENTER_FUNCTION
    CHECK_RET_FMT(m_pSceneTree->IsInsideScene(pos.x, pos.y), {}, "pos:{},{}", pos.x, pos.y);
    return m_pMap->FindPosNearby(pos, range);
    __LEAVE_FUNCTION
    return {};
}

void CSceneBase::AddDynaRegion(uint32_t nRegionType, const FloatRect& rect)
{
    __ENTER_FUNCTION
    m_DynaRegionDataSet[nRegionType].AddDynaRegion(rect);
    __LEAVE_FUNCTION
}

void CSceneBase::ClearDynaRegion(uint32_t nRegionType)
{
    __ENTER_FUNCTION
    m_DynaRegionDataSet[nRegionType].Clear();
    __LEAVE_FUNCTION
}

bool CSceneBase::IsPassDisable(float x, float y, uint32_t actor_type) const
{
    __ENTER_FUNCTION
    if(m_pMap->IsPassDisable(x, y) == true)
        return true;

    auto it = m_DynaRegionDataSet.find(REGION_PASS_DISABLE);
    if(it != m_DynaRegionDataSet.end() && it->second.IsIntersect(x, y) == true)
        return true;

    // Collision layer
    if(m_pMap->HasMapFlag(MAPFLAG_COLLISION_ENABLE) == true)
    {
        if(m_pSceneTree && m_pSceneTree->CollisionTest(x, y, actor_type) == true)
            return true;
    }

    return false;
    __LEAVE_FUNCTION
    return true;
}

bool CSceneBase::IsPvPDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(m_pMap->HasMapFlag(MAPFLAG_DISABLE_PK) == true)
        return true;

    if(m_pMap->IsPvPDisable(x, y) == true)
        return true;

    auto it = m_DynaRegionDataSet.find(REGION_PVP_DISABLE);
    if(it != m_DynaRegionDataSet.end() && it->second.IsIntersect(x, y) == true)
        return true;

    return false;
    __LEAVE_FUNCTION
    return true;
}

bool CSceneBase::IsPvPFree(float x, float y) const
{
    __ENTER_FUNCTION
    if(m_pMap->IsPvPFree(x, y) == true)
        return true;

    auto it = m_DynaRegionDataSet.find(REGION_PVP_FREE);
    if(it != m_DynaRegionDataSet.end() && it->second.IsIntersect(x, y) == true)
        return true;

    return false;
    __LEAVE_FUNCTION
    return true;
}

bool CSceneBase::IsRecordDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(m_pMap->IsRecordDisable(x, y) == true)
        return true;

    auto it = m_DynaRegionDataSet.find(REGION_RECORD_DISABLE);
    if(it != m_DynaRegionDataSet.end() && it->second.IsIntersect(x, y) == true)
        return true;

    return false;
    __LEAVE_FUNCTION
    return true;
}

bool CSceneBase::IsDropDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(m_pMap->IsDropDisable(x, y) == true)
        return true;

    auto it = m_DynaRegionDataSet.find(REGION_DROP_DISABLE);
    if(it != m_DynaRegionDataSet.end() && it->second.IsIntersect(x, y) == true)
        return true;

    // Drop layer
    if(m_pSceneTree && m_pSceneTree->CollisionTest(x, y, ActorType::ACT_MAPITEM) == true)
        return true;

    return false;
    __LEAVE_FUNCTION
    return true;
}

bool CSceneBase::IsDeadNoDrop(float x, float y) const
{
    __ENTER_FUNCTION
    if(m_pMap->IsDeadNoDrop(x, y) == true)
        return true;

    auto it = m_DynaRegionDataSet.find(REGION_DEAD_NO_DROP);
    if(it != m_DynaRegionDataSet.end() && it->second.IsIntersect(x, y) == true)
        return true;

    return false;
    __LEAVE_FUNCTION
    return true;
}

bool CSceneBase::IsStallDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(m_pMap->IsStallDisable(x, y) == true)
        return true;

    auto it = m_DynaRegionDataSet.find(REGION_STALL_DISABLE);
    if(it != m_DynaRegionDataSet.end() && it->second.IsIntersect(x, y) == true)
        return true;

    if(m_pSceneTree && m_pSceneTree->CollisionTest(x, y, ActorType::ACT_NPC) == true)
        return true;

    return false;
    __LEAVE_FUNCTION
    return true;
}

bool CSceneBase::IsPlaceDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(m_pMap->IsPlaceDisable(x, y) == true)
        return true;

    auto it = m_DynaRegionDataSet.find(REGION_PLACE_DISABLE);
    if(it != m_DynaRegionDataSet.end() && it->second.IsIntersect(x, y) == true)
        return true;

    if(m_pSceneTree && m_pSceneTree->CollisionTest(x, y, ActorType::ACT_NPC) == true)
        return true;

    return false;
    __LEAVE_FUNCTION
    return true;
}

uint32_t CSceneBase::GetSPRegionIdx(float x, float y) const
{
    return m_pMap->GetSPRegionIdx(x, y);
}

float CSceneBase::GetHigh(float x, float y) const
{
    return m_pMap->GetHigh(x, y);
}
