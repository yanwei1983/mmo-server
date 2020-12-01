#ifndef MAPDATA_H
#define MAPDATA_H

#include "BaseCode.h"

export_lua enum MAP_REGION_TYPE {
    REGION_PASS_DISABLE = 0x0001, //阻挡
    REGION_JUMP_DISABLE = 0x0002, //不允许跳跃

    REGION_PVP_DISABLE   = 0x0010, //禁止pk
    REGION_STALL_DISABLE = 0x0020, //不允许摆摊
    REGION_PLACE_DISABLE = 0x0040, //不允许防止物品

    REGION_RECORD_DISABLE = 0x0100, //不允许记录下线坐标
    REGION_DROP_DISABLE   = 0x0200, //不允许丢弃物品到地面
    REGION_PVP_FREE       = 0x0400, // pk不增加PK值，不会记录仇人
    REGION_DEAD_NO_DROP   = 0x0800, //死亡不掉落物品
};

export_lua struct MapGridData
{
    export_lua MapGridData(uint32_t nData = 0)
        : data(nData)
    {
    }

    union {
        struct
        {
            uint32_t bPassDisable : 1; //是否可行走
            uint32_t bJumpDisable : 1; //是否可跳跃
            uint32_t reserver2 : 1;
            uint32_t reserver3 : 1;

            uint32_t bPvPDisable : 1;   //是否允许PK
            uint32_t bStallDisable : 1; //是否允许摆摊
            uint32_t bPlaceDisable : 1; //是否允许放置地面物品
            uint32_t reserver1 : 1;

            uint32_t bRecordDisable : 1; //是否禁止记录下线点
            uint32_t bDropDisable : 1;   //不允许丢弃物品到地面
            uint32_t bPvPFree : 1;       //是否pk不增加PK值
            uint32_t bDeadNoDrop : 1;    //死亡是否无掉落

            uint32_t nSPRegionIdx : 4; // 0~16  特殊区域编号
            uint32_t nHigh : 8;        // 0~255 地面高度
        };
        uint32_t data;
    };
};

export_lua class CMapData : public NoncopyableT<CMapData>
{
    CMapData();
    bool Init(uint32_t idMapTemplate);

public:
    CreateNewImpl(CMapData);

    ~CMapData();

    export_lua uint32_t GetMapTemplateID() const { return m_idMapTemplate; }

    export_lua float    GetWidthMap() const { return m_fWidthMap; }
    export_lua float    GetHeightMap() const { return m_fHeightMap; }
    export_lua uint32_t GetWidth() const { return m_nWidth; }
    export_lua uint32_t GetHeight() const { return m_nHeight; }
    export_lua float    GirdWidth() const { return m_fGirdWidth; }
    export_lua float    GirdHeight() const { return m_fGirdHeight; }
    export_lua uint32_t GetGirdCount() const { return m_nWidth * m_nHeight; }

    export_lua Vector2 LineFindCanStand(const Vector2& src, const Vector2& dest) const;
    export_lua Vector2 LineFindCanJump(const Vector2& src, const Vector2& dest) const;
    export_lua bool    IsPassDisable(float x, float y) const { return _getGridData(x, y).bPassDisable == TRUE; }
    export_lua bool    IsJumpDisable(float x, float y) const { return _getGridData(x, y).bJumpDisable == TRUE; }
    export_lua bool    IsPVPDisable(float x, float y) const { return _getGridData(x, y).bPvPDisable == TRUE; }
    export_lua bool    IsStallDisable(float x, float y) const { return _getGridData(x, y).bStallDisable == TRUE; }
    export_lua bool    IsPlaceDisable(float x, float y) const { return _getGridData(x, y).bPlaceDisable == TRUE; }
    export_lua bool    IsRecordDisable(float x, float y) const { return _getGridData(x, y).bRecordDisable == TRUE; }
    export_lua bool    IsDropDisable(float x, float y) const { return _getGridData(x, y).bDropDisable == TRUE; }
    export_lua bool    IsPvPFree(float x, float y) const { return _getGridData(x, y).bPvPFree == TRUE; }
    export_lua bool    IsDeadNoDrop(float x, float y) const { return _getGridData(x, y).bDeadNoDrop == TRUE; }
    export_lua bool    CanCollision(uint32_t self_type, uint32_t other_type) const { return HasFlag(m_CollisionMask[self_type], (1 << other_type)); }

    export_lua uint32_t GetSPRegionIdx(float x, float y) const { return _getGridData(x, y).nSPRegionIdx; }
    export_lua float    GetHigh(float x, float y) const { return (float(_getGridData(x, y).nHigh) / 255.0f) * m_fGridHighFactor; }

    export_lua const MapGridData& _getGridData(float x, float y) const;
    export_lua std::tuple<uint32_t, uint32_t> Pos2Grid(float x, float y) const;
    export_lua uint32_t                       Pos2Idx(float x, float y) const;

private:
    uint32_t m_idMapTemplate;
    //具体地图数据
    float m_fWidthMap;  //地图宽
    float m_fHeightMap; //地图高

    uint32_t m_nWidth;  //地图宽格子数
    uint32_t m_nHeight; //地图高格子数

    const float m_fGirdWidth  = 0.25f;
    const float m_fGirdHeight = 0.25f;

    float m_fGridHighFactor; //地形高度缩放系数， 实际高度=地形高度值/255*地形高度缩放系数

    //地图信息
    std::unique_ptr<MapGridData[]> m_pGridData;
    std::array<uint32_t,32>        m_CollisionMask;

    //
};
#endif /* MAPDATA_H */
