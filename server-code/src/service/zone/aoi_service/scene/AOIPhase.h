#ifndef AOIPHASE_H
#define AOIPHASE_H

#include <memory>

#include "SceneBase.h"
#include "SceneID.h"

class CAOIScene;
class Cfg_Phase;

export_lua class CAOIPhase : public CSceneBase
{
protected:
    CAOIPhase();
    bool Init(CAOIScene* pScene, const SceneIdx& idxScene, uint64_t idPhase, const Cfg_Phase* pPhaseData);

public:
    CreateNewImpl(CAOIPhase);

public:
    virtual ~CAOIPhase();

public:
    export_lua uint64_t GetPhaseID() const { return m_idPhase; }

private:
    uint64_t m_idPhase;
};

#endif /* AOIPHASE_H */
