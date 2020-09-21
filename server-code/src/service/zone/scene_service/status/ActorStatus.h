#ifndef ACTORSTATUS_H
#define ACTORSTATUS_H

#include "BaseCode.h"
#include "IStatus.h"
#include "msg/zone_service.pb.h"

class CActor;

export_lua class CActorStatus : public NoncopyableT<CActorStatus>
{
    CActorStatus();
    bool Init(CActor* pActor);

public:
    CreateNewImpl(CActorStatus);

public:
    ~CActorStatus();

    export_lua CStatus* QueryStatusByType(uint16_t idStatusType) const;

    export_lua bool AttachStatus(const AttachStatusInfo& info);
    export_lua bool AttachStatus(uint32_t idStatus, OBJID idCaster);
    export_lua bool DetachStatus(uint16_t idStatusType);
    export_lua bool DetachStatusByType(uint32_t nStatusType);
    export_lua bool DetachStatusByFlag(uint32_t nStatusFlag, bool bHave = true);
    export_lua bool TestStatusByType(uint32_t nStatusType) const;
    export_lua bool TestStatusByFlag(uint32_t nFlag) const;
    template<typename Func>
    void for_each(Func func) const
    {
        for(const auto& [k, v]: m_setStatusByType)
        {
            func(v);
        }
    }

    export_lua void   SyncTo(CActor* pActor);
    export_lua void   FillStatusMsg(SC_STATUS_LIST& status_msg);
    void              SaveInfo();
    void              Stop();
    export_lua size_t size() const { return m_setStatusByType.size(); }

public:
    void OnMove();
    void OnSkill(uint32_t idSkill);
    void OnAttack(CActor* pTarget, uint32_t idSkill, int32_t nDamage);
    void OnDead(CActor* pKiller);
    void OnBeAttack(CActor* pAttacker, int32_t nDamage);
    void OnLeaveMap();
    void OnLogin();
    void OnLogout();

    template<class Func, class... Args>
    void OnEventDetach(Func func, int32_t flag, Args&&... args)
    {
        __ENTER_FUNCTION
        for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end();)
        {
            CStatus* pStatus = it->second;
            if(HasFlag(pStatus->GetFlag(), flag) == true || std::invoke(func, it->second, std::forward<Args>(args)...))
            {
                pStatus->OnDeatch();
                SAFE_DELETE(pStatus);
                it = m_setStatusByType.erase(it);
            }
            else
            {
                it++;
            }
        }
        __LEAVE_FUNCTION
    }

    template<class Func, class... Args>
    void OnEventDeatchExcept(Func func, int32_t except_flag, Args&&... args)
    {
        __ENTER_FUNCTION
        for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end();)
        {
            CStatus* pStatus = it->second;
            if(HasFlag(pStatus->GetFlag(), except_flag) == false && std::invoke(func, it->second, std::forward<Args>(args)...))
            {
                pStatus->OnDeatch();
                SAFE_DELETE(pStatus);
                it = m_setStatusByType.erase(it);
            }
            else
            {
                it++;
            }
        }
        __LEAVE_FUNCTION
    }

private:
    void _AddStatus(CStatus* pStatus);

    void _RemoveStatus(CStatus* pStatus);

private:
    CActor*                                m_pOwner;
    std::unordered_map<uint16_t, CStatus*> m_setStatusByType;
};
#endif /* ACTORSTATUS_H */