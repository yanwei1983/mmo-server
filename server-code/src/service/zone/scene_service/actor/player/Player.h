#ifndef PLAYER_H
#define PLAYER_H

#include "Actor.h"
#include "DBRecord.h"
#include "EventManager.h"
#include "NetworkDefine.h"
#include "SceneID.h"

class CNetworkMessage;
class CCommonDataSet;
class CDataCountSet;
class CPackage;
class CStoragePackage;
class CEquipment;
class CPlayerTask;
class CPlayerAchievement;
class CPlayerSkillManager;
class CPetSet;
class CPlayerDialog;

export_lua class CPlayer : public CActor
{
protected:
    CPlayer();
    bool Init(OBJID idPlayer, const VirtualSocket& socket);

public:
    CreateNewImpl(CPlayer);

public:
    virtual ~CPlayer();

    export_lua bool FlyMap(uint16_t idMap, uint16_t idPhaseType, uint16_t _nPhaseType, float fPosX, float fPosY, float fRange, float fFace);
    export_lua bool FlyToPhase(CSceneBase* pTargetScene, float fPosX, float fPosY, float fRange, float fFace);
    
public:
    export_lua bool SendMsg(const proto_msg_t& msg) const override;

public:
    export_lua virtual ActorType GetActorType() const override { return ActorType::ACT_PLAYER; }
    export_lua static ActorType  GetActorTypeStatic() { return ActorType::ACT_PLAYER; }

    export_lua virtual float GetHeight() const { return 1.0f; }
    export_lua virtual float GetVolume() const { return 0.25f; }

    export_lua virtual uint16_t GetWorldID() const override;
    export_lua const std::string&         GetOpenID() const;
    export_lua virtual const std::string& GetName() const override;
    export_lua uint32_t                   GetProf() const;
    export_lua uint32_t                   GetBaseLook() const;
    export_lua uint32_t                   GetVipLev() const;
    export_lua uint32_t                   GetPKVal() const;
    export_lua uint32_t                   GetHonor() const;
    export_lua SceneIdx                   GetRecordSceneIdx() const;
    export_lua SceneIdx                   GetHomeSceneIdx() const;
    export_lua virtual uint32_t           GetLev() const override;
    export_lua uint64_t                   GetExp() const;
    export_lua uint64_t                   GetMoney() const;
    export_lua uint64_t                   GetMoneyBind() const;
    export_lua uint64_t                   GetGold() const;
    export_lua uint64_t                   GetGoldBind() const;
    export_lua uint32_t                   GetBagMaxSize() const;
    export_lua uint32_t                   GetStrogeMaxSize() const;
    export_lua uint32_t                   GetAchiPoint() const;
    export_lua virtual uint32_t           GetHP() const override;
    export_lua virtual uint32_t           GetMP() const override;
    export_lua virtual uint32_t           GetFP() const override;
    export_lua virtual uint32_t           GetNP() const override;
    export_lua virtual uint32_t           GetHPMax() const override;
    export_lua virtual uint32_t           GetMPMax() const override;
    export_lua virtual uint32_t           GetFPMax() const override;
    export_lua virtual uint32_t           GetNPMax() const override;
    export_lua virtual void               _SetHP(uint32_t v) override;
    export_lua virtual void               _SetMP(uint32_t v) override;
    export_lua virtual void               _SetFP(uint32_t v) override;
    export_lua virtual void               _SetNP(uint32_t v) override;

    export_lua float GetRecordPosX() const;
    export_lua float GetRecordPosY() const;
    export_lua float GetRecordFace() const;
    export_lua float GetHomePosX() const;
    export_lua float GetHomePosY() const;
    export_lua float GetHomeFace() const;

    export_lua uint32_t GetPKMode() const;
    export_lua void     SetPKMode(uint32_t val);
    export_lua OBJID    GetTeamID() const;
    export_lua void     SetTeamID(OBJID val);
    export_lua bool     HasTeam() const;
    export_lua OBJID    GetGuildID() const;
    export_lua void     SetGuildID(OBJID val);

    export_lua uint32_t GetTeamMemberCount() const;
    export_lua uint32_t GetGuildLev() const;

    export_lua void BroadcastShow();
    export_lua bool IsTalkEnable(uint32_t nTalkChannel);

public:
    //属性
    export_lua virtual void     _SetProperty(uint32_t nType, uint32_t nVal, uint32_t nSync = SYNC_TRUE) override;
    export_lua virtual uint32_t GetPropertyMax(uint32_t nType) const override;
    export_lua virtual uint32_t GetProperty(uint32_t nType) const override;

    virtual void MakeShowData(SC_AOI_NEW& msg) override;

public:
    // part
    export_lua CCommonDataSet* GetCommonDataSet() const { return m_pCommonDataSet.get(); }
    export_lua CDataCountSet* GetDataCountSet() const { return m_pDataCountSet.get(); }
    export_lua CPetSet* GetPetSet() const { return m_pPetSet.get(); }
    export_lua CPlayerSkillManager* GetSkillManager() const { return m_pUserSkillManager.get(); }
    export_lua CPackage* QueryPackage(uint32_t nPosition);
    export_lua CPackage* GetBag() { return m_pBag.get(); }
    export_lua CStoragePackage* GetStroagePackage();
    export_lua CEquipment* GetEquipmentSet() const { return m_pEquipmentSet.get(); }
    export_lua CPlayerTask* GetTaskSet() const { return m_pTaskSet.get(); }
    export_lua CPlayerAchievement* GetAchievement() const { return m_pAchievement.get(); }
    export_lua CPlayerDialog* GetDialog() const { return m_pPlayerDialog.get(); }

public:
    //货币
    export_lua bool CheckMoney(uint32_t nMoneyType, uint32_t nVal);
    export_lua bool SpendMoney(uint32_t nMoneyType, uint32_t nVal);
    export_lua bool AwardMeony(uint32_t nMoneyType, uint32_t nVal);

public:
    //给以经验
    export_lua void AwardExp(uint32_t nExp);
    export_lua void AwardBattleExp(uint32_t nExp, bool bKillBySelf);

public:
    export_lua bool CheckItem(uint32_t idItemType, uint32_t nCount = 1, uint32_t dwFlag = 0);
    export_lua bool SpendItem(uint32_t nItemLogType, uint32_t idItemType, uint32_t nCount = 1, bool bUseBindFirst = true);
    export_lua bool AwardItem(uint32_t nItemLogType, uint32_t idItemType, uint32_t nCount, uint32_t dwFlag);
    export_lua bool UseItem(uint32_t nGridInBag, uint32_t nCount = 1);

public:
    void TeamCreate();
    void TeamQuit();
    void TeamKickMember(OBJID idMember);
    void TeamInviteMember(OBJID idTarget);
    void TeamAcceptInvite(uint64_t idTeam, OBJID idInviter, bool bResult);
    void TeamApplyMember(OBJID idTarget);
    void TeamAcceptApply(OBJID idApplicant, bool bResult);
    void TeamChangeLeader(OBJID idMember);

public:
    export_lua void SendTalkMsg(uint32_t nTalkChannel, const std::string& txt);

    //回写数据库
    void SaveInfo();
    //处理网络消息
    void             ProcessMsg();
    export_lua const VirtualSocket& GetSocket() const { return m_Socket; }
    export_lua virtual void         RecalcAttrib(bool bClearCache = false) override;

public:
    void OnRecvGameData(CNetworkMessage* pMsg);

    virtual void OnEnterMap(CSceneBase* pScene) override;
    virtual void OnLeaveMap(uint16_t idTargetMap) override;

public:
    void OnTimer();
    void OnLogin(bool bLogin, const SceneIdx& idxScene, float fPosX, float fPosY, float fRange, float fFace);
    void OnLogout();
    void OnChangeZoneSaveFinish(const TargetSceneID& idTargetScene, float fPosX, float fPosY, float fRange, float fFace);
    void OnLoadMapSucc();

public:
    export_lua bool Reborn(uint32_t nRebornType);
    bool            TryChangeMap(uint32_t nLeavePointIdx);

public:
    export_lua virtual bool CanDamage(CActor* pTarget) const override;
    export_lua virtual void BeKillBy(CActor* pAttacker) override;
    export_lua virtual bool IsEnemy(CSceneObject* pTarget) const override;

private:
    void _ChangeZone(const TargetSceneID& idTargetScene, float fPosX, float fPosY, float fRange, float fFace);
    void _FlyMap(const TargetSceneID& idTargetScene, float fPosX, float fPosY, float fRange, float fFace);
    void _FlyPhase(CSceneBase* pTargetPhase, float fPosX, float fPosY, float fRange, float fFace);
    void SendGameData(const TargetSceneID& idTargetScene);

    void SendPlayerInfoToClient();
    void SendAttribToClient();
    void OnLevUp(uint32_t nLev);

public:
    bool CheckTaskPhase(uint64_t idPhase) const;
    void AddTaskPhase(uint64_t idPhase, bool bNotify = true);
    void RemoveTaskPhase(uint64_t idPhase, bool bNotify = true);

public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    CDBRecordPtr                         m_pRecord;
    VirtualSocket                        m_Socket;
    CEventEntryPtr                       m_pEventOnTimer;
    std::deque<CNetworkMessage*>         m_MessageList;
    std::unique_ptr<CCommonDataSet>      m_pCommonDataSet;
    std::unique_ptr<CDataCountSet>       m_pDataCountSet;
    std::unique_ptr<CPetSet>             m_pPetSet;
    std::unique_ptr<CPlayerSkillManager> m_pUserSkillManager;

    SceneIdx m_idLoadingScene = 0;
    float    m_fLoadingPosX   = 0.0f;
    float    m_fLoadingPosY   = 0.0f;
    float    m_fLoadingFace   = 0.0f;

    uint32_t m_nPKMode = 0;
    OBJID    m_idTeam  = 0;
    OBJID    m_idGuild = 0;

    std::unique_ptr<CPackage>           m_pBag;
    std::unique_ptr<CStoragePackage>    m_pStoragePackage;
    std::unique_ptr<CEquipment>         m_pEquipmentSet;
    std::unique_ptr<CPlayerTask>        m_pTaskSet;
    std::unique_ptr<CPlayerAchievement> m_pAchievement;
    std::unique_ptr<CPlayerDialog>      m_pPlayerDialog;

    std::unordered_map<uint64_t, uint32_t> m_TaskPhase;
};
#endif /* PLAYER_H */
