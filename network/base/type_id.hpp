#pragma once
#include <cstdint>

namespace cytx
{
    using type_id_t = int32_t;

    class TypeId
    {
    public:
        template<typename T>
        static auto id()
        {
            static type_id_t _id = get_id();
            return _id;
        }
    private:
        static auto get_id()
        {
            static type_id_t _id = 0;
            return ++_id;
        }
    };
}
