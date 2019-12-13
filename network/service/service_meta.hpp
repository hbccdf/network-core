#pragma once
#include "iservice.hpp"
#include "network/traits/traits.hpp"
#include "network/base/world.hpp"
#include "network/meta/type_meta.hpp"

namespace cytx
{

#define SERVICE_FUNC_IMPL(func) \
public:                                                                 \
    void func() override                                                \
    {                                                                   \
        func ## _impl<T>();                                             \
    }                                                                   \
private:                                                                \
    template<typename TT>                                               \
    auto func ## _impl() -> std::enable_if_t<has_##func##_v<TT>>        \
    {                                                                   \
        val_->func();                                                   \
    }                                                                   \
    template<typename TT>                                               \
    auto func ## _impl()->std::enable_if_t<!has_##func##_v<TT>>         \
    {                                                                   \
    }

#define SERVICE_FUNC_IMPL_BOOL(func) \
public:                                                                 \
    bool func() override                                                \
    {                                                                   \
        return func ## _impl<T>();                                      \
    }                                                                   \
private:                                                                \
    template<typename TT>                                               \
    auto func ## _impl() -> std::enable_if_t<has_##func##_v<TT>, bool>  \
    {                                                                   \
        return val_->func();                                            \
    }                                                                   \
    template<typename TT>                                               \
    auto func ## _impl()->std::enable_if_t<!has_##func##_v<TT>, bool>   \
    {                                                                   \
        return true;                                                    \
    }

    HAS_FUNC(init);
    HAS_FUNC(start);
    HAS_FUNC(stop);
    HAS_FUNC(reset);
    HAS_FUNC(reload);

    template<typename T>
    class service_helper : public iservice
    {
        using this_t = service_helper;
    public:
        service_helper(const std::string& service_name)
            : service_helper(new T(), service_name)
        {
        }
        service_helper(T* val, const std::string& service_name)
            : val_(val)
            , name_(service_name)
        {
        }
        virtual ~service_helper()
        {
            delete val_;
        }

        T* get_val() const
        {
            return val_;
        }

    public:
        void set_world(world_ptr_t world_ptr) override
        {
            set_world_impl<T>(world_ptr);
        }

        SERVICE_FUNC_IMPL_BOOL(init);
        SERVICE_FUNC_IMPL_BOOL(start);
        SERVICE_FUNC_IMPL_BOOL(reload);
        SERVICE_FUNC_IMPL(stop)
        SERVICE_FUNC_IMPL(reset)

    public:
        type_id_t get_type_id() const override
        {
            return TypeId::id<T>();
        }

        std::string get_name() const override
        {
            return name_;
        }

        void* get_ptr() const override
        {
            return val_;
        }

    private:
        template<typename TT>
        auto set_world_impl(world_ptr_t world_ptr) -> std::enable_if_t<has_set_world_v<TT>>
        {
            val_->set_world(world_ptr);
        }
        template<typename TT>
        auto set_world_impl(world_ptr_t world_ptr) -> std::enable_if_t<!has_set_world_v<TT>>
        {
        }
    private:
        T* val_;
        std::string name_;
    };


    class service_factory
    {
    public:
        service_factory() {}
        ~service_factory()
        {
            for (auto& p : service_map_)
            {
                delete p.second;
            }
            service_map_.clear();
        }

        static service_factory& ins()
        {
            static service_factory s;
            return s;
        }

        template<typename T>
        int register_service(const std::string& service_name)
        {
            using real_type = service_helper<T>;

            type_id_t type_id = TypeId::id<T>();
            auto it = service_map_.find(type_id);
            if (it != service_map_.end())
                return 0;

            service_map_.emplace(type_id, new real_type(service_name));
            name_map_.emplace(service_name, type_id);
            return 0;
        }

        template<typename T, typename BaseT>
        int register_service(const std::string& service_name)
        {
            using real_type = service_helper<T>;

            type_id_t type_id = TypeId::id<T>();
            auto it = service_map_.find(type_id);
            if (it != service_map_.end())
                return 0;

            service_map_.emplace(type_id, new real_type(service_name));
            name_map_.emplace(service_name, type_id);

            type_id_t base_type_id = TypeId::id<BaseT>();
            auto& type_list = derived_service_map_[base_type_id];
            type_list.push_back(type_id);
            return 0;
        }

