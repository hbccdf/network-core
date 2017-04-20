#pragma once
#include "meta_common.hpp"

#define ADD_REFERENCE(t)        std::reference_wrapper<decltype(t)>(t)
#define ADD_REFERENCE_CONST(t)  std::reference_wrapper<std::add_const_t<decltype(t)>>(t)
#define PAIR_OBJECT(t)          std::make_pair(#t, ADD_REFERENCE(t))
#define PAIR_OBJECT_CONST(t)    std::make_pair(#t, ADD_REFERENCE_CONST(t))
#define SINGLE_OBJECT(t)        t

#define MAKE_TUPLE(...)         auto Meta() { return std::make_tuple(__VA_ARGS__); }
#define MAKE_TUPLE_CONST(...)   auto Meta() const { return std::make_tuple(__VA_ARGS__); }
#define MAKE_META_TYPE(...)     using meta_type = decltype(std::make_tuple(__VA_ARGS__))

#define EMMBED_TUPLE(N, ...) \
MAKE_TUPLE(MAKE_ARG_LIST(N, PAIR_OBJECT, __VA_ARGS__)) \
MAKE_TUPLE_CONST(MAKE_ARG_LIST(N, PAIR_OBJECT_CONST, __VA_ARGS__)) \
MAKE_META_TYPE(MAKE_ARG_LIST(N, SINGLE_OBJECT, __VA_ARGS__))

#define META(...) EMMBED_TUPLE(GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)