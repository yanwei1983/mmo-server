#include "AOIPhase.h"

#include "AOIActor.h"
#include "AOIActorManager.h"

#include "AOIScene.h"
#include "AOIService.h"
#include "MapManager.h"

#include "config/Cfg_Scene.pb.h"
#include "config/Cfg_Phase.pb.h"
CAOIPhase::CAOIPhase() {}

CAOIPhase::~CAOIPhase()
{
    LOGDEBUG("PhaseDestory:{} {} idPhase:{}", GetSceneIdx().GetMapID(), GetSceneIdx().GetPhaseIdx(), m_idPhase);
    while(m_setActor.empty() == false)
    {
        CAOIActor* pActor = static_cast<CAOIActor*>(m_setActor.begin()->second);
        LeaveMap(pActor);
        AOIActorManager()->DelActor(pActor);
    }
}

bool CAOIPhase::Init(CAOIScene* pScene, const SceneIdx& idxScene, uint64_t idPhase, const Cfg_Phase* pPhaseData)
{
    __ENTER_FUNCTION
    m_idPhase = idPhase;
    CHECKF(CSceneBase::Init(idxScene, MapManager()));

    
    if(pPhaseData)
    {
        uint16_t idPhaseLink = pPhaseData->link_phase();
        auto     pPhase      = pScene->QueryPhaseByID(idPhaseLink);
        if(pPhase)
        {
            CHECKF(LinkSceneTree(pPhase));
        }
        else
        {
            CPos2D vBasePos{pPhaseData->left(), pPhaseData->top()};
            float  fWidth  = pPhaseData->right() - pPhaseData->left();
            float  fHeight = pPhaseData->bottom() - pPhaseData->top();
            CHECKF(InitSceneTree(vBasePos, fWidth, fHeight, pPhaseData->viewgrid_width(), true));
        }
    }
    else if(idPhase == 0)
    {
        CHECKF(InitSceneTree({0.0f, 0.0f}, 0.0f, 0.0f, 0, true));
    }
    else
    {
        CHECKF(InitSceneTree({0.0f, 0.0f}, 0.0f, 0.0f, 0, true));
    }

    LOGDEBUG("CAOIPhase {} Created Map:{} Idx:{}", idPhase, idxScene.GetMapID(), idxScene.GetPhaseIdx());
    return true;

    __LEAVE_FUNCTION
    return false;
}