#ifndef AIPATHFINDER_H
#define AIPATHFINDER_H

#include "BaseCode.h"
#include <optional>
class CAIActor;

class CAIPathFinder
{
public:
    CAIPathFinder() {}
    virtual ~CAIPathFinder() {}

    virtual std::optional<Vector2> SearchStep(const Vector2& dest, float move_spd) = 0;
    virtual std::vector<Vector2> SearchAll(const Vector2& dest, float move_spd)  = 0;
};

class CAIPathFinder_Normal : public CAIPathFinder
{
public:
    CAIPathFinder_Normal(CAIActor* pActor);
    virtual std::optional<Vector2> SearchStep(const Vector2& dest, float move_spd) override;
    virtual std::vector<Vector2> SearchAll(const Vector2& dest, float move_spd) override { return m_SearchResult; }

private:
    CAIActor*            m_pActor;
    std::deque<Vector2>  m_LastSearchList;
    std::vector<Vector2> m_SearchResult;
};

class CAIPathFinder_AStar : public CAIPathFinder
{
public:
    CAIPathFinder_AStar(CAIActor* pActor);
    virtual std::optional<Vector2> SearchStep(const Vector2& dest, float move_spd) override { return dest; }
    virtual std::vector<Vector2> SearchAll(const Vector2& dest, float move_spd) override { return m_SearchResult; }

private:
    CAIActor*            m_pActor;
    std::deque<Vector2>  m_LastSearchList;
    std::vector<Vector2> m_SearchResult;
};

#endif /* AIPATHFINDER_H */
