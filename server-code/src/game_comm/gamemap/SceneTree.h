#ifndef SCENETREE_H
#define SCENETREE_H

#include "BaseCode.h"

class CSceneObject;

class CSceneTile : public std::unordered_set<CSceneObject*>
{
public:
    CSceneTile() = default;

    void AddActor(CSceneObject* pActor) { this->insert(pActor); }
    void RemoveActor(CSceneObject* pActor) { this->erase(pActor); }

public:
    OBJECTHEAP_DECLARATION(s_heap);
};

class CSceneCollisionTile
{
public:
    CSceneCollisionTile() = default;

    void                                 AddActor(CSceneObject* pActor);
    void                                 RemoveActor(CSceneObject* pActor);
    size_t                               size(uint32_t actor_type) const;
    bool                                 find_if(const std::function<bool(uint32_t, size_t)>& func) const;
    std::unordered_map<uint32_t, size_t> m_setCount;

public:
    OBJECTHEAP_DECLARATION(s_heap);
};

class CGameMap;
class CSceneTree : NoncopyableT<CSceneTree>
{
protected:
    CSceneTree();

public:
    virtual ~CSceneTree();
    CreateNewImpl(CSceneTree);

    bool Init(const CGameMap* pMap, const CPos2D& vBasePos, float fWidth, float fHeight, uint32_t nTileGridRange = 0, bool bDynamicSetLev = false);

    const CGameMap* GetMap() const { return m_pMap; }

    bool IsInsideScene(float x, float y) const;

    bool IsViewManhattanDistance() const { return m_bUseManhattanDistance; }
    void SetViewManhattanDistance(bool val) { m_bUseManhattanDistance = val; }

    float GetViewChangeMin() const { return m_fViewChangeMin; }
    void  SetViewChangeMin(float val) { m_fViewChangeMin = val; }

    uint32_t GetViewRangeIn() const { return m_nViewRangeIn; }
    void     SetViewRangeIn(uint32_t val)
    {
        m_nViewRangeIn       = val;
        m_nViewRangeInSquare = m_nViewRangeIn * m_nViewRangeIn;
    }
    uint32_t GetViewRangeInSquare() const { return m_nViewRangeInSquare; }

    uint32_t GetViewRangeOut() const { return m_nViewRangeOut; }
    void     SetViewRangeOut(uint32_t val)
    {
        m_nViewRangeOut       = val;
        m_nViewRangeOutSquare = m_nViewRangeOut * m_nViewRangeOut;
    }
    uint32_t GetViewRangeOutSquare() const { return m_nViewRangeOutSquare; }

    uint32_t GetViewCount() const { return m_nViewCount; }
    void     SetViewCount(uint32_t val) { m_nViewCount = val; }

    uint32_t GetCurTileDynamicLevel() const { return m_nCurTileDynamicLevel; }
    void     SetCurTileDynamicLevel(uint32_t val) { m_nCurTileDynamicLevel = val; }

    // AOI相关的
    CSceneTile* GetSceneTileByPos(float x, float y);
    uint32_t    GetSceneTileIndexByPos(float x, float y) const;
    // 遍历自己视野内的SceneTile
    bool foreach_SceneTileInSight(float x, float y, std::function<void(CSceneTile*)>&& func);

    void       CheckNeedResizeSceneTile(uint32_t nPlayerCount);
    void       SetDynamicAdjustTileLevel(bool v) { m_bDynamicAdjustTileLevel = v; }
    export_lua CSceneCollisionTile* GetCollisionTileByPos(float x, float y, uint32_t actor_type);
    export_lua bool                 CollisionTest(float x, float y, uint32_t actor_type) const;

private:
    void _SetSceneTileGridRange(uint32_t v);
    void SetTileDynamicLev(uint32_t new_level);

public:
    OBJECTHEAP_DECLARATION(s_heap);

protected:
    std::deque<CSceneTile>          m_setTile;
    std::deque<CSceneCollisionTile> m_setCollision;

    const CGameMap* m_pMap = nullptr;
    //左上角坐标
    CPos2D   m_BasePos;
    float    m_fWidth                = 0.0f;
    float    m_fHeight               = 0.0f;
    uint32_t m_nTileDefaultGridRange = 0; //一个Tile默认的XY
    uint32_t m_nTileGridRange        = 0; //一个Tile当前的XY
    uint32_t m_nTileWidth            = 0; //场景x轴多少个Tile
    uint32_t m_nTileHeight           = 0; //场景y轴多少个Tile

    bool     m_bUseManhattanDistance = false; //是否使用麦哈顿距离判断
    float    m_fViewChangeMin        = 0.0f;  // 视野变动所需位置变化最小距离
    uint32_t m_nViewRangeIn          = 0;     // view in
    uint32_t m_nViewRangeInSquare    = 0;

    uint32_t m_nViewRangeOut       = 0; // view out
    uint32_t m_nViewRangeOutSquare = 0;
    uint32_t m_nViewCount          = 0; // view count
    uint32_t m_nViewCountDefault   = 0;

    uint32_t m_nCurTileDynamicLevel    = 0; //视野动态扩缩等级
    bool     m_bDynamicAdjustTileLevel = false;
};
#endif /* SCENETREE_H */
