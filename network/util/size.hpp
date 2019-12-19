#pragma once
#include <cstdint>
#include <string>
#include <fmt/format.h>

namespace cytx
{
    namespace util
    {
        class size_util
        {
        public:
            static std::string to_string(int64_t size)
            {
                if (size >= part_size * TB)
                    return fmt::format("{:.3f}T", size / (TB * 1.0f));
                else if (size >= part_size * GB)
                    return fmt::format("{:.3f}G", size / (GB * 1.0f));
                else if (size >= part_size * MB)
                    return fmt::format("{:.3f}M", size / (MB * 1.0f));
                else if (size >= part_size)
                    return fmt::format("{:.3f}K", size / (KB * 1.0f));
                else
                    return fmt::format("{}B", size);
            }
        public:
            constexpr static int64_t KB = 1024;
            constexpr static int64_t MB = 1024 * KB;
            constexpr static int64_t GB = 1024 * MB;
            constexpr static int64_t TB = 1024 * GB;

        private:
            constexpr static int64_t part_size = 100;
        };
    }

    using size_util = util::size_util;
}