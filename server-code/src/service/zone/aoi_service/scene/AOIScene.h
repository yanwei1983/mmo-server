#ifndef AOISCENE_H
#define AOISCENE_H

#include <unordered_map>

#include "SceneID.h"
class CAOIPhase;
class Cfg_Phase;
class CAOISceneManager;
class CAOIPhase;

export_lua class CAOIScene
{
protected:
    CAOIScene();
    bool Init(uint16_t idMap);

public:
    CreateNewImpl(CAOIScene);

public:
    virtual ~CAOIScene();

public:
    export_lua CAOIPhase* CreatePhase(const SceneIdx& idxScene, uint64_t idPhase);
    export_lua CAOIPhase* CreatePhase(const SceneIdx& idxScene, uint64_t idPhase, const Cfg_Phase* pPhaseData);

    export_lua bool DestoryPhase(uint64_t idPhase);
    export_lua bool DestoryPhaseByIdx(uint32_t idxPhase);

    export_lua CAOIPhase* QueryPhaseByIdx(uint32_t idxPhase) const;
    export_lua CAOIPhase* QueryPhaseByID(uint64_t idPhase) const;

private:
    uint16_t m_idMap = 0;

    std::unordered_map<uint64_t, std::unique_ptr<CAOIPhase>> m_pPhaseSet;
    std::unordered_map<uint32_t, CAOIPhase*>                 m_pPhaseSetByIdx;
};

#endif /* AOISCENE_H */
