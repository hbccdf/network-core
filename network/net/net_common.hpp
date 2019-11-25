#pragma once
#include "network/meta/meta.hpp"

namespace cytx
{
    namespace net
    {
        enum class server_thread_mode : uint8_t
        {
            no_io_thread,
            one_io_thread,
            more_io_thread,
        };

        inline uint32_t get_thread_count(server_thread_mode thread_mode)
        {
            switch (thread_mode)
            {
            case server_thread_mode::no_io_thread:
                return 0;
            case server_thread_mode::one_io_thread:
                return 1;
            case server_thread_mode::more_io_thread:
                return std::thread::hardware_concurrency();
            default:
                return 0;
            }
        }
    }
}