#pragma once
#include "expr.hpp"

namespace cytx
{

#define MAKE_GENERAL_ARG_LIST_1(name, op, arg, ...)   op(name, arg)
#define MAKE_GENERAL_ARG_LIST_2(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_1(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_3(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_2(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_4(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_3(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_5(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_4(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_6(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_5(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_7(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_6(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_8(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_7(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_9(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_8(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_10(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_9(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_11(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_10(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_12(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_11(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_13(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_12(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_14(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_13(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_15(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_14(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_16(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_15(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_17(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_16(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_18(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_17(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_19(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_18(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_20(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_19(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_21(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_20(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_22(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_21(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_23(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_22(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_24(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_23(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_25(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_24(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_26(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_25(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_27(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_26(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_28(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_27(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_29(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_28(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_30(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_29(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_31(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_30(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_32(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_31(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_33(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_32(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_34(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_33(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_35(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_34(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_36(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_35(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_37(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_36(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_38(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_37(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_39(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_38(name, op, __VA_ARGS__))
#define MAKE_GENERAL_ARG_LIST_40(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_GENERAL_ARG_LIST_39(name, op, __VA_ARGS__))


#define MAKE_GENERAL_SEM_ARG_LIST_1(name, op, arg, ...)   op(name, arg)
#define MAKE_GENERAL_SEM_ARG_LIST_2(name, op, arg, ...)   op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_1(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_3(name, op, arg, ...)   op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_2(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_4(name, op, arg, ...)   op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_3(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_5(name, op, arg, ...)   op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_4(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_6(name, op, arg, ...)   op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_5(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_7(name, op, arg, ...)   op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_6(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_8(name, op, arg, ...)   op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_7(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_9(name, op, arg, ...)   op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_8(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_10(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_9(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_11(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_10(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_12(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_11(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_13(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_12(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_14(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_13(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_15(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_14(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_16(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_15(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_17(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_16(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_18(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_17(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_19(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_18(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_20(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_19(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_21(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_20(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_22(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_21(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_23(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_22(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_24(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_23(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_25(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_24(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_26(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_25(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_27(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_26(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_28(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_27(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_29(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_28(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_30(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_29(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_31(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_30(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_32(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_31(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_33(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_32(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_34(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_33(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_35(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_34(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_36(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_35(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_37(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_36(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_38(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_37(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_39(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_38(name, op, __VA_ARGS__))
#define MAKE_GENERAL_SEM_ARG_LIST_40(name, op, arg, ...)  op(name, arg); MARCO_EXPAND(MAKE_GENERAL_SEM_ARG_LIST_39(name, op, __VA_ARGS__))


#define MAKE_GENERAL_ARG_LIST(name, N, op, arg, ...) \
        MACRO_CONCAT(MAKE_GENERAL_ARG_LIST, N)(name, op, arg, __VA_ARGS__)

#define MAKE_GENERAL_SEM_ARG_LIST(name, N, op, arg, ...) \
        MACRO_CONCAT(MAKE_GENERAL_SEM_ARG_LIST, N)(name, op, arg, __VA_ARGS__)

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


#define MAKE_FIELD_PROXY(name, t) cytx::orm::field_proxy<decltype(t)> _##t{ &type_name(), t, #t }
#define MAKE_FIELD_PROXY_TUPLE(db_name, ...) \
static const std::string& type_name() { static std::string str_db_name = #db_name; return str_db_name; } \
__VA_ARGS__;


#define EMMBED_DB_TUPLE(name, N, ...) \
MAKE_FIELD_PROXY_TUPLE(name, MAKE_GENERAL_SEM_ARG_LIST(name, N, MAKE_FIELD_PROXY, __VA_ARGS__)) \
MAKE_DB_TUPLE(name, MAKE_GENERAL_ARG_LIST(name, N, DB_PAIR_OBJECT, __VA_ARGS__)) \
MAKE_DB_META_TYPE(name, MAKE_GENERAL_ARG_LIST(name, N, RAW_DB_SINGLE_OBJECT, __VA_ARGS__)) \
constexpr static const std::array<const char*, N> __arr = {MAKE_DB_NAMES(name, MAKE_GENERAL_ARG_LIST(name, N, DB_OBJ_NAME, __VA_ARGS__))}

#define DB_META(name, ...) EMMBED_DB_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__) 

HAS_FUNC(type_name);

template <typename T, class = std::void_t<>>
struct is_reflection : std::false_type
{
};

// this way of using SFINEA is type reference and cv qualifiers immuned
template <typename T>
struct is_reflection<T, std::void_t<std::enable_if_t<has_type_name_v<T>>>> : std::true_type
{
};

template<size_t I, typename F, typename T>
void apply_value(F&& f, T&& t, bool is_last)
{
    std::forward<F>(f)(std::forward<T>(t), I, is_last);
};

template<size_t I, typename F, typename T>
constexpr void for_each_impl(F&& f, T&&t, bool is_last, std::void_t<std::enable_if_t<is_reflection<T>::value>> *)
{
    apply(std::forward<F>(f), std::forward<T>(t), std::forward<T>(t).Meta(), std::make_index_sequence<get_value<T>()>{});
}

//-------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------//
template<size_t I, typename F, typename F1, typename T>
std::enable_if_t<is_reflection<T>::value> apply_value(F&& f, F1&& f1, T&& t, bool is_last)
{
    std::forward<F1>(f1)(std::forward<T>(t), I, is_last);
}

template<size_t I, typename F, typename F1, typename T>
std::enable_if_t<!is_reflection<T>::value> apply_value(F&& f, F1&& f1, T&& t, bool is_last)
{
    std::forward<F>(f)(std::forward<T>(t), I, is_last);
}

template<size_t I, typename F, typename F1, typename T>
constexpr void for_each_impl(F&& f, F1&& f1, T&&t, bool is_last, std::void_t<std::enable_if_t<is_reflection<T>::value>> *)
{
    apply(std::forward<F>(f), std::forward<F1>(f1), std::forward<T>(t), std::forward<T>(t).Meta(), std::make_index_sequence<get_value<T>()>{});
}

template<typename F, typename F1, typename T, typename... Rest, std::size_t I0, std::size_t... I>
constexpr void apply(F&& f, F1&& f1, T&&t, std::tuple<Rest...>&& tp, std::index_sequence<I0, I...>)
{
    apply_value<I0>(std::forward<F>(f), std::forward<F1>(f1), std::forward<T>(t).*(std::get<I0>(tp)), sizeof...(I) == 0);
    apply(std::forward<F>(f), std::forward<F1>(f1), std::forward<T>(t), (std::tuple<Rest...>&&)tp, std::index_sequence<I...>{});
}

template<typename F, typename F1, typename T, typename... Rest>
constexpr void apply(F&& f, F1&&, T&& t, std::tuple<Rest...>&&, std::index_sequence<>)
{
}
//-------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------//

template<typename F, typename T, typename... Rest, std::size_t I0, std::size_t... I>
constexpr void apply(F&& f, T&&t, std::tuple<Rest...>&& tp, std::index_sequence<I0, I...>)
{
    apply_value<I0>(std::forward<F>(f), (std::get<I0>(tp)), sizeof...(I) == 0);
    apply(std::forward<F>(f), std::forward<T>(t), (std::tuple<Rest...>&&)tp, std::index_sequence<I...>{});
}

template<typename F, typename T, typename... Rest>
constexpr void apply(F&& f, T&& t, std::tuple<Rest...>&&, std::index_sequence<>)
{
}

template<typename F, typename... Rest, std::size_t I0, std::size_t... I>
constexpr void apply_tuple(F&& f, std::tuple<Rest...>& tp, std::index_sequence<I0, I...>)
{
    apply_value<I0>(std::forward<F>(f), std::get<I0>(tp), sizeof...(I) == 0);
    apply_tuple(std::forward<F>(f), tp, std::index_sequence<I...>{});
}

template<typename F, typename... Rest>
constexpr void apply_tuple(F&& f, std::tuple<Rest...>&, std::index_sequence<>)
{
}

template<size_t I, typename T>
constexpr decltype(auto) get(T&& t)
{
    return std::forward<T>(t).*(std::get<I>(T::Meta()));
}

template<typename T, typename F>
constexpr auto for_each(T&& t, F&& f) -> std::enable_if_t<is_reflection<T>::value>
{
    for_each_impl<0>(std::forward<F>(f), std::forward<T>(t), false, (void *)nullptr);
}

template<typename T, typename F, typename F1>
constexpr auto for_each(T&& t, F&& f, F1&& f1) -> std::enable_if_t<is_reflection<T>::value>
{
    for_each_impl<0>(std::forward<F>(f), std::forward<F1>(f1), std::forward<T>(t), false, (void *)nullptr);
}

template<typename T, typename F>
constexpr auto for_each(T&& tp, F&& f) -> std::enable_if_t<!is_reflection<T>::value>
{
    apply_tuple(std::forward<F>(f), std::forward<T>(tp), std::make_index_sequence<std::tuple_size<std::remove_reference_t <T>>::value>{});
}

template<typename T, size_t  I>
constexpr const char* get_name()
{
    static_assert(I < get_value<T>(), "out of range");
    return T::__arr[i];
}

template<typename T>
const char* get_name()
{
    return T::type_name();
}

template<typename T>
const char* get_name(size_t i)
{
    return i >= get_value<T>() ? "" : T::__arr[i];
}

template<typename T>
constexpr auto get_value() -> std::enable_if_t<is_reflection<T>::value, size_t>
{
    return std::tuple_size<typename std::decay_t<T>::meta_type>::value;
}

template<typename T>
constexpr auto get_value() -> std::enable_if_t<!is_reflection<T>::value, size_t>
{
    return 0;
}

}