#include "SceneObject.h"

#include "SceneBase.h"
#include "SceneTree.h"
//#include "taskflow/taskflow.hpp"

CSceneObject::CSceneObject() {}

CSceneObject::~CSceneObject() {}

void CSceneObject::SetPos(const Vector2& pos)
{
    __ENTER_FUNCTION
    if(m_Pos.x == pos.x && m_Pos.y == pos.y)
        return;
    CHECK(Math::isNaN(pos.x) == false);
    CHECK(Math::isNaN(pos.y) == false);

    m_Pos = pos;
    if(m_pScene)
    {
        auto pSceneTree = m_pScene->GetSceneTree();
        if(pSceneTree)
        {
            SetSceneTile(pSceneTree->GetSceneTileByPos(GetPosX(), GetPosY()));
            SetCollisionTile(pSceneTree->GetCollisionTileByPos(GetPosX(), GetPosY(), GetActorType()));
        }
    }
    __LEAVE_FUNCTION
}

void CSceneObject::FaceTo(const Vector2& pos)
{
    if(pos != m_Pos)
        SetFace(GameMath::getAngleFrom((pos - m_Pos).normalisedCopy()));
}

void CSceneObject::OnEnterMap(CSceneBase* pScene)
{
    m_pScene = pScene;
    auto scene_tree = pScene->GetSceneTree();
    if(scene_tree)
    {
        SetSceneTile(scene_tree->GetSceneTileByPos(GetPosX(), GetPosY()));
        SetCollisionTile(scene_tree->GetCollisionTileByPos(GetPosX(), GetPosY(), GetActorType()));
    }
}

void CSceneObject::OnLeaveMap(uint16_t idTargetMap)
{
    SetSceneTile(nullptr);
    SetCollisionTile(nullptr);
    m_pScene = nullptr;
}

void CSceneObject::SetHideCoude(int32_t nHideCount)
{
    if(nHideCount == 0)
    {
        m_nHideCount = nHideCount;
        auto scene_tree = GetCurrentScene()->GetSceneTree();
        if(scene_tree)
        {
            SetSceneTile(scene_tree->GetSceneTileByPos(GetPosX(), GetPosY()));
            SetCollisionTile(scene_tree->GetCollisionTileByPos(GetPosX(), GetPosY(), GetActorType()));
        }
    }
    else
    {
        SetSceneTile(nullptr);
        SetCollisionTile(nullptr);
        m_nHideCount = nHideCount;
    }
}

void CSceneObject::AddHide()
{
    SetSceneTile(nullptr);
    SetCollisionTile(nullptr);

    m_nHideCount++;
}

void CSceneObject::RemoveHide()
{
    m_nHideCount--;
    if(m_nHideCount == 0)
    {
        auto scene_tree = GetCurrentScene()->GetSceneTree();
        if(scene_tree)
        {
            SetSceneTile(scene_tree->GetSceneTileByPos(GetPosX(), GetPosY()));
            SetCollisionTile(scene_tree->GetCollisionTileByPos(GetPosX(), GetPosY(), GetActorType()));
        }
    }
}

uint32_t CSceneObject::GetCurrentViewActorCount() const
{
    return m_ViewActors.size();
}

uint32_t CSceneObject::GetCurrentViewPlayerCount()
{
    return m_ViewActorsByType[ACT_PLAYER].size();
}

uint32_t CSceneObject::GetCurrentViewMonsterCount()
{
    return m_ViewActorsByType[ACT_MONSTER].size();
}

void CSceneObject::SetSceneTile(CSceneTile* val)
{
    __ENTER_FUNCTION

    if(m_pSceneTile == val)
        return;
    if(m_nHideCount != 0)
        return;

    if(m_pSceneTile)
        m_pSceneTile->RemoveActor(this);
    m_pSceneTile = val;
    if(m_pSceneTile)
        m_pSceneTile->AddActor(this);
    __LEAVE_FUNCTION
}

void CSceneObject::SetCollisionTile(CSceneCollisionTile* val)
{
    __ENTER_FUNCTION

    if(m_pCollisionTile == val)
        return;
    if(m_nHideCount > 0)
        return;

    if(m_pCollisionTile)
        m_pCollisionTile->RemoveActor(this);
    m_pCollisionTile = val;
    if(m_pCollisionTile)
        m_pCollisionTile->AddActor(this);
    __LEAVE_FUNCTION
}

void CSceneObject::ChangePhase(uint64_t idPhaseID)
{
    if(GetPhaseID() == idPhaseID)
    {
        return;
    }

    _SetPhaseID(idPhaseID);
    UpdateViewList(true);
}

bool CSceneObject::IsEnemy(CSceneObject* pTarget) const
{
    return false;
}

bool CSceneObject::IsInViewActor(CSceneObject* pActor) const
{
    if(!pActor)
    {
        return false;
    }
    if(pActor == this)
    {
        return true;
    }

    uint64_t id = pActor->GetID();
    return std::binary_search(m_ViewActors.begin(), m_ViewActors.end(), id);
}

