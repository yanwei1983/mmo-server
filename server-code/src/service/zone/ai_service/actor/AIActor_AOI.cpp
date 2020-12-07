#include "AIActor.h"
#include "AIActorManager.h"
#include "AIMonster.h"
#include "AIPlayer.h"
#include "AIScene.h"
#include "AIService.h"

//////////////////////////////////////////////////////////////////////
void CAIActor::AddToViewList(CSceneObject* pActor)
{
    __ENTER_FUNCTION
    CSceneObject::AddToViewList(pActor);

    //如果自己是怪物
    if(IsMonster())
    {
        CastTo<CAIMonster>()->SetAISleep(false);
    }
    LOGACTORDEBUG(GetID(), "ViewList add:{}", pActor->GetID());
    __LEAVE_FUNCTION
}

void CAIActor::RemoveFromViewList(CSceneObject* pActor, OBJID idActor, bool bErase)
{
    __ENTER_FUNCTION
    CSceneObject::RemoveFromViewList(pActor,idActor,bErase);

    LOGACTORDEBUG(GetID(), "ViewList del:{}", idActor);
    __LEAVE_FUNCTION
}
