﻿#pragma once
#include "imodule.hpp"
#include "network/service/service_manager.hpp"

namespace cytx
{
    class base_module : public imodule
    {
    public:
        base_module(world_prt_t world_ptr)
            : world_ptr_(world_ptr)
            , auto_register_all_service_(false)
        {
        }

        bool init() override
        {
            service_mgr_.register_all_service();
            world_ptr_->set("service_mgr", &service_mgr_);

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

    protected:
        template<typename ... Args>
        void register_service()
        {
            char a[]{ (service_mgr_.register_service<Args>(), 0) ... };
        }

        void register_service(const std::string& service_name)
        {
            service_mgr_.register_service(service_name);
        }
    private:
        world_prt_t world_ptr_;
        service_manager service_mgr_;
        bool auto_register_all_service_;
    };
}