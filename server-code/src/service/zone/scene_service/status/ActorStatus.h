#ifndef ISTATUS_H
#define ISTATUS_H

#include "BaseCode.h"
#include "DBRecord.h"
#include "EventEntry.h"

export_lua enum StatusType {
    STATUSTYPE_NORMAL    = 1,
    STATUSTYPE_CRIME     = 2, //犯罪
    STATUSTYPE_DEAD      = 3, //死亡
    STATUSTYPE_COMP      = 4, //阵营 power=阵营id
    STATUSTYPE_TRANSFORM = 5, //变形 power=变形后的模型ID
    STATUSTYPE_DAMAGE    = 6, //受到伤害 每隔nSecs秒受到nPower的伤害
    STATUSTYPE_HIDE      = 7, //隐形

};

export_lua enum StatusExpireType {
    STATUSEXPIRETYPE_TIME  = 0, //默认时间计时销毁
    STATUSEXPIRETYPE_POINT = 1, // POWER为0时销毁
    STATUSEXPIRETYPE_NEVER = 2, //必须手动/达到条件销毁
};

export_lua enum StatusFlag {
    STATUSFLAG_NONE         = 0x0000, //默认不可叠加,不可覆盖
    STATUSFLAG_OVERRIDE_LEV = 0x0001, //高等级可以覆盖低等级
    STATUSFLAG_OVERLAP      = 0x0002, //不考虑等级, 直接叠加， 时间形的叠加时间， 数值型的叠加数值
    STATUSFLAG_PAUSE_ATTACH = 0x0008, //附加时默认暂停

    STATUSFLAG_DISABLE_MOVE     = 0x0010, //禁止移动
    STATUSFLAG_DISABLE_BEATTACK = 0x0020, //禁止被攻击
    STATUSFLAG_DISABLE_ATTACK   = 0x0040, //禁止攻击
    STATUSFLAG_BREAK_SKILL      = 0x0080, //附加状态时打断目标的技能释放

    STATUSFLAG_EXCEPT_DEATCH_DEAD = 0x0100, //死亡不会自动移除
    STATUSFLAG_DEATCH_MOVE        = 0x0200, //移动自动移除
    STATUSFLAG_DEATCH_BEATTACK    = 0x0400, //被攻击自动移除
    STATUSFLAG_DEATCH_SKILL       = 0x0800, //使用技能自动移除
    STATUSFLAG_DEATCH_ATTACK      = 0x1000, //攻击他人自动移除
    STATUSFLAG_DEATCH_LEAVEMAP    = 0x2000, //离开地图自动移除
    STATUSFLAG_DEATCH_OFFLINE     = 0x4000, //离线自动移除

    STATUSFLAG_OFFLINE_PAUSE = 0x00010000, //下线自动暂停
    STATUSFLAG_ONLINE_RESUME = 0x00020000, //上线自动恢复
    STATUSFLAG_ALLOW_PAUSE   = 0x00040000, //允许手动暂停

};

export_lua struct AttachStatusInfo
{
    uint32_t id_status_type = 0;
    uint8_t  lev            = 0;
    OBJID    id_caster      = 0;
    int32_t  power          = 0;
    int32_t  secs           = 0;
    int32_t  times          = 0;
    uint32_t flag           = 0;
    uint32_t id_status      = 0;
    uint32_t expire_type    = 0;
};

export_lua struct ST_STATUS_INFO : public AttachStatusInfo
{
    bool     pause          = false;
    uint32_t last_timestamp = 0;

    ST_STATUS_INFO() = default;
    ST_STATUS_INFO(const AttachStatusInfo& rht)
        : AttachStatusInfo(rht)
    {
    }
};

class CActor;
class CStatusType;
export_lua class CActorStatus : public NoncopyableT<CActorStatus>
{
    CActorStatus();
    bool Init(CActor* pOwner, CDBRecordPtr&& pRow);
    bool Init(CActor* pOwner, const AttachStatusInfo& info);

public:
    CreateNewImpl(CActorStatus);

public:
    virtual ~CActorStatus();

public:
public:
    uint32_t Release()
    {
        delete this;
        return 0;
    }
    export_lua CActor* GetOwner() const { return m_pOwner; }
    export_lua const CStatusType* Type() const { return m_pType; }
    export_lua uint64_t           GetScriptID() const;

    export_lua bool     IsValid() const;
    export_lua uint32_t GetID() const { return m_info.id_status; }
    export_lua uint16_t GetTypeID() const { return m_info.id_status_type; }
    export_lua int32_t  GetPower() const { return m_info.power; }
    export_lua OBJID    GetCasterID() const { return m_info.id_caster; }
    export_lua uint8_t  GetLevel() const { return m_info.lev; }
    export_lua int32_t  GetSecs() const { return m_info.secs; }
    export_lua int32_t  GetTimes() const { return m_info.times; }
    export_lua uint32_t GetLastTimeStamp() const { return m_info.last_timestamp; }
    export_lua uint32_t GetFlag() const { return m_info.flag; }
    export_lua int32_t  GetRemainTime() const;

    export_lua void SetPower(int32_t nPower) { m_info.power = nPower; }
    export_lua void AddSecs(int32_t nSecs);
    export_lua void AddTimes(int32_t nTimes);
    export_lua bool ChangeData(const AttachStatusInfo& info);

    export_lua bool IsPaused() const { return m_info.pause; }
    export_lua void Pause(bool bSynchro = true);
    export_lua void Resume(bool bSynchro = true);
    export_lua void SaveInfo();
    export_lua void SendStatus();

public:
    export_lua bool ScheduleEvent(time_t tIntervalMS = 0);
    export_lua void CancelEvent();
    export_lua void ClearEvent();
    export_lua void ProcessEvent();

public:
    export_lua void OnAttach();
    export_lua void OnDeatch();
    export_lua bool OnMove();
    export_lua bool OnSkill(uint32_t idSkill);
    export_lua bool OnAttack(CActor* pTarget, uint32_t idSkill, int32_t nDamage);
    export_lua bool OnBeAttack(CActor* pAttacker, int32_t nDamage);
    export_lua bool OnDead(CActor* pKiller);
    export_lua bool OnLeaveMap();
    void            OnLogin();
    void            OnLogout();

private:
    void OnEffect();

protected:
    CActor*            m_pOwner = nullptr;
    const CStatusType* m_pType  = nullptr;
    ST_STATUS_INFO     m_info;
    CDBRecordPtr       m_pRecord;

    CEventEntryPtr m_StatusEvent;

public:
    OBJECTHEAP_DECLARATION(s_heap)
};
#endif /* ISTATUS_H */
