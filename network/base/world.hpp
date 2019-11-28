#pragma once
#include <unordered_map>
#include <string>
#include "network/traits/traits.hpp"
#include "network/msgbus/event.hpp"
#include "instance_factory.hpp"

namespace cytx
{
    namespace detail
    {
        using world_basic_type_variant_t = cytx::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double>;
    }

    HAS_FUNC(set_world, (*(world_ptr_proxy*)nullptr));

    class world_proxy
    {
        friend class world_map;
        friend class world_ptr_proxy;
    public:
        world_proxy(world_proxy&& other)
            : world_ptr_(other.world_ptr_)
            , name_(std::move(other.name_))
        {}

        template<typename T>
        operator T() const;

        template<typename T>
        world_proxy& operator=(T&& t);

    protected:
        world_proxy(world_map* world_ptr, const std::string& name)
            : world_ptr_(world_ptr)
            , name_(name)
        {}

        world_map* world_ptr_;
        std::string name_;
    };

    class world_map
    {
        using dispatcher_t = msg_dispatcher;
    public:
        static world_map& global()
        {
            static world_map world;
            return world;
        }

    public:
        world_proxy operator[](const std::string& name)
        {
            return world_proxy(this, name);
        }
        world_proxy operator[](const char* name)
        {
            return world_proxy(this, name);
        }
    public:
        void set(const std::string& name, void* obj)
        {
            obj_map_[name] = obj;
        }

        template<typename T>
        void set(T* obj)
        {
            type_id_t tid = TypeId::id<T>();
            type_obj_map_[tid] = obj;
            set_world_impl(obj);
        }

        template<typename T>
        auto set(const std::string& name, const T& value) -> std::enable_if_t<is_string_v<T>>
        {
            set_string(name, value);
        }

        template<typename T>
        auto set(const std::string& name, T value) -> std::enable_if_t<is_number_type_v<T>>
        {
            set_basic(name, value);
        }

        template<typename T>
        auto set(const std::string& name, T value) -> std::enable_if_t<is_enum_type_v<T>>
        {
            using under_type = std::underlying_type_t<T>;
            set_basic(name, (under_type)value);
        }

        template<typename T>
        auto set(const std::string& name, T* obj) -> std::enable_if_t<!is_string_v<T> && !is_number_type_v<T> && !is_enum_type_v<T> && !is_callable_v<T>>
        {
            set(name, (void*)obj);
            set(obj);
        }

        template<typename F>
        auto set(const std::string& name, F&& f) -> std::enable_if_t<is_callable_v<F>>
        {
            set_function(name, std::forward<F>(f));
        }

        template<typename T>
        T* get() const
        {
            type_id_t tid = TypeId::id<T>();
            auto it = type_obj_map_.find(tid);
            if (it != type_obj_map_.end())
                return (T*)it->second;

            return nullptr;
        }

        template<typename T>
        auto get(const std::string& name) const -> std::enable_if_t<is_string_v<T>, T>
        {
            return get_string(name);
        }

        template<typename T>
        auto get(const std::string& name) const -> std::enable_if_t<is_number_type_v<T>, T>
        {
            return get_basic<T>(name);
        }

        template<typename T>
        auto get(const std::string& name) const -> std::enable_if_t<is_enum_type_v<T>, T>
        {
            using under_type = std::underlying_type_t<T>;
            return (T)get_basic<under_type>(name);
        }

        template<typename T>
        auto get(const std::string& name) const -> std::enable_if_t<!is_string_v<T> && !is_number_type_v<T> && !is_enum_type_v<T> && !is_callable_v<T>, T*>
        {
            auto it = obj_map_.find(name);
            if (it != obj_map_.end())
            {
                return (T*)it->second;
            }

            return nullptr;
        }

        template<typename F>
        auto get(const std::string& name) -> std::enable_if_t<is_callable_v<F>, F>
        {
            return get_function<F>(name);
        }

        void set_string(const std::string& name, const std::string& value)
        {
            str_map_[name] = value;
        }

        std::string get_string(const std::string& name) const
        {
            auto it = str_map_.find(name);
            if (it != str_map_.end())
            {
                return it->second;
            }

            return "";
        }

        std::string get_string_or(const std::string& name, const std::string& or_value) const
        {
            auto it = str_map_.find(name);
            if (it != str_map_.end())
            {
                return it->second;
            }

            return or_value;
        }

        template<typename T>
        auto set_basic(const std::string& name, const T& value) -> std::enable_if_t<is_number_type_v<T>>
        {
            basic_map_[name] = value;
        }

        template<typename T>
        auto get_basic(const std::string& name) const -> std::enable_if_t<is_number_type_v<T>, T>
        {
            auto it = basic_map_.find(name);
            if (it != basic_map_.end())
            {
                return boost::get<T>(it->second);
            }

            return T{};
        }

