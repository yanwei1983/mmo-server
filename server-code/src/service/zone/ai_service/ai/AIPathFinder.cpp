#include "AIPathFinder.h"

#include "AIActor.h"
#include "AIPhase.h"
#include "AIScene.h"
#include "GameMap.h"

CAIPathFinder_Normal::CAIPathFinder_Normal(CAIActor* pActor)
    : m_pActor(pActor)
{
    
}

std::optional<Vector2> CAIPathFinder_Normal::SearchStep(const Vector2& dest, float move_spd)
{
    __ENTER_FUNCTION
    // src->dest ddlline
    // 计算行走方向
    Vector2 dis = (dest - m_pActor->GetPos());
    if(dis.length() < move_spd)
    {
        //可以行走到了
        //检查目标点是否可行走
        if(m_pActor->GetCurrentScene()->IsPassDisable(dest.x, dest.y, m_pActor->GetActorType()) == false)
        {
            return dest;
        }
        else
        {
            //从dest反向查找第一个可移动的格子
            return m_pActor->GetCurrentScene()->GetMap()->LineFindCanStand(dest, m_pActor->GetPos());
        }
    }

    auto dir     = dis.normalisedCopy();
    auto new_pos = m_pActor->GetPos() + dir * move_spd;
    if(m_pActor->GetCurrentScene()->IsPassDisable(new_pos.x, new_pos.y, m_pActor->GetActorType()) == false)
    {
        return new_pos;
    }
    else
    {
        //检查正向180度内是否有位置可以移动
        for(int32_t i = 0; i < 6; i++)
        {
            float angle = 30.0f * i;
            if((i % 2) != 0)
                angle *= -1.0f;
            auto new_dir = GameMath::rotate(dir, angle);
            new_pos      = new_dir * move_spd;
            if(m_pActor->GetCurrentScene()->IsPassDisable(new_pos.x, new_pos.y, m_pActor->GetActorType()) == false)
            {
                return new_pos;
            }

        }
        return {};
    }

    // record pos
    __LEAVE_FUNCTION
    return {};
}
