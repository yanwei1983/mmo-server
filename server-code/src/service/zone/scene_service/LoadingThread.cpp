#include "LoadingThread.h"

#include "ActorManager.h"
#include "Player.h"
#include "SceneService.h"

CLoadingThread::CLoadingThread() {}

CLoadingThread::~CLoadingThread()
{
    Destory();
}

bool CLoadingThread::Init(CSceneService* pZoneRef)
{
    m_pZone  = pZoneRef;
    m_Thread = std::make_unique<CNormalThread>(50,
                                               pZoneRef->GetServiceName() + "_Loading",
                                               std::bind(&CLoadingThread::OnThreadProcess, this),
                                               std::bind(&CLoadingThread::OnThreadCreate, this),
                                               std::bind(&CLoadingThread::OnThreadExit, this));
    return true;
}

void CLoadingThread::Destory()
{
    __ENTER_FUNCTION
    LOGDEBUG("CLoadingThread::Destory");
    m_bStop = true;
    m_cv.notify_one();
    if(m_Thread)
    {
        m_Thread->Stop();
        m_Thread->Join();
        m_Thread.reset();
    }
    //把剩余的东西处理完
    {
        ST_LOADINGTHREAD_PROCESS_DATA* pData = nullptr;
        for(auto& pData: m_WaitingList)
        {
            if(pData->nPorcessType == LPT_SAVE)
            {
                if(pData->pPlayer)
                {
                    pData->pPlayer->SaveInfo();
                    SAFE_DELETE(pData->pPlayer);
                }
                
            }
            SAFE_DELETE(pData);
        }
        m_WaitingList.clear();
    }

    {
        ST_LOADINGTHREAD_PROCESS_DATA* pData = nullptr;
        for(auto& pData: m_ReadyList)
        {
            SAFE_DELETE(pData);
        }
        m_ReadyList.clear();
    }
    __LEAVE_FUNCTION
}

bool CLoadingThread::AddLoginPlayer(ST_LOADINGTHREAD_PROCESS_DATA&& data)
{
    __ENTER_FUNCTION
    auto pData = new ST_LOADINGTHREAD_PROCESS_DATA(std::move(data));

    m_nLoadingCount++;
    {
        std::lock_guard locker(m_csWaitingList);
        m_WaitingList.push_back(pData);
    }
    {
        m_cv.notify_one();
    }
    return true;

    __LEAVE_FUNCTION
    return false;
}

