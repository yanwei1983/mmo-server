#include "MonsterGenerator.h"

#include "AIActorManager.h"
#include "AIMonster.h"
#include "AIPhase.h"
#include "AIService.h"
#include "GameEventDef.h"
#include "GameMap.h"
#include "GameMapDef.h"
#include "MapManager.h"
#include "MonsterType.h"
#include "config/Cfg_Scene_MonsterGenerator.pb.h"
#include "server_msg/server_side.pb.h"

MonsterGenData::MonsterGenData(const Cfg_Scene_MonsterGenerator& _gen_data)
    : gen_data(_gen_data)
    , nIdxGen(_gen_data.idx())
    , nCurGen(0)
{
}

CMonsterGenerator::CMonsterGenerator() {}

CMonsterGenerator::~CMonsterGenerator()
{
    for(auto& [key, pData]: m_setGen)
    {
        SAFE_DELETE(pData);
    }
    m_setGen.clear();
}

bool CMonsterGenerator::Init(CAIPhase* pPhase)
{
    __ENTER_FUNCTION
    m_pMap               = pPhase->GetMap();
    m_idxScene           = pPhase->GetSceneIdx();
    const auto& gen_list = m_pMap->GetGeneratorData();
    for(const auto& [key, cfg_ptr]: gen_list)
    {
        if(m_setGen.find(key) != m_setGen.end())
        {
            LOGDEBUG("CMonsterGenerator::Init Map:{} Add{} twice", m_pMap->GetMapID(), key);
            continue;
        }
        auto pGenData = new MonsterGenData{*cfg_ptr};
        m_setGen[key] = pGenData;
        if(pGenData->gen_data.active())
        {
            StartGen(pGenData);
        }
        else
        {
            StopGen(pGenData);
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}
void CMonsterGenerator::OnGenTimer(MonsterGenData* pData)
{
    __ENTER_FUNCTION
    CHECK(pData);
    if(pData->nCurGen >= pData->gen_data.gen_max())
        return;
    _GenMonster(pData);
    StartGen(pData, false);
    __LEAVE_FUNCTION
}

void CMonsterGenerator::_GenMonster(MonsterGenData* pData, uint64_t idPhase)
{
    __ENTER_FUNCTION
    for(size_t i = 0; i < pData->gen_data.per_gen(); i++)
    {
        CPos2D newPos(pData->gen_data.x(), pData->gen_data.y());
        switch(pData->gen_data.shape())
        {
            case MONSTERGENERATOR_CIRCLE: // x,z为中心range的圆形
            {
                newPos = newPos + CPos2D::UNIT_X().randomDeviant(1.0f) * random_float(0.0f, pData->gen_data.range());
            }
            break;
            case MONSTERGENERATOR_RECT: // x,z为左上角,width宽,range高的矩形
            {
                newPos.x += random_float() * pData->gen_data.width();
                newPos.y += random_float() * pData->gen_data.range();
            }
            break;
        }

        ServerMSG::MonsterGen msg;
        msg.set_scene_id(m_idxScene);
        msg.set_gen_id(pData->gen_data.idx());
        msg.set_camp_id(pData->gen_data.camp_id());
        msg.set_monster_type(pData->gen_data.monsterid());
        msg.set_posx(newPos.x);
        msg.set_posy(newPos.y);
        if(idPhase == 0)
        {
            msg.set_phase_id(pData->gen_data.phase_id());
        }
        else
        {
            msg.set_phase_id(idPhase);
        }
        AIService()->SendProtoMsgToScene(msg);
        pData->nCurGen++;
    }
    LOGDEBUG("MonsterGen: map:{} phase:{} - idx:{} GenOnce", m_idxScene.GetMapID(), m_idxScene.GetPhaseIdx(), pData->nIdxGen);

    __LEAVE_FUNCTION
}

void CMonsterGenerator::StartGen(MonsterGenData* pData, bool bCheckRunning)
{
    __ENTER_FUNCTION
    CHECK(pData);
    if(pData->nCurGen < pData->gen_data.gen_max())
    {
        if(bCheckRunning == false || pData->m_pEvent.IsWaitTrigger() == false)
        {
            CEventEntryCreateParam param;
            param.evType    = EVENTID_MONSTER_GENERATOR;
            param.cb        = std::bind(&CMonsterGenerator::OnGenTimer, this, pData);
            param.tWaitTime = pData->gen_data.wait_time();
            EventManager()->ScheduleEvent(param, pData->m_pEvent);
        }
    }
    else
    {
        StopGen(pData);
    }

    __LEAVE_FUNCTION
}

void CMonsterGenerator::StopGen(MonsterGenData* pData)
{
    __ENTER_FUNCTION
    CHECK(pData);
    pData->m_pEvent.Cancel();

    __LEAVE_FUNCTION
}

void CMonsterGenerator::ActiveAll(bool bActive)
{
    __ENTER_FUNCTION
    for(auto it = m_setGen.begin(); it != m_setGen.end(); it++)
    {
        MonsterGenData* pData = it->second;
        if(bActive == true)
        {
            StopGen(pData);
        }
        else
        {
            StartGen(pData);
        }
    }

    __LEAVE_FUNCTION
}

void CMonsterGenerator::ActiveGen(uint32_t idGen, bool bActive)
{
    __ENTER_FUNCTION
    if(idGen == 0xFFFFFFFF)
    {
        ActiveAll(bActive);
        return;
    }
    auto it = m_setGen.find(idGen);
    if(it == m_setGen.end())
        return;
    auto pData = it->second;
    if(bActive == true)
    {
        StopGen(pData);
    }
    else
    {
        StartGen(pData);
    }

    __LEAVE_FUNCTION
}

void CMonsterGenerator::KillAllGen()
{
    __ENTER_FUNCTION
    for(auto it = m_setGen.begin(); it != m_setGen.end(); it++)
    {
        auto                      pData = it->second;
        ServerMSG::MonsterDestory msg;
        for(auto pMonster: pData->m_setMonster)
        {
            msg.add_monster_id(pMonster->GetID());
            AIActorManager()->DelActor(pMonster);
        }
        if(msg.monster_id_size() > 0)
            AIService()->SendProtoMsgToScene(msg);

        pData->nCurGen = 0;
        pData->m_pEvent.Clear();
    }

    __LEAVE_FUNCTION
}

void CMonsterGenerator::KillGen(uint32_t idGen)
{
    __ENTER_FUNCTION
    if(idGen == 0xFFFFFFFF)
    {
        KillAllGen();
        return;
    }
    auto it = m_setGen.find(idGen);
    if(it == m_setGen.end())
        return;
    auto                      pData = it->second;
    ServerMSG::MonsterDestory msg;
    for(auto pMonster: pData->m_setMonster)
    {
        msg.add_monster_id(pMonster->GetID());
        AIActorManager()->DelActor(pMonster);
    }
    if(msg.monster_id_size() > 0)
        AIService()->SendProtoMsgToScene(msg);

    pData->nCurGen = 0;
    pData->m_pEvent.Clear();

    __LEAVE_FUNCTION
}

void CMonsterGenerator::GenOnce(uint32_t idGen, uint64_t idPhase)
{
    __ENTER_FUNCTION
    auto pData = QueryGen(idGen);
    if(pData)
    {
        _GenMonster(pData, idPhase);
    }

    __LEAVE_FUNCTION
}

MonsterGenData* CMonsterGenerator::QueryGen(uint32_t idGen)
{
    __ENTER_FUNCTION
    auto it = m_setGen.find(idGen);
    if(it == m_setGen.end())
        return nullptr;
    return it->second;

    __LEAVE_FUNCTION
    return nullptr;
}

void CMonsterGenerator::OnMonsterBorn(CAIMonster* pMonster)
{
    __ENTER_FUNCTION
    uint32_t idGen = pMonster->GetGenID();
    auto     pData = QueryGen(idGen);
    if(pData == nullptr)
        return;
    pData->m_setMonster.insert(pMonster);
    LOGDEBUG("MonsterGen: map:{} phase:{} idx:{} MonsterBorn:{} ID:{}",
             m_pMap->GetMapID(),
             pMonster->GetPhaseID(),
             pData->nIdxGen,
             pMonster->Type()->GetID(),
             pMonster->GetID());

    __LEAVE_FUNCTION
}

void CMonsterGenerator::OnMonsterDead(CAIMonster* pMonster)
{
    __ENTER_FUNCTION
    uint32_t idGen = pMonster->GetGenID();
    auto     pData = QueryGen(idGen);
    if(pData == nullptr)
        return;
    pData->m_setMonster.erase(pMonster);
    pData->nCurGen--;

    LOGDEBUG("MonsterGen: map:{} phase:{} idx:{} MonsterDead:{}",
             m_pMap->GetMapID(),
             pMonster->GetPhaseID(),
             pData->nIdxGen,
             pMonster->Type()->GetID());

    __LEAVE_FUNCTION
}
