#ifndef HATELIST_H
#define HATELIST_H

#include <functional>
#include <unordered_map>
#include <vector>

#include "BaseCode.h"

struct ST_HATE_DATA
{
    OBJID  idTarget;
    int32_t nHate;
    time_t tNextInvaildTime;
};

class CHateList
{
public:
    CHateList() {}
    ~CHateList() {}

    void ClearHateList()
    {
        __ENTER_FUNCTION
        m_HateListOrderByHate.clear();
        m_HateList.clear();
        __LEAVE_FUNCTION
    }

    int32_t AddHate(OBJID idTarget, int32_t nHate)
    {
        __ENTER_FUNCTION
        auto& pHateData = m_HateList[idTarget];
        if(pHateData == nullptr)
        {
            pHateData.reset(new ST_HATE_DATA{idTarget});
            m_HateListOrderByHate.push_back(pHateData.get());
        }

        time_t now = TimeGetSecond();
        if(pHateData->tNextInvaildTime < now)
            pHateData->nHate = 0.0f;
        pHateData->nHate += nHate;
        pHateData->tNextInvaildTime = now + 5 * 60;

        return pHateData->nHate;

        __LEAVE_FUNCTION
        return 0.0f;
    }

    ST_HATE_DATA* GetHate(OBJID idTarget) { return m_HateList[idTarget].get(); }

    void FindIF(const std::function<bool(ST_HATE_DATA*)>& func)
    {
        __ENTER_FUNCTION
        //将失效数据分离
        auto it_end =
            std::stable_partition(m_HateListOrderByHate.begin(),
                                  m_HateListOrderByHate.end(),
                                  [timeNow = TimeGetSecond()](const ST_HATE_DATA* left) -> bool { return left->tNextInvaildTime < timeNow; });

        //排序Hate列表
        std::stable_sort(m_HateListOrderByHate.begin(), it_end, [](const ST_HATE_DATA* left, const ST_HATE_DATA* right) -> bool {
            return left->nHate < right->nHate;
        });

        //找到第一个在范围内的敌人
        for(auto it = m_HateListOrderByHate.begin(); it != it_end; it++)
        {
            auto pHateData = *it;
            if(func(pHateData) == false)
                continue;
            else
                return;
        }
        __LEAVE_FUNCTION
    }

public:
    std::unordered_map<OBJID, std::unique_ptr<ST_HATE_DATA>> m_HateList;
    std::vector<ST_HATE_DATA*>               m_HateListOrderByHate;
};
#endif /* HATELIST_H */
