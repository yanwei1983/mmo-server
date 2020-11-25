#include "AIPhase.h"

#include "AIActor.h"
#include "AIActorManager.h"
#include "AIGroup.h"
#include "AIScene.h"
#include "AIService.h"
#include "MapManager.h"
#include "MonsterGenerator.h"
#include "config/Cfg_Phase.pb.h"
#include "config/Cfg_Scene.pb.h"
CAIPhase::CAIPhase() {}

CAIPhase::~CAIPhase()
{
    LOGDEBUG("PhaseDestory:{} {} idPhase:{}", GetSceneIdx().GetMapID(), GetSceneIdx().GetPhaseIdx(), m_idPhase);
    while(m_setActor.empty() == false)
    {
        CAIActor* pActor = static_cast<CAIActor*>(m_setActor.begin()->second);
        LeaveMap(pActor);
        AIActorManager()->DelActor(pActor);
    }
}

bool CAIPhase::Init(CAIScene* pScene, const SceneIdx& idxScene, uint64_t idPhase, const Cfg_Phase* pPhaseData)
{
    __ENTER_FUNCTION
    m_idPhase = idPhase;
    CHECKF(CSceneBase::Init(idxScene, MapManager()));

    m_MonsterGen.reset(CMonsterGenerator::CreateNew(this));
    m_pAIGroupManager.reset(CAIGroupManager::CreateNew(this));
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
            CHECKF(InitSceneTree(vBasePos, fWidth, fHeight, pPhaseData->viewgrid_width()));
        }
    }
    else if(idPhase == 0)
    {
        CHECKF(InitSceneTree({0.0f, 0.0f}, 0.0f, 0.0f, 0));
    }
    else
    {
        CHECKF(InitSceneTree({0.0f, 0.0f}, 0.0f, 0.0f, 0));
    }

    LOGDEBUG("CAIPhase {} Created Map:{} Idx:{}", idPhase, idxScene.GetMapID(), idxScene.GetPhaseIdx());
    return true;

    __LEAVE_FUNCTION
    return false;
}