#pragma once
#include "../meta/meta.hpp"
#include <fmt/format.h>
#include <boost/lexical_cast.hpp>
#include "date_time.hpp"

namespace cytx
{
    template<typename T>
    using nullable = boost::optional<T>;

    template<typename T> struct is_nullable : is_specialization_of<std::decay_t<T>, boost::optional> {};

    namespace util
    {
        template<typename T>
        auto to_str(T t, const char* split_str = "::") -> std::enable_if_t<enum_meta<T>::value, std::string>
        {
            boost::optional<std::string> v = cytx::to_string(t, split_str);
            if (v)
                return v.get();
            else
                return fmt::format("{}", (uint32_t)t);
        }

        inline std::string to_str(uint32_t t, const char* enum_name, const char* split_str = "::")
        {
            boost::optional<std::string> v = cytx::to_string(t, enum_name, split_str);
            if (v)
                return v.get();
            else
                return fmt::format("{}", (uint32_t)t);
        }

        template<typename T>
        auto cast(const std::string& str) -> std::enable_if_t<std::is_same<T, bool>::value, T>
        {
            return str == "true" || str == "1" || str == "on" || (!str.empty() && str[0] != '0');
        }

        template<typename T>
        auto cast(const std::string& str) -> std::enable_if_t<!std::is_same<T, bool>::value && is_basic_type<T>::value, T>
        {
            return boost::lexical_cast<T>(str);
        }

        template<typename T>
        auto cast(const char* str) -> std::enable_if_t<std::is_same<T, date_time>::value, T>
        {
            return date_time::parse(str);
        }

        template<typename T>
        auto cast(const char* str) -> std::enable_if_t<is_nullable<T>::value, T>
        {
            if (str == nullptr)
                return T();
            else
            {
                return T(cast<std::decay_t<T::value_type>>(str));
            }
        }

        template<typename T>
        auto cast_string(const T& t) -> std::enable_if_t<std::is_same<T, date_time>::value, std::string>
        {
            return fmt::format("{}", t.to_string());
        }

        template<typename T>
        auto cast_string(const T& t) -> std::enable_if_t<is_nullable<T>::value, std::string>
        {
            if (t)
                return cast_string(t.get());
            else
                return "null";
        }
    }
}