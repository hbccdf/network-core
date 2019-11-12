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

        static type_id_t invalid_id()
        {
            return -1;
        }

        static bool is_invalid_id(type_id_t tid)
        {
            return tid <= 0;
        }
    private:
        static type_id_t get_id()
        {
            static type_id_t _id = 0;
            return ++_id;
        }
    };
}
