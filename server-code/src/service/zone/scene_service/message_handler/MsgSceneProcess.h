#ifndef MSGZONEPROCESS_H
#define MSGZONEPROCESS_H

#include "ActorManager.h"
#include "MsgProcessRegister.h"
#include "NetworkMessage.h"
#include "Player.h"
#include "SceneService.h"

template<class MsgType, class FuncType>
void ProcPlayerMsg(CNetworkMessage* pMsg, FuncType func)
{
    __ENTER_FUNCTION

    CPlayer* pPlayer = ActorManager()->QueryPlayer(pMsg->GetFrom());
    CHECK(pPlayer);
    MsgType msg;
    if(msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
    {
        return;
    }
    std::invoke(func, pPlayer, msg, pMsg);

    __LEAVE_FUNCTION
}

#define ON_PLAYERMSG(MsgType)                                                                                    \
    void OnMsg_##MsgType(CPlayer* pPlayer, const MsgType& msg, CNetworkMessage* pMsg);                           \
                                                                                                                 \
    MsgProcRegister<CSceneService> register_##MsgType(                                                           \
        CMD_##MsgType,                                                                                           \
        TO_CSTR(CMD_##MsgType),                                                                                  \
        std::bind(&ProcPlayerMsg<MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)); \
                                                                                                                 \
    void OnMsg_##MsgType(CPlayer* pPlayer, const MsgType& msg, CNetworkMessage* pMsg)

#endif /* MSGZONEPROCESS_H */
