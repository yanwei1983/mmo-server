#include "Player.h"
#include "PlayerTask.h"
#include "SceneService.h"
#include "server_msg/server_side.pb.h"
bool CPlayer::CheckTaskPhase(uint64_t idPhase) const
{
    __ENTER_FUNCTION
    auto it = m_TaskPhase.find(idPhase);
    if(it != m_TaskPhase.end())
    {
        return it->second == 0;
    }
    __LEAVE_FUNCTION
    return false;
}

void CPlayer::AddTaskPhase(uint64_t idPhase, bool bNotify /* = true*/)
{
    __ENTER_FUNCTION
    auto& refData = m_TaskPhase[idPhase];
    refData++;
    // notify ai
    if(bNotify)
    {
        ServerMSG::AddTaskPhase msg;
        msg.set_player_id(GetID());
        msg.set_task_phase_id(idPhase);
        SceneService()->SendProtoMsgToAIService(msg);
    }
    __LEAVE_FUNCTION
}

void CPlayer::RemoveTaskPhase(uint64_t idPhase, bool bNotify /* = true*/)
{
    __ENTER_FUNCTION
    auto& refData = m_TaskPhase[idPhase];
    refData--;
    if(refData == 0)
    {
        m_TaskPhase.erase(idPhase);
    }
    // notify ai
    if(bNotify)
    {
        ServerMSG::RemoveTaskPhase msg;
        msg.set_player_id(GetID());
        msg.set_task_phase_id(idPhase);
        SceneService()->SendProtoMsgToAIService(msg);
    }
    __LEAVE_FUNCTION
}