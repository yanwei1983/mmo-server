#ifndef STATICREFLECTION_H
#define STATICREFLECTION_H

#include <tuple>
#include <type_traits>

#include "StaticHash.h"
#include "TupleHelper.h"

namespace static_reflection
{

    namespace detail
    {

        template<typename T>
        struct is_field_pointer : std::false_type
        {
        };

        template<typename C, typename T>
        struct is_field_pointer<T C::*> : std::true_type
        {
        };

        template<typename Value, typename Func>
        struct ForEachStructSchemaLambda
        {
            Value&      value;
            const Func& fn;

            template<typename Info, typename Tuple, std::size_t... Index>
            void invoke_one_impl(Info&& info, Tuple&& t, std::index_sequence<Index...>) const
            {
                if constexpr(sizeof...(Index) > 0)
                {
                    // invoke(FieldName, MemberPtr,Tag);
                    // invoke(FieldName. MemberPtr,Func);
                    fn(std::forward<Info>(info), value.*(std::get<0>(std::forward<Tuple>(t))), std::get<Index + 1>(std::forward<Tuple>(t))...);
                }
                else
                {
                    // invoke(FieldName, MemberPtr);
                    fn(std::forward<Info>(info), value.*(std::get<0>(std::forward<Tuple>(t))));
                }
            }

            template<typename Info, typename Tuple>
            void invoke_one(Info&& info, Tuple&& t) const
            {
                constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
                invoke_one_impl(std::forward<Info>(info), std::forward<Tuple>(t), std::make_index_sequence<size - 1>{});
            }

            template<typename Info, typename Tuple, std::size_t first, std::size_t... is>
            void invoke_impl(Info&& info, Tuple&& t, std::index_sequence<first, is...>) const
            {
                // invoke(FieldName, std::tuple<MemberPtr>);
                // invoke(FieldName, std::tuple<MemberPtr,Tag>);
                // invoke(FieldName. std::tuple<MemberPtr,Func>);
                invoke_one(std::forward<Info>(info), std::get<first + 1>(std::forward<Tuple>(t)));
                if constexpr(sizeof...(is) > 0)
                {
                    invoke_impl(std::forward<Info>(info), std::forward<Tuple>(t), std::index_sequence<is...>{});
                }
            }

            template<typename Tuple>
            void operator()(Tuple&& field_schema) const
            {
                using FieldSchema         = std::decay_t<decltype(field_schema)>;
                constexpr auto size_tuple = std::tuple_size<FieldSchema>::value;
                static_assert(size_tuple >= 2, "FieldSchema<T>() must have 1 field");
                // tuple like std::tuple<FieldName, std::tuple<MemberPtr>, std::tuple<MemberPtr,Tag> ,
                // std::tuple<MemberPtr,Func> >
                invoke_impl(std::get<0>(std::forward<Tuple>(field_schema)),
                            std::forward<Tuple>(field_schema),
                            std::make_index_sequence<size_tuple - 1>{});
            }
        };

        template<typename Value, typename Func>
        struct FindInStructSchemaLambda
        {
            Value&      value;
            const Func& fn;

            template<typename Info, typename Tuple, std::size_t... Index>
            bool invoke_one_impl(Info&& info, Tuple&& t, std::index_sequence<Index...>) const
            {
                if constexpr(sizeof...(Index) > 0)
                {
                    // invoke(FieldName, MemberPtr,Tag);
                    // invoke(FieldName. MemberPtr,Func);
                    return fn(std::forward<Info>(info), value.*(std::get<0>(std::forward<Tuple>(t))), std::get<Index + 1>(std::forward<Tuple>(t))...);
                }
                else
                {
                    // invoke(FieldName, MemberPtr);
                    return fn(std::forward<Info>(info), value.*(std::get<0>(std::forward<Tuple>(t))));
                }
            }

