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
        };
    }

    using string_util = util::string_util;
}