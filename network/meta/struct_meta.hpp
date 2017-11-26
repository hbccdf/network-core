#pragma once
#include "meta_common.hpp"

#define ST_PAIR_OBJECT(name, t)         std::make_pair(#t, std::reference_wrapper<decltype(val.t)>(val.t))
#define ST_SINGLE_OBJECT(name, t)       ((name*)nullptr)->t
#define ST_OBJ_NAME(name, t)            #t

#define MAKE_ST_META_TYPE(name, ...)    using meta_type = decltype(std::make_tuple(__VA_ARGS__));
#define MAKE_ST_TUPLE(name, ...)        static auto Meta(name& val) { return std::make_tuple(__VA_ARGS__); }

#define EMBED_ST_TUPLE(name, N, ...) \
template<>  \
struct st_meta<name> : public std::true_type {  \
MAKE_ST_TUPLE(name, MAKE_ARG_LIST(name, N, ST_PAIR_OBJECT, COMMA_DELIMITER, __VA_ARGS__))   \
MAKE_ST_META_TYPE(name, MAKE_ARG_LIST(name, N, ST_SINGLE_OBJECT, COMMA_DELIMITER, __VA_ARGS__)) \
static const char* meta_name() { return #name; } \
}

#define REG_META(name, ...) EMBED_ST_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)


namespace cytx
{
    namespace detail
    {
        /*template<typename T, typename Field>
        struct field_proxy
        {
            using type_t = T;
            using val_t = Field;

        private:
            val_t* val;

        public:
            val_t& value(T& t)
            {
                return
            }
        };*/
    }
}

template<typename T>
struct st_meta : public std::false_type {};

template<typename T>
constexpr bool st_meta_v = st_meta<std::decay_t<T>>::value;