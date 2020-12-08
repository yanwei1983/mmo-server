#ifndef MONITOR_MGR_H
#define MONITOR_MGR_H

#include <chrono>
#include <map>

#include "BaseCode.h"

#define TICK_EVAL(Func)                                                                                                  \
    ({                                                                                                                   \
        auto beginTime = std::chrono::high_resolution_clock::now();                                                      \
        Func;                                                                                                            \
        auto endTime = std::chrono::high_resolution_clock::now();                                                        \
                                                                                                                         \
        std::chrono::microseconds costTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime); \
        MonitorMgr()->Add(#Func, costTime.count());                                                                      \
    })

struct RecvMsgNode
{
    int32_t                                        iCount;
    std::chrono::high_resolution_clock::time_point nStartFrequence;
    uint64_t                                       nTotalFrequence;
};

//监视下发消息
struct SendToClientMsgNode
{
    SendToClientMsgNode()
        : iCount(0)
        , iUserCount(0)
        , bytes_size(0)
    {
    }
    uint32_t iCount;     //发送累积数量
    uint32_t iUserCount; //发送user数量
    uint32_t bytes_size; //发送字节累积数
};

class CMonitorMgr
{
    CMonitorMgr() {}

public:
    ~CMonitorMgr() { Uninit(); }
    CreateNewImpl(CMonitorMgr);

    bool Init() { return true; }
    void Uninit()
    {
        m_TickEvalMap.clear();
        m_RecvMsgMap.clear();
        m_SendToClientMsgMap.clear();
        m_SendToClientMsgMap_broad.clear();
        m_SendToClientMsgMap_some.clear();
    }
    void CmdProcessStart(uint32_t iCmdID);
    void CmdProcessEnd(uint32_t iCmdID);
    void CmdProcessAdd(uint32_t iCmdID);
    //记录下发消息
    // para uint32_t bytes_size 本次字节数
    void AddSendInfo(uint32_t iCmdID, uint32_t bytes_size);
    void AddSendInfo_broad(uint32_t iCmdID, uint32_t bytes_size);
    void AddSendInfo_some(uint32_t iCmdID, uint32_t bytes_size, uint32_t count);

    void Print();
    void Add(const char* name, uint32_t uTime);
    void Tick10s();

private:
    void PrintSendInfo(); //打印下发消息信息

private:
    typedef std::map<uint32_t, RecvMsgNode>           RecvMsgMap;
    typedef std::map<uint32_t, RecvMsgNode>::iterator RecvMsgIter;
    typedef std::map<uint32_t, SendToClientMsgNode>   SendToClientMsgMap;

    typedef std::map<const char*, uint32_t>           TickEvalMap;
    typedef std::map<const char*, uint32_t>::iterator TickEvalIter;

    TickEvalMap        m_TickEvalMap;
    RecvMsgMap         m_RecvMsgMap;
    SendToClientMsgMap m_SendToClientMsgMap;
    SendToClientMsgMap m_SendToClientMsgMap_broad; //广播
    SendToClientMsgMap m_SendToClientMsgMap_some;  //多发
};

#endif /* MONITOR_MGR_H */
