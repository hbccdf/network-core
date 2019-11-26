#pragma once
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include <fmt/format.h>
#include <boost/optional.hpp>

#include "base_meta.hpp"
#include "enum_meta.hpp"
#include "db_meta.hpp"
#include "struct_meta.hpp"
#include "type_meta.hpp"

namespace cytx
{
    namespace detail
    {
        template<typename F, typename T>
        struct is_result_bool
        {
            constexpr static bool value = !std::is_void_v<std::result_of_t<F(T, size_t, bool)>>;
        };

        TEMPLATE_VALUE(is_result_bool);
    }

    template<typename T>
    using meta_t = typename detail::get_meta_impl<T>::type;

    template<typename T>
    using is_reflection = detail::is_reflection<T>;

    TEMPLATE_VALUE(is_reflection);


    template<typename T>
    auto get_meta()
    {
        return detail::get_meta_impl<T>::meta();
    }

    template<typename T>
    auto get_meta(T&& t)
    {
        return detail::get_meta_impl<T>::meta(std::forward<T>(t));
    }

    template<typename T>
    const char* get_name()
    {
        return meta_t<T>::meta_name();
    }


    template<typename T>
    constexpr auto get_meta_size() -> std::enable_if_t<is_reflection_v<T>, size_t>
    {
        return std::tuple_size<typename meta_t<T>::meta_type>::value;
    }

    template<typename T>
    constexpr auto get_meta_size() -> std::enable_if_t<is_tuple<T>::value, size_t>
    {
        return std::tuple_size<std::decay_t<T>>::value;
    }

    template<typename T>
    constexpr auto get_meta_size() -> std::enable_if_t<!is_reflection<T>::value && !is_tuple<T>::value, size_t>
    {
        return 0;
    }

    template<size_t I, typename T>
    constexpr auto get(T&& t)
    {
        return detail::get_meta_elem_impl<T>::template get<I>(std::forward<T>(t));
    }

    template<size_t I, typename T, typename MetaTuple>
    constexpr auto get(T&& t, MetaTuple&& tp)
    {
        return detail::get_meta_elem_impl<T>::template get<I>(std::forward<T>(t), std::forward<MetaTuple>(tp));
    }

    template<size_t I, typename F, typename T>
    auto apply_value(F&& f, T&& t, bool is_last) -> std::enable_if_t<!std::is_void<decltype(f(t, I, is_last))>::value, bool>
    {
        //return std::forward<F>(f)(std::forward<T>(t), I, is_last);
        return f(t, I, is_last);
    };

    template<size_t I, typename F, typename T>
    auto apply_value(F&& f, T&& t, bool is_last) -> std::enable_if_t<std::is_void<decltype(f(t, I, is_last))>::value, bool>
    {
        //std::forward<F>(f)(std::forward<T>(t), I, is_last);
        f(t, I, is_last);
        return true;
    };

    template<typename ... Args>
    constexpr void apply(Args&& ... args)
    {
    }

    //meta
    template<typename F, typename T, typename Tuple, std::size_t I0, std::size_t... I>
    constexpr void apply(F&& f, T&& t, Tuple&& tp, std::index_sequence<I0, I...>)
    {
        if (apply_value<I0>(std::forward<F>(f), get<I0>(t, tp), sizeof...(I) == 0))
        {
            apply(std::forward<F>(f), std::forward<T>(t), std::forward<Tuple>(tp), std::index_sequence<I...>{});
        }
    }

    //tuple
    template<typename F, typename Tuple, std::size_t I0, std::size_t... I>
    constexpr auto apply(F&& f, Tuple&& tp, std::index_sequence<I0, I...>) -> std::enable_if_t<is_tuple<Tuple>::value>
    {
        if (apply_value<I0>(std::forward<F>(f), std::get<I0>(tp), sizeof...(I) == 0))
        {
            apply(std::forward<F>(f), std::forward<Tuple>(tp), std::index_sequence<I...>{});
        }
    }


    template<typename T, typename F>
    constexpr auto for_each(T&& t, F&& f) -> std::enable_if_t<is_reflection<T>::value>
    {
        apply(std::forward<F>(f), std::forward<T>(t), get_meta(std::forward<T>(t)), std::make_index_sequence<get_meta_size<T>()>{});
    }

    template<typename T, typename F>
    constexpr auto for_each(T&& t, F&& f) -> std::enable_if_t<!is_reflection<T>::value>
    {
        apply(std::forward<F>(f), std::forward<T>(t), std::make_index_sequence<get_meta_size<T>()>{});
    }
}