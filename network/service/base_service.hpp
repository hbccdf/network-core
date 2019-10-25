#pragma once
#include "service_manager.hpp"

namespace cytx
{
    class base_service
    {
    public:
        void set_world(world_ptr_t world_ptr)
        {
            world_ptr_ = world_ptr;
            service_mgr_ = world_ptr_->get<service_manager>("service_mgr");
        }

    protected:
        world_ptr_t world_ptr_ = nullptr;
        service_manager* service_mgr_ = nullptr;
    };
}