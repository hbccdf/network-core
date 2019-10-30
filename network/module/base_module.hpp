#pragma once
#include "imodule.hpp"
#include "network/service/service_manager.hpp"

namespace cytx
{
    class base_module : public imodule
    {
    public:
        base_module()
            : world_ptr_(nullptr)
            , auto_register_all_service_(false)
        {
        }

        void set_world(world_ptr_t world_ptr)
        {
            world_ptr_ = world_ptr;
        }

        bool init() override
        {
            if(auto_register_all_service_)
                service_mgr_.register_all_service();

            world_ptr_->set("service_mgr", &service_mgr_);

            return service_mgr_.init_service();
        }

        bool start() override
        {
            return service_mgr_.start_service();
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
        world_ptr_t world_ptr_;
        service_manager service_mgr_;
        bool auto_register_all_service_;
    };
}