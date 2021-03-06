﻿#pragma once
#include "network/base/type_id.hpp"
#include "network/base/world.hpp"

namespace cytx
{
    class iservice
    {
    public:
        virtual ~iservice() {}
        virtual void set_world(world_ptr_t) = 0;
        virtual bool init() = 0;
        virtual bool start() = 0;
        virtual void stop() = 0;
        virtual void reset() = 0;
        virtual bool reload() = 0;
    public:
        virtual type_id_t get_type_id() const = 0;
        virtual std::string get_name() const = 0;
        virtual void* get_ptr() const = 0;
    };
}