            template<typename Info, typename Tuple>
            bool invoke_one(Info&& info, Tuple&& t) const
            {
                constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
                return invoke_one_impl(std::forward<Info>(info), std::forward<Tuple>(t), std::make_index_sequence<size - 1>{});
            }

            template<typename Info, typename Tuple, std::size_t first, std::size_t... is>
            bool invoke_impl(Info&& info, Tuple&& t, std::index_sequence<first, is...>) const
            {
                // invoke(FieldName, std::tuple<MemberPtr>);
                // invoke(FieldName, std::tuple<MemberPtr,Tag>);
                // invoke(FieldName. std::tuple<MemberPtr,Func>);
                bool bInvoke = invoke_one(std::forward<Info>(info), std::get<first + 1>(std::forward<Tuple>(t)));

                // first invoke all, then all result ||;
                if constexpr(sizeof...(is) > 0)
                {
                    return invoke_impl(std::forward<Info>(info), std::forward<Tuple>(t), std::index_sequence<is...>{}) || bInvoke;
                }
                else
                {
                    return bInvoke;
                }
            }

            template<typename Tuple>
            bool operator()(Tuple&& field_schema) const
            {
                using FieldSchema         = std::decay_t<decltype(field_schema)>;
                constexpr auto size_tuple = std::tuple_size<FieldSchema>::value;
                static_assert(size_tuple >= 2, "FieldSchema<T>() must have 1 field");
                // tuple like std::tuple<FieldName, std::tuple<MemberPtr>, std::tuple<MemberPtr,Tag> ,
                // std::tuple<MemberPtr,Func> >
                return invoke_impl(std::get<0>(std::forward<Tuple>(field_schema)),
                                   std::forward<Tuple>(field_schema),
                                   std::make_index_sequence<size_tuple - 1>{});
            }
        };

    } // namespace detail
} // namespace static_reflection

template<typename T>
struct StructSchema
{
    static inline constexpr void GetSchema() {}
};

template<typename T, size_t FieldIndex>
struct FieldSchemaHelper
{
    using ThisStructSchema = StructSchema<T>;
    static inline constexpr size_t field_size()
    {
        constexpr auto struct_schema = ThisStructSchema::GetSchema();
        using field_type_list        = decltype(struct_schema);
        return std::tuple_size<field_type_list>::value;
    }

    static inline constexpr auto field()
    {
        constexpr auto   struct_schema = ThisStructSchema::GetSchema();
        constexpr size_t idx           = FieldIndex;
        return std::get<idx>(struct_schema);
    }

    static inline constexpr auto field_tuple_size()
    {
        constexpr auto field_schema = field();
        using MemberTuple           = decltype(field_schema);
        return std::tuple_size<MemberTuple>::value;
    }

    static inline constexpr auto field_name_tuple()
    {
        constexpr auto field_schema = field();
        return std::get<0>(field_schema);
    }

    static inline constexpr auto field_name()
    {
        constexpr auto field_name_schema = field_name_tuple();
        return std::get<0>(field_name_schema);
    }

    static inline constexpr auto field_name_hash()
    {
        constexpr auto field_name_schema = field_name_tuple();
        return std::get<1>(field_name_schema);
    }

    template<size_t TagIdx>
    static inline constexpr auto member_ptr_tuple()
    {
        constexpr auto field_schema = field();
        return std::get<TagIdx>(field_schema);
    }

    static inline constexpr auto member_ptr_tuple_size() { return field_tuple_size() - 1; }
};

#define DEFINE_STRUCT_SCHEMA(ThisStruct, ...)    \
    template<>                                   \
    struct StructSchema<ThisStruct>              \
    {                                            \
        using _Struct = ThisStruct;              \
        template<typename E = void>              \
        static inline constexpr auto GetSchema() \
        {                                        \
            return std::make_tuple(__VA_ARGS__); \
        }                                        \
    };

#define MAKE_FIELD_INFO(FieldName) std::make_tuple(FieldName, FieldName##_HASH)

