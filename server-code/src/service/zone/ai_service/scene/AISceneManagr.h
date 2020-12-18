#ifndef AISCENEMANAGR_H
#define AISCENEMANAGR_H

#include <unordered_map>

#include "BaseCode.h"
#include "SceneID.h"
class CAIScene;
class CAIPhase;

class CAISceneManager : public NoncopyableT<CAISceneManager>
{
    CAISceneManager();
    bool Init(uint32_t idZone);

public:
    CreateNewImpl(CAISceneManager);

public:
    ~CAISceneManager();

    void Destroy();

    CAIScene* CreateScene(uint16_t idMap);

    CAIScene* QueryScene(const SceneIdx& idxScene);
    CAIPhase* QueryPhase(const SceneIdx& idxScene);
    void      OnTimer();

protected:
private:
    std::unordered_map<uint16_t, CAIScene*> m_mapScene;
    size_t                                  m_nStaticScene;
};
#endif /* AISCENEMANAGR_H */
