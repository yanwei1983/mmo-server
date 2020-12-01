#include "MapData.h"

#include <fmt/format.h>

#include "BaseCode.h"
#include "DataPack.h"
#include "ProtobuffUtil.h"
#include "config/Cfg_MapData.pb.h"
CMapData::CMapData() {}

CMapData::~CMapData() {}

bool CMapData::Init(uint32_t idMapTemplate)
{
    __ENTER_FUNCTION
    std::string file_path = "res/map/map_" + std::to_string(idMapTemplate) + ".scene";
    Cfg_MapData cfg;
    if(pb_util::LoadFromBinaryFile(file_path, cfg) == false)
    {
        // try us json format
        if(pb_util::LoadFromJsonFile(file_path + ".json", cfg) == false)
        {
            LOGERROR("InitFromFile {} Fail.", file_path.c_str());
            return false;
        }
    }

    m_idMapTemplate = idMapTemplate;
    m_nWidth        = cfg.width();
    m_nHeight       = cfg.height();
    CHECKF_M(m_nWidth > 0, std::to_string(idMapTemplate).c_str());
    CHECKF_M(m_nHeight > 0, std::to_string(idMapTemplate).c_str());

    m_fWidthMap  = m_fGirdWidth * m_nWidth;
    m_fHeightMap = m_fGirdHeight * m_nHeight;

    // CHECKF_M((size_t)cfg.griddata().size() == m_nWidth * m_nHeight, std::to_string(idMapTemplate).c_str());

    m_pGridData = std::make_unique<MapGridData[]>(m_nWidth * m_nHeight);
    memset(m_pGridData.get(), 0, m_nWidth * m_nHeight * sizeof(MapGridData));
    for(size_t i = 0; i < m_nWidth * m_nHeight && i < (size_t)cfg.griddata().size(); i++)
    {
        m_pGridData[i] = cfg.griddata().Get(i);
    }
    for(size_t i = 0; i < m_CollisionMask.size() && i < cfg.collision().size(); i++)
    {
        m_CollisionMask[i] = cfg.collision().Get(i);
    }
    LOGDEBUG("MapData {} LoadSucc.", file_path.c_str());

    return true;
    __LEAVE_FUNCTION
    return false;
}

const MapGridData& CMapData::_getGridData(float x, float y) const
{
    __ENTER_FUNCTION
    return m_pGridData[Pos2Idx(x, y)];
    __LEAVE_FUNCTION
    static MapGridData s_Empty;
    return s_Empty;
}

uint32_t CMapData::Pos2Idx(float x, float y) const
{
    __ENTER_FUNCTION
    if(x < 0.0f)
        x = 0.0f;
    if(y < 0.0f)
        y = 0.0f;

    uint32_t xWidth  = x / m_fGirdWidth;
    uint32_t yHeight = y / m_fGirdHeight;
    if(xWidth < 0)
        xWidth = 0;
    if(xWidth >= m_nWidth)
        xWidth = m_nWidth - 1;
    if(yHeight < 0)
        yHeight = 0;
    if(yHeight >= m_nHeight)
        yHeight = m_nHeight - 1;

    return xWidth + yHeight * m_nWidth;
    __LEAVE_FUNCTION
    return 0;
}

std::tuple<uint32_t, uint32_t> CMapData::Pos2Grid(float x, float y) const
{
    __ENTER_FUNCTION
    if(x < 0.0f)
        x = 0.0f;
    if(y < 0.0f)
        y = 0.0f;

    uint32_t xWidth  = x / m_fGirdWidth;
    uint32_t yHeight = y / m_fGirdHeight;
    if(xWidth < 0)
        xWidth = 0;
    if(xWidth >= m_nWidth)
        xWidth = m_nWidth - 1;
    if(yHeight < 0)
        yHeight = 0;
    if(yHeight >= m_nHeight)
        yHeight = m_nHeight - 1;

    return std::make_tuple(xWidth, yHeight);
    __LEAVE_FUNCTION
    return std::make_tuple(0, 0);
}

