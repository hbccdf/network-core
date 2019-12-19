#pragma once
#include <cstdint>

namespace cytx
{
    namespace util
    {
        class time_util
        {
        public:
            constexpr static int64_t millisecond = 1;
            constexpr static int64_t second = millisecond * 1000;
            constexpr static int64_t minute = second * 60;
            constexpr static int64_t hour = minute * 60;
            constexpr static int64_t day = hour * 24;
        };
    }

    using time_util = util::time_util;
}