        template<typename T>
        T* get_service() const
        {
            type_id_t type_id = TypeId::id<T>();
            auto it = service_map_.find(type_id);
            if (it != service_map_.end())
            {
                return static_cast<T*>(it->second->get_ptr());
            }

            std::vector<T*> type_list = get_derived_services<T>();
            if (!type_list.empty())
                return type_list.front();

            return nullptr;
        }

        template<typename T>
        std::vector<T*> get_all_service() const
        {
            std::vector<T*> type_list;

            type_id_t type_id = TypeId::id<T>();
            auto it = service_map_.find(type_id);
            if (it != service_map_.end())
            {
                type_list.push_back(static_cast<T*>(it->second->get_ptr()));
            }

            auto derived_type_list = get_derived_services<T>();
            for (auto t : derived_type_list)
            {
                type_list.push_back(t);
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
            return get_service(service_name) != nullptr;
        }

        template<typename T>
        iservice* get_iservice() const
        {
            type_id_t type_id = TypeId::id<T>();
            auto it = service_map_.find(type_id);
            if (it != service_map_.end())
            {
                return it->second;
            }

            return nullptr;
        }

        iservice* get_service(type_id_t type_id) const
        {
            auto it = service_map_.find(type_id);
            if (it == service_map_.end())
                return nullptr;

            return it->second;
        }

        iservice* get_service(const std::string& service_name) const
        {
            type_id_t type_id = get_service_typeid(service_name);
            return get_service(type_id);
        }

        type_id_t get_service_typeid(const std::string& service_name) const
        {
            auto it = name_map_.find(service_name);
            if (it == name_map_.end())
                return 0;

            return it->second;
        }

        template<typename T>
        std::vector<type_id_t> get_derived_typeid_list() const
        {
            std::vector<type_id_t> type_list;
            type_id_t base_type_id = TypeId::id<T>();

            auto it = derived_service_map_.find(base_type_id);
            if (it == derived_service_map_.end() || it->second.empty())
                return type_list;

            type_list = it->second;
            return type_list;
        }

    public:
        template<typename T>
        iservice* reg_inter_service(T* ptr, const::std::string& service_name)
        {
            using real_type = service_helper<T>;

            type_id_t type_id = TypeId::id<T>();
            auto it = service_map_.find(type_id);
            if (it != service_map_.end())
                return it->second;

            iservice* service_ptr = new real_type(ptr, service_name);
            service_map_.emplace(type_id, service_ptr);
            name_map_.emplace(service_name, type_id);
            return service_ptr;
        }

        const std::map<type_id_t, iservice*>& get_all_service_map() const
        {
            return service_map_;
        }

    private:
        template<typename BaseT>
        std::vector<BaseT*> get_derived_services() const
        {
            std::vector<BaseT*> derived_list;
            type_id_t base_type_id = TypeId::id<BaseT>();

            auto it = derived_service_map_.find(base_type_id);
            if (it == derived_service_map_.end() || it->second.empty())
                return derived_list;

            auto& typeid_list = it->second;

            for (type_id_t type_id : typeid_list)
            {
                iservice* service_ptr = get_service(type_id);
                if (service_ptr)
                {
                    derived_list.push_back(static_cast<BaseT*>(service_ptr->get_ptr()));
                }
            }
            return derived_list;
        }
    private:
        std::map<type_id_t, iservice*> service_map_;
        std::map<std::string, type_id_t> name_map_;
        std::map<type_id_t, std::vector<type_id_t>> derived_service_map_;
    };
}


#define REG_SERVICE(type, ...)                                                                  \
REG_TYPE(type);                                                                                 \
namespace MACRO_CONCAT(__reg_service_ ## type, __LINE__)                                        \
{                                                                                               \
    static int r = cytx::service_factory::ins().register_service<type, ##__VA_ARGS__>(#type);   \
}

