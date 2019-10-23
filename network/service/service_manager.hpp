#pragma once
#include "service_meta.hpp"

namespace cytx
{
    class service_manager
    {
    public:
        void service_set_world(world_map* world_ptr)
        {
            for (auto& p : service_map_)
            {
                p.second->set_world(world_ptr);
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
        void reset_service()
        {
            for (auto& p : service_map_)
            {
                p.second->reset();
            }
        }

    public:
        template<typename T>
        T* get_service() const
        {
            if (service_map_.empty())
                return nullptr;

            using real_type = service_helper<T>;
            type_id_t type_id = TypeId::id<real_type>();

            T* service_ptr = internal_get_service<T>(type_id);
            if (service_ptr)
                return service_ptr;

            std::vector<type_id_t> typeid_list = service_factory::ins().get_derived_typeid_list<T>();
            for (auto derived_type_id : typeid_list)
            {
                service_ptr = internal_get_service<T>(derived_type_id);
                if (service_ptr)
                    return service_ptr;
            }

            return nullptr;
        }

        template<typename T>
        std::vector<T*> get_all_service() const
        {
            std::vector<T*> type_list;

            if (service_map_.empty())
                return type_list;

            using real_type = service_helper<T>;
            type_id_t type_id = TypeId::id<real_type>();

            T* service_ptr = internal_get_service<T>(type_id);
            if (service_ptr)
            {
                type_list.push_back(service_ptr);
            }

            std::vector<type_id_t> typeid_list = service_factory::ins().get_derived_typeid_list<T>();
            for (auto derived_type_id : typeid_list)
            {
                service_ptr = internal_get_service<T>(derived_type_id);
                if (service_ptr)
                {
                    type_list.push_back(service_ptr);
                }
            }

            return type_list;
        }

        template<typename T>
        bool find_service() const
        {
            return get_service<T>() != nullptr;
        }

        bool find_service(const std::string& service_name) const
        {
            if (service_map_.empty())
                return false;

            auto& factory = service_factory::ins();
            auto service_ptr = factory.get_service(service_name);
            if (!service_ptr)
                return false;

            auto it = service_map_.find(service_ptr->get_type_id());
            return it != service_map_.end();
        }

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
        void register_all_service()
        {
            service_map_ = service_factory::ins().get_all_service_map();
        }
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
        template<typename T>
        T* internal_get_service(type_id_t type_id) const
        {
            using real_type = service_helper<T>;
            auto it = service_map_.find(type_id);
            if (it != service_map_.end())
            {
                real_type* val = static_cast<real_type*>(it->second);
                return val->get_val();
            }
            return nullptr;
        }
    private:
        std::map<type_id_t, iservice*> service_map_;
    };
}