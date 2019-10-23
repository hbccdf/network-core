#pragma once
#include "network/base/type_id.hpp"
#include "network/base/world.hpp"

namespace cytx
{
    class iservice
    {
    public:
        virtual ~iservice() {}
        virtual void set_world(world_map*) = 0;
        virtual void init() = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void reset() = 0;
    public:
        virtual type_id_t get_type_id() const = 0;
    };
}