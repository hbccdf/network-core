#pragma once
#include "network/meta/meta.hpp"
#include <fmt/format.h>
#include <boost/lexical_cast.hpp>
#include "date_time.hpp"

namespace cytx
{
    namespace util
    {
        template<typename T>
        auto to_str(T t, bool has_enum_name = true) -> std::enable_if_t<std::is_enum<T>::value, std::string>
        {
            boost::optional<std::string> v = cytx::to_string(t, has_enum_name);
            if (v)
                return v.get();
            else
                return fmt::format("{}", (uint32_t)t);
        }

        inline std::string to_str(uint32_t t, const char* enum_name, bool has_enum_name = true)
        {
            boost::optional<std::string> v = cytx::to_string(t, enum_name, has_enum_name);
            if (v)
                return v.get();
            else
                return fmt::format("{}", (uint32_t)t);
        }

        template<typename T>
        auto cast(const std::string& str) -> std::enable_if_t<std::is_same<T, bool>::value, T>
        {
            return !(str.empty() || str == "false" || str == "FALSE" || str == "0" || str == "off" || str == "OFF");
        }

        template<typename T>
        auto cast(const std::string& str) -> std::enable_if_t<std::is_enum<std::decay_t<T>>::value, T>
        {
            using enum_t = std::decay_t<T>;
            using under_type = std::underlying_type_t<enum_t>;

            auto ret = cytx::to_enum<T>(str.c_str(), true);
            if (ret)
            {
                return ret.value();
            }
            else
            {
                return (enum_t)boost::lexical_cast<under_type>(str);
            }
        }

        template<typename T>
        auto cast(const std::string& str) -> std::enable_if_t<!std::is_same<T, bool>::value && !std::is_enum<std::decay_t<T>>::value && is_basic_type<T>::value, T>
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
                return T(cast<std::decay_t<typename T::value_type>>(str));
            }
        }

        template<typename T>
        auto cast_string(const T& t) -> std::enable_if_t<is_basic_type<T>::value, std::string>
        {
            return fmt::format("{}", t);
        }

        inline std::string cast_string(const char* str)
        {
            return str;
        }

        template<size_t N>
        std::string cast_string(char(&str)[N])
        {
            return str;
        }

        template<typename T>
        auto cast_string(const T& t) -> std::enable_if_t<std::is_same<T, date_time>::value, std::string>
        {
            return t.to_string();
        }

        template<typename T>
        auto cast_string(const T& t) -> std::enable_if_t<is_nullable<T>::value, std::string>
        {
            if (t)
                return cast_string(t.get());
            else
                return "";
        }
    }
}