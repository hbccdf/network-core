#pragma once
#include <thread>

namespace cytx
{
    namespace util
    {
        inline void sleep(int millisecond)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
        }
    }
}