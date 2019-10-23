#pragma once
#include <unordered_map>
#include <string>
#include "network/traits/traits.hpp"

namespace cytx
{
    namespace detail
    {
        using world_basic_type_variant_t = cytx::variant<bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double>;
    }

    template <typename T>
    struct has_set_world_func
    {
    private:
        template <typename P, typename = decltype(std::declval<P>().set_world((world_map*)nullptr))>
        static std::true_type test(int);
        template <typename P>
        static std::false_type test(...);
        using result_type = decltype(test<T>(0));
    public:
        static constexpr bool value = result_type::value;
    };
    template<typename T>
    constexpr bool has_set_world_func_v = has_set_world_func<T>::value;

    class world_map
    {
    public:
        static world_map& global()
        {
            static world_map world;
            return world;
        }
    public:
        void reg(const std::string& name, void* obj)
        {
            obj_map_[name] = obj;
        }

        template<typename T>
        auto reg(const std::string& name, T* obj) -> std::enable_if_t<has_set_world_func_v<T>>
        {
            reg(name, (void*)obj);
            set_world_impl(obj);
        }

        template<typename T>
        void reg(T* obj)
        {
            using obj_type = std::decay_t<T>;
            std::string type_name = typeid(T).name();
            reg(type_name, obj);
            set_world_impl(obj);
        }

        template<typename T>
        T* get()
        {
            using obj_type = std::decay_t<T>;
            std::string type_name = typeid(T).name();
            return get<T>(type_name);
        }

        template<typename T>
        T* get(const std::string& name)
        {
            auto it = obj_map_.find(name);
            if (it != obj_map_.end())
            {
                return (T*)it->second;
            }

            return nullptr;
        }

        void reg_string(const std::string& name, const std::string& value)
        {
            str_map_[name] = value;
        }

        std::string get_string(const std::string& name)
        {
            auto it = str_map_.find(name);
            if (it != str_map_.end())
            {
                return it->second;
            }

            return "";
        }

        std::string get_string_or(const std::string& name, const std::string& or_value)
        {
            auto it = str_map_.find(name);
            if (it != str_map_.end())
            {
                return it->second;
            }

            return or_value;
        }

        template<typename T>
        auto reg_basic(const std::string& name, const T& value) -> std::enable_if_t<std::is_arithmetic<detail::decay_t<T>>::value>
        {
            basic_map_[name] = value;
        }

        template<typename T>
        auto get_basic(const std::string& name) -> std::enable_if_t<std::is_arithmetic<T>::value, T>
        {
            auto it = basic_map_.find(name);
            if (it != basic_map_.end())
            {
                return boost::get<T>(it->second);
            }

            return T{};
        }

        template<typename T>
        auto get_basic_or(const std::string& name, const T& or_value) -> std::enable_if_t<std::is_arithmetic<T>::value, T>
        {
            auto it = basic_map_.find(name);
            if (it != basic_map_.end())
            {
                return boost::get<T>(it->second);
            }

            return or_value;
        }

    private:
        template<typename T>
        auto set_world_impl(T* obj_ptr) -> std::enable_if_t<has_set_world_func_v<T>>
        {
            obj_ptr->set_world(this);
        }
        template<typename T>
        auto set_world_impl(T* obj_ptr) -> std::enable_if_t<!has_set_world_func_v<T>>
        {
        }

    private:
        std::unordered_map<std::string, void*> obj_map_;
        std::unordered_map<std::string, std::string> str_map_;
        std::unordered_map<std::string, detail::world_basic_type_variant_t> basic_map_;
    };
}