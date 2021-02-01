#ifndef TUPLEHELPER_H
#define TUPLEHELPER_H

#include <tuple>
#include <type_traits>

#if(defined(_MSVC_LANG) && _MSVC_LANG < 201402L) || ((!defined(_MSVC_LANG)) && __cplusplus < 201402L)
namespace std
{
    template<bool B, class T = void>
    using enable_if_t = typename enable_if<B, T>::type;

    template<class T>
    using decay_t = typename decay<T>::type;

    template<typename T, T... ints>
    struct integer_sequence
    {
    };

    template<typename T, T N, typename = void>
    struct make_integer_sequence_impl
    {
        template<typename>
        struct tmp;

        template<T... Prev>
        struct tmp<integer_sequence<T, Prev...>>
        {
            using type = integer_sequence<T, Prev..., N - 1>;
        };

        using type = typename tmp<typename make_integer_sequence_impl<T, N - 1>::type>::type;
    };

    template<typename T, T N>
    struct make_integer_sequence_impl<T, N, typename std::enable_if<N == 0>::type>
    {
        using type = integer_sequence<T>;
    };

    template<typename T, T N>
    using make_integer_sequence = typename make_integer_sequence_impl<T, N>::type;

    template<size_t... _Vals>
    using index_sequence = integer_sequence<size_t, _Vals...>;

    template<size_t _Size>
    using make_index_sequence = make_integer_sequence<size_t, _Size>;

    // TEMPLATE CLASS _Cat_base
    template<bool _Val>
    struct _Cat_base : integral_constant<bool, _Val>
    { // base class for type predicates
    };

    template<class _Ty>
    struct is_null_pointer : _Cat_base<is_same<typename remove_cv<_Ty>::type, nullptr_t>::value>
    { // determine whether _Ty is nullptr_t
    };
} // namespace std
#endif //#if __cplusplus != 201402L

//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Lambda, typename Tuple, std::size_t first, std::size_t... is>
inline constexpr void for_each_tupleImpl(Tuple&& tuple, Lambda&& lambda, std::index_sequence<first, is...>)
{
    lambda(std::get<first>(std::forward<Tuple>(tuple)));
    if constexpr(sizeof...(is) > 0)
    {
        for_each_tupleImpl(std::forward<Tuple>(tuple), std::forward<Lambda>(lambda), std::index_sequence<is...>{});
    }
}

template<typename Lambda, typename Tuple>
inline constexpr void for_each_tuple(Tuple&& tuple, Lambda&& lambda)
{
    for_each_tupleImpl(std::forward<Tuple>(tuple),
                       std::forward<Lambda>(lambda),
                       std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Lambda, typename Tuple, std::size_t first, std::size_t... is>
inline constexpr void for_each_tuple_indexImpl(Tuple&& tuple, Lambda&& lambda, std::index_sequence<first, is...>)
{
    lambda(std::integral_constant<std::size_t, first>(), std::forward<Tuple>(tuple));
    if constexpr(sizeof...(is) > 0)
    {
        for_each_tuple_indexImpl(std::forward<Tuple>(tuple), std::forward<Lambda>(lambda), std::index_sequence<is...>{});
    }
}

template<typename Lambda, typename Tuple>
inline constexpr void for_each_tuple_index(Tuple&& tuple, Lambda&& lambda)
{
    for_each_tuple_indexImpl(std::forward<Tuple>(tuple),
                             std::forward<Lambda>(lambda),
                             std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Lambda, typename Tuple>
inline constexpr bool find_if_tupleImpl(Tuple&& tuple, Lambda&& lambda, std::index_sequence<>)
{
    return false;
}

template<typename Lambda, typename Tuple, std::size_t first, std::size_t... is>
inline constexpr bool find_if_tupleImpl(Tuple&& tuple, Lambda&& lambda, std::index_sequence<first, is...>)
{
    // if any lambda return true, return turel
    auto result = lambda(std::get<first>(std::forward<Tuple>(tuple)));
    if constexpr(sizeof...(is) > 0)
    {
        return result || find_if_tupleImpl(std::forward<Tuple>(tuple), std::forward<Lambda>(lambda), std::index_sequence<is...>{});
    }
    else
    {
        return result;
    }
}

template<typename Lambda, typename Tuple>
inline constexpr bool find_if_tuple(Tuple&& tuple, Lambda&& lambda)
{
    return find_if_tupleImpl(std::forward<Tuple>(tuple),
                             std::forward<Lambda>(lambda),
                             std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Lambda, typename Tuple, std::size_t first, std::size_t... is>
inline constexpr bool find_if_tuple_indexImpl(Tuple&& tuple, Lambda&& lambda, std::index_sequence<first, is...>)
{
    // if any lambda return true, return ture
    auto result = lambda(std::integral_constant<std::size_t, first>(), std::forward<Tuple>(tuple));
    if constexpr(sizeof...(is) > 0)
    {
        return result || find_if_tuple_indexImpl(std::forward<Tuple>(tuple), std::forward<Lambda>(lambda), std::index_sequence<is...>{});
    }
    else
    {
        return result;
    }
}

template<typename Lambda, typename Tuple>
inline constexpr bool find_if_tuple_index(Tuple&& tuple, Lambda&& lambda)
{
    return find_if_tuple_indexImpl(std::forward<Tuple>(tuple),
                                   std::forward<Lambda>(lambda),
                                   std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

template<class... Ts>
struct overloaded_t : Ts...
{
    using Ts::operator()...;
};
template<class... Ts>
overloaded_t(Ts...)->overloaded_t<Ts...>;



//////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Tuple>
struct unpack_warp_t 
{
  Tuple&& tuple;
  using count = std::tuple_size<std::remove_reference_t<Tuple>>;
  using indexes = std::make_index_sequence<count{}>;
 
  template<class F, std::size_t...Is>
  decltype(auto) _unpacker(F&& f, std::index_sequence<Is...>) 
  {
    return f( std::get<Is>(std::forward<Tuple>(tuple))... ); 
  }

  template<class F>
  decltype(auto) unpack( F&& f )
  {
    return _unpacker( std::forward<F>(f), indexes{} );
  }
};

template<class Tuple, class F>
decltype(auto) unpack(Tuple&& tuple, F&& f ) 
{
    unpack_warp_t<Tuple> unpack_warp{std::forward<Tuple>(tuple)};
    return unpack_warp.unpack(std::forward<F>(f));
}

/*
for(const auto& pair_v : map_val)
{
    unpack(pair_v, [](auto& x, auto&y)
    {
    });
}
*/

#endif /* TUPLEHELPER_H */
