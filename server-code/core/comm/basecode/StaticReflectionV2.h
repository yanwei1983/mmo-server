#ifndef STATICREFLECTIONV2_H
#define STATICREFLECTIONV2_H

#include <tuple>
#include <type_traits>
#include <utility>

#include "StaticHash.h"
#include "TupleHelper.h"

namespace static_reflection_v2
{

    enum class FieldType
    {
        MemberPtr,
        MemberPtrInfoTag,
        MemberPtrInfoFunc,
        FuncInfo
    };

    template<class T, FieldType field_type>
    struct FieldInfo
    {
        static constexpr FieldType this_field_type = field_type;

        const char* field_name;
        size_t      field_name_hash;
    };

    template<class T, class member_ptr, FieldType field_type = FieldType::MemberPtr>
    struct MemberPtrInfo : public FieldInfo<T, field_type>
    {
        member_ptr ptr;
    };

    template<class T, class member_ptr, class Tag>
    struct MemberPtrInfoTag : public MemberPtrInfo<T, member_ptr, FieldType::MemberPtrInfoTag>
    {
        Tag tag;
    };

    template<class T, class member_ptr, class Func>
    struct MemberPtrInfoFunc : public MemberPtrInfo<T, member_ptr, FieldType::MemberPtrInfoFunc>
    {
        Func func;
    };

    template<class T, class member_func_ptr>
    struct FuncInfo : public FieldInfo<T, FieldType::FuncInfo>
    {
        member_func_ptr ptr;
    };

    template<class T, class MemberTuple, class FuncTuple>
    struct ClassInfo
    {
        const char* class_name;
        MemberTuple member_info_tuple;
        FuncTuple   func_info_tuple;
    };

    template<class T, class C>
    constexpr auto make_member_ptr(const char* field_name, size_t field_name_hash, C T::*field_ptr)
    {
        return MemberPtrInfo<T, decltype(field_ptr)>{{field_name, field_name_hash}, field_ptr};
    }
    template<class T, class C, class Tag>
    constexpr auto make_member_ptr_tag(const char* field_name, size_t field_name_hash, C T::*field_ptr, Tag&& tag)
    {
        return MemberPtrInfoTag<T, decltype(field_ptr), Tag>{{{field_name, field_name_hash}, field_ptr}, std::move(tag)};
    }
    template<class T, class C, class Func>
    constexpr auto make_member_ptr_func(const char* field_name, size_t field_name_hash, C T::*field_ptr, Func&& func)
    {
        return MemberPtrInfoFunc<T, decltype(field_ptr), Func>{{{field_name, field_name_hash}, field_ptr}, std::forward<Func>(func)};
    }

    template<class T, class C>
    constexpr auto make_func_info(const char* field_name, size_t field_name_hash, C T::*func_ptr)
    {
        return FuncInfo<T, decltype(func_ptr)>{{field_name, field_name_hash}, func_ptr};
    }

    template<class T, class MemberTuple, class FuncTuple>
    constexpr auto make_class_info(const char* class_name, MemberTuple&& member_tuple, FuncTuple&& func_tuple)
    {
        return ClassInfo<T, MemberTuple, FuncTuple>{class_name, std::forward<MemberTuple>(member_tuple), std::forward<FuncTuple>(func_tuple)};
    }

    template<class T, class MemberTuple>
    constexpr auto make_class_info(const char* class_name, MemberTuple&& member_tuple)
    {
        return ClassInfo<T, MemberTuple, std::tuple<>>{class_name, std::forward<MemberTuple>(member_tuple)};
    }

    template<class FieldInfo>
    constexpr bool is_member_ptr()
    {
        return std::decay_t<FieldInfo>::this_field_type == FieldType::MemberPtr;
    }

    template<class FieldInfo>
    constexpr bool is_member_ptr_tag()
    {
        return std::decay_t<FieldInfo>::this_field_type == FieldType::MemberPtrInfoTag;
    }

    template<class FieldInfo>
    constexpr bool is_member_ptr_func()
    {
        return std::decay_t<FieldInfo>::this_field_type == FieldType::MemberPtrInfoFunc;
    }

