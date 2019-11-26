#pragma once
#include "meta_common.hpp"

#define ADD_REFERENCE(t)                std::reference_wrapper<decltype(t)>(t)
#define ADD_REFERENCE_CONST(t)          std::reference_wrapper<std::add_const_t<decltype(t)>>(t)
#define PAIR_OBJECT(name, t)            std::make_pair(#t, ADD_REFERENCE(t))
#define PAIR_OBJECT_CONST(name, t)      std::make_pair(#t, ADD_REFERENCE_CONST(t))
#define SINGLE_OBJECT(name, t)          t



#define MAKE_TUPLE(...)                 auto Meta() { return std::make_tuple(__VA_ARGS__); }
#define MAKE_TUPLE_CONST(...)           auto Meta() const { return std::make_tuple(__VA_ARGS__); }
#define MAKE_META_TYPE(...)             using meta_type = decltype(std::make_tuple(__VA_ARGS__))

#define EMMBED_TUPLE(N, ...) \
    MAKE_TUPLE(MAKE_ARG_LIST("", N, PAIR_OBJECT, COMMA_DELIMITER, __VA_ARGS__)) \
    MAKE_TUPLE_CONST(MAKE_ARG_LIST("", N, PAIR_OBJECT_CONST, COMMA_DELIMITER, __VA_ARGS__)) \
    MAKE_META_TYPE(MAKE_ARG_LIST("", N, SINGLE_OBJECT, COMMA_DELIMITER, __VA_ARGS__))

#define META(...) EMMBED_TUPLE(GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)




#define MAKEB_TUPLE(name, ...)          auto Meta() { return std::tuple_cat(name::Meta(), std::make_tuple(__VA_ARGS__)); }
#define MAKEB_TUPLE_CONST(name, ...)    auto Meta() const { return std::tuple_cat(name::Meta(), std::make_tuple(__VA_ARGS__)); }
#define MAKEB_META_TYPE(name, ...)      using meta_type = decltype(std::tuple_cat(((name*)nullptr)->Meta(), std::make_tuple(__VA_ARGS__)))

#define EMMBEDB_TUPLE(name, N, ...) \
    MAKEB_TUPLE(name, MAKE_ARG_LIST("", N, PAIR_OBJECT, COMMA_DELIMITER, __VA_ARGS__)) \
    MAKEB_TUPLE_CONST(name, MAKE_ARG_LIST("", N, PAIR_OBJECT_CONST, COMMA_DELIMITER, __VA_ARGS__)) \
    MAKEB_META_TYPE(name, MAKE_ARG_LIST("", N, SINGLE_OBJECT, COMMA_DELIMITER, __VA_ARGS__))

#define METAB(name, ...) EMMBEDB_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)


namespace cytx
{
    namespace detail
    {
        template<typename T>
        struct get_meta_impl<T, std::void_t<std::enable_if_t<has_meta_macro_v<T>>>>
        {
            using type = std::decay_t<T>;

            static auto meta(T&& t)
            {
                return t.Meta();
            }
        };

        template<typename T>
        struct get_meta_elem_impl<T, std::void_t<std::enable_if_t<has_meta_macro_v<T>>>>
        {
            template<size_t I, typename MetaTuple>
            static auto get(T&& t, MetaTuple&& tp)
            {
                return std::get<I>(tp);
            }

            template<size_t I>
            static auto get(T&& t)
            {
                return get<I>(std::forward<T>(t), get_meta_impl<T>::meta(std::forward<T>(t)));
            }
        };

        template<typename T>
        struct is_reflection<T, std::void_t<std::enable_if_t<has_meta_macro_v<T>>>> : std::true_type {};

        template<typename T>
        struct tuple_total_size<T, std::void_t<std::enable_if_t<has_meta_macro_v<T>>>>
        {
            constexpr static int value = tuple_total_size<typename get_meta_impl<T>::type::meta_type>::value;
        };
    }
}