#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "BaseCode.h"

class Cfg_Phase;
class Cfg_Scene;
class Cfg_Scene_EnterPoint;
class Cfg_Scene_LeavePoint;
class Cfg_Scene_MonsterGenerator;
class Cfg_Scene_Patrol;
class Cfg_Scene_Reborn;

class CMapManager;
class CMapData;

export_lua class CGameMap : public NoncopyableT<CGameMap>
{
    CGameMap();
    bool Init(CMapManager* pManager, const Cfg_Scene& data, const CMapData* pMapData);

public:
    ~CGameMap();

    CreateNewImpl(CGameMap);

    export_lua bool     IsInsideMap(float x, float y) const;
    export_lua bool     IsZoneMap(uint16_t idZone) const { return m_idZone == 0 || idZone == m_idZone; }
    export_lua uint16_t GetZoneID() const { return m_idZone; }
    export_lua uint16_t GetMapID() const { return m_idMap; }
    export_lua uint16_t GetMapTemplateID() const { return m_idMapTemplate; }
    export_lua uint32_t GetMapType() const { return m_nMapType; }
    export_lua uint32_t GetMapFlag() const { return m_nMapFlag; }
    export_lua uint64_t GetScriptID() const { return m_idScript; }
    export_lua bool     HasMapFlag(uint32_t flag) const { return ::HasFlag(GetMapFlag(), flag); }
    export_lua bool     IsDynaMap() const;

    export_lua bool IsNearLeavePoint(float x, float y, uint32_t& destMapID, uint32_t& destEnterPointIdx) const;
    export_lua bool IsNearLeavePointX(uint32_t nLeavePointIdx, float x, float y, uint32_t& destMapID, uint32_t& destEnterPointIdx) const;

    export_lua bool IsPassDisable(float x, float y) const;
    export_lua bool IsJumpDisable(float x, float y) const;
    export_lua bool IsPvPDisable(float x, float y) const;
    export_lua bool IsStallDisable(float x, float y) const;
    export_lua bool IsPlaceDisable(float x, float y) const;
    export_lua bool IsRecordDisable(float x, float y) const;
    export_lua bool IsDropDisable(float x, float y) const;
    export_lua bool IsPvPFree(float x, float y) const;
    export_lua bool IsDeadNoDrop(float x, float y) const;

    export_lua uint32_t GetSPRegionIdx(float x, float y) const;
    export_lua float    GetHigh(float x, float y) const;

    export_lua Vector2 FindPosNearby(const Vector2& pos, float range) const;
    export_lua std::optional<Vector2> LineFindCanStand(const Vector2& src, const Vector2& dest) const;
    export_lua std::optional<Vector2> LineFindCanJump(const Vector2& src, const Vector2& dest) const;

public:
    export_lua const CMapData* GetMapData() const { return m_pMapData; }
    export_lua const auto&     GetPhaseData() const { return m_PhaseDataSet; }
    export_lua const auto&     GetRebornData() const { return m_RebornDataSet; }
    export_lua const auto&     GetGeneratorData() const { return m_MonsterGeneratorList; }
    export_lua const auto&     GetPatrolData() const { return m_PatrolSet; }

    export_lua const Cfg_Phase* GetPhaseDataById(uint64_t idPhase) const;
    export_lua const Cfg_Scene_EnterPoint* GetEnterPointByIdx(uint32_t idx) const;
    export_lua const Cfg_Scene_LeavePoint* GetLeavePointByIdx(uint32_t idx) const;
    export_lua const Cfg_Scene_Reborn* GetRebornDataByIdx(uint32_t idx) const;
    export_lua const Cfg_Scene_Patrol* GetPatrolDataByIdx(uint32_t idx) const;

    void _AddData(const Cfg_Scene_EnterPoint& iter);
    void _AddData(const Cfg_Scene_LeavePoint& iter);
    void _AddData(const Cfg_Scene_Reborn& iter);
    void _AddData(const Cfg_Scene_MonsterGenerator& iter);
    void _AddData(const Cfg_Scene_Patrol& iter);
    void _AddData(const Cfg_Phase& iter);

private:
    CMapManager* m_pManager = nullptr;
    uint16_t     m_idMap    = 0;
    std::string  m_MapName;
    uint16_t     m_idZone        = 0;
    uint16_t     m_idMapTemplate = 0;
    uint32_t     m_nMapType      = 0;
    uint32_t     m_nMapFlag      = 0;
    uint64_t     m_idScript      = 0;

    std::unordered_map<uint16_t, std::unique_ptr<Cfg_Phase>>                  m_PhaseDataSet;
    std::unordered_map<uint16_t, std::unique_ptr<Cfg_Scene_EnterPoint>>       m_EnterPointSet;
    std::unordered_map<uint16_t, std::unique_ptr<Cfg_Scene_LeavePoint>>       m_LeavePointSet;
    std::unordered_map<uint16_t, std::unique_ptr<Cfg_Scene_MonsterGenerator>> m_MonsterGeneratorList;
    std::unordered_map<uint16_t, std::unique_ptr<Cfg_Scene_Patrol>>           m_PatrolSet;
    std::unordered_map<uint16_t, std::unique_ptr<Cfg_Scene_Reborn>>           m_RebornDataSet;

    const CMapData* m_pMapData = nullptr;
};
#endif /* GAMEMAP_H */
