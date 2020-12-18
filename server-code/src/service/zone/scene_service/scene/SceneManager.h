#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "BaseCode.h"
#include "SceneID.h"
class CScene;
class CPhase;
export_lua class CSceneManager : public NoncopyableT<CSceneManager>
{
    CSceneManager();

public:
    CreateNewImpl(CSceneManager);

public:
    ~CSceneManager();

    bool       Init(uint32_t idZone);
    void       Destroy();
    export_lua CScene* _CreateStaticScene(uint16_t idMap);
    export_lua CPhase* CreatePhase(uint16_t idMap, uint16_t idPhaseType, uint64_t idPhase);

    export_lua CPhase* QueryPhase(const SceneIdx& idxScene);
    export_lua CScene* QueryScene(uint16_t idMap);
    export_lua size_t  GetSceneCount();
    export_lua size_t  GetDynaSceneCount();

    export_lua void ForEach(const std::function<void(CScene*)>& func);
    void            OnTimer();

protected:
private:
    std::unordered_map<uint16_t, CScene*> m_mapScene;
    size_t                                m_nStaticScene;
};
#endif /* SCENEMANAGER_H */
