#pragma once
#include <random>
#include <ctime>
#include <thread>

namespace cytx
{
    namespace util
    {
        namespace detail
        {
            using random_engine_t = std::mt19937;

            class random_util
            {

            private:
                static random_engine_t& get_engine()
                {
                    static thread_local random_engine_t* generator = nullptr;
                    if (!generator)
                    {
                        generator = new random_engine_t(clock());
                    }
                    return *generator;
                }

            };
        }
    }
}