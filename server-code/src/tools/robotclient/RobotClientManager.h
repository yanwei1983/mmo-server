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
class RobotClientManager : public CNetworkService
{
public:
    RobotClientManager(uint32_t nRobStart, uint32_t nRobAmount, const std::string& lua_file_name);
    ~RobotClientManager();

public:
    RobotClient*    ConnectServer(const char* addr, int32_t port);
    void            DelClient(RobotClient* pClient);
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

private:
    std::unique_ptr<CNetMSGProcess> m_pNetMsgProcess;
    std::unique_ptr<CEventManager>  m_pEventManager;
    std::set<RobotClient*>          m_setClient;
    CEventEntryPtr                  m_Event;

    std::unordered_map<uint16_t, std::string> m_CMDFuncMap;
    std::unique_ptr<CLUAScriptManager>        m_pScriptManager;
};

#endif // RobotClient_h__