#define BIND_FIELD(StructField) std::make_tuple(&_Struct::StructField)

#define BIND_FIELD_TAG(StructField, Tag) std::make_tuple(&_Struct::StructField, Tag)

#define BIND_FIELD_FUNC(StructField, Func) std::make_tuple(&_Struct::StructField, Func)

#define DEFINE_STRUCT_FIELD_BIND(FieldName, ...) std::make_tuple(MAKE_FIELD_INFO(FieldName), __VA_ARGS__)

#define DEFINE_STRUCT_FIELD(StructField, FieldName) std::make_tuple(MAKE_FIELD_INFO(FieldName), BIND_FIELD(StructField))

#define DEFINE_STRUCT_FIELD_TAG(StructField, FieldName, Tag) std::make_tuple(MAKE_FIELD_INFO(FieldName), BIND_FIELD_TAG(StructField, Tag))

#define DEFINE_STRUCT_FIELD_FUNC(StructField, FieldName, Func) std::make_tuple(MAKE_FIELD_INFO(FieldName), BIND_FIELD_FUNC(StructField, Func))

// every Field Will MakeTuple Like
// std::tuple<FieldName, std::tuple<MemberPtr>, std::tuple<MemberPtr,Tag> , std::tuple<MemberPtr,Func> >

namespace static_reflection
{
    template<typename T, typename Fn>
    inline constexpr void ForEachField(T&& value, Fn&& fn)
    {
        constexpr auto struct_schema = StructSchema<std::decay_t<T>>::GetSchema();
        static_assert(std::tuple_size<decltype(struct_schema)>::value != 0,
                      "StructSchema<T>() for type T should be specialized to return "
                      "FieldSchema tuples, like ((&T::field, field_name), ...)");
        detail::ForEachStructSchemaLambda<T, Fn> lambda{std::forward<T>(value), std::forward<Fn>(fn)};
        for_each_tuple(struct_schema, std::move(lambda));
    }

    template<typename T, typename Fn>
    inline constexpr void ForEachField_Index(T&& value, Fn&& fn)
    {
        using ThisStructSchema       = StructSchema<std::decay_t<T>>;
        constexpr auto struct_schema = ThisStructSchema::GetSchema();
        static_assert(std::tuple_size<decltype(struct_schema)>::value != 0,
                      "StructSchema<T>() for type T should be specialized to return "
                      "FieldSchema tuples, like ((&T::field, field_name), ...)");
        for_each_tuple_index(std::move(struct_schema), [&fn, &value](auto FieldIndex, auto&&) {
            constexpr size_t field_idx = FieldIndex;
            using ThisField            = FieldSchemaHelper<std::decay_t<T>, field_idx>;
            // field_schema = std::tuple<FieldName, std::tuple<MemberPtr>, std::tuple<MemberPtr,Tag>,
            // std::tuple<MemberPtr,Func> >
            constexpr auto field_schema = ThisField::field();
            constexpr auto size_tuple   = ThisField::field_tuple_size();
            static_assert(size_tuple >= 2, "FieldSchema() must have 1 field");
            constexpr auto field_name_tuple = ThisField::field_name_tuple();

            for_each_tuple_index(std::move(field_schema), [&fn, &value, &field_name_tuple](auto TagIndex, const auto&&) {
                constexpr size_t tag_idx = TagIndex;
                if constexpr(tag_idx == 0) // skip name_tuple
                {
                    return;
                }
                else
                {
                    // invoke(FieldName, std::tuple<MemberPtr>);
                    // invoke(FieldName, std::tuple<MemberPtr,Tag>);
                    // invoke(FieldName. std::tuple<MemberPtr,Func>);

                    // member_ptr_tuple = std::tuple<MemberPtr,Tag>;
                    constexpr auto member_ptr_tuple = ThisField::template member_ptr_tuple<TagIndex>();

                    overloaded_t overload{[&fn, &value, &field_name_tuple](auto member_ptr) {
                                              // invoke(fn, FieldName, MemberPtr);
                                              fn(field_name_tuple, value.*(member_ptr));
                                          },
                                          [&fn, &value, &field_name_tuple](auto member_ptr, auto tag) {
                                              // invoke(fn, FieldName, MemberPtr, tag);
                                              fn(field_name_tuple, value.*(member_ptr), tag);
                                          }};

                    std::apply(overload, member_ptr_tuple);
                }
            });
        });
    }

