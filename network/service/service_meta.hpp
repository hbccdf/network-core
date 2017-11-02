#pragma once
#include "iservice.hpp"
#include "../traits/traits.hpp"

namespace cytx
{
    inline void to_extend(...) {}

    HAS_FUNC(init);
    HAS_FUNC(start);
    HAS_FUNC(stop);

    template<typename T>
    class service_helper : public iservice
    {
        using this_t = service_helper;
    public:
        service_helper()
            : service_helper(new T())
        {
        }
        service_helper(T* val)
            : val_(val)
            , type_id_(TypeId::id<this_t>())
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
        void init() override
        {
            init_impl<T>();
        }

        void start() override
        {
            start_impl<T>();
        }

        void stop() override
        {
            stop_impl<T>();
        }

    public:
        type_id_t get_type_id() const override
        {
            return type_id_;
        }

    private:
        template<typename TT>
        auto init_impl() -> std::enable_if_t<has_init_v<TT>>
        {
            val_->init();
        }
        template<typename TT>
        auto init_impl()->std::enable_if_t<!has_init_v<TT>>
        {
        }

        template<typename TT>
        auto start_impl() -> std::enable_if_t<has_start_v<TT>>
        {
            val_->start();
        }
        template<typename TT>
        auto start_impl()->std::enable_if_t<!has_start_v<TT>>
        {
        }

        template<typename TT>
        auto stop_impl() -> std::enable_if_t<has_stop_v<TT>>
        {
            val_->stop();
        }
        template<typename TT>
        auto stop_impl()->std::enable_if_t<!has_stop_v<TT>>
        {
        }
    private:
        T* val_;
        type_id_t type_id_;
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

            type_id_t type_id = TypeId::id<real_type>();
            auto it = service_map_.find(type_id);
            if (it != service_map_.end())
                return 0;

            service_map_.emplace(type_id, new real_type());
            if (!service_name.empty())
            {
                name_map_.emplace(service_name, type_id);
            }
            return 0;
        }

        template<typename T>
        iservice* reg_inter_service(T* ptr, const::std::string& service_name)
        {
            using real_type = service_helper<T>;

            type_id_t type_id = TypeId::id<real_type>();
            auto it = service_map_.find(type_id);
            if (it != service_map_.end())
                return it->second;

            iservice* service_ptr = new real_type(ptr);
            service_map_.emplace(type_id, service_ptr);
            if (!service_name.empty())
            {
                name_map_.emplace(service_name, type_id);
            }
            return service_ptr;
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
            auto name_it = name_map_.find(service_name);
            if (name_it == name_map_.end())
                return false;

            auto it = service_map_.find(name_it->second);
            return it != service_map_.end();
        }

        iservice* get_service(type_id_t type_id) const
        {
            auto it = service_map_.find(type_id);
            if (it == service_map_.end())
                return nullptr;

            return it->second;
        }

        iservice* get_service(const std::string& service_name)
        {
            type_id_t type_id = get_service_typeid(service_name);
            return get_service(type_id);
        }

        type_id_t get_service_typeid(const std::string& service_name)
        {
            auto it = name_map_.find(service_name);
            if (it == name_map_.end())
                return 0;

            return it->second;
        }
    private:
        std::map<type_id_t, iservice*> service_map_;
        std::map<std::string, type_id_t> name_map_;
    };
}


#define REG_SERVICE(type)                                                   \
inline auto to_extend(const type&)                                          \
{                                                                           \
    struct type ## meta                                                     \
    {                                                                       \
        static constexpr const char* name()                                 \
        {                                                                   \
            return #type;                                                   \
        }                                                                   \
    };                                                                      \
}                                                                           \
                                                                            \
namespace __reg_service_ ## type ## __LINE__                                \
{                                                                           \
    static int r = service_factory::ins().register_service<type>(#type);    \
}