#pragma once
#include "meta_common.hpp"

#define RAW_DB_ADD_REFERENCE(t)         std::reference_wrapper<decltype(t)>(t)
#define RAW_DB_PAIR_OBJECT(name, t)     std::make_pair(#t, RAW_DB_ADD_REFERENCE(t))
#define RAW_DB_SINGLE_OBJECT(name, t)   t

#define DB_ADD_REFERENCE(t)             std::reference_wrapper<decltype(_##t)>(_##t)
#define DB_PAIR_OBJECT(name, t)         std::make_pair(#t, DB_ADD_REFERENCE(t))
#define DB_SINGLE_OBJECT(name, t)       _##t
#define DB_OBJ_NAME(name, t)            #t

#define MAKE_DB_META_TYPE(db_name, ...) using meta_type = decltype(std::make_tuple(__VA_ARGS__));
#define MAKE_DB_TUPLE(db_name, ...)     auto Meta() { return std::make_tuple(__VA_ARGS__); }
#define MAKE_DB_NAMES(db_name, ...)     __VA_ARGS__


#define MAKE_FIELD_PROXY(name, t) cytx::orm::field_proxy<decltype(t)> _##t{ &type_name(), &t, #t }
#define MAKE_FIELD_PROXY_TUPLE(db_name, ...) \
static const std::string& type_name() { static std::string str_db_name = #db_name; return str_db_name; } \
__VA_ARGS__; \

#define MAKE_DB_CONSTRACT_FILED(name, t) _##t = o._##t
#define MAKE_DB_CONSTRACT(db_name, ...) \
db_name(){} \
db_name(const db_name& o) \
{   \
    __VA_ARGS__; \
}


#define EMMBED_DB_TUPLE(name, N, ...) \
MAKE_FIELD_PROXY_TUPLE(name, MAKE_GENERAL_SEM_ARG_LIST(name, N, MAKE_FIELD_PROXY, __VA_ARGS__)) \
MAKE_DB_TUPLE(name, MAKE_GENERAL_ARG_LIST(name, N, DB_PAIR_OBJECT, __VA_ARGS__)) \
MAKE_DB_META_TYPE(name, MAKE_GENERAL_ARG_LIST(name, N, RAW_DB_SINGLE_OBJECT, __VA_ARGS__)) \
MAKE_DB_CONSTRACT(name, MAKE_GENERAL_SEM_ARG_LIST(name, N, MAKE_DB_CONSTRACT_FILED, __VA_ARGS__)) \
constexpr static const std::array<const char*, N> __arr = {MAKE_DB_NAMES(name, MAKE_GENERAL_ARG_LIST(name, N, DB_OBJ_NAME, __VA_ARGS__))} 

#define DB_META(name, ...) EMMBED_DB_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__) 