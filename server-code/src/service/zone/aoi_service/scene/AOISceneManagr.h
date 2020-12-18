#ifndef AOISCENEMANAGR_H
#define AOISCENEMANAGR_H

#include <unordered_map>

#include "BaseCode.h"
#include "SceneID.h"
class CAOIScene;
class CAOIPhase;

class CAOISceneManager : public NoncopyableT<CAOISceneManager>
{
    CAOISceneManager();
    bool Init(uint32_t idZone);

public:
    CreateNewImpl(CAOISceneManager);

public:
    ~CAOISceneManager();

    void Destroy();

    CAOIScene* CreateScene(uint16_t idMap);

    CAOIScene* QueryScene(const SceneIdx& idxScene);
    CAOIPhase* QueryPhase(const SceneIdx& idxScene);
    void       OnTimer();

protected:
private:
    std::unordered_map<uint16_t, CAOIScene*> m_mapScene;
    size_t                                   m_nStaticScene;
};
#endif /* AOISCENEMANAGR_H */