bool CLoadingThread::AddClosePlayer(ST_LOADINGTHREAD_PROCESS_DATA&& data)
{
    __ENTER_FUNCTION
    auto pData = new ST_LOADINGTHREAD_PROCESS_DATA(std::move(data));

    m_nSaveingCount++;
    {
        std::lock_guard locker(m_csWaitingList);
        m_WaitingList.push_back(pData);
    }
    {
        m_cv.notify_one();
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CLoadingThread::CancleOnReadyList(OBJID idPlayer)
{
    __ENTER_FUNCTION
    //遍历Ready列表
    std::lock_guard locker(m_csReadyList);
    for(auto it = m_ReadyList.begin(); it != m_ReadyList.end();)
    {
        auto& pData = *it;
        if(pData == nullptr)
        {
            it = m_ReadyList.erase(it);
            continue;
        }
        if(pData->idPlayer != idPlayer)
        {
            it++;
            continue;
        }

        if(pData->nPorcessType == LPT_LOADING)
        {
            // loading ready
            LOGACTORDEBUG(idPlayer, "remove from loading_ready");
        }
        else
        {
            // save ready
            LOGACTORDEBUG(idPlayer, "remove from save_ready");
        }
        SAFE_DELETE(pData->pPlayer);
        SAFE_DELETE(pData);
        it = m_ReadyList.erase(it);
        continue;
    }

    __LEAVE_FUNCTION
}

void CLoadingThread::CancleOnWaitList(OBJID idPlayer)
{
    __ENTER_FUNCTION
    for(auto it = m_WaitingList.begin(); it != m_WaitingList.end();)
    {
        auto& pLoadData = *it;
        if(pLoadData == nullptr)
        {
            it = m_WaitingList.erase(it);
            continue;
        }
        if(pLoadData->idPlayer != idPlayer)
        {
            it++;
            continue;
        }

        if(pLoadData->nPorcessType == LPT_LOADING)
        {
            // remove this
            m_nLoadingCount--;
            LOGACTORDEBUG(idPlayer, "remove from loading" );
        }
        else
        {
            //优先写入
            if(pLoadData->pPlayer)
                pLoadData->pPlayer->SaveInfo();
            //必然是后一个顶前一个,应该已经在World被Kick了
            m_nSaveingCount--;
            LOGACTORDEBUG(idPlayer, "remove from saving");
        }
        SAFE_DELETE(pLoadData->pPlayer);
        SAFE_DELETE(pLoadData);

        it = m_WaitingList.erase(it);
        continue;
    }
    __LEAVE_FUNCTION
}

bool CLoadingThread::CancleWaiting(OBJID idPlayer)
{
    __ENTER_FUNCTION
    //这个函数必然是在主函数上调用的
    if(m_idCurProcess == idPlayer)
    {
        m_idNeedCancle = idPlayer;
    }
    //先处理已经Ready的列表
    CancleOnReadyList(idPlayer);
    //再清理等待列表
    CancleOnWaitList(idPlayer);

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CLoadingThread::OnThreadCreate()
{
    SetSceneServicePtr(m_pZone);
}

void CLoadingThread::OnThreadExit()
{
    SetSceneServicePtr(nullptr);
}

void CLoadingThread::OnThreadProcess()
{
    //处理
    while(m_bStop.load() == false)
    {
        __ENTER_FUNCTION
        ST_LOADINGTHREAD_PROCESS_DATA* pCurData = nullptr;
        // take from list
        {
            std::unique_lock<std::mutex> locker(m_csWaitingList);
            if(m_WaitingList.empty())
            {
                if(m_bStop)
                {
                    return;
                }
                m_cv.wait(locker);
                continue;
            }
            pCurData = m_WaitingList.front();
            m_WaitingList.pop_front();
        }
        if(pCurData == nullptr)
            continue;

        m_idCurProcess = pCurData->idPlayer;
        if(pCurData->nPorcessType == LPT_LOADING)
        {
            m_nLoadingCount--;
            CPlayer* pPlayer = CPlayer::CreateNew(pCurData->idPlayer, pCurData->socket);
            //如果当前处理的这个需要被cancle
            if(pPlayer == nullptr)
            {
                // log error
                LOGACTORDEBUG(pCurData->idPlayer, "LoadingFail");
                SAFE_DELETE(pCurData);
            }
            else if(m_idNeedCancle == m_idCurProcess)
            {
                // log cancle
                LOGACTORDEBUG(pPlayer->GetID(), "CacleLoading");
                SAFE_DELETE(pCurData);
                SAFE_DELETE(pPlayer);
            }
            else
            {
                //放入ready列表
                LOGACTORDEBUG(pCurData->idPlayer, "LoadingReady");

                pCurData->pPlayer = pPlayer;
                m_nReadyCount++;
                {
                    std::lock_guard locker(m_csReadyList);
                    m_ReadyList.push_back(pCurData);
                }
            }
        }
        else
        {
            m_nSaveingCount--;
            if(pCurData->pPlayer == nullptr)
            {
                SAFE_DELETE(pCurData);
            }
            else
            {
                //保存成功， 放入ready列表，准备发送ChangeZone消息给TargetZone
                pCurData->pPlayer->SaveInfo();
                LOGACTORDEBUG(pCurData->idPlayer, "SaveingReady" );
                if(pCurData->bChangeZone)
                {
                    m_nReadyCount++;
                    {
                        std::lock_guard locker(m_csReadyList);
                        m_ReadyList.push_back(pCurData);
                    }
                }
                else
                {
                    //可以直接删除了
                    SAFE_DELETE(pCurData->pPlayer);
                    SAFE_DELETE(pCurData);
                }
            }
        }

        m_idCurProcess = 0;
        m_idNeedCancle = 0;

        __LEAVE_FUNCTION
    }
}

void CLoadingThread::OnMainThreadExec()
{
    int32_t nCount = 0;
    while(true)
    {
        __ENTER_FUNCTION

        ST_LOADINGTHREAD_PROCESS_DATA* pData = nullptr;
        {
            std::lock_guard locker(m_csReadyList);
            if(m_ReadyList.empty())
                return;
            pData = m_ReadyList.front();
            m_ReadyList.pop_front();
        }
        if(pData == nullptr)
            continue;

        if(pData->nPorcessType == LPT_LOADING)
        {
            if(pData->pPlayer)
            {
                ActorManager()->AddActor(pData->pPlayer);
                pData->pPlayer->OnLogin(!pData->bChangeZone, pData->idTargetScene, pData->fPosX, pData->fPosY, pData->fRange, pData->fFace);
            }
        }
        else // save ready
        {
            pData->pPlayer->OnChangeZoneSaveFinish(pData->idTargetScene, pData->fPosX, pData->fPosY, pData->fRange, pData->fFace);
            SAFE_DELETE(pData->pPlayer);
        }
        m_nReadyCount--;
        SAFE_DELETE(pData);

        __LEAVE_FUNCTION
    }
}

size_t CLoadingThread::GetReadyCount()
{
    return m_nReadyCount;
}
