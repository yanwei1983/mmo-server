#ifndef DBFIELD_H
#define DBFIELD_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <fmt/format.h>

#include "BaseCode.h"
#include "CallStackDumper.h"
#include "DBFieldInfo.h"
#include "ObjectHeap.h"

const char* const EMPTY_STR = "";

class CDBRecord;
class CDBField
{
public:
    CDBField();
    CDBField(CDBRecord* pDBRecord, const CDBFieldInfo* pFieldInfo, char* pVal, uint32_t len);

    CDBField& operator=(const CDBField&) = delete;
    CDBField(const CDBField&)            = delete;

    bool IsString() const;
    void ClearDirty();

public:
    operator const std::string&() const
    {
        if(CheckType<std::string>())
        {
            return std::get<std::string>(m_Val);
        }
        static std::string s_Empty;
        return s_Empty;
    }

    template<class T>
    T get() const
    {
        if constexpr(std::is_same<T, char*>::value)
        {
            if(CheckType<std::string>())
            {
                return const_cast<char*>(std::get<std::string>(m_Val).c_str());
            }
            return const_cast<char*>(EMPTY_STR);
        }
        else if constexpr(std::is_same<T, const char*>::value)
        {
            if(CheckType<std::string>())
            {
                return std::get<std::string>(m_Val).c_str();
            }
            return EMPTY_STR;
        }
        else if constexpr(std::is_same<T, const std::string&>::value)
        {
            if(CheckType<std::string>())
            {
                return std::get<std::string>(m_Val).c_str();
            }
            return EMPTY_STR;
        }
        else if constexpr(std::is_same<T, bool>::value)
        {
            if(CheckType<uint32_t>())
            {
                return std::get<uint32_t>(m_Val) != 0;
            }
            else
            {
                return T();
            }
        }
        else if constexpr(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value)
        {
            if(CheckType<uint32_t>())
            {
                return std::get<uint32_t>(m_Val);
            }
            else
            {
                return T();
            }
        }
        else if constexpr(std::is_same<T, int8_t>::value || std::is_same<T, int16_t>::value)
        {
            if(CheckType<int32_t>())
            {
                return std::get<int32_t>(m_Val);
            }
            else
            {
                return T();
            }
        }
        else
        {
            if(CheckType<T>())
            {
                return std::get<T>(m_Val);
            }
            else
            {
                return T();
            }
        }
    }

    template<class T>
    operator T() const
    {
        return get<T>();
    }

    template<class T>
    CDBField& set(const T& v)
    {
        if constexpr(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value)
        {
            return set<uint32_t>(v);
        }
        else if constexpr(std::is_same<T, int8_t>::value || std::is_same<T, int16_t>::value)
        {
            return set<int32_t>(v);
        }

        if(CanModify() == true)
        {
            if(get<T>() == v)
                return *this;

            if(CheckType<T>())
            {
                m_Val = v;
                MakeDirty();
            }
        }

        return *this;
    }

    CDBField& set(const char* v) { return set<std::string>(v); }
    CDBField& set(char* v) { return set<std::string>(v); }
    CDBField& set(bool v) { return set<uint32_t>(v); }

    template<class T>
    CDBField& operator=(T&& v)
    {
        return set(std::forward<T>(v));
    }

    template<class T>
    bool operator==(T&& v) const
    {
        return this->operator T() == v;
    }

    uint32_t GetType() const { return (m_pFieldInfo == nullptr) ? 0 : m_pFieldInfo->GetFieldType(); }

    template<class T>
    bool CheckType() const
    {
        __ENTER_FUNCTION
        if(m_pFieldInfo == nullptr)
            return false;

        if(CheckFieldType<T>(m_pFieldInfo->GetFieldType()) == true)
            return true;

        __LEAVE_FUNCTION

        LOGDBERROR("Field {}.{} get/set type[{}->{}]error .", m_pFieldInfo->GetTableName(), m_pFieldInfo->GetFieldName(), m_pFieldInfo->GetFieldType(), nameof::nameof_type<T>());
        LOGDBERROR("CallStack：{}", GetStackTraceString(CallFrameMap(3, 7)));
        return false;
    }

    std::string GetValString() const;

    template<class Func>
    void BindGetValString(Func&& func)
    {
        m_funcGetValString = std::forward<Func>(func);
    }

    bool                CanModify() const;
    bool                IsDirty() const;
    void                MakeDirty();
    const CDBFieldInfo* GetFieldInfo() const { return m_pFieldInfo; }
    int32_t             GetFieldIndex() const;
    const char*         GetFieldName() const;

private:
    template<class T>
    static constexpr bool CheckFieldType(uint32_t field_type)
    {
        switch(field_type)
        {
            case DB_FIELD_TYPE_TINY_UNSIGNED:
            case DB_FIELD_TYPE_SHORT_UNSIGNED:
            case DB_FIELD_TYPE_LONG_UNSIGNED:
            {
                if constexpr(std::is_same<typename std::decay<T>::type, uint8_t>::value ||
                             std::is_same<typename std::decay<T>::type, uint16_t>::value ||
                             std::is_same<typename std::decay<T>::type, uint32_t>::value || std::is_same<typename std::decay<T>::type, bool>::value)
                    return true;
            }
            break;
            case DB_FIELD_TYPE_TINY:
            case DB_FIELD_TYPE_SHORT:
            case DB_FIELD_TYPE_LONG:
            {
                if constexpr(std::is_same<typename std::decay<T>::type, int8_t>::value ||
                             std::is_same<typename std::decay<T>::type, int16_t>::value || std::is_same<typename std::decay<T>::type, int32_t>::value)
                    return true;
            }
            break;
            case DB_FIELD_TYPE_LONGLONG_UNSIGNED:
            {
                if constexpr(std::is_same<typename std::decay<T>::type, uint64_t>::value)
                    return true;
            }
            break;
            case DB_FIELD_TYPE_LONGLONG:
            {
                if constexpr(std::is_same<typename std::decay<T>::type, int64_t>::value)
                    return true;
            }
            break;
            case DB_FIELD_TYPE_FLOAT:
            {
                if constexpr(std::is_same<typename std::decay<T>::type, float>::value)
                    return true;
            }
            break;
            case DB_FIELD_TYPE_DOUBLE:
            {
                if constexpr(std::is_same<typename std::decay<T>::type, double>::value)
                    return true;
            }
            break;
            case DB_FIELD_TYPE_VARCHAR:
            case DB_FIELD_TYPE_BLOB:
            {
                if constexpr(std::is_same<typename std::decay<T>::type, std::string>::value ||
                             std::is_same<typename std::decay<T>::type, const char*>::value ||
                             std::is_same<typename std::decay<T>::type, char*>::value)
                    return true;
            }
            break;
            default:
            {
            }
            break;
        }
        return false;
    }

public:
    OBJECTHEAP_DECLARATION(s_Heap);

private:
    CDBRecord*          m_pDBRecord  = nullptr;
    const CDBFieldInfo* m_pFieldInfo = nullptr;
    using value_store_t              = std::variant<int32_t, uint32_t, int64_t, uint64_t, float, double, std::string>;
    value_store_t m_Val;
    bool          m_bDirty       = false;
    using GetValStringCallBack_t = std::function<std::string()>;
    GetValStringCallBack_t m_funcGetValString;
};

#endif /* DBFIELD_H */
