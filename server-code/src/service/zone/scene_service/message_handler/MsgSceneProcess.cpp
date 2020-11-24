#include "MsgProcessRegister.h"
#include "SceneService.h"

void SceneMessageHandlerRegister()
{
    __ENTER_FUNCTION

    auto pNetMsgProcess = SceneService()->GetNetMsgProcess();
    for(const auto& [k, v]: MsgProcRegCenter<CSceneService>::instance().m_MsgProc)
    {
        pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
    }

    __LEAVE_FUNCTION
}

