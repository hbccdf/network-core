#pragma once
#include "../base/type_id.hpp"

namespace cytx
{
    class iservice
    {
    public:
        virtual ~iservice() {}
        virtual void init() = 0;
        virtual void start() = 0;
        virtual void stop() = 0;

    public:
        virtual type_id_t get_type_id() const = 0;
    };
}