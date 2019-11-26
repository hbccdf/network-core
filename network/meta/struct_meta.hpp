#pragma once
#include "meta_common.hpp"
#include "type_meta.hpp"

#define ST_PAIR_OBJECT(name, t)                 std::make_pair(#t, std::reference_wrapper<decltype(val.t)>(val.t))
#define ST_PAIR_CONST_OBJECT(name, t)           std::make_pair(#t, std::reference_wrapper<std::add_const_t<decltype(val.t)>>(val.t))
#define ST_SINGLE_OBJECT(name, t)               ((name*)nullptr)->t
#define ST_OBJ_NAME(name, t)                    #t



#define MAKE_ST_META_TYPE(name, ...)            using meta_type = decltype(std::make_tuple(__VA_ARGS__));
#define MAKE_ST_TUPLE(name, ...)                static auto Meta(name& val) { return std::make_tuple(__VA_ARGS__); }
#define MAKE_ST_CONST_TUPLE(name, ...)          static auto Meta(const name& val) { return std::make_tuple(__VA_ARGS__); }

#define EMBED_ST_TUPLE(name, N, ...)                                                                                \
REG_TYPE(name);                                                                                                     \
inline auto to_st_extend(const name*) {                                                                             \
    struct st_meta_##name{                                                                                          \
        static const char* meta_name() { return #name; }                                                            \
        MAKE_ST_TUPLE(name, MAKE_ARG_LIST(name, N, ST_PAIR_OBJECT, COMMA_DELIMITER, __VA_ARGS__))                   \
        MAKE_ST_META_TYPE(name, MAKE_ARG_LIST(name, N, ST_SINGLE_OBJECT, COMMA_DELIMITER, __VA_ARGS__))             \
        MAKE_ST_CONST_TUPLE(name, MAKE_ARG_LIST(name, N, ST_PAIR_CONST_OBJECT, COMMA_DELIMITER, __VA_ARGS__))       \
    };                                                                                                              \
    return st_meta_##name{};                                                                                        \
}

#define REG_META(name, ...) EMBED_ST_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)




#define MAKEB_ST_META_TYPE(name, ...)           using meta_type = decltype(std::tuple_cat(typename cytx::meta_t<base_t>::meta_type{}, std::make_tuple(__VA_ARGS__)));
#define MAKEB_ST_TUPLE(name, ...)               static auto Meta(name& val) { return std::tuple_cat(cytx::get_meta((base_t&)val), std::make_tuple(__VA_ARGS__)); }
#define MAKEB_ST_CONST_TUPLE(name, ...)         static auto Meta(const name& val) { return std::tuple_cat(cytx::get_meta((const base_t&)val), std::make_tuple(__VA_ARGS__)); }

#define EMBEDB_ST_TUPLE(name, base_name, N, ...)                                                                    \
REG_TYPE(name);                                                                                                     \
inline auto to_st_extend(const name*) {                                                                             \
    struct st_meta_##name{                                                                                          \
        using base_t = base_name;                                                                                   \
        static const char* meta_name() { return #name; }                                                            \
        MAKEB_ST_TUPLE(name, MAKE_ARG_LIST(name, N, ST_PAIR_OBJECT, COMMA_DELIMITER, __VA_ARGS__))                  \
        MAKEB_ST_META_TYPE(name, MAKE_ARG_LIST(name, N, ST_SINGLE_OBJECT, COMMA_DELIMITER, __VA_ARGS__))            \
        MAKEB_ST_CONST_TUPLE(name, MAKE_ARG_LIST(name, N, ST_PAIR_CONST_OBJECT, COMMA_DELIMITER, __VA_ARGS__))      \
    };                                                                                                              \
    return st_meta_##name{};                                                                                        \
}

#define REG_METAB(name, base_name, ...) EMBEDB_ST_TUPLE(name, base_name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)


namespace cytx
{
    inline void to_st_extend(...) {}

    namespace detail
    {
        template<typename T>
        struct get_st_extend_type
        {
            using type = decltype(to_st_extend((std::decay_t<T>*)nullptr));
        };
        TEMPLATE_TYPE(get_st_extend_type);

        template<typename T>
        struct st_meta : std::integral_constant<bool, !std::is_void_v<get_st_extend_type_t<T>>> {};

        TEMPLATE_VALUE(st_meta);

        template<typename T>
        struct get_meta_impl<T, std::void_t<std::enable_if_t<st_meta_v<T>>>>
        {
            using type = get_st_extend_type_t<std::decay_t<T>>;

            static auto meta(T&& t)
            {
                return get_st_extend_type_t<std::decay_t<T>>::Meta(t);
            }
        };

        template<typename T>
        struct get_meta_elem_impl<T, std::void_t<std::enable_if_t<st_meta_v<T>>>>
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
        struct is_reflection<T, std::void_t<std::enable_if_t<st_meta_v<T>>>> : std::true_type {};

        template<typename T>
        struct tuple_total_size<T, std::void_t<std::enable_if_t<st_meta_v<T>>>>
        {
            constexpr static int value = tuple_total_size<typename get_meta_impl<T>::type::meta_type>::value;
        };
    }

}