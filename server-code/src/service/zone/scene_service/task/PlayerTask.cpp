#include "PlayerTask.h"

#include "Package.h"
#include "Player.h"
#include "PlayerAchievement.h"
#include "PlayerDialog.h"
#include "PlayerTaskData.h"
#include "SceneService.h"
#include "ScriptCallBackType.h"
#include "ScriptManager.h"
#include "TaskType.h"
#include "config/Cfg_Achievement.pb.h"
#include "msg/zone_service.pb.h"

CPlayerTask::CPlayerTask() {}

CPlayerTask::~CPlayerTask()
{
    __ENTER_FUNCTION
    for(auto& [k, v]: m_setTask)
    {
        SAFE_DELETE(v);
    }
    m_setTask.clear();
    __LEAVE_FUNCTION
}

bool CPlayerTask::Init(CPlayer* pPlayer)
{
    __ENTER_FUNCTION
    CHECKF(pPlayer);
    m_pOwner = pPlayer;

    auto pDB = SceneService()->GetGameDB(m_pOwner->GetWorldID());
    CHECKF(pDB);
    auto pResult = pDB->QueryKey<TBLD_TASK, TBLD_TASK::USERID>(m_pOwner->GetID());
    if(pResult)
    {
        for(size_t i = 0; i < pResult->get_num_row(); i++)
        {
            auto             row       = pResult->fetch_row(true);
            CPlayerTaskData* pTaskData = CreateNew<CPlayerTaskData>(std::move(row));
            if(pTaskData == nullptr)
            {
                LOGERROR("load player {} task:{} fail", m_pOwner->GetID(), i);
                continue;
            }
            auto pType = TaskTypeSet()->QueryObj(pTaskData->GetTaskID());
            if(pType == nullptr)
            {
                LOGERROR("load player {} taskid:{} is already deleted!!!!", m_pOwner->GetID(), pTaskData->GetTaskID());
                pTaskData->DelRecord();
                SAFE_DELETE(pTaskData);
                continue;
            }

            auto it = m_setTask.find(pTaskData->GetTaskID());
            if(it != m_setTask.end())
            {
                LOGERROR("load player {} taskid:{} twice!!!!", m_pOwner->GetID(), pTaskData->GetTaskID());
                auto pOldTaskData = it->second;
                if(pOldTaskData->IsTaskDoing() && pType->HasFlag(TASKFLAG_HIDE) == false)
                {
                    --m_nCurAcceptNum;
                }
                RemoveTaskPhase(pOldTaskData);
                pOldTaskData->DelRecord();
                SAFE_DELETE(pOldTaskData);
            }

            m_setTask[pTaskData->GetTaskID()] = pTaskData;
            if(pTaskData->IsTaskDoing())
            {
                AddTaskPhase(pTaskData);
                if(pType->HasFlag(TASKFLAG_HIDE) == false)
                {
                    ++m_nCurAcceptNum;
                }
            }
        }
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

CPlayerTaskData* CPlayerTask::QueryTaskData(uint32_t idTask)
{
    __ENTER_FUNCTION
    auto it = m_setTask.find(idTask);
    if(it != m_setTask.end())
    {
        return it->second;
    }
    __LEAVE_FUNCTION
    return nullptr;
}

void CPlayerTask::AddTaskPhase(CPlayerTaskData* pData)
{
    CHECK(pData);
    auto pType = TaskTypeSet()->QueryObj(pData->GetTaskID());
    CHECK(pType);
    if(pType->GetTaskPhaseID() != 0)
    {
        m_pOwner->AddTaskPhase(pType->GetTaskPhaseID());
    }
}

void CPlayerTask::RemoveTaskPhase(CPlayerTaskData* pData)
{
    CHECK(pData);
    auto pType = TaskTypeSet()->QueryObj(pData->GetTaskID());
    CHECK(pType);
    if(pType->GetTaskPhaseID() != 0)
    {
        m_pOwner->RemoveTaskPhase(pType->GetTaskPhaseID());
    }
}

bool CPlayerTask::AcceptTask(uint32_t idTask, bool bChkCondition /*= true*/, bool bIgnoreChkNum /*= false*/)
{
    __ENTER_FUNCTION
    auto pType = TaskTypeSet()->QueryObj(idTask);
    CHECKF(pType);

    if(bIgnoreChkNum == false && pType->HasFlag(TASKFLAG_HIDE) == false)
    {
        if(m_nCurAcceptNum > MAX_ACCEPT_TASK)
            return false;
    }

    CPlayerTaskData* pData = QueryTaskData(idTask);
    if(pData)
    {
        if(pData->GetState() == TASKSTATE_FINISHED)
        {
            if(pType->HasFlag(TASKFLAG_REPEATABLE) == false)
                return false;
        }
        else
        {
            // 进行中的任务，不能重复接
            if(pData->IsExpire() == false && pData->GetState() != TASKSTATE_GIVEUP)
                return false;
        }

        uint32_t now = TimeGetSecond();
        if(CheckSameDay(pData->GetFinishTime(), now) == false)
        {
            // 重置完成次数
            pData->SetDayCount(0, UPDATE_FALSE);
            pData->SetDayCountMax(pType->GetDayTimes(), UPDATE_TRUE);
            if(pType->HasFlag(TASKFLAG_HIDE) == false)
                SendTaskInfo(pData);
        }

        if(pData->GetDayCount() >= pData->GetDayCountMax())
        {
            return false;
        }
    }
    else
    {
        // 任务条件检测
        if(bChkCondition)
        {
            if(!CanAccept(pType))
                return false;
        }

        // 创建新任务记录
        pData = CreateNew<CPlayerTaskData>(m_pOwner, idTask);
        CHECKF(pData);
        m_setTask[idTask] = pData;
    }

    if(pType->HasFlag(TASKFLAG_ACCEPT_ADDCOUNT))
    {
        pData->SetDayCount(pData->GetDayCount() + 1, UPDATE_FALSE);
    }
    uint32_t now = TimeGetSecond();
    pData->SetAcceptTime(now, UPDATE_FALSE);
    pData->SetFinishTime(0, UPDATE_FALSE);
    pData->SetAcceptUserLev(m_pOwner->GetLev(), UPDATE_FALSE);
    if(pType->GetExpireSec() > 0)
        pData->SetExpireTime(pType->GetExpireSec() + now, UPDATE_FALSE);
    else if(pType->HasFlag(TASKFLAG_NEXTDAY_EXPIRE))
        pData->SetExpireTime(GetNextDayBeginTime(), UPDATE_FALSE);

    pData->SetState(TASKSTATE_ACCEPTED);
    for(uint32_t i = 0; i < MAX_TASKDATA_NUM; i++)
        pData->SetNum(i, 0, UPDATE_FALSE);

    if(pType->HasFlag(TASKFLAG_HIDE) == false)
    {
        ++m_nCurAcceptNum;
    }
    AddTaskPhase(pData);

    ScriptManager()->TryExecScript<void>(SCRIPT_TASK, pType->GetScriptID(), "OnAccept", m_pOwner, pData);

    for(int32_t i = 0; i < pType->GetDataRef().finish_data_list_size(); i++)
    {
        const auto& v = pType->GetDataRef().finish_data_list(i);
        switch(v.finish_type())
        {
            case TFTYPE_ITEM:
            {
                uint32_t nCount = m_pOwner->GetBag()->GetItemTypeAmount(v.finish_data());
                pData->SetNum(i, nCount, UPDATE_FALSE);
            }
            break;
            case TFTYPE_ACHI:
            {
                // if(m_pOwner->GetAchievement()->TestFinish(v.finish_data()) == true)
                //	pData->SetNum(i, 1, false);
            }
            break;
            case TFTYPE_FINISHTASK:
            {
                //本任务不能是循环任务，才可以把以前的数据统计进来
                if(pType->HasFlag(TASKFLAG_REPEATABLE) == false)
                {
                    //检查是否曾经已经完成了
                    uint32_t         idTargetTask = v.finish_data();
                    CPlayerTaskData* pTargetTask  = QueryTaskData(idTargetTask);
                    if(pTargetTask)
                    {
                        auto pTargetTaskType = TaskTypeSet()->QueryObj(idTargetTask);
                        if(pTargetTaskType->HasFlag(TASKFLAG_REPEATABLE))
                        {
                            if(CheckSameDay(pTargetTask->GetFinishTime(), TimeGetSecond()) == true)
                            {
                                //完成过任务,则把今天的计数设置为任务目标计数
                                pData->SetNum(i, pTargetTask->GetDayCount(), UPDATE_FALSE);
                            }
                        }
                        else if(pTargetTask->GetState() == TASKSTATE_FINISHED)
                        {
                            //主线任务已完成，则直接设置为已完成
                            pData->SetNum(i, 1, UPDATE_FALSE);
                        }
                    }
                }
            }
            break;
        }
    }
    pData->SaveInfo();

    if(pType->HasFlag(TASKFLAG_HIDE) == false)
        SendTaskInfo(pData);

    if(pType->GetNextAuotAcceptTaskID() != 0)
        return AcceptTask(pType->GetNextAuotAcceptTaskID(), true, false);
    return true;
    __LEAVE_FUNCTION
    return false;
}
bool CPlayerTask::SubmitTaskByMessage(uint32_t idTask, uint32_t nSubmitMultiple)
{
    __ENTER_FUNCTION
    auto pType = TaskTypeSet()->QueryObj(idTask);
    CHECKF(pType);
    if(pType->HasFlag(TASKFLAG_MSGSUBMIT) == false)
        return false;
    return SubmitTask(idTask, nSubmitMultiple);
    __LEAVE_FUNCTION
    return false;
}
bool CPlayerTask::SubmitTask(uint32_t idTask, uint32_t nSubmitMultiple)
{
    __ENTER_FUNCTION
    CPlayerTaskData* pData = QueryTaskData(idTask);
    CHECKF_V(pData, idTask);
    auto pType = TaskTypeSet()->QueryObj(idTask);
    CHECKF_V(pType, idTask);

    CHECKF_V(pData->GetState() == TASKSTATE_ACCEPTED, pData->GetState());

    if(CanSubmit(idTask) == false)
        return false;

    if(pType->HasFlag(TASKFLAG_REPEATABLE))
    {
        uint32_t now = TimeGetSecond();
        if(CheckSameDay(pData->GetAcceptTime(), now) == false)
        {
            // 重置完成次数
            pData->SetDayCount(0, UPDATE_FALSE);
            pData->SetDayCountMax(pType->GetDayTimes(), UPDATE_TRUE);
        }
    }

    if(pType->HasFlag(TASKFLAG_SUBMIT_ADDCOUNT))
    {
        pData->SetDayCount(pData->GetDayCount() + 1, UPDATE_FALSE);
    }
    pData->SetFinishTime(TimeGetSecond(), UPDATE_FALSE);
    pData->SetState(TASKSTATE_FINISHED, UPDATE_TRUE);
    if(pType->HasFlag(TASKFLAG_HIDE) == false)
    {
        m_nCurAcceptNum--;
        SendTaskInfo(pData);
    }
    RemoveTaskPhase(pData);

    if(nSubmitMultiple > 0 && nSubmitMultiple <= pType->GetSubmitMultipleMax() && pType->GetSubmitMultipleCost() > 0)
    {
        if(m_pOwner->CheckMoney(MONEY_TYPE::MT_GOLD, pType->GetSubmitMultipleCost() * nSubmitMultiple) == false)
        {
            nSubmitMultiple = 0;
        }
        else
        {
            m_pOwner->SpendMoney(MONEY_TYPE::MT_GOLD, pType->GetSubmitMultipleCost() * nSubmitMultiple);
        }
    }

    ScriptManager()->TryExecScript<void>(SCRIPT_TASK, pType->GetScriptID(), "OnCommit", m_pOwner, pData, nSubmitMultiple);

    //给予奖励
    if(pType->GetAwardExp() > 0)
        m_pOwner->AwardExp(pType->GetAwardExp() * (1 + nSubmitMultiple));
    //物品
    for(int32_t i = 0; i < pType->GetDataRef().award_item_list_size(); i++)
    {
        const auto& v = pType->GetDataRef().award_item_list(i);
        m_pOwner->AwardItem(0, v.itemtype(), v.itemamount() * (1 + nSubmitMultiple), v.itemflag());
    }

    m_pOwner->GetAchievement()->CheckAchiCondition(CONDITION_TASK, idTask);

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTask::QuickFinish(uint32_t idTask)
{
    __ENTER_FUNCTION
    CPlayerTaskData* pData = QueryTaskData(idTask);
    CHECKF_V(pData, idTask);
    auto pType = TaskTypeSet()->QueryObj(idTask);
    CHECKF_V(pType, idTask);

    CHECKF_V(pData->GetState() == TASKSTATE_ACCEPTED, pData->GetState());

    CHECKF_V(pType->GetQuickSubmitCost() > 0, idTask);

    if(m_pOwner->SpendMoney(MONEY_TYPE::MT_GOLD, pType->GetQuickSubmitCost()) == false)
    {
        return false;
    }

    for(int32_t i = 0; i < pType->GetDataRef().finish_data_list_size(); i++)
    {
        const auto& v = pType->GetDataRef().finish_data_list(i);
        pData->SetNum(i, v.finish_num(), UPDATE_FALSE);
        SendTaskDataChange(pData, i);
    }
    pData->SaveInfo();

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTask::GiveupTask(uint32_t idTask)
{
    __ENTER_FUNCTION

    CPlayerTaskData* pData = QueryTaskData(idTask);
    CHECKF_V(pData, idTask);
    auto pTaskType = TaskTypeSet()->QueryObj(idTask);
    CHECKF_V(pTaskType, idTask);

    CHECKF_V(pData->GetState() == TASKSTATE_ACCEPTED, pData->GetState());
    CHECKF_V(pTaskType->HasFlag(TASKFLAG_CAN_GIVEUP), idTask);

    pData->SetFinishTime(TimeGetSecond(), UPDATE_FALSE);
    pData->SetState(TASKSTATE_FINISHED, UPDATE_TRUE);
    RemoveTaskPhase(pData);

    ScriptManager()->TryExecScript<void>(SCRIPT_TASK, pTaskType->GetScriptID(), "OnGiveup", m_pOwner, pData);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTask::CanAccept(uint32_t idTask)
{
    __ENTER_FUNCTION

    auto pTaskType = TaskTypeSet()->QueryObj(idTask);
    CHECKF_V(pTaskType, idTask);
    return CanAccept(pTaskType);
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTask::CanAccept(const CTaskType* pType)
{
    __ENTER_FUNCTION
    if(m_pOwner->GetLev() < pType->GetLevReq())
        return false;

    if(HasFlag(pType->GetProfReq(), m_pOwner->GetProf()) == false)
        return false;

    for(uint32_t idTaskReq: pType->GetTaskReqList())
    {
        if(IsFinished(idTaskReq) == false)
            return false;
    }

    if(pType->GetScriptID() != 0)
    {
        bool find = ScriptManager()->QueryScriptFunc(SCRIPT_TASK, pType->GetScriptID(), "CanAccept");
        if(find)
        {
            return ScriptManager()->ExecStackScriptFunc<bool>(m_pOwner);
        }
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPlayerTask::OnFinishAchi(uint32_t idAchi)
{
    __ENTER_FUNCTION
    for(auto& [k, pTaskData]: m_setTask)
    {
        if(pTaskData->IsTaskDoing() == false)
            continue;

        auto pType = TaskTypeSet()->QueryObj(pTaskData->GetTaskID());
        if(pType == nullptr)
            continue;

        for(int32_t i = 0; i < pType->GetDataRef().finish_data_list_size(); i++)
        {
            const auto& v = pType->GetDataRef().finish_data_list(i);
            if(v.finish_type() == TFTYPE_ACHI && v.finish_data() == idAchi)
            {
                pTaskData->SetNum(i, 1, UPDATE_TRUE);
                SendTaskDataChange(pTaskData, i);
            }
        }
    }
    __LEAVE_FUNCTION
}

bool CPlayerTask::CanSubmit(const CTaskType* pTaskType)
{
    __ENTER_FUNCTION
    CPlayerTaskData* pData = QueryTaskData(pTaskType->GetID());
    CHECKF_V(pData, pTaskType->GetID());

    if(pData->GetState() != TASKSTATE_ACCEPTED)
        return false;

    if(pData->IsExpire())
        return false;
    //先检查
    for(int32_t i = 0; i < pTaskType->GetDataRef().finish_data_list_size(); i++)
    {
        const auto& v = pTaskType->GetDataRef().finish_data_list(i);
        if(pData->GetNum(i) < v.finish_num())
            return false;
        switch(v.finish_type())
        {
            case TFTYPE_ITEM:
            {
                CHECKF(m_pOwner->GetBag()->GetItemTypeAmount(v.finish_data(), 0) >= v.finish_num());
            }
            break;
            default:
                break;
        }
    }

    //额外的扣物品
    if(pTaskType->HasFlag(TASKFLAG_SUBMIT_DELITEM))
        for(int32_t i = 0; i < pTaskType->GetDataRef().finish_data_list_size(); i++)
        {
            const auto& v = pTaskType->GetDataRef().finish_data_list(i);
            switch(v.finish_type())
            {
                case TFTYPE_ITEM:
                {
                    CHECKF(m_pOwner->GetBag()->DelItemByType(v.finish_data(), v.finish_num(), 0, true));
                }
                break;
                default:
                    break;
            }
        }

    if(pTaskType->GetScriptID() != 0)
    {
        auto find = ScriptManager()->QueryScriptFunc(SCRIPT_TASK, pTaskType->GetScriptID(), "CanCommit");
        if(find)
        {
            return ScriptManager()->ExecStackScriptFunc<bool>(m_pOwner, pData);
        }
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}
bool CPlayerTask::CanSubmit(uint32_t idTask)
{
    __ENTER_FUNCTION
    auto pType = TaskTypeSet()->QueryObj(idTask);
    CHECKF_V(pType, idTask);
    return CanSubmit(pType);

    __LEAVE_FUNCTION
    return false;
}

int32_t CPlayerTask::GetLeftTimes(uint32_t idTask)
{
    __ENTER_FUNCTION
    CPlayerTaskData* pData = QueryTaskData(idTask);
    CHECKF_V(pData, idTask);
    auto pType = TaskTypeSet()->QueryObj(idTask);
    CHECKF_V(pType, idTask);

    if(pType->HasFlag(TASKFLAG_REPEATABLE))
    {
        uint32_t now = TimeGetSecond();
        if(CheckSameDay(pData->GetFinishTime(), now) == false)
        {
            // 重置完成次数
            pData->SetDayCount(0, UPDATE_FALSE);
            pData->SetDayCountMax(pType->GetDayTimes(), UPDATE_TRUE);
        }
    }

    return pData->GetDayCountMax() - pData->GetDayCount();
    __LEAVE_FUNCTION
    return 0;
}

void CPlayerTask::SaveInfo()
{
    __ENTER_FUNCTION
    for(auto& [k, pTaskData]: m_setTask)
    {
        if(pTaskData)
            pTaskData->SaveInfo();
    }
    __LEAVE_FUNCTION
}

void CPlayerTask::SendTaskInfo()
{
    __ENTER_FUNCTION
    SC_TASK_INFO msg;
    for(auto& [k, pTaskData]: m_setTask)
    {
        if(pTaskData)
        {
            auto pData = msg.add_task_info_list();
            pData->set_task_id(pTaskData->GetTaskID());
            pData->set_state(pTaskData->GetState());
            pData->set_accept_time(pTaskData->GetAcceptTime());
            pData->set_finish_time(pTaskData->GetFinishTime());
            pData->set_expire_time(pTaskData->GetExpireTime());
            pData->set_daycount(pTaskData->GetDayCount());
            pData->set_daycount_max(pTaskData->GetDayCountMax());
            pData->set_num1(pTaskData->GetNum(0));
            pData->set_num2(pTaskData->GetNum(1));
            pData->set_num3(pTaskData->GetNum(2));
            pData->set_num4(pTaskData->GetNum(3));
            if(msg.task_info_list_size() > 50)
            {
                m_pOwner->SendMsg(msg);
                msg.clear_task_info_list();
            }
        }
    }

    if(msg.task_info_list_size() > 0)
        m_pOwner->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CPlayerTask::SendTaskDataChange(CPlayerTaskData* pTaskData, uint32_t i)
{
    __ENTER_FUNCTION
    SC_TASK_DATA msg;
    msg.set_task_id(pTaskData->GetTaskID());
    msg.set_idx(i);
    msg.set_num(pTaskData->GetNum(i));
    m_pOwner->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CPlayerTask::SendTaskInfo(CPlayerTaskData* pTaskData)
{
    __ENTER_FUNCTION
    SC_TASK_INFO msg;
    auto         pData = msg.add_task_info_list();
    pData->set_task_id(pTaskData->GetTaskID());
    pData->set_state(pTaskData->GetState());
    pData->set_accept_time(pTaskData->GetAcceptTime());
    pData->set_finish_time(pTaskData->GetFinishTime());
    pData->set_expire_time(pTaskData->GetExpireTime());
    pData->set_daycount(pTaskData->GetDayCount());
    pData->set_daycount_max(pTaskData->GetDayCountMax());
    pData->set_num1(pTaskData->GetNum(0));
    pData->set_num2(pTaskData->GetNum(1));
    pData->set_num3(pTaskData->GetNum(2));
    pData->set_num4(pTaskData->GetNum(3));
    m_pOwner->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CPlayerTask::OnAwardTaskItem(uint32_t idItemType, uint32_t nNum)
{
    __ENTER_FUNCTION

    for(auto& [k, pTaskData]: m_setTask)
    {
        if(pTaskData->IsTaskDoing() == false)
            continue;

        auto pType = TaskTypeSet()->QueryObj(pTaskData->GetTaskID());
        if(pType == nullptr)
            continue;

        for(int32_t i = 0; i < pType->GetDataRef().finish_data_list_size(); i++)
        {
            const auto& v = pType->GetDataRef().finish_data_list(i);
            if(v.finish_type() == TFTYPE_ITEM && v.finish_data() == idItemType)
            {
                pTaskData->SetNum(i, pTaskData->GetNum(i) + nNum, UPDATE_TRUE);
                SendTaskDataChange(pTaskData, i);
            }
        }
    }
    __LEAVE_FUNCTION
}

void CPlayerTask::OnKillMonster(uint32_t idMonster, bool bKillBySelf)
{
    __ENTER_FUNCTION

    for(auto& [k, pTaskData]: m_setTask)
    {
        if(pTaskData->IsTaskDoing() == false)
            continue;

        auto pType = TaskTypeSet()->QueryObj(pTaskData->GetTaskID());
        if(pType == nullptr)
            continue;

        for(int32_t i = 0; i < pType->GetDataRef().finish_data_list_size(); i++)
        {
            const auto& v = pType->GetDataRef().finish_data_list(i);
            if(v.finish_type() == TFTYPE_MONSTER && v.finish_data() == idMonster &&
               (bKillBySelf == true || (bKillBySelf == false && v.share() == false)) && (v.rate() != 0 && random_hit(v.rate()) == true))
            {
                pTaskData->SetNum(i, pTaskData->GetNum(i) + 1, UPDATE_TRUE);
                SendTaskDataChange(pTaskData, i);
            }
        }
    }
    __LEAVE_FUNCTION
}

void CPlayerTask::OnDelTaskItem(uint32_t idItemType, uint32_t nNum)
{
    __ENTER_FUNCTION

    for(auto& [k, pTaskData]: m_setTask)
    {
        if(pTaskData->IsTaskDoing() == false)
            continue;

        auto pType = TaskTypeSet()->QueryObj(pTaskData->GetTaskID());
        if(pType == nullptr)
            continue;

        for(int32_t i = 0; i < pType->GetDataRef().finish_data_list_size(); i++)
        {
            const auto& v = pType->GetDataRef().finish_data_list(i);
            if(v.finish_type() == TFTYPE_ITEM && v.finish_data() == idItemType)
            {
                pTaskData->SetNum(i, pTaskData->GetNum(i) - nNum, UPDATE_TRUE);
                SendTaskDataChange(pTaskData, i);
            }
        }
    }

    __LEAVE_FUNCTION
}

bool CPlayerTask::ShowTaskDialog(uint32_t idTask, OBJID idNpc)
{
    __ENTER_FUNCTION
    auto pTaskType = TaskTypeSet()->QueryObj(idTask);
    if(pTaskType->GetScriptID() != 0)
    {
        auto find = ScriptManager()->QueryScriptFunc(SCRIPT_TASK, pTaskType->GetScriptID(), "ShowTaskDialog");
        if(find)
        {
            ScriptManager()->ExecStackScriptFunc<void>(m_pOwner);
            return true;
        }
    }
    auto dialog    = m_pOwner->GetDialog();
    auto pTaskData = QueryTaskData(idTask);
    if(pTaskData == nullptr)
    {
        //接任务对话框

        dialog->DialogBegin(pTaskType->GetName());
        dialog->DialogAddText(pTaskType->GetAcceptText());

        dialog->DialogAddLink(DIALOGLINK_TYPE_BUTTON,
                              STR_TASK_ACCEPT,
                              DIALOG_FUNC_ACCEPTTASK,
                              MAKE64(0, idTask),
                              "",
                              (pTaskType->HasFlag(TASKFLAG_AUTO_ACITVE_NPC) ? idNpc : 0));
        dialog->DialogSend(DIALOGTYPE_ACCEPT_TASK);
        return true;
    }

    if(CanSubmit(idTask) == false)
    {
        //还没完成对话框

        dialog->DialogBegin(pTaskType->GetName());
        dialog->DialogAddText(pTaskType->GetDoingText());
        //允许快速提交
        if(pTaskType->GetQuickSubmitCost() > 0)
        {
            //快速提交
            dialog->DialogAddLink(DIALOGLINK_TYPE_BUTTON,
                                  STR_TASK_QUICK_FINISH,
                                  DIALOG_FUNC_QUICKFINISHTASK,
                                  MAKE64(0, idTask),
                                  "",
                                  (pTaskType->HasFlag(TASKFLAG_AUTO_ACITVE_NPC) ? idNpc : 0));
        }

        dialog->DialogSend(DIALOGTYPE_WAITFINISH_TASK);
        return true;
    }
    else
    {
        //交任务对话框
        dialog->DialogBegin(pTaskType->GetName());
        dialog->DialogAddText(pTaskType->GetSubmitText());
        //多倍提交
        for(uint32_t i = 0; i <= pTaskType->GetSubmitMultipleMax(); i++)
        {
            dialog->DialogAddLink(DIALOGLINK_TYPE_BUTTON,
                                  STR_TASK_SUBMIT[i],
                                  DIALOG_FUNC_SUBMITTASK,
                                  MAKE64(i, idTask),
                                  "",
                                  (pTaskType->HasFlag(TASKFLAG_AUTO_ACITVE_NPC) ? idNpc : 0));
        }
        dialog->DialogSend(DIALOGTYPE_SUBMIT_TASK);
        return true;
    }

    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTask::IsFinished(uint32_t idTask)
{
    __ENTER_FUNCTION
    CPlayerTaskData* pTaskData = QueryTaskData(idTask);
    if(pTaskData)
    {
        return pTaskData->GetState() == TASKSTATE_FINISHED;
    }
    return false;
    __LEAVE_FUNCTION
    return false;
}
