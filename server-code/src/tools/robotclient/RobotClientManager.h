#ifndef RobotClient_h__
#define RobotClient_h__

#include <deque>
#include <functional>
#include <vector>

#include "EventManager.h"
#include "NetSocket.h"
#include "NetworkService.h"
#include "ScriptManager.h"
class CNetMSGProcess;
class RobotClient;
using RobotClientPtr = std::shared_ptr<RobotClient>;
class RobotClientManager : public CNetworkService
{
public:
    RobotClientManager(uint32_t nRobStart, uint32_t nRobAmount, const std::string& lua_file_name);
    ~RobotClientManager();

public:
    RobotClientPtr  ConnectServer(const char* addr, int32_t port);
    void            DelClient(const RobotClientPtr& pClient);
    size_t          GetClientCount()const {return m_setClient.size();}
    CNetMSGProcess* GetNetMessageProcess() const { return m_pNetMsgProcess.get(); }
    CEventManager*  GetEventManager() const { return m_pEventManager.get(); }

    CLUAScriptManager* GetScriptManager() { return m_pScriptManager.get(); }

    template<typename RVal, typename... Args>
    RVal ExecScript(const char* pszFuncName, Args&&... args)
    {
        m_pScriptManager->_ExecScript<RVal>(pszFuncName, std::forward<Args>(args)...);
    }

    void RegisterCMD(uint16_t cmd, const std::string& func) { m_CMDFuncMap[cmd] = func; }

    const std::string& GetProcessCMD(uint16_t cmd) const
    {
        auto it = m_CMDFuncMap.find(cmd);
        if(it == m_CMDFuncMap.end())
        {
            static const std::string s_Empty;
            return s_Empty;
        }

        return it->second;
    }

    void AddTimedCallback(uint32_t tIntervalMS, const std::string& func_name, bool bPersist);

private:
    std::unique_ptr<CNetMSGProcess> m_pNetMsgProcess;
    std::unique_ptr<CEventManager>  m_pEventManager;
    std::set<RobotClientPtr>        m_setClient;
    CEventEntryPtr                  m_EventScriptGC;
    CEventEntryQueue                m_EventList;
    std::unordered_map<uint16_t, std::string> m_CMDFuncMap;
    std::unique_ptr<CLUAScriptManager>        m_pScriptManager;
};

#endif // RobotClient_h__
