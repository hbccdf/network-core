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
MAKE_DB_CONSTRACT(name, MAKE_GENERAL_SEM_ARG_LIST(name, N, MAKE_DB_CONSTRACT_FILED, __VA_ARGS__)) \
MAKE_FIELD_PROXY_TUPLE(name, MAKE_GENERAL_SEM_ARG_LIST(name, N, MAKE_FIELD_PROXY, __VA_ARGS__)) \
MAKE_DB_TUPLE(name, MAKE_GENERAL_ARG_LIST(name, N, DB_PAIR_OBJECT, __VA_ARGS__)) \
MAKE_DB_META_TYPE(name, MAKE_GENERAL_ARG_LIST(name, N, RAW_DB_SINGLE_OBJECT, __VA_ARGS__)) \
constexpr static const std::array<const char*, N> _arr = {MAKE_DB_NAMES(name, MAKE_GENERAL_ARG_LIST(name, N, DB_OBJ_NAME, __VA_ARGS__))}; \
}; \
inline auto to_db_extend(name const*) {  \
return db_meta_##name{}; \
}\
using name##_query = cytx::get_db_extend_type_t<name>

#define DB_META(name, ...) EMMBED_DB_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