bool CSceneObject::IsInViewActorByID(OBJID idActor) const
{
    return std::binary_search(m_ViewActors.begin(), m_ViewActors.end(), idActor);
}

//////////////////////////////////////////////////////////////////////
void CSceneObject::RemoveFromViewList(CSceneObject* pActor, OBJID idActor, bool bErase)
{
    // 通知自己对方消失
    if(pActor)
    {
        m_ViewActorsByType[pActor->GetActorType()].erase(idActor);
    }
    else
    {
        for(auto& refList: m_ViewActorsByType)
        {
            refList.second.erase(idActor);
        }
    }

    if(bErase)
    {
        m_ViewActors.erase(idActor);
    }
}

//////////////////////////////////////////////////////////////////////
void CSceneObject::AddToViewList(CSceneObject* pActor)
{
    if(pActor == nullptr)
        return;

    _AddToViewList(pActor->GetActorType(), pActor->GetID());
}

void CSceneObject::_AddToViewList(uint32_t actor_type, uint64_t id)
{
    if(m_ViewActors.find(id) != m_ViewActors.end())
    {
        return;
    }
    m_ViewActors.insert(id);
    m_ViewActorsByType[actor_type].insert(id);
}

//////////////////////////////////////////////////////////////////////
void CSceneObject::ClearViewList(bool bSendMsgToSelf)
{
    __ENTER_FUNCTION
    OnBeforeClearViewList(bSendMsgToSelf);

    if(GetCurrentScene())
    {
        for(uint64_t id: m_ViewActors)
        {
            // 通知对方自己消失
            CSceneObject* pActor = GetCurrentScene()->QuerySceneObj(id);
            if(pActor)
            {
                pActor->RemoveFromViewList(this, GetID(), true);
            }
        }
    }
    //发送删除包

    m_ViewActorsByType.clear();
    m_ViewActors.clear();

    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////
bool CSceneObject::UpdateViewList(bool bForce)
{
    //只有当自己的位置变化大于x，才主动刷新视野列表，否则由他人触发
    CHECKF(GetCurrentScene());
    CHECKF(GetSceneTile());
    auto pSceneTree = GetCurrentScene()->GetSceneTree();
    if(pSceneTree == nullptr)
        return false;

    float view_change_min = pSceneTree->GetViewChangeMin();
    auto  use_manhattan   = pSceneTree->IsViewManhattanDistance();
    if(bForce || ((use_manhattan) ? GameMath::manhattanDistance(m_LastUpdateViewPos, m_Pos) >= view_change_min
                                  : GameMath::simpleDistance(m_LastUpdateViewPos, m_Pos) >= view_change_min))
    {
        m_LastUpdateViewPos = m_Pos;
        return _UpdateViewList();
    }
    return true;
}

bool CSceneObject::_UpdateViewList()
{
    CHECKF(GetCurrentScene());
    CHECKF(GetSceneTile());
    //////////////////////////////////////////////////////////////////////////
    // 为了减少重新搜索广播集的次数，这里采用的策略是划分3*3格的逻辑格子，
    // 只有首次进入地图或逻辑位置发生变化的时候才重新进行搜索

    // 寻找新的目标集
    BROADCAST_SET setBCActor;
    struct ACTOR_MAP_BY_DIS_DATA
    {
        float         dis;
        CSceneObject* pActor;
        bool operator<(float rht) const { return dis < rht; }
        bool operator<(const ACTOR_MAP_BY_DIS_DATA& rht) const { return dis < rht.dis; }
    };
    typedef std::deque<ACTOR_MAP_BY_DIS_DATA> ACTOR_MAP_BY_DIS;

    ACTOR_MAP_BY_DIS actor_viewin_withdis;
    ACTOR_MAP_BY_DIS actor_viewout_withdis;
    auto             scene_tree = GetCurrentScene()->GetSceneTree();
    CHECKF(scene_tree);
    uint32_t viewcount_max         = scene_tree->GetViewCount();
    uint32_t use_manhattan         = scene_tree->IsViewManhattanDistance();
    uint32_t view_range_in_square  = scene_tree->GetViewRangeInSquare();
    uint32_t view_range_out_square = scene_tree->GetViewRangeOutSquare();
    // 广播集算法修改测试
    //////////////////////////////////////////////////////////////////////////
    // step1: 获取当前广播集范围内的对象
    {
        scene_tree->foreach_SceneTileInSight(
            GetPosX(),
            GetPosY(),
            [thisActor = this, &setBCActor, &actor_viewin_withdis, &actor_viewout_withdis, 
                view_range_in_square, view_range_out_square, viewcount_max, use_manhattan](CSceneTile* pSceneTile) {
                const auto& actor_list = *pSceneTile;
                for(CSceneObject* pActor: actor_list)
                {
                    if(pActor == thisActor)
                        continue;
                    if(pActor == nullptr)
                        continue;

                    // 判断目标是否需要加入广播集
                    if(thisActor->ViewTest(pActor) == false)
                        continue;
                    //不需要视野剪裁，那么就都加入
                    if(viewcount_max <= 0)
                    {
                        setBCActor.insert(pActor->GetID());
                        continue;
                    }
                    //! 目标进入视野，需要加入广播集
                    if(thisActor->IsMustAddToViewList(pActor) == true)
                    {
                        //强制加入视野的，优先合入
                        setBCActor.insert(pActor->GetID());
                    }
                    else
                    {
                        float distance_square = 0;
                        if(use_manhattan)
                        {
                            distance_square = GameMath::manhattanDistance(thisActor->GetPos(), pActor->GetPos());
                        }
                        else
                        {
                            distance_square = GameMath::simpleDistance(thisActor->GetPos(), pActor->GetPos());
                        }

                        if(view_range_in_square <= 0)
                        {
                            //如果view_in == 0,那么所有人全部根据麦哈顿距离进入视野
                            actor_viewin_withdis.emplace_back(ACTOR_MAP_BY_DIS_DATA{distance_square, pActor});
                        }
                        else
                        {
                            if(distance_square <= view_range_in_square)
                            {
                                actor_viewin_withdis.emplace_back(ACTOR_MAP_BY_DIS_DATA{distance_square, pActor});
                            }
                            else if(distance_square <= view_range_out_square)
                            {
                                actor_viewout_withdis.emplace_back(ACTOR_MAP_BY_DIS_DATA{distance_square, pActor});
                            }
                        }
                    }
                }
            });
    }

    if(viewcount_max > 0 && setBCActor.size() < viewcount_max)
    {
        //需要视野裁剪
        //如果当前视野人数还没满

        //对可以进入视野集合进行排序,只取距离自己最近的N个,加入视野列表
        uint32_t nNeedInsert = viewcount_max - setBCActor.size();
        auto     nCanInsert  = std::min<uint32_t>(actor_viewin_withdis.size(), nNeedInsert);

        std::nth_element(actor_viewin_withdis.begin(), actor_viewin_withdis.begin() + nCanInsert - 1, actor_viewin_withdis.end(), std::less<>());

        int32_t i = 0;
        for(auto it = actor_viewin_withdis.begin(); i < nCanInsert && it != actor_viewin_withdis.end(); i++)
        {
            CSceneObject* pActor = it->pActor;
            uint64_t      id     = pActor->GetID();
            setBCActor.insert(id);
            it++;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // setp2: 计算当前广播集与旧广播集的差集——这部分是新进入视野的
    BROADCAST_SET setBCActorAdd;
    set_difference(setBCActor.begin(),
                   setBCActor.end(),
                   m_ViewActors.begin(),
                   m_ViewActors.end(),
                   std::insert_iterator(setBCActorAdd, setBCActorAdd.begin()));

    //////////////////////////////////////////////////////////////////////////
    // step3: 计算旧广播集与当前广播集的差集——这部分是可能需要离开视野的
    BROADCAST_SET setBCActorDel;
    set_difference(m_ViewActors.begin(),
                   m_ViewActors.end(),
                   setBCActor.begin(),
                   setBCActor.end(),
                   std::insert_iterator(setBCActorDel, setBCActorDel.begin()));

    if(viewcount_max > 0 && setBCActor.size() < viewcount_max)
    {
        //计算待删除列表还可以保留多少个
        auto nNeedReback = viewcount_max - setBCActor.size();
        //将脱离列表中， 可以不删除的那些，放回到viewlist

        auto nCanReback = std::min<uint32_t>(actor_viewout_withdis.size(), nNeedReback);
        //对大于view_in, 小于view_out的 进行排序, 取距离自己最近的N个,重新加入视野列表
        std::nth_element(actor_viewout_withdis.begin(), actor_viewout_withdis.begin() + nCanReback - 1, actor_viewout_withdis.end(), std::less<>());

        for(auto it = actor_viewout_withdis.begin(); it != actor_viewout_withdis.end() && nCanReback > 0;)
        {
            CSceneObject* pActor = it->pActor;
            uint64_t      id     = pActor->GetID();
            nCanReback--;
            setBCActor.insert(id);
            setBCActorDel.erase(id);
            it++;
        }
    }

    OnAOIProcess(setBCActorDel, setBCActor, setBCActorAdd);
    return true;
}

void CSceneObject::OnAOIProcess(const BROADCAST_SET& setBCActorDel, const BROADCAST_SET& setBCActor, const BROADCAST_SET& setBCActorAdd)
{
    m_ViewActors = setBCActor;
}

void CSceneObject::ForeachViewActorList(const std::function<void(OBJID)>& func)
{
    __ENTER_FUNCTION
    std::for_each(m_ViewActors.begin(), m_ViewActors.end(), func);
    __LEAVE_FUNCTION
}