        template<typename T>
        auto get_basic_or(const std::string& name, const T& or_value) const -> std::enable_if_t<is_number_type_v<T>, T>
        {
            auto it = basic_map_.find(name);
            if (it != basic_map_.end())
            {
                return boost::get<T>(it->second);
            }

            return or_value;
        }

        template<typename F>
        F get_function(const std::string& name) const
        {
            using stl_func_t = function_stl_func_t<F>;

            auto it = func_map_.find(name);
            if (it == func_map_.end())
                return nullptr;

            auto func = boost::any_cast<stl_func_t>(it->second);
            return func;
        }

        template<typename F>
        void set_function(const std::string key, F&& func)
        {
            using stl_func_t = function_stl_func_t<F>;
            stl_func_t stl_func = std::forward<F>(func);
            func_map_[key] = boost::any(stl_func);
        }

    public:
        template <typename Receiver, typename ... ARGS>
        void subscribe(Receiver &receiver)
        {
            char a[]{ (dispatcher_.subscribe<ARGS>(receiver), 0) ... };
        }

        template <typename Receiver, typename ... ARGS>
        void unsubscribe(Receiver &receiver)
        {
            char a[]{ (dispatcher_.unsubscribe<ARGS>(receiver), 0) ... };
        }

        template<typename Receiver>
        void unsubscribe_all(Receiver &receiver)
        {
            dispatcher_.unsubscribe_all(receiver);
        }

        template <typename Receiver, typename ... ARGS>
        void subscribe(Receiver* receiver)
        {
            char a[]{ (dispatcher_.subscribe<ARGS>(*receiver), 0) ... };
        }

        template <typename Receiver, typename ... ARGS>
        void unsubscribe(Receiver* receiver)
        {
            char a[]{ (dispatcher_.unsubscribe<ARGS>(*receiver), 0) ... };
        }

        template<typename Receiver>
        void unsubscribe_all(Receiver* receiver)
        {
            dispatcher_.unsubscribe_all(*receiver);
        }

        template <typename E>
        void emit(const E &event)
        {
            dispatcher_.emit(event);
        }

        template <typename E>
        void emit(std::unique_ptr<E> event)
        {
            dispatcher_.emit(event);
        }

        template <typename E, typename ... Args>
        void emit(Args && ... args)
        {
            dispatcher_.emit<E>(std::forward<Args>(args) ...);
        }

        template<typename E>
        bool has_subscribed() const
        {
            return dispatcher_.has_subscribed<E>();
        }

    public:
        instance_factory& factory()
        {
            return instance_factory::ins();
        }

    private:
        template<typename T>
        auto set_world_impl(T* obj_ptr) -> std::enable_if_t<has_set_world_v<T>>
        {
            obj_ptr->set_world(this);
        }
        template<typename T>
        auto set_world_impl(T* obj_ptr) -> std::enable_if_t<!has_set_world_v<T>>
        {
        }

    private:
        std::unordered_map<type_id_t, void*> type_obj_map_;
        std::unordered_map<std::string, void*> obj_map_;
        std::unordered_map<std::string, std::string> str_map_;
        std::unordered_map<std::string, detail::world_basic_type_variant_t> basic_map_;
        std::unordered_map<std::string, boost::any> func_map_;

    private:
        dispatcher_t dispatcher_;
    };

    template<typename T>
    world_proxy::operator T() const
    {
        return world_ptr_->get<std::remove_pointer_t<T>>(name_);
    }

    template<typename T>
    world_proxy& world_proxy::operator=(T&& t)
    {
        world_ptr_->set(name_, std::forward<T>(t));
        return *this;
    }

    class world_ptr_proxy
    {
    public:
        world_ptr_proxy()
            : world_ptr_(nullptr)
        {}

        world_ptr_proxy(world_map* world_ptr)
            : world_ptr_(world_ptr)
        {}

        world_map& operator*() const
        {
            return *world_ptr_;
        }

        world_map* operator->() const
        {
            return world_ptr_;
        }

        world_ptr_proxy& operator= (const world_ptr_proxy& other)
        {
            if (this == &other)
                return *this;
            world_ptr_ = other.world_ptr_;
            return *this;
        }
        world_ptr_proxy& operator= (world_map* ptr)
        {
            world_ptr_ = ptr;
            return *this;
        }

        world_proxy operator[](const std::string& name)
        {
            return world_proxy(world_ptr_, name);
        }

        world_proxy operator[](const char* name)
        {
            return world_proxy(world_ptr_, name);
        }

        world_map* get() const {
            return world_ptr_;
        }

        operator world_map*() const
        {
            return world_ptr_;
        }

        operator bool() const
        {
            return world_ptr_ != nullptr;
        }

        bool operator< (const world_ptr_proxy& other)
        {
            return world_ptr_ < other.world_ptr_;
        }

    protected:
        world_map* world_ptr_;
    };

    using world_t = world_map;
    using world_ptr_t = world_ptr_proxy;

    template<typename T>
    using receiver_t = cytx::Receiver<T>;
}