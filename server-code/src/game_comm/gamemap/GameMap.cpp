#include "GameMap.h"

#include "GameMapDef.h"
#include "MapData.h"
#include "config/Cfg_Phase.pb.h"
#include "config/Cfg_Scene.pb.h"
#include "config/Cfg_Scene_EnterPoint.pb.h"
#include "config/Cfg_Scene_LeavePoint.pb.h"
#include "config/Cfg_Scene_MonsterGenerator.pb.h"
#include "config/Cfg_Scene_Patrol.pb.h"
#include "config/Cfg_Scene_Reborn.pb.h"
CGameMap::CGameMap() {}

CGameMap::~CGameMap() {}

bool CGameMap::Init(CMapManager* pManager, const Cfg_Scene& data, const CMapData* pMapData)
{
    CHECKF(pManager);
    m_pManager      = pManager;
    m_idMap         = data.idmap();
    m_MapName       = data.name();
    m_idZone        = data.idzone();
    m_idMapTemplate = data.idmapdata();
    m_nMapType      = data.maptype();
    m_nMapFlag      = data.mapflag();
    m_idScript      = data.idscript();
    m_pMapData      = pMapData;
    return true;
}

bool CGameMap::IsInsideMap(float x, float y) const
{
    __ENTER_FUNCTION
    if(m_pMapData == nullptr)
        return false;

    return x >= 0 && x < m_pMapData->GetWidthMap() && y >= 0 && y < m_pMapData->GetHeightMap();

    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsNearLeavePoint(float x, float y, uint32_t& destMapID, uint32_t& destEnterPointIdx) const
{
    __ENTER_FUNCTION
    for(const auto& leave_point_pair: m_LeavePointSet)
    {
        const auto& pLeavePoint = leave_point_pair.second;
        if(GameMath::manhattanDistance(Vector2(x, y), Vector2(pLeavePoint->x(), pLeavePoint->y())) > pLeavePoint->range())
            continue;

        if(GameMath::distance(Vector2(x, y), Vector2(pLeavePoint->x(), pLeavePoint->y())) > pLeavePoint->range())
            continue;

        destMapID         = pLeavePoint->dest_map_id();
        destEnterPointIdx = pLeavePoint->dest_enter_point_idx();
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsNearLeavePointX(uint32_t nLeavePointIdx, float x, float y, uint32_t& destMapID, uint32_t& destEnterPointIdx) const
{
    __ENTER_FUNCTION
    auto pLeavePoint = GetLeavePointByIdx(nLeavePointIdx);
    CHECKF(pLeavePoint);

    if(GameMath::manhattanDistance(Vector2(x, y), Vector2(pLeavePoint->x(), pLeavePoint->y())) > pLeavePoint->range())
        return false;

    if(GameMath::distance(Vector2(x, y), Vector2(pLeavePoint->x(), pLeavePoint->y())) > pLeavePoint->range())
        return false;

    destMapID         = pLeavePoint->dest_map_id();
    destEnterPointIdx = pLeavePoint->dest_enter_point_idx();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsDynaMap() const
{
    return HasFlag(GetMapFlag(), MAPFLAG_DYNAMAP);
}

bool CGameMap::IsPassDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(m_pMapData == nullptr)
        return false;
    if(!IsInsideMap(x, y))
        return false;

    return m_pMapData->IsPassDisable(x, y);
    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsJumpDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(m_pMapData == nullptr)
        return false;
    if(!IsInsideMap(x, y))
        return false;

    return m_pMapData->IsJumpDisable(x, y);
    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsPvPDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(!IsInsideMap(x, y))
        return true;

    return m_pMapData->IsPVPDisable(x, y);
    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsStallDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(!IsInsideMap(x, y))
        return true;

    return m_pMapData->IsStallDisable(x, y);
    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsPlaceDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(!IsInsideMap(x, y))
        return true;

    return m_pMapData->IsPlaceDisable(x, y);
    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsRecordDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(!IsInsideMap(x, y))
        return true;

    return m_pMapData->IsRecordDisable(x, y);
    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsDropDisable(float x, float y) const
{
    __ENTER_FUNCTION
    if(!IsInsideMap(x, y))
        return true;

    return m_pMapData->IsDropDisable(x, y);
    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsPvPFree(float x, float y) const
{
    __ENTER_FUNCTION
    if(!IsInsideMap(x, y))
        return true;

    return m_pMapData->IsPvPFree(x, y);
    __LEAVE_FUNCTION
    return false;
}

bool CGameMap::IsDeadNoDrop(float x, float y) const
{
    __ENTER_FUNCTION
    if(!IsInsideMap(x, y))
        return true;

    return m_pMapData->IsDeadNoDrop(x, y);
    __LEAVE_FUNCTION
    return false;
}

const Cfg_Phase* CGameMap::GetPhaseDataById(uint64_t idPhase) const
{
    __ENTER_FUNCTION
    auto it = m_PhaseDataSet.find(idPhase);
    if(it == m_PhaseDataSet.end())
        return nullptr;
    return it->second.get();
    __LEAVE_FUNCTION
    return nullptr;
}

const Cfg_Scene_EnterPoint* CGameMap::GetEnterPointByIdx(uint32_t idx) const
{
    __ENTER_FUNCTION
    auto it = m_EnterPointSet.find(idx);
    if(it == m_EnterPointSet.end())
        return nullptr;
    return it->second.get();
    __LEAVE_FUNCTION
    return nullptr;
}

const Cfg_Scene_LeavePoint* CGameMap::GetLeavePointByIdx(uint32_t idx) const
{
    __ENTER_FUNCTION
    auto it = m_LeavePointSet.find(idx);
    if(it == m_LeavePointSet.end())
        return nullptr;
    return it->second.get();
    __LEAVE_FUNCTION
    return nullptr;
}

const Cfg_Scene_Reborn* CGameMap::GetRebornDataByIdx(uint32_t idx) const
{
    auto it = m_RebornDataSet.find(idx);
    if(it == m_RebornDataSet.end())
        return nullptr;
    else
        return it->second.get();
}

const Cfg_Scene_Patrol* CGameMap::GetPatrolDataByIdx(uint32_t idx) const
{
    auto it = m_PatrolSet.find(idx);
    if(it == m_PatrolSet.end())
        return nullptr;
    else
        return it->second.get();
}

uint32_t CGameMap::GetSPRegionIdx(float x, float y) const
{
    __ENTER_FUNCTION
    if(!IsInsideMap(x, y))
        return -1;

    return m_pMapData->GetSPRegionIdx(x, y);
    __LEAVE_FUNCTION
    return -1;
}

float CGameMap::GetHigh(float x, float y) const
{
    __ENTER_FUNCTION
    if(!IsInsideMap(x, y))
        return -1;

    return m_pMapData->GetHigh(x, y);
    __LEAVE_FUNCTION
    return -1.0f;
}

Vector2 CGameMap::LineFindCanStand(const Vector2& src, const Vector2& dest) const
{
    __ENTER_FUNCTION
    if(m_pMapData == nullptr)
        return dest;
    return m_pMapData->LineFindCanStand(src, dest);
    __LEAVE_FUNCTION
    return dest;
}

Vector2 CGameMap::LineFindCanJump(const Vector2& src, const Vector2& dest) const
{
    __ENTER_FUNCTION
    if(m_pMapData == nullptr)
        return dest;
    return m_pMapData->LineFindCanJump(src, dest);
    __LEAVE_FUNCTION
    return dest;
}

Vector2 CGameMap::FindPosNearby(const Vector2& pos, float range) const
{
    __ENTER_FUNCTION
    if(range <= 0.0f)
        return pos;

    if(m_pMapData == nullptr)
        return pos;

    for(int32_t i = 0; i < 10; i++)
    {
        CPos2D newPos = pos + CPos2D::UNIT_X().randomDeviant(1.0f) * random_float(0.0f, range);

        if(IsPassDisable(newPos.x, newPos.y) == false)
        {
            return newPos;
        }
    }
    __LEAVE_FUNCTION
    return pos;
}

void CGameMap::_AddData(const Cfg_Scene_EnterPoint& iter)
{
    m_EnterPointSet[iter.idx()] = std::make_unique<Cfg_Scene_EnterPoint>(iter);
}

void CGameMap::_AddData(const Cfg_Scene_LeavePoint& iter)
{
    m_LeavePointSet[iter.idx()] = std::make_unique<Cfg_Scene_LeavePoint>(iter);
}

void CGameMap::_AddData(const Cfg_Scene_Reborn& iter)
{
    m_RebornDataSet[iter.idx()] = std::make_unique<Cfg_Scene_Reborn>(iter);
}

void CGameMap::_AddData(const Cfg_Scene_MonsterGenerator& iter)
{
    m_MonsterGeneratorList[iter.idx()] = std::make_unique<Cfg_Scene_MonsterGenerator>(iter);
}

void CGameMap::_AddData(const Cfg_Scene_Patrol& iter)
{
    m_PatrolSet[iter.patrol_idx()] = std::make_unique<Cfg_Scene_Patrol>(iter);
}

void CGameMap::_AddData(const Cfg_Phase& iter)
{
    m_PhaseDataSet[iter.idphase()] = std::make_unique<Cfg_Phase>(iter);
}