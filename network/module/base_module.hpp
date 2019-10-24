#pragma once
#include "imodule.hpp"
#include "network/service/service_manager.hpp"

namespace cytx
{
    class base_module : public imodule
    {
    public:
        base_module(world_map* world_ptr)
            : world_ptr_(world_ptr)
        {
            world_ptr_->set("service_mgr", &service_mgr_);
        }

        bool init() override
        {
            service_mgr_.register_all_service();
            service_mgr_.init_service();
            return true;
        }

        bool start() override
        {
            service_mgr_.start_service();
            return true;
        }

        bool reload() override
        {
            return service_mgr_.reload_service();
        }

        void stop() override
        {
            service_mgr_.stop_service();
        }

        service_manager* get_service_mgr()
        {
            return &service_mgr_;
        }
    private:
        world_map* world_ptr_;
        service_manager service_mgr_;
    };
}