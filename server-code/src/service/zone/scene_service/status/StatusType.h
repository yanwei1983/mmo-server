#ifndef STATUSTYPE_H
#define STATUSTYPE_H

#include "ActorAttrib.h"
#include "ActorStatus.h"
#include "T_GameDataMap.h"
#include "config/Cfg_Status.pb.h"

export_lua class CStatusType : public NoncopyableT<CStatusType>
{
    CStatusType() {}
    bool Init(const Cfg_Status& row);

public:
    CreateNewImpl(CStatusType);

public:
    ~CStatusType() {}

    using PB_T = Cfg_Status;
    export_lua uint32_t GetID() const;
    export_lua uint8_t  GetLevel() const;
    export_lua uint32_t GetTypeID() const;
    export_lua uint32_t GetExpireType() const;
    export_lua uint32_t GetFlag() const;
    export_lua int32_t  GetPower() const;
    export_lua int32_t  GetSecs() const;
    export_lua int32_t  GetTimes() const;
    export_lua int32_t  GetMaxTimes() const;
    export_lua int32_t  GetMaxSecs() const;
    export_lua OBJID    GetScriptID() const;

    export_lua const std::vector<CActorAttribChange>& GetAttribChangeList() const;

    export_lua AttachStatusInfo CloneInfo() const;

private:
    Cfg_Status                      m_Data;
    std::vector<CActorAttribChange> m_AttribChangeList;
};

DEFINE_GAMEMAPDATA(CStatusTypeSet, CStatusType);

#endif /* STATUSTYPE_H */
