#include "NetMSGProcess.h"

#include "LoggingMgr.h"

bool CNetMSGProcess::Process(CNetworkMessage* pMsg) const
{
    __ENTER_FUNCTION
    CHECKF(pMsg);
    auto itFind = m_FuncMap.find(pMsg->GetCmd());
    if(itFind == m_FuncMap.end())
    {
        if(m_funcDefault)
        {
            LOGTRACE("MSGPROC_DEFAULT: cmd:{} from:{}", pMsg->GetCmd(), pMsg->GetFrom());
            m_funcDefault(pMsg->GetCmd(), pMsg);
            return true;
        }

        return false;
    }
    LOGTRACE("MSGPROC: cmd:{} from:{}", pMsg->GetCmd(), pMsg->GetFrom());
    (itFind->second)(pMsg);
    return true;
    __LEAVE_FUNCTION
    return false;
}
