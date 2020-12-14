#include "SceneTree.h"

#include "GameMap.h"
#include "GameMapDef.h"
#include "MapData.h"
#include "SceneObject.h"

OBJECTHEAP_IMPLEMENTATION(CSceneTree, s_heap);
OBJECTHEAP_IMPLEMENTATION(CSceneTile, s_heap);
OBJECTHEAP_IMPLEMENTATION(CSceneCollisionTile, s_heap);

constexpr int32_t CONST_SCENE_TILE_GRIDRANGE = 30;

CSceneTree::CSceneTree() {}

CSceneTree::~CSceneTree()
{
    m_setTile.clear();
    m_setCollision.clear();
    m_pMap = nullptr;
}

bool CSceneTree::Init(const CGameMap* pMap, const CPos2D& vBasePos, float fWidth, float fHeight, uint32_t nTileGridRange, bool bDynamicSetLev)
{
    CHECKF(pMap);
    m_pMap                    = pMap;
    m_BasePos                 = vBasePos;
    m_fWidth                  = fWidth;
    m_fHeight                 = fHeight;
    m_bDynamicAdjustTileLevel = bDynamicSetLev;
    auto pMapData             = m_pMap->GetMapData();
    CHECKF(pMapData);
    if(m_fWidth <= 0.0f || m_fWidth >= pMapData->GetWidthMap() - m_BasePos.x)
        m_fWidth = pMapData->GetWidthMap() - m_BasePos.x;
    if(m_fHeight <= 0.0f || m_fHeight >= pMapData->GetHeightMap() - m_BasePos.y)
        m_fHeight = pMapData->GetHeightMap() - m_BasePos.y;

    m_nTileDefaultGridRange = nTileGridRange;
    if(m_nTileDefaultGridRange == 0)
        m_nTileDefaultGridRange = CONST_SCENE_TILE_GRIDRANGE;

    SetTileDynamicLev(m_nCurTileDynamicLevel);
    if(m_pMap->HasMapFlag(MAPFLAG_COLLISION_ENABLE) == true)
    {
        m_setCollision.resize(pMapData->GetGirdCount());
    }

    LOGDEBUG("CSceneTree::Init {:p} MapData:{},width:{},height{},BasePos:{},{} GridRange:{}", 
            (void*)this,
            pMapData->GetMapTemplateID(), m_fWidth, m_fHeight, m_BasePos.x, m_BasePos.y, m_nTileDefaultGridRange);
    return true;
}

bool CSceneTree::IsInsideScene(float x, float y) const
{
    return (x >= m_BasePos.x && x <= m_BasePos.x + m_fWidth && y >= m_BasePos.x && x <= m_BasePos.y + m_fHeight);
}

CSceneTile* CSceneTree::GetSceneTileByPos(float x, float y)
{
    uint32_t tower_idx = GetSceneTileIndexByPos(x, y);
    if(tower_idx >= m_setTile.size())
        return nullptr;
    return &m_setTile[tower_idx];
}

uint32_t CSceneTree::GetSceneTileIndexByPos(float x, float y) const
{
    float dx = x - m_BasePos.x;
    float dy = y - m_BasePos.y;

    uint32_t cellx = (dx < 0.0f) ? 0 : std::ceil(dx / float(m_nTileGridRange));
    uint32_t celly = (dy < 0.0f) ? 0 : std::ceil(dy / float(m_nTileGridRange));
    if(cellx >= m_nTileWidth)
        cellx = m_nTileWidth - 1;
    if(celly >= m_nTileHeight)
        celly = m_nTileHeight - 1;

    return cellx + celly * m_nTileWidth;
}

bool CSceneTree::foreach_SceneTileInSight(float x, float y, const std::function<void(CSceneTile*)>& func)
{
    float dx = x - m_BasePos.x;
    float dy = y - m_BasePos.y;

    uint32_t cellx = (dx < 0.0f) ? 0 : std::ceil(dx / float(m_nTileGridRange));
    uint32_t celly = (dy < 0.0f) ? 0 : std::ceil(dy / float(m_nTileGridRange));
    if(cellx >= m_nTileWidth)
        cellx = m_nTileWidth - 1;
    if(celly >= m_nTileHeight)
        celly = m_nTileHeight - 1;

    uint32_t xx_min = __max(0, cellx - 1);
    uint32_t xx_max = __min(m_nTileWidth, cellx + 1);

    uint32_t yy_min = __max(0, celly - 1);
    uint32_t yy_max = __min(m_nTileHeight, celly + 1);

    for(uint32_t xx = xx_min; xx < xx_max; xx++)
    {
        for(uint32_t yy = yy_min; yy < yy_max; yy++)
        {
            CSceneTile* pTile = &m_setTile[xx + yy * m_nTileWidth];
            func(pTile);
        }
    }

    return true;
}

struct TOWER_RESIZE_WATER_MARK
{
    uint32_t low_water_mark;
    uint32_t hight_water_mark;
    uint32_t tower_gridrange;
    uint32_t view_count;
    float    view_change_min;
    bool     use_manhattan;
    bool     view_in;
    bool     view_out;
};
TOWER_RESIZE_WATER_MARK TOWER_RESIZE_WATER_MARK_ARRAY[] = {
    {0, 300, CONST_SCENE_TILE_GRIDRANGE, 0, 0.0f, false, true, true},
    {200, 600, 25, 0, 0.2f, false, true, true},
    {400, 1000, 20, 0, 0.4f, false, true, true},
    {700, 1500, 18, 80, 0.6f, false, true, false},
    {1100, 1900, 15, 80, 0.8f, false, true, false},
    {1500, 2200, 12, 80, 1.0f, false, true, false},
    {1800, 2500, 10, 40, 1.5f, false, true, false},
    {2100, 30000, 8, 40, 2.0f, true, false, false},
};

