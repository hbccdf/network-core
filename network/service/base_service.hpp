#pragma once
#include "service_manager.hpp"

namespace cytx
{
    class base_service
    {
    public:
        void set_world(world_map* world_ptr)
        {
            world_ptr_ = world_ptr;
            svc_manager_ = world_ptr_->get<service_manager>("service_manager");
        }

    protected:
        world_map* world_ptr_ = nullptr;
        service_manager* svc_manager_ = nullptr;
    };
}