#ifndef PLAYERTASK_H
#define PLAYERTASK_H

#include "BaseCode.h"

export_lua constexpr const char* STR_TASK_ACCEPT       = "接受任务";
export_lua constexpr const char* STR_TASK_SUBMIT[]     = {"领取奖励", "一倍奖励", "两倍奖励", "三倍奖励"};
export_lua constexpr const char* STR_TASK_QUICK_FINISH = "快速完成";

export_lua constexpr uint32_t MAX_ACCEPT_TASK = 20;

class CTaskType;
class CPlayerTaskData;
class CPlayer;
export_lua class CPlayerTask : public NoncopyableT<CPlayerTask>
{
    CPlayerTask();
    bool Init(CPlayer* pPlayer);

public:
    CreateNewImpl(CPlayerTask);

public:
    ~CPlayerTask();

    export_lua CPlayerTaskData* QueryTaskData(uint32_t idTask);

    export_lua bool AcceptTask(uint32_t idTask, bool bChkCondition = true, bool bIgnoreChkNum = false);
    export_lua bool SubmitTaskByMessage(uint32_t idTask, uint32_t nSubmitMultiple);
    export_lua bool SubmitTask(uint32_t idTask, uint32_t nSubmitMultiple);
    export_lua bool QuickFinish(uint32_t idTask);
    export_lua bool GiveupTask(uint32_t idTask);

    export_lua bool    CanAccept(uint32_t idTask);
    export_lua bool    CanAccept(const CTaskType* pType);
    export_lua bool    CanSubmit(uint32_t idTask);
    export_lua bool    CanSubmit(const CTaskType* pTaskType);
    export_lua int32_t GetLeftTimes(uint32_t idTask);
    export_lua bool    IsFinished(uint32_t idTask);

    export_lua void SaveInfo();
    export_lua void SendTaskInfo();
    export_lua void SendTaskInfo(CPlayerTaskData* pTaskData);
    export_lua void SendTaskDataChange(CPlayerTaskData* pTaskData, uint32_t i);
    export_lua bool ShowTaskDialog(uint32_t idTask, OBJID idNpc);

    void OnAwardTaskItem(uint32_t idItemType, uint32_t nNum);
    void OnKillMonster(uint32_t idMonster, bool bKillBySelf);
    void OnDelTaskItem(uint32_t idItemType, uint32_t nNum);
    void OnFinishAchi(uint32_t idAchi);

private:
    void AddTaskPhase(CPlayerTaskData* pData);
    void RemoveTaskPhase(CPlayerTaskData* pData);

private:
    CPlayer*                             m_pOwner = nullptr;
    std::set<uint32_t>                   m_setFinishedTaskID;
    std::map<uint32_t, CPlayerTaskData*> m_setTask;
    uint32_t                             m_nCurAcceptNum = 0;
};
#endif /* PLAYERTASK_H */