Vector2 CMapData::LineFindCanStand(const Vector2& src, const Vector2& dest) const
{
    uint32_t xWidth      = 0;
    uint32_t yHeight     = 0;
    uint32_t xDestWidth  = 0;
    uint32_t yDestHeight = 0;

    std::tie(xWidth, yHeight)         = Pos2Grid(src.x, src.y);
    std::tie(xDestWidth, yDestHeight) = Pos2Grid(dest.x, dest.y);
    if(xWidth == xDestWidth && yHeight == yDestHeight)
        return dest;

    Vector2 result = src;
    int32_t last_x = xWidth;
    int32_t last_y = yHeight;
    GameMath::DDALineForeach(xWidth, yHeight, xDestWidth, yDestHeight, [this, src, dest, &last_x, &last_y, &result](int32_t x, int32_t y) -> bool {
        if(_getGridData(x, y).bPassDisable == true)
        {
            Vector2 pos((float(last_x) + 0.5f) * m_fGirdWidth, (float(last_y) + 0.5f) * m_fGirdHeight);
            if((pos - src).squaredLength() > (dest - src).squaredLength())
            {
                result = dest;
                return false;
            }
            else
            {
                result = pos;
                return false;
            }
        }
        last_x = x;
        last_y = y;
        return true;
    });

    return result;
}

constexpr float SLOPE_MAX = 0.5f;
Vector2         CMapData::LineFindCanJump(const Vector2& src, const Vector2& dest) const
{
    uint32_t xWidth      = 0;
    uint32_t yHeight     = 0;
    uint32_t xDestWidth  = 0;
    uint32_t yDestHeight = 0;

    std::tie(xWidth, yHeight)         = Pos2Grid(src.x, src.y);
    std::tie(xDestWidth, yDestHeight) = Pos2Grid(dest.x, dest.y);
    float fSrcHigh                    = (float(_getGridData(xWidth, yHeight).nHigh) / 255.0f) * m_fGridHighFactor;
    float fDestHigh                   = (float(_getGridData(xDestWidth, yDestHeight).nHigh) / 255.0f) * m_fGridHighFactor;
    float fHighDiff                   = std::fabs(fSrcHigh - fDestHigh);
    float fSlope                      = 0.0f;
    if(fHighDiff != 0.0f)
        fSlope = m_fGirdWidth / fHighDiff;
    if(fSlope < SLOPE_MAX)
    {
        return src;
    }

    Vector2 result    = src;
    int32_t last_x    = xWidth;
    int32_t last_y    = yHeight;
    float   last_high = fSrcHigh;
    GameMath::DDALineForeach(xWidth,
                             yHeight,
                             xDestWidth,
                             yDestHeight,
                             [this, src, dest, &result, &last_x, &last_y, &last_high](int32_t x, int32_t y) -> bool {
                                 if(_getGridData(x, y).bPassDisable == true || _getGridData(x, y).bJumpDisable == true)
                                 {
                                     Vector2 pos((float(last_x) + 0.5f) * m_fGirdWidth, (float(last_y) + 0.5f) * m_fGirdHeight);
                                     if((pos - src).squaredLength() > (dest - src).squaredLength())
                                     {
                                         result = dest;
                                         return false;
                                     }
                                     else
                                     {
                                         result = pos;
                                         return false;
                                     }
                                 }

                                 float fTileHigh = (float(_getGridData(x, y).nHigh) / 255.0f) * m_fGridHighFactor;
                                 float fHighDiff = std::fabs(last_high - fTileHigh);
                                 float fSlope    = 0.0f;
                                 if(fHighDiff != 0.0f)
                                     fSlope = m_fGirdWidth / fHighDiff;
                                 if(fSlope < SLOPE_MAX)
                                 {
                                     Vector2 pos((float(last_x) + 0.5f) * m_fGirdWidth, (float(last_y) + 0.5f) * m_fGirdHeight);
                                     if((pos - src).squaredLength() > (dest - src).squaredLength())
                                     {
                                         result = dest;
                                         return false;
                                     }
                                     else
                                     {
                                         result = pos;
                                         return false;
                                     }
                                 }

                                 last_high = fTileHigh;
                                 last_x    = x;
                                 last_y    = y;

                                 return true;
                             });

    return result;
}
