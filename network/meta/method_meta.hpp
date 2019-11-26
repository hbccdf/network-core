#pragma once
#include "meta_common.hpp"


namespace cytx
{
    inline void to_method_extend(...) {}

    namespace detail
    {
        /*template<typename T>
        struct get_method_extend_type
        {
            using type = decltype(to_method_extend((std::decay_t<T>*)nullptr));
        };
        TEMPLATE_TYPE(get_method_extend_type);*/

        /*template<typename T>
        struct method_meta : std::integral_constant<bool, !std::is_void_v<get_method_extend_type_t<T>>> {};

        TEMPLATE_VALUE(method_meta);*/

        /*template<typename T>
        struct get_meta_impl<T, std::void_t<std::enable_if_t<method_meta_v<T>>>>
        {
        using type = get_method_extend_type_t<std::decay_t<T>>;

        static auto meta()
        {
        return get_method_extend_type_t<T>::Meta();
        }
        };*/

        /*template<typename T>
        struct get_meta_elem_impl<T, std::void_t<std::enable_if_t<method_meta_v<T>>>>
        {
        template<size_t I, typename MetaTuple>
        static auto get(MetaTuple&& tp)
        {
        return std::get<I>(tp);
        }

        template<size_t I>
        static auto get()
        {
        return get<I>(get_meta_impl<T>::meta());
        }
        };*/

        /*template<typename T>
        struct is_reflection<T, std::void_t<std::enable_if_t<method_meta_v<T>>>> : std::true_type {};*/
    }
}