void CSceneTree::SetTileDynamicLev(uint32_t new_level)
{
    m_nCurTileDynamicLevel = new_level;
    if(m_nCurTileDynamicLevel == 0)
    {
        _SetSceneTileGridRange(m_nTileDefaultGridRange);
    }
    else
    {
        _SetSceneTileGridRange(TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurTileDynamicLevel].tower_gridrange);
    }

    SetViewCount(TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurTileDynamicLevel].view_count);
    SetViewChangeMin(TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurTileDynamicLevel].view_change_min);
    SetViewManhattanDistance(TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurTileDynamicLevel].use_manhattan);
    if(TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurTileDynamicLevel].view_out == false)
    {
        SetViewRangeOut(0);
    }
    if(TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurTileDynamicLevel].view_in == false)
    {
        SetViewRangeIn(0);
    }
}
void CSceneTree::CheckNeedResizeSceneTile(uint32_t nPlayerCount)
{
    if(m_bDynamicAdjustTileLevel == false)
        return;

    if(nPlayerCount < TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurTileDynamicLevel].low_water_mark)
    {
        if(m_nCurTileDynamicLevel == 0)
            return;
        // dec_tower_size
        auto new_level = m_nCurTileDynamicLevel - 1;
        SetTileDynamicLev(new_level);
    }
    else if(nPlayerCount > TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurTileDynamicLevel].hight_water_mark)
    {
        // inc_tower_size
        if(m_nCurTileDynamicLevel >= sizeOfArray(TOWER_RESIZE_WATER_MARK_ARRAY))
            return;
        auto new_level = m_nCurTileDynamicLevel + 1;
        SetTileDynamicLev(new_level);
    }
}

void CSceneTree::_SetSceneTileGridRange(uint32_t val)
{
    if(val == 0)
    {
        return;
    }

    //先将所有的角色保存好,并将它们从Tower中移除
    std::deque<CSceneObject*> actors;
    for(uint32_t i = 0; i < m_setTile.size(); ++i)
    {
        const std::unordered_set<CSceneObject*>& this_actors = m_setTile[i];
        for(CSceneObject* actor: this_actors)
        {
            if(actor != nullptr)
            {
                actor->SetSceneTile(nullptr);
                actors.push_back(actor);
            }
        }
    }

    //删除所有的Tower
    m_setTile.clear();

    //重新分配
    m_nTileGridRange      = val;
    m_nViewRangeIn        = m_nTileGridRange;
    m_nViewRangeOut       = m_nViewRangeIn + m_nViewRangeIn / 4;
    m_nViewRangeInSquare  = m_nViewRangeIn * m_nViewRangeIn;
    m_nViewRangeOutSquare = m_nViewRangeOut * m_nViewRangeOut;

    m_nTileWidth  = (m_fWidth + m_nTileGridRange - 1) / m_nTileGridRange;
    m_nTileHeight = (m_fHeight + m_nTileGridRange - 1) / m_nTileGridRange;
    auto pMapData = m_pMap->GetMapData();
    if(m_nTileWidth <= 0 || m_nTileHeight <= 0)
    {
        LOGERROR("map size error!!!!! width = {}, height = {}", pMapData->GetWidth(), pMapData->GetHeight());
        return;
    }
    
    LOGDEBUG("map:{} resize  ViewIn:{} ViewOut:{} Tiles:{}", pMapData->GetMapTemplateID(),  m_nViewRangeIn, m_nViewRangeOut, m_nTileWidth * m_nTileHeight);

    m_setTile.resize(m_nTileWidth * m_nTileHeight);

    for(CSceneObject* actor: actors)
    {
        actor->SetSceneTile(GetSceneTileByPos(actor->GetPosX(), actor->GetPosY()));
    }
}

CSceneCollisionTile* CSceneTree::GetCollisionTileByPos(float x, float y, uint32_t actor_type)
{
    if(m_pMap->HasMapFlag(MAPFLAG_COLLISION_ENABLE) == false)
        return nullptr;
    uint32_t nIdx = m_pMap->GetMapData()->Pos2Idx(x, y);
    CHECKF_V(nIdx < m_setCollision.size(), nIdx);

    return &(m_setCollision[nIdx]);
}

bool CSceneTree::CollisionTest(float x, float y, uint32_t actor_type) const
{
    if(m_pMap->HasMapFlag(MAPFLAG_COLLISION_ENABLE) == false)
        return false;
    uint32_t nIdx = m_pMap->GetMapData()->Pos2Idx(x, y);
    CHECKF_V(nIdx < m_setCollision.size(), nIdx);

    const auto& refData = m_setCollision[nIdx];
    return refData.find_if([this, actor_type](uint32_t k, size_t v) {
        if(m_pMap->GetMapData()->CanCollision(actor_type, k) == true)
        {
            if(v > 0)
                return true;
        }
        return false;
    });
}

void CSceneCollisionTile::AddActor(CSceneObject* pActor)
{
    m_setCount[pActor->GetActorType()]++;
}

void CSceneCollisionTile::RemoveActor(CSceneObject* pActor)
{
    m_setCount[pActor->GetActorType()]--;
}

size_t CSceneCollisionTile::size(uint32_t actor_type) const
{
    auto it = m_setCount.find(actor_type);
    if(it == m_setCount.end())
        return 0;
    else
        return it->second;
}

bool CSceneCollisionTile::find_if(const std::function<bool(uint32_t, size_t)>& func) const
{
    for(const auto& [k, v]: m_setCount)
    {
        if(func(k, v) == true)
            return true;
    }
    return false;
}