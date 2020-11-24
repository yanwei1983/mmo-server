//
// 数据库记录的集。
// 以map容器存储数据
//
//////////////////////////////////////////////////////////////////////

#ifndef T_GAMEDATAMAP_H
#define T_GAMEDATAMAP_H

#include <memory>
#include <vector>

#include "BaseCode.h"
#include "DataPack.h"
#include "MysqlConnection.h"
#include "ProtobuffUtil.h"
#include "Singleton.h"
#include "nameof.h"
#include "type_traits_ext.h"
//////////////////////////////////////////////////////////////////////
// 说明：需要GetID()函数支持    另外，还需要这种形式的函数支持：
// void Init(CDBRecordPtr pRes);

namespace details
{
    CAT_CLASS_HAS_MEMBER(Merge);
}; // namespace details

template<typename T>
struct hasFunc_Merge : bool_type<details::has_Merge<T>::value>
{
};

template<class T>
class CGameDataContainer : public Noncopyable
{
protected:
    static constexpr std::string_view GET_NAME()
    {
        constexpr auto t_name = NAMEOF_TYPE(T);
        return t_name;
    }
    CGameDataContainer() {}

public:
    using value_type = T;
    virtual ~CGameDataContainer() { ClearRawData(); }
    void ClearRawData() { m_vecData.clear(); }

    virtual void Clear() { ClearRawData(); }
    virtual void BuildIndex() = 0;

    static inline std::string GetCfgFileName()
    {
        using PB_T               = typename T::PB_T;
        constexpr auto pb_t_name = NAMEOF_TYPE(PB_T);
        // return PB_T::descriptor()->name()+std::string(".bytes");
        return static_cast<std::string>(pb_t_name).append(".bytes");
    }

public:
    bool Reload(CMysqlConnection* pDb, const std::string& table_name, const std::string& szSQL)
    {
        Clear();
        return Init(pDb, table_name, szSQL);
    }
    bool Reload(const std::string& szFileName)
    {
        Clear();
        return Init(szFileName);
    }

    const std::vector<std::unique_ptr<value_type>>& GetRawData() const { return m_vecData; }

protected:
    bool Init(CMysqlConnection* pDb, const std::string& table_name, const std::string& szSQL)
    {
        auto result_ptr = pDb->Query(table_name, szSQL);
        if(result_ptr)
        {
            for(size_t i = 0; i < result_ptr->get_num_row(); i++)
            {
                auto db_record_ptr = result_ptr->fetch_row(false);
                T*   pData         = T::CreateNew(std::move(db_record_ptr));
                if(pData == nullptr)
                {
                    return false;
                }

                this->AddObj(pData);
            }
        }
        LOGDEBUG("Init {} Succ.", GET_NAME());
        return true;
    }

    auto Init(const std::string& szFileName)
    {
        using PB_T = typename T::PB_T;
        std::vector<PB_T> vecData;
        if(DataPack::LoadFromBinaryFile(szFileName, vecData) == false)
        {
            LOGERROR("InitFromFile {} Fail.", szFileName);
            return false;
        }

        for(const auto& cfg: vecData)
        {
            T* pData = T::CreateNew(cfg);
            if(pData == nullptr)
            {
                return false;
            }
            std::unique_ptr<T> ptr(pData);
            m_vecData.emplace_back(std::move(ptr));
        }

        BuildIndex();

        LOGDEBUG("Init {} Succ.", szFileName);
        return true;
    }

protected:
    std::vector<std::unique_ptr<value_type>> m_vecData;
};

template<class T>
class CGameDataVector : public CGameDataContainer<T>
{
protected:
    CGameDataVector() {}

public:
    using KEY_T = typename std::result_of<decltype (&T::GetID)(T)>::type;
    ~CGameDataVector() {}
    CreateNewImpl(CGameDataVector);
    virtual void BuildIndex() override {}
    const T*     QueryObj(KEY_T id) const
    {
        const auto& refset = CGameDataContainer<T>::GetRawData();
        CHECKF_V(id < refset.size(), id);
        return refset[id].get();
    }
};

template<class T>
class CGameDataMap : public CGameDataContainer<T>
{
protected:
    CGameDataMap() {}

public:
    using KEY_T      = typename std::result_of<decltype (&T::GetID)(T)>::type;
    using MAP_SET_T  = std::unordered_map<KEY_T, T*>;
    using MAP_ITER_T = typename MAP_SET_T::const_iterator;
    class Iterator
    {
    private: // create by parent class
        Iterator(const MAP_SET_T& refSet)
            : m_refSet(refSet)
            , m_iter(m_refSet.begin())
        {
        }

    public: // application
        Iterator()                = delete;
        Iterator(const Iterator&) = default;
        void MoveNext() { m_iter++; }

