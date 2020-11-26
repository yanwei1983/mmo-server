#ifndef DATACOUNT_H
#define DATACOUNT_H

#include "BaseCode.h"
#include "DBRecord.h"

//计数及各种每日次数限制
export_lua enum DATA_ACC_TYPE {
    DATA_ACC_SYSTEM      = 0, //其他的计数？？？
    DATA_ACC_MONSTERKILL = 1, //杀怪计数 keyidx=怪物类型id
    DATA_ACC_USEITEM     = 2, //使用物品计数 keyidx=物品类型id
    DATA_ACC_TASKID      = 3, //完成任务计数 keyidx=任务类型id
    DATA_ACC_BUYITEM     = 4, //购买物品计数 keyidx=物品类型id

};

export_lua enum DATA_ACC_SYSTEM_TYPE {
    DATA_ACC_SYSTEM_LOGINDAY        = 1, //累计登陆天数
    DATA_ACC_SYSTEM_TXTATR_GLOBAL   = 2, //累计使用小喇叭次数
    DATA_ACC_SYSTEM_KILLPLAYER      = 3, //击杀玩家数量
    DATA_ACC_SYSTEM_BEKILL_BYPLAYER = 4, //被玩家杀死次数
    DATA_ACC_SYSTEM_KILL_MONSTER    = 5, //累计击杀BOSS数量
    DATA_ACC_SYSTEM_KILL_BOSS       = 6, //累计击杀BOSS数量
};

export_lua enum DATA_ACC_COUNT_TYPE {
    DATA_COUNT_RESET_NEVER    = 0, //不重置
    DATA_COUNT_RESET_BY_DAY   = 1, //跨N天重置
    DATA_COUNT_RESET_BY_WEEK  = 2, //跨N周重置
    DATA_COUNT_RESET_BY_MONTH = 3, //跨N月重置
};

class CPlayer;

export_lua class CDataCount
{
public:
    CDataCount(CPlayer* pPlayer, CDBRecordPtr&& pRecord);
    ~CDataCount();

    export_lua uint32_t GetType() const;
    export_lua uint32_t GetIdx() const;
    export_lua uint64_t GetDataNum();
    export_lua uint32_t GetNextResetTime() const;

    export_lua uint64_t AddData(uint64_t nVal, bool bUpdate);
    export_lua uint64_t SetData(uint64_t nVal, bool bUpdate);

    export_lua void Sync();
    export_lua void Save();
    export_lua void DeleteRecord();

private:
    void CheckReset(uint32_t nNextTime);

private:
    CPlayer*     m_pOwner = nullptr;
    CDBRecordPtr m_pRecord;

    OBJECTHEAP_DECLARATION(s_heap);
};

export_lua class CDataCountSet : public NoncopyableT<CDataCountSet>
{
    CDataCountSet();

public:
    CreateNewImpl(CDataCountSet);

public:
    ~CDataCountSet();

    bool            Init(CPlayer* pPlayer);
    export_lua void SyncAll();
    export_lua void Save();

    export_lua uint64_t GetCount(uint32_t nType, uint32_t nIdx);
    export_lua uint64_t GetMaxCount(uint32_t nType, uint32_t nIdx);
    export_lua uint64_t AddCount(uint32_t nType, uint32_t nIdx, uint64_t nVal, bool bUpdate = false);
    export_lua uint64_t _AddCount(uint32_t nType, uint32_t nIdx, uint64_t nVal, bool bUpdate = false);
    export_lua uint64_t SetCount(uint32_t nType, uint32_t nIdx, uint64_t nVal, bool bUpdate = false);
    export_lua void     DeleteCount(uint32_t nType, uint32_t nIdx);

private:
    void CreateData(uint32_t nType, uint32_t nIdx, uint64_t nVal);

private:
    CPlayer* m_pOwner = nullptr;

    std::unordered_map<uint64_t, std::unique_ptr<CDataCount> > m_setDataMap;
};
#endif /* DATACOUNT_H */
