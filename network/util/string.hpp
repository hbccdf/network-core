#pragma once
#include <string>
#include <boost/algorithm/string.hpp>

namespace cytx
{
    namespace util
    {
        inline bool is_empty_or_white_spate(const std::string& str)
        {
            return str.empty() || boost::trim_copy(str).empty();
        }
    }
}