    template<class FieldInfo>
    constexpr bool is_member_func()
    {
        return std::decay_t<FieldInfo>::this_field_type == FieldType::FuncInfo;
    }

} // end namespace static_reflection_v2

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
struct MetaClass
{
    static inline constexpr void GetMetaInfo() {}
};

#define DEFINE_MEMBER(...)   std::make_tuple(__VA_ARGS__)
#define DEFINE_FUNCTION(...) std::make_tuple(__VA_ARGS__)

#define DEFINE_META(ClassT, ...)                                                            \
    template<>                                                                              \
    struct MetaClass<ClassT>                                                                \
    {                                                                                       \
        using _ThisClass = ClassT;                                                          \
        template<typename E = void>                                                         \
        static inline constexpr auto GetMetaInfo()                                          \
        {                                                                                   \
            return static_reflection_v2::make_class_info<_ThisClass>(#ClassT, __VA_ARGS__); \
        }                                                                                   \
    };

#define META_MEMBER(ClassField, FieldName) static_reflection_v2::make_member_ptr(FieldName, FieldName##_HASH, &_ThisClass::ClassField)
#define META_MEMBER_TAG(ClassField, FieldName, Tag) \
    static_reflection_v2::make_member_ptr_tag(FieldName, FieldName##_HASH, &_ThisClass::ClassField, Tag{})
#define META_MEMBER_FUNC(ClassField, FieldName, Func) \
    static_reflection_v2::make_member_ptr_func(FieldName, FieldName##_HASH, &_ThisClass::ClassField, Func)

#define META_FUNCTION(ClassField, FieldName) static_reflection_v2::make_func_info(FieldName, FieldName##_HASH, &_ThisClass::ClassField)

namespace static_reflection_v2
{
    template<typename T, typename Fn>
    inline constexpr void ForEachField(T&& value, Fn&& fn)
    {
        using ThisMetaClass       = MetaClass<std::decay_t<T>>;
        constexpr auto meta_class = ThisMetaClass::GetMetaInfo();
        static_assert(std::tuple_size<decltype(meta_class.member_info_tuple)>::value != 0,
                      "MetaClass<T>() for type T should be specialized to return "
                      "FieldSchema tuples, like ((&T::field, field_name), ...)");
        for_each_tuple(
            meta_class.member_info_tuple,
            [&fn, &value](const auto& field_info) constexpr {
                if constexpr(is_member_ptr<decltype(field_info)>())
                {
                    fn(field_info, value.*(field_info.ptr));
                }
                else if constexpr(is_member_ptr_tag<decltype(field_info)>())
                {
                    fn(field_info, value.*(field_info.ptr), field_info.tag);
                }
                else if constexpr(is_member_ptr_func<decltype(field_info)>())
                {
                    field_info.func(field_info, value.*(field_info.ptr));
                }
            });
    }

    template<typename T, typename Fn>
    inline constexpr void FindInField(T&& value, size_t name_hash, Fn&& fn)
    {
        using ThisMetaClass       = MetaClass<std::decay_t<T>>;
        constexpr auto meta_class = ThisMetaClass::GetMetaInfo();
        static_assert(std::tuple_size<decltype(meta_class.member_info_tuple)>::value != 0,
                      "MetaClass<T>() for type T should be specialized to return "
                      "FieldSchema tuples, like ((&T::field, field_name), ...)");
        find_if_tuple(
            meta_class.member_info_tuple,
            [&fn, &value, name_hash](const auto& field_info) constexpr->bool {
                if(field_info.field_name_hash != name_hash)
                    return false;

                if constexpr(is_member_ptr<decltype(field_info)>())
                {
                    return fn(field_info, value.*(field_info.ptr));
                }
                else if constexpr(is_member_ptr_tag<decltype(field_info)>())
                {
                    return fn(field_info, value.*(field_info.ptr), field_info.tag);
                }
                else if constexpr(is_member_ptr_func<decltype(field_info)>())
                {
                    return fn(field_info, value.*(field_info.ptr), field_info.func);
                }

                return false;
            });
    }

} // namespace static_reflection_v2

#endif /* STATICREFLECTIONV2_H */
