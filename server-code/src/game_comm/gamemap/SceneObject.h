#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "BaseCode.h"
#include "game_common_def.h"

class CSceneBase;
class CSceneTile;
class CSceneCollisionTile;

class CSceneObject;
typedef std::set<OBJID>                                         BROADCAST_SET;
typedef std::unordered_map<uint32_t, std::unordered_set<OBJID>> BROADCAST_SET_BYTYPE;
typedef std::unordered_map<OBJID, CSceneObject*>                ACTOR_MAP;

export_lua class CSceneObject : public NoncopyableT<CSceneObject>
{
protected:
    CSceneObject();

public:
    virtual ~CSceneObject();

public:
    template<typename T>
    T* CastTo()
    {
        if(CanCastTo(T::GetActorTypeStatic()) == true)
            return static_cast<T*>(this);
        else
            return nullptr;
    }
    template<typename T>
    const T* CastTo() const
    {
        if(CanCastTo(T::GetActorTypeStatic()) == true)
            return static_cast<const T*>(this);
        else
            return nullptr;
    }
    export_lua virtual bool CanCastTo(ActorType actor_type) const { return GetActorType() == actor_type; }

    export_lua virtual ActorType GetActorType() const = 0;

    export_lua bool IsPlayer() const { return GetActorType() == ACT_PLAYER; }
    export_lua bool IsMonster() const { return GetActorType() == ACT_MONSTER; }
    export_lua bool IsNpc() const { return GetActorType() == ACT_NPC; }
    export_lua bool IsPet() const { return GetActorType() == ACT_PET; }
    export_lua bool IsMapItem() const { return GetActorType() == ACT_MAPITEM; }
    export_lua bool IsBullet() const { return GetActorType() == ACT_BULLET; }
    
    export_lua CSceneBase* GetCurrentScene() const { return m_pScene; }
    export_lua CSceneTile* GetSceneTile() const { return m_pSceneTile; }
    export_lua CSceneCollisionTile* GetCollisionTile() const { return m_pCollisionTile; }

    export_lua void SetSceneTile(CSceneTile* val);
    export_lua void SetCollisionTile(CSceneCollisionTile* val);

    export_lua uint64_t     GetPhaseID() const { return m_idPhaseID; }
    export_lua void         _SetPhaseID(uint64_t idPhaseID) { m_idPhaseID = idPhaseID; }
    export_lua virtual void ChangePhase(uint64_t idPhaseID);

    export_lua OBJID GetID() const { return m_ID; }
    void             SetID(OBJID v) { m_ID = v; }

    export_lua virtual OBJID GetOwnerID() const { return 0; }

    export_lua virtual const CPos2D& GetPos() const { return m_Pos; }
    export_lua virtual CPos2D&       GetPosRef() { return m_Pos; }
    export_lua virtual float         GetPosX() const { return m_Pos.x; }
    export_lua virtual float         GetPosY() const { return m_Pos.y; }
    export_lua virtual float         GetFace() const { return m_Face; }
    export_lua virtual void          SetPos(const CPos2D& pos);
    export_lua virtual void          SetFace(float face)
    {
        if(Math::isNaN(face) == false)
            m_Face = face;
    }
    export_lua void FaceTo(const CPos2D& pos);

public:
    // AOI
    export_lua virtual bool UpdateViewList(bool bForce);
    export_lua virtual void ClearViewList(bool bSendMsgToSelf);
    virtual void            OnBeforeClearViewList(bool bSendMsgToSelf) {}

public:
    // AOI
    export_lua virtual bool IsEnemy(CSceneObject* pTarget) const;
    
    virtual void RemoveFromViewList(CSceneObject* pActor, OBJID idActor, bool bErase);
    virtual void AddToViewList(CSceneObject* pActor);
    virtual void _AddToViewList(uint32_t actor_type, uint64_t id);

    export_lua bool IsInViewActor(CSceneObject* actor) const;
    export_lua bool IsInViewActorByID(OBJID idActor) const;

    export_lua uint32_t GetCurrentViewActorCount() const;
    export_lua uint32_t GetCurrentViewPlayerCount();
    export_lua uint32_t GetCurrentViewMonsterCount();

    export_lua const BROADCAST_SET& _GetViewList() const { return m_ViewActors; }
    export_lua const BROADCAST_SET_BYTYPE& _GetViewListByType() const { return m_ViewActorsByType; }

    export_lua void ForeachViewActorList(const std::function<void(OBJID)>& func);

protected:
    virtual bool _UpdateViewList();
    // AOI
    virtual void OnAOIProcess(const BROADCAST_SET& setBCActorDel, const BROADCAST_SET& setBCActor, const BROADCAST_SET& setBCActorAdd);

    virtual bool ViewTest(CSceneObject* pActor) const { return false; }
    virtual bool IsMustAddToViewList(CSceneObject* pActor) const { return false; }

public:
    virtual void OnEnterMap(CSceneBase* pScene);
    virtual void OnLeaveMap(uint16_t idTargetMap);

    export_lua void         SetHideCoude(int32_t nHideCount);
    export_lua virtual void AddHide();
    export_lua virtual void RemoveHide();

protected:
    OBJID    m_ID        = 0; // id
    uint64_t m_idPhaseID = 0; //位面ID

    CSceneBase*          m_pScene         = nullptr; //场景
    CSceneTile*          m_pSceneTile     = nullptr;
    CSceneCollisionTile* m_pCollisionTile = nullptr;
    CPos2D               m_Pos;               //当前的位置
    CPos2D               m_LastUpdateViewPos; //最后一次更新viewlist时的位置
    float                m_Face = 0.0f;       //当前的朝向
    BROADCAST_SET        m_ViewActors;        // 视野内的生物
    BROADCAST_SET_BYTYPE m_ViewActorsByType;
    int32_t              m_nHideCount = 0;
};
#endif /* SCENEOBJECT_H */
