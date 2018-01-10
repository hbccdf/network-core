#pragma once
#include "meta_common.hpp"

#define PAIR_ENUM(name, t) std::make_pair(#t, name::t)
#define MAKE_ENUM_TUPLE(enum_name, ...) \
inline auto to_enum_extend(enum_name const*) {  \
struct enum_meta_##enum_name \
    { static auto Meta() { return std::make_tuple(__VA_ARGS__);  } \
    static const char* name() { return #enum_name; }


#define EMMBED_ENUM_TUPLE(name, N, ...) \
MAKE_ENUM_TUPLE(name, MAKE_ARG_LIST(name, N, PAIR_ENUM, COMMA_DELIMITER, __VA_ARGS__))

#define REG_ENUM(name, ...) EMMBED_ENUM_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__) \
    static const char* alias_name() { return nullptr; } \
}; \
return enum_meta_##name{}; \
} \
namespace ___reg_enum_helper_value___ ## name ## __LINE__ { static int val = cytx::detail::reg_enum<name>(); }

#define REG_ALIAS_ENUM(name, alias_name_str, ...) EMMBED_ENUM_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__) \
    static const char* alias_name() { return #alias_name_str; } \
}; \
return enum_meta_##name{}; \
} \
namespace ___reg_enum_helper_value___ ## name ## __LINE__ { static int val = cytx::detail::reg_enum<name>(); }
