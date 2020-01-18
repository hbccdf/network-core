#pragma once
#include <vector>

namespace cytx
{
    namespace meta
    {
        template<typename T>
        class Array : public std::vector<T>
        {
        public:
            Array() { }

            Array(const std::vector<T> &rhs)
                : std::vector<T>(rhs) {}

            Array(const std::vector<T> &&rhs)
                : std::vector<T>(std::move(rhs)) { }

            Array(const std::initializer_list<T> &rhs)
                : std::vector<T>(rhs) { }

            Array(const std::initializer_list<T> &&rhs)
                : std::vector<T>(std::move(rhs)) { }
        };
    }
}