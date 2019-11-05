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
            public:
                //range [0.0f, 1.0f]
                static float get_float()
                {
                    int ret = get_int(100);
                    return ret / 100.0f;
                }
                //range [begin_value, end_value]
                static int get_int(int begin_value, int end_value)
                {
                    std::uniform_int_distribution<int> distribution(begin_value, end_value);
                    int ret = distribution(get_engine());
                    return ret;
                }
                //range [0, end_value]
                static int get_int(int end_value)
                {
                    return get_int(0, end_value);
                }

            public:
                static bool hit(int chance, int total)
                {
                    return chance < get_int(total);
                }
                static bool hit(int chance)
                {
                    return hit(chance, _base_chance);
                }

                static bool miss(int chance, int total)
                {
                    return chance > get_int(total);
                }
                static bool miss(int chance)
                {
                    return miss(chance, _base_chance);
                }
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

                static const int _base_chance = 10000;

            };
        }

        using random_util = detail::random_util;
    }
}