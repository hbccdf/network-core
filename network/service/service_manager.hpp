#pragma once
#include "service_meta.hpp"

namespace cytx
{
    class service_manager
    {
    public:
        template<typename ... Args>
        void register_service()
        {
            char a[]{ (reg_impl<Args>(), 0) ... };
        }
        void register_service(const std::string& service_name)
        {
            auto& factory = service_factory::ins();
            iservice* service_ptr = factory.get_service(service_name);
            if (service_ptr)
            {
                auto type_id = service_ptr->get_type_id();
                service_map_.emplace(type_id, service_ptr);
            }
        }
        void register_service(const std::vector<std::string>& services)
        {
            for (auto& str : services)
            {
                register_service(str);
            }
        }
        void service_set_server(game_server_base_t* server_ptr)
        {
            for (auto& p : service_map_)
            {
                p.second->set_server(server_ptr);
            }
        }
        void init_service()
        {
            for (auto& p : service_map_)
            {
                p.second->init();
            }
        }
        void start_service()
        {
            for (auto& p : service_map_)
            {
                p.second->start();
            }
        }
        void stop_service()
        {
            for (auto& p : service_map_)
            {
                p.second->stop();
            }
        }

        template<typename T>
        T* get_service() const
        {
            using real_type = service_helper<T>;
            type_id_t type_id = TypeId::id<real_type>();
            auto it = service_map_.find(type_id);
            if (it == service_map_.end())
                return nullptr;

            real_type* val = static_cast<real_type*>(it->second);
            return val->get_val();
        }

        template<typename T>
        bool find_service() const
        {
            using real_type = service_helper<T>;
            type_id_t type_id = TypeId<real_type>::id();
            auto it = service_map_.find(type_id);
            return (it != service_map_.end());
        }

        bool find_service(const std::string& service_name) const
        {
            auto& factory = service_factory::ins();
            auto service_ptr = factory.get_service(service_name);
            if (!service_ptr)
                return false;

            auto it = service_map_.find(service_ptr->get_type_id());
            return it != service_map_.end();
        }

    public:
        template<typename T>
        void reg_inter_service(T* ptr, const std::string& service_name = "")
        {
            iservice* service_ptr = service_factory::ins().reg_inter_service<T>(ptr, service_name);
            if (service_ptr)
            {
                auto type_id = service_ptr->get_type_id();
                service_map_.emplace(type_id, service_ptr);
            }
        }
    private:
        template<typename T>
        void reg_impl()
        {
            iservice* service_ptr = service_factory::ins().get_service<T>();
            if (service_ptr)
            {
                auto type_id = service_ptr->get_type_id();
                service_map_.emplace(type_id, service_ptr);
            }
        }
    private:
        std::map<type_id_t, iservice*> service_map_;
    };
}