        bool HasMore() const { return m_iter != m_refSet.end(); }

        T* PeekVal() const { return m_iter->second; }

        friend class CGameDataMap<T>;

    protected:
        const MAP_SET_T& m_refSet;
        MAP_ITER_T       m_iter;
    };

    virtual ~CGameDataMap() { m_setData.clear(); }
    CreateNewImpl(CGameDataMap);

    const T* QueryObj(KEY_T id) const
    {
        auto it_find = m_setData.find(id);
        if(it_find != m_setData.end())
        {
            return it_find->second;
        }
        return nullptr;
    }

    Iterator GetIter() const { return Iterator(m_setData); }

public:
    virtual void Clear() override
    {
        m_setData.clear();
        CGameDataContainer<T>::ClearRawData();
    }
    virtual void BuildIndex() override
    {
        const auto& refset = CGameDataContainer<T>::GetRawData();
        for(const auto& ptr: refset)
        {
            BuildOne(ptr.get());
        }
    }

private:
    void BuildOne(T* ptr)
    {
        if constexpr(hasFunc_Merge<T>::value)
        {
            T* pData = _QueryObj(ptr->GetID());
            if(pData)
            {
                pData->Merge(ptr);
                return;
            }
        }

        AddObj(ptr);
    }

    void AddObj(T* pData)
    {
        auto it_find = m_setData.find(pData->GetID());
        if(it_find != m_setData.end())
        {
            LOGWARNING("AddObj twice {}, id:{}", CGameDataContainer<T>::GET_NAME(), pData->GetID());
        }
        else
        {
            m_setData[pData->GetID()] = pData;
        }
    }

    T* _QueryObj(KEY_T id)
    {
        auto it_find = m_setData.find(id);
        if(it_find != m_setData.end())
        {
            return it_find->second;
        }
        return nullptr;
    }

private:
    MAP_SET_T m_setData;
};

template<class T>
class CGameMultiDataMap : public CGameDataContainer<T>
{
protected:
    CGameMultiDataMap() {}

public:
public:
    using KEY_T      = typename std::result_of<decltype (&T::GetID)(T)>::type;
    using MAP_SET_T  = std::unordered_multimap<KEY_T, T*>;
    using MAP_ITER_T = typename MAP_SET_T::const_iterator;

    class Iterator
    {
    private: // create by parent class
        Iterator(const MAP_SET_T& refSet)
            : m_itBeg(refSet.begin())
            , m_itEnd(refSet.end()())
        {
        }
        Iterator(MAP_ITER_T it_beg, MAP_ITER_T it_end)
            : m_itBeg(it_beg)
            , m_itEnd(it_end)
        {
        }

    public: // application
        Iterator()                = delete;
        Iterator(const Iterator&) = default;
        void MoveNext() { m_itBeg++; }

        bool HasMore() const { return m_itBeg != m_itEnd; }

        const T* PeekVal() const { return m_itBeg->second; }
        size_t   Count() const { return std::distance(m_itBeg, m_itEnd); }
        friend class CGameMultiDataMap<T>;

    protected:
        MAP_ITER_T m_itBeg;
        MAP_ITER_T m_itEnd;
    };

public:
    ~CGameMultiDataMap() { m_setData.clear(); }
    CreateNewImpl(CGameMultiDataMap);
    Iterator QueryObj(KEY_T id) const
    {
        auto it_find = m_setData.equal_range(id);
        if(it_find.first != it_find.second && it_find.first != m_setData.end())
        {
            return Iterator(it_find.first, it_find.second);
        }
        return Iterator(m_setData.end(), m_setData.end());
    }

    Iterator GetIter() const { return Iterator(m_setData); }

public:
    virtual void Clear() override
    {
        m_setData.clear();
        CGameDataContainer<T>::ClearRawData();
    }

    virtual void BuildIndex() override
    {
        const auto& refset = CGameDataContainer<T>::GetRawData();
        for(const auto& ptr: refset)
        {
            AddObj(ptr.get());
        }
    }

private:
    void AddObj(T* pData) { m_setData.insert(std::make_pair(pData->GetID(), pData)); }

private:
    MAP_SET_T m_setData;
};

#define DEFINE_GAMEMAPDATA(TSet, T)     \
    class TSet : public CGameDataMap<T> \
    {                                   \
        TSet() {}                       \
                                        \
    public:                             \
        ~TSet() {}                      \
        CreateNewImpl(TSet);            \
    };

#define DEFINE_MULTIGAMEMAPDATA(TSet, T)     \
    class TSet : public CGameMultiDataMap<T> \
    {                                        \
        TSet() {}                            \
                                             \
    public:                                  \
        ~TSet() {}                           \
        CreateNewImpl(TSet);                 \
    };

#endif // T_GAMEDATAMAP_H
