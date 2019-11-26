#pragma once
#include "meta_common.hpp"

#define RAW_DB_ADD_REFERENCE(t)         std::reference_wrapper<decltype(t)>(t)
#define RAW_DB_PAIR_OBJECT(name, t)     std::make_pair(#t, RAW_DB_ADD_REFERENCE(t))
#define RAW_DB_SINGLE_OBJECT(name, t)   t

#define DB_ADD_REFERENCE(t)             std::reference_wrapper<decltype(t)>(t)
#define DB_PAIR_OBJECT(name, t)         std::make_pair(#t, DB_ADD_REFERENCE(t))
#define DB_SINGLE_OBJECT(name, t)       t
#define DB_OBJ_NAME(name, t)            #t


#define MAKE_DB_META_TYPE(db_name, ...) using meta_type = decltype(std::make_tuple(__VA_ARGS__));
#define MAKE_DB_TUPLE(db_name, ...)     auto Meta() { return std::make_tuple(__VA_ARGS__); }
#define MAKE_DB_NAMES(db_name, ...)     __VA_ARGS__


#define MAKE_FIELD_PROXY(name, t) cytx::orm::field_proxy<db_meta_##name, decltype(st_.t)> t{ st_ptr_, &value_type::t, #t }
#define MAKE_FIELD_PROXY_TUPLE(db_name, ...) \
static const char* meta_name() { return #db_name; } \
__VA_ARGS__; \

#define MAKE_DB_CONSTRACT_FILED(name, t) t = o.t
#define MAKE_DB_CONSTRACT(db_name, ...) \
    db_meta_##db_name(const db_meta_##db_name& o) \
    {   \
        __VA_ARGS__; \
    }

#define EMMBED_DB_TUPLE(name, N, ...) \
struct db_meta_##name : public cytx::base_db_meta { \
private: \
    name st_; \
    name* st_ptr_{&st_}; \
public: \
    using value_type = name; \
    value_type& Value() { return st_; } \
    db_meta_##name(){} \
    db_meta_##name(value_type& v) { st_ptr_ = &v; } \
    MAKE_DB_CONSTRACT(name, MAKE_ARG_LIST(name, N, MAKE_DB_CONSTRACT_FILED, SEM_DELIMITER, __VA_ARGS__)) \
    MAKE_FIELD_PROXY_TUPLE(name, MAKE_ARG_LIST(name, N, MAKE_FIELD_PROXY, SEM_DELIMITER, __VA_ARGS__)) \
    MAKE_DB_TUPLE(name, MAKE_ARG_LIST(name, N, DB_PAIR_OBJECT, COMMA_DELIMITER, __VA_ARGS__)) \
    MAKE_DB_META_TYPE(name, MAKE_ARG_LIST(name, N, RAW_DB_SINGLE_OBJECT, COMMA_DELIMITER, __VA_ARGS__)) \
    constexpr static const std::array<const char*, N> _arr = {MAKE_DB_NAMES(name, MAKE_ARG_LIST(name, N, DB_OBJ_NAME, COMMA_DELIMITER, __VA_ARGS__))}; \
}; \
inline auto to_db_extend(name const*) {  \
    return db_meta_##name{}; \
}\
using name##_query = cytx::detail::get_db_extend_type_t<name>

#define DB_META(name, ...) EMMBED_DB_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)


namespace cytx
{
    inline void to_db_extend(...) {}

    struct base_db_meta {};

    namespace detail
    {
        template<typename T>
        struct get_db_extend_type
        {
            using type = decltype(to_db_extend((std::decay_t<T>*)nullptr));
        };
        TEMPLATE_TYPE(get_db_extend_type);

        template<typename T>
        struct db_meta : std::integral_constant<bool, !std::is_void_v<get_db_extend_type_t<T>>> {};

        TEMPLATE_VALUE(db_meta);

        template<typename T> struct is_db_meta : std::integral_constant<bool, std::is_base_of<base_db_meta, std::decay_t<T>>::value> {};

        TEMPLATE_VALUE(is_db_meta);

        template<typename T>
        struct get_meta_impl<T, std::void_t<std::enable_if_t<db_meta_v<T>>>>
        {
            using type = get_db_extend_type_t<T>;

            static auto meta(T&& t)
            {
                return t.Meta();
            }
        };

        template<typename T>
        struct get_meta_elem_impl<T, std::void_t<std::enable_if_t<db_meta_v<T>>>>
        {
            template<size_t I, typename MetaTuple>
            static auto get(T&& t, MetaTuple&& tp)
            {
                return std::forward<T>(t).*(std::get<I>(tp));
            }

            template<size_t I>
            static auto get(T&& t)
            {
                return get<I>(std::forward<T>(t), get_meta_impl<T>::meta(std::forward<T>(t)));
            }
        };

        template<typename T>
        struct is_reflection<T, std::void_t<std::enable_if_t<db_meta_v<T>>>> : std::true_type {};
    }
}
