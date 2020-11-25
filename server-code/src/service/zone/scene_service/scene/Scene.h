#ifndef SCENE_H
#define SCENE_H

#include <functional>

#include "EventEntry.h"
#include "IDGenPool.h"

//每个场景有一个主位面，默认创建
//有N个静态位面，与主位面链接视野，主要用来控制特定任务可见的NPC的显示，以及刷新部分特定任务可见的怪物
//有N个动态位面，独立视野，以玩家ID作为位面ID，其他玩家可以通过组队/跳转等方式切换位面进入某个玩家的独立位面

class CActor;
class CMonster;
class CNpc;
class CPlayer;
class CPhase;
class Cfg_Phase;
class CGameMap;
export_lua class CScene
{
protected:
    CScene();

public:
    CreateNewImpl(CScene);

public:
    virtual ~CScene();

public:
    bool       Init(uint16_t idMap, uint16_t idMainPhaseType, uint64_t idPhase);
    void       Destory();
    export_lua CPhase* CreatePhase(uint16_t idPhaseType, uint64_t idPhase);
    export_lua CPhase* CreatePhase(uint16_t idPhaseType, const Cfg_Phase* pPhaseData, uint64_t idPhase);
    export_lua CPhase* QueryPhase(uint64_t idPhase) const;
    export_lua CPhase* _QueryPhase(uint64_t idPhase) const;
    export_lua CPhase*  QueryPhaseByIdx(uint32_t idxPhase) const;
    export_lua bool     DestoryPhase(uint64_t idPhase);
    export_lua void     ForEach(const std::function<void(const CPhase*)>& func) const;
    export_lua size_t   GetStaticPhaseCount() const { return m_nStaticPhaseCount; }
    export_lua uint16_t GetMapID() const { return m_idMap; }

public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    uint16_t        m_idMap = 0;
    const CGameMap* m_pMap  = nullptr;

    std::unordered_map<uint64_t, std::unique_ptr<CPhase>> m_setPhase;
    std::unordered_map<uint64_t, CPhase*>                 m_setPhaseByIdx;

    IDGenPool<uint32_t> m_DynaIDPool;
    size_t              m_nStaticPhaseCount = 0;
};
#endif /* SCENE_H */
