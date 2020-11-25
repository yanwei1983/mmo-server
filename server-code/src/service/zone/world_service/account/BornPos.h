#ifndef BORNPOS_H
#define BORNPOS_H

#include "BaseCode.h"
#include "DBRecord.h"
#include "ProtobuffUtil.h"
#include "RandomGet.h"
#include "T_GameDataMap.h"
#include "config/Cfg_BornPos.pb.h"
//////////////////////////////////////////////////////////////////////
//
class CBornPos : public NoncopyableT<CBornPos>
{
    CBornPos() {}
    bool Init(const Cfg_BornPos& row)
    {
        m_row = row;
        return true;
    }

public:
    CreateNewImpl(CBornPos);

public:
    ~CBornPos() {}
    using PB_T = Cfg_BornPos;

public:
    static uint32_t GetKey(const Cfg_BornPos& row) { return row.prof(); }
    uint32_t        GetID() const { return m_row.id(); }
    uint32_t        GetProf() const { return m_row.prof(); }
    uint16_t        GetMapID() const { return m_row.mapid(); }
    float           GetPosX() const { return m_row.posx(); }
    float           GetPoxY() const { return m_row.posy(); }
    float           GetRange() const { return m_row.range(); }
    float           GetFace() const { return m_row.face(); }

protected:
    Cfg_BornPos m_row;
};

class CBornPosSet : public CGameMultiDataMap<CBornPos>
{
    CBornPosSet() {}

public:
    CreateNewImpl(CBornPosSet);

public:
    ~CBornPosSet() {}

public:
    const CBornPos* RandGet(uint32_t dwProf) const
    {
        auto iter = QueryObj(dwProf);
        if(iter.HasMore() == false)
        {
            return nullptr;
        }
        size_t count = iter.Count();
        size_t idx   = random_uint32_range(0, count - 1);

        while(iter.HasMore() && idx > 0)
        {
            idx--;
            iter.MoveNext();
        }
        return iter.PeekVal();
    }
};

#endif /* BORNPOS_H */