    template<typename T, typename Fn>
    inline constexpr void FindInField(T&& value, Fn&& fn)
    {
        constexpr auto struct_schema = StructSchema<std::decay_t<T>>::GetSchema();
        static_assert(std::tuple_size<decltype(struct_schema)>::value != 0,
                      "StructSchema<T>() for type T should be specialized to return "
                      "FieldSchema tuples, like ((&T::field, field_name), ...)");
        detail::FindInStructSchemaLambda<T, Fn> lambda{std::forward<T>(value), std::forward<Fn>(fn)};
        find_if_tuple(struct_schema, std::move(lambda));
    }

    template<typename T, typename Fn>
    inline constexpr void FindInField_Index(T&& value, size_t name_hash, Fn&& fn)
    {
        using ThisStructSchema       = StructSchema<std::decay_t<T>>;
        constexpr auto struct_schema = ThisStructSchema::GetSchema();
        static_assert(std::tuple_size<decltype(struct_schema)>::value != 0,
                      "StructSchema<T>() for type T should be specialized to return "
                      "FieldSchema tuples, like ((&T::field, field_name), ...)");
        find_if_tuple_index(std::move(struct_schema), [&fn, &value, name_hash](auto FieldIndex, auto &&) -> bool {
            constexpr size_t field_idx = FieldIndex;
            using ThisField            = FieldSchemaHelper<std::decay_t<T>, field_idx>;
            // field_schema = std::tuple<FieldName, std::tuple<MemberPtr>, std::tuple<MemberPtr,Tag>,
            // std::tuple<MemberPtr,Func> >
            constexpr auto field_schema = ThisField::field();
            constexpr auto size_tuple   = ThisField::field_tuple_size();
            static_assert(size_tuple >= 2, "FieldSchema() must have 1 field");
            constexpr auto field_name_hash = ThisField::field_name_hash();
            if(field_name_hash == name_hash)
            {
                constexpr auto field_name_tuple = ThisField::field_name_tuple();
                for_each_tuple_index(std::move(field_schema), [&fn, &value, &field_name_tuple](auto TagIndex, const auto&&) {
                    constexpr size_t tag_idx = TagIndex;
                    if constexpr(tag_idx == 0) // skip name_tuple
                    {
                        return;
                    }
                    else
                    {
                        // invoke(FieldName, std::tuple<MemberPtr>);
                        // invoke(FieldName, std::tuple<MemberPtr,Tag>);
                        // invoke(FieldName. std::tuple<MemberPtr,Func>);

                        // member_ptr_tuple = std::tuple<MemberPtr,Tag>;
                        constexpr auto member_ptr_tuple = ThisField::template member_ptr_tuple<TagIndex>();

                        overloaded_t overload{[&fn, &value, &field_name_tuple](auto member_ptr) {
                                                  // invoke(fn, FieldName, MemberPtr);
                                                  fn(field_name_tuple, value.*(member_ptr));
                                              },
                                              [&fn, &value, &field_name_tuple](auto member_ptr, auto tag) {
                                                  // invoke(fn, FieldName, MemberPtr, tag);
                                                  fn(field_name_tuple, value.*(member_ptr), tag);
                                              }};

                        std::apply(overload, member_ptr_tuple);
                    }
                });

                return true;
            }
            else
            {
                return false;
            }
        });
    }

} // namespace static_reflection

#endif /* STATICREFLECTION_H */
