#include "MonitorMgr.h"
#include "BaseCode.h"
#include "protomsg_to_cmd.h"

void CMonitorMgr::CmdProcessAdd(uint32_t iCmdID)
{
    RecvMsgIter iter = m_RecvMsgMap.find(iCmdID);
    if(m_RecvMsgMap.end() != iter)
    {
        iter->second.iCount++;
    }
    else
    {
        RecvMsgNode node;
        node.iCount          = 1;
        node.nTotalFrequence = 0;

        m_RecvMsgMap.emplace(iCmdID, node);
    }
}

void CMonitorMgr::CmdProcessStart(uint32_t iCmdID)
{
    RecvMsgIter iter = m_RecvMsgMap.find(iCmdID);
    if(m_RecvMsgMap.end() != iter)
    {
        iter->second.nStartFrequence = std::chrono::high_resolution_clock::now();
        iter->second.iCount++;
    }
    else
    {
        RecvMsgNode node;
        node.iCount          = 1;
        node.nTotalFrequence = 0;
        node.nStartFrequence = std::chrono::high_resolution_clock::now();

        m_RecvMsgMap.emplace(iCmdID, node);
    }
}

void CMonitorMgr::CmdProcessEnd(uint32_t iCmdID)
{
    RecvMsgIter iter = m_RecvMsgMap.find(iCmdID);
    if(m_RecvMsgMap.end() != iter)
    {
        std::chrono::microseconds costTime =
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - iter->second.nStartFrequence);
        iter->second.nTotalFrequence = iter->second.nTotalFrequence + costTime.count();
    }
    else
    {
        LOGERROR("invalid cmdid {}", iCmdID);
    }
}

void CMonitorMgr::AddSendInfo(uint32_t iCmdID, uint32_t bytes_size)
{
    SendToClientMsgNode& d = m_SendToClientMsgMap[iCmdID];
    d.iCount++;
    d.bytes_size += bytes_size;
}

void CMonitorMgr::AddSendInfo_broad(uint32_t iCmdID, uint32_t bytes_size)
{
    SendToClientMsgNode& d = m_SendToClientMsgMap_broad[iCmdID];
    d.iCount++;
    d.bytes_size += bytes_size;
}

void CMonitorMgr::AddSendInfo_some(uint32_t iCmdID, uint32_t bytes_size, uint32_t count)
{
    SendToClientMsgNode& d = m_SendToClientMsgMap_some[iCmdID];
    d.iCount++;
    d.iUserCount += count;
    d.bytes_size += bytes_size;
}

void CMonitorMgr::Add(const char* name, uint32_t uTime)
{
    TickEvalIter iter = m_TickEvalMap.find(name);

    if(m_TickEvalMap.end() != iter)
    {
        iter->second += uTime;
    }
    else
    {
        m_TickEvalMap.insert(std::make_pair(name, uTime));
    }
}

void CMonitorMgr::Print()
{
    // 打印监控日志
    LOGMONITOR("=========recv msg handle begin=========");
    LOGMONITOR("{:<25}{:<10}{:<15}{:<10}", "cmdid", "count", "total", "pertime"); // total 总耗时间  pertime 每次消耗时间
    for(RecvMsgIter iter = m_RecvMsgMap.begin(); iter != m_RecvMsgMap.end(); ++iter)
    {
        LOGMONITOR("{:<25}{:<10}{:<15}{:<10}",
                   cs_cmd_to_name(iter->first),
                   iter->second.iCount,
                   iter->second.nTotalFrequence,
                   iter->second.nTotalFrequence / iter->second.iCount);
    }

    LOGMONITOR("=========recv msg handle end=========");
    PrintSendInfo();
    // 清理数据
    m_RecvMsgMap.clear();

    LOGMONITOR("tick handle begin");
    // 打印tick监控日志
    for(TickEvalIter iter = m_TickEvalMap.begin(); iter != m_TickEvalMap.end(); ++iter)
    {
        LOGMONITOR("{}:{}", iter->first, iter->second);
    }

    LOGMONITOR("tick handle end");

    m_TickEvalMap.clear();
}

void CMonitorMgr::PrintSendInfo()
{
    LOGMONITOR("=========send msg handle begin=========");
    LOGMONITOR("{:<25}{:<10}{:<15}{:<10}", "cmdid", "count", "total_size", "per_size"); // total_size 总发字节数  per_size 每次字节数
    for(const auto& e: m_SendToClientMsgMap)
    {
        const SendToClientMsgNode& d = e.second;
        LOGMONITOR("{:<25}{:<10}{:<15}{:<10}", sc_cmd_to_name(e.first), d.iCount, d.bytes_size, d.bytes_size / d.iCount);
    }

    LOGMONITOR("{:<25}{:<10}{:<15}{:<10}{:<15}{:<10}", "some_cmd", "count", "total_size", "per_size", "user_count", "per_user");
    for(const auto& e: m_SendToClientMsgMap_some)
    {
        const SendToClientMsgNode& d = e.second;
        LOGMONITOR("{:<25}{:<10}{:<15}{:<10}{:<15}{:<10}",
                   sc_cmd_to_name(e.first),
                   d.iCount,
                   d.bytes_size,
                   d.bytes_size / d.iCount,
                   d.iUserCount,
                   d.iUserCount / d.iCount);
    }

    LOGMONITOR("{:<25}{:<10}{:<15}{:<10}", "board_cmd", "count", "total_size", "per_size");
    for(const auto& e: m_SendToClientMsgMap_broad)
    {
        const SendToClientMsgNode& d = e.second;
        LOGMONITOR("{:<25}{:<10}{:<15}{:<10}", sc_cmd_to_name(e.first), d.iCount, d.bytes_size, d.bytes_size / d.iCount);
    }
    m_SendToClientMsgMap_some.clear();
    m_SendToClientMsgMap_broad.clear();
    m_SendToClientMsgMap.clear();
    LOGMONITOR("=========send msg handle end=========");
}
