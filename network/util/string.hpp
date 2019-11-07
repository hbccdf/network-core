#pragma once
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace cytx
{
    namespace util
    {
        inline bool is_empty_or_white_spate(const std::string& str)
        {
            return str.empty() || boost::trim_copy(str).empty();
        }

        class string_util
        {
        public:
            static void split(std::vector<std::string>& strs, const std::string& input, const std::string& chars)
            {
                boost::split(strs, input, boost::is_any_of(chars), boost::token_compress_on);
            }

            static std::vector<std::string> split(const std::string& input, const std::string& chars)
            {
                std::vector<std::string> strs;
                split(strs, input, chars);
                return strs;
            }

            static std::string trim_copy(const std::string& str)
            {
                return boost::trim_copy(str);
            }

            static void trim(std::string& str)
            {
                boost::trim(str);
            }

            static std::string trim_copy(const std::string& str, const std::string& chars)
            {
                return boost::trim_copy_if(str, boost::is_any_of(chars));
            }

            static void trim(std::string& str, const std::string& chars)
            {
                boost::trim_if(str, boost::is_any_of(chars));
            }

            static bool contains_chars(const std::string& str, const std::string& chars)
            {
                return std::any_of(str.begin(), str.end(), [&chars](auto c) -> bool { return chars.find(c) != std::string::npos; });
            }

            static bool contains(const std::string& str, const std::string& other)
            {
                return str.find(other) != std::string::npos;
            }

            static void replace(std::string& str, const std::string& search_str, const std::string& replace_str)
            {
                boost::replace_all(str, search_str, replace_str);
            }
        };
    }

    using string_util = util::string_util;
}