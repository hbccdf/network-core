#pragma once
#include "meta_common.hpp"

#define ST_PAIR_OBJECT(name, t)                 std::make_pair(#t, std::reference_wrapper<decltype(val.t)>(val.t))
#define ST_PAIR_CONST_OBJECT(name, t)           std::make_pair(#t, std::reference_wrapper<std::add_const_t<decltype(val.t)>>(val.t))
#define ST_SINGLE_OBJECT(name, t)               ((name*)nullptr)->t
#define ST_OBJ_NAME(name, t)                    #t

#define MAKE_ST_META_TYPE(name, ...)            using meta_type = decltype(std::make_tuple(__VA_ARGS__));
#define MAKE_ST_TUPLE(name, ...)                static auto Meta(name& val) { return std::make_tuple(__VA_ARGS__); }
#define MAKE_ST_CONST_TUPLE(name, ...)          static auto Meta(const name& val) { return std::make_tuple(__VA_ARGS__); }

#define EMBED_ST_TUPLE(name, N, ...) \
template<>  \
struct st_meta<name> : public std::true_type {  \
static const char* meta_name() { return #name; } \
MAKE_ST_TUPLE(name, MAKE_ARG_LIST(name, N, ST_PAIR_OBJECT, COMMA_DELIMITER, __VA_ARGS__))   \
MAKE_ST_META_TYPE(name, MAKE_ARG_LIST(name, N, ST_SINGLE_OBJECT, COMMA_DELIMITER, __VA_ARGS__)) \
MAKE_ST_CONST_TUPLE(name, MAKE_ARG_LIST(name, N, ST_PAIR_CONST_OBJECT, COMMA_DELIMITER, __VA_ARGS__))   \
}

#define REG_META(name, ...) EMBED_ST_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)




#define MAKEB_ST_META_TYPE(name, ...)           using meta_type = decltype(std::tuple_cat(typename cytx::meta_t<base_t>::meta_type{}, std::make_tuple(__VA_ARGS__)));
#define MAKEB_ST_TUPLE(name, ...)               static auto Meta(name& val) { return std::tuple_cat(get_meta((base_t&)val), std::make_tuple(__VA_ARGS__)); }
#define MAKEB_ST_CONST_TUPLE(name, ...)         static auto Meta(const name& val) { return std::tuple_cat(get_meta((const base_t&)val), std::make_tuple(__VA_ARGS__)); }

#define EMBEDB_ST_TUPLE(name, base_name, N, ...) \
template<>  \
struct st_meta<name> : public std::true_type {  \
using base_t = base_name;   \
static const char* meta_name() { return #name; } \
MAKEB_ST_TUPLE(name, MAKE_ARG_LIST(name, N, ST_PAIR_OBJECT, COMMA_DELIMITER, __VA_ARGS__))   \
MAKEB_ST_META_TYPE(name, MAKE_ARG_LIST(name, N, ST_SINGLE_OBJECT, COMMA_DELIMITER, __VA_ARGS__)) \
MAKEB_ST_CONST_TUPLE(name, MAKE_ARG_LIST(name, N, ST_PAIR_CONST_OBJECT, COMMA_DELIMITER, __VA_ARGS__))   \
}

#define REG_METAB(name, base_name, ...) EMBEDB_ST_TUPLE(name, base_name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)


template<typename T>
struct st_meta : public std::false_type {};

template<typename T>
constexpr bool st_meta_v = st_meta<std::decay_t<T>>::value;