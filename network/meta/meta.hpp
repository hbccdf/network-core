#pragma once
#include "base_meta.hpp"
#include "enum_meta.hpp"
#include "db_meta.hpp"

#include "../traits/traits.hpp"

#include <fmt/format.h>
#include <boost/optional.hpp>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>

template<typename T>
struct enum_meta : public std::false_type {};

namespace cytx
{
    template<typename T> struct is_db_meta : is_specialization_of<std::decay_t<T>, db_meta> {};

    template <typename T, class = std::void_t<>>
    struct is_reflection : std::false_type
    {
    };

    // this way of using SFINEA is type reference and cv qualifiers immuned
    template <typename T>
    struct is_reflection<T, std::void_t<std::enable_if_t<db_meta<std::decay_t<T>>::value>>> : std::true_type
    {
    };

    template <typename T>
    struct is_reflection<T, std::void_t<std::enable_if_t<has_meta_macro_v<T>>>> : std::true_type
    {
    };

    template<typename T>
    struct is_reflection <T, std::void_t<std::enable_if_t<enum_meta<T>::value>>> : std::true_type
    {};

    template<typename T, class = std::void_t<>>
    struct get_meta_t { };

    template<typename T>
    struct get_meta_t<T, std::void_t<std::enable_if_t<db_meta<std::decay_t<T>>::value>>>
    {
        using type = db_meta<std::decay_t<T>>;
    };

    template<typename T>
    struct get_meta_t<T, std::void_t<std::enable_if_t<enum_meta<T>::value>>>
    {
        using type = enum_meta<T>;
    };

    template<typename T>
    struct get_meta_t<T, std::void_t<std::enable_if_t<is_db_meta<T>::value>>>
    {
        using type = std::decay_t<T>;
    };

    template<typename T>
    using meta_t = typename get_meta_t<T>::type;


    template<typename T>
    auto get_meta()
    {
        return get_meta_impl<T>::meta();
    }

    template<typename T>
    auto get_meta(T&& t)
    {
        return get_meta_impl<T>::meta(std::forward<T>(t));
    }

    template<typename T, class = std::void_t<>>
    struct get_meta_impl;

    template<typename T>
    struct get_meta_impl<T, std::void_t<std::enable_if_t<has_meta_macro_v<T>>>>
    {
        static auto meta(T&& t)
        {
            return t.Meta();
        }
    };

    template<typename T>
    struct get_meta_impl<T, std::void_t<std::enable_if_t<enum_meta<T>::value>>>
    {
        static auto meta(T&& t)
        {
            return enum_meta<T>::Meta();
        }

        static auto meta()
        {
            return enum_meta<T>::Meta();
        }
    };

    template<typename T>
    struct get_meta_impl<T, std::void_t<std::enable_if_t<db_meta<std::decay_t<T>>::value>>>
    {
        static auto meta(T&& t)
        {
            return t.Meta();
        }
    };

    template<size_t I, typename F, typename T>
    void apply_value(F&& f, T&& t, bool is_last)
    {
        std::forward<F>(f)(std::forward<T>(t), I, is_last);
    };

    template<size_t I, typename F, typename T>
    constexpr void for_each_impl(F&& f, T&&t, bool is_last, std::void_t<std::enable_if_t<is_reflection<T>::value>> *)
    {
        apply(std::forward<F>(f), std::forward<T>(t), get_meta(std::forward<T>(t)), std::make_index_sequence<get_value<T>()>{});
    }

    //-------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------//
    template<size_t I, typename F, typename F1, typename T>
    std::enable_if_t<is_reflection<T>::value> apply_value(F&& f, F1&& f1, T&& t, bool is_last)
    {
        std::forward<F1>(f1)(std::forward<T>(t), I, is_last);
    }

    template<size_t I, typename F, typename F1, typename T>
    std::enable_if_t<!is_reflection<T>::value> apply_value(F&& f, F1&& f1, T&& t, bool is_last)
    {
        std::forward<F>(f)(std::forward<T>(t), I, is_last);
    }

    template<size_t I, typename F, typename F1, typename T>
    constexpr void for_each_impl(F&& f, F1&& f1, T&&t, bool is_last, std::void_t<std::enable_if_t<is_reflection<T>::value>> *)
    {
        apply(std::forward<F>(f), std::forward<F1>(f1), std::forward<T>(t), get_meta(std::forward<T>(t)), std::make_index_sequence<get_value<T>()>{});
    }

    template<typename F, typename F1, typename T, typename... Rest, std::size_t I0, std::size_t... I>
    constexpr void apply(F&& f, F1&& f1, T&&t, std::tuple<Rest...>&& tp, std::index_sequence<I0, I...>)
    {
        apply_value<I0>(std::forward<F>(f), std::forward<F1>(f1), std::forward<T>(t).*(std::get<I0>(tp)), sizeof...(I) == 0);
        apply(std::forward<F>(f), std::forward<F1>(f1), std::forward<T>(t), (std::tuple<Rest...>&&)tp, std::index_sequence<I...>{});
    }

    template<typename F, typename F1, typename T, typename... Rest>
    constexpr void apply(F&& f, F1&&, T&& t, std::tuple<Rest...>&&, std::index_sequence<>)
    {
    }
    //-------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------//

    template<typename F, typename T, typename... Rest, std::size_t I0, std::size_t... I>
    constexpr void apply(F&& f, T&&t, std::tuple<Rest...>&& tp, std::index_sequence<I0, I...>)
    {
        apply_value<I0>(std::forward<F>(f), (std::get<I0>(tp)), sizeof...(I) == 0);
        apply(std::forward<F>(f), std::forward<T>(t), (std::tuple<Rest...>&&)tp, std::index_sequence<I...>{});
    }

    template<typename F, typename T, typename... Rest>
    constexpr void apply(F&& f, T&& t, std::tuple<Rest...>&&, std::index_sequence<>)
    {
    }

    template<typename F, typename... Rest, std::size_t I0, std::size_t... I>
    constexpr void apply_tuple(F&& f, std::tuple<Rest...>& tp, std::index_sequence<I0, I...>)
    {
        apply_value<I0>(std::forward<F>(f), std::get<I0>(tp), sizeof...(I) == 0);
        apply_tuple(std::forward<F>(f), tp, std::index_sequence<I...>{});
    }

    template<typename F, typename... Rest>
    constexpr void apply_tuple(F&& f, std::tuple<Rest...>&, std::index_sequence<>)
    {
    }

    template<typename F, typename... Rest, std::size_t I0, std::size_t... I>
    constexpr void apply_tuple(F&& f, const std::tuple<Rest...>& tp, std::index_sequence<I0, I...>)
    {
        apply_value<I0>(std::forward<F>(f), std::get<I0>(tp), sizeof...(I) == 0);
        apply_tuple(std::forward<F>(f), tp, std::index_sequence<I...>{});
    }

    template<typename F, typename... Rest>
    constexpr void apply_tuple(F&& f, const std::tuple<Rest...>&, std::index_sequence<>)
    {
    }

    template<size_t I, typename T>
    constexpr decltype(auto) get(T&& t)
    {
        return std::forward<T>(t).*(std::get<I>(get_meta(std::forward<T>(t))));
    }

    template<typename T, typename F>
    constexpr auto for_each(T&& t, F&& f) -> std::enable_if_t<is_reflection<T>::value>
    {
        for_each_impl<0>(std::forward<F>(f), std::forward<T>(t), false, (void *)nullptr);
    }

    template<typename T, typename F, typename F1>
    constexpr auto for_each(T&& t, F&& f, F1&& f1) -> std::enable_if_t<is_reflection<T>::value>
    {
        for_each_impl<0>(std::forward<F>(f), std::forward<F1>(f1), std::forward<T>(t), false, (void *)nullptr);
    }

    template<typename T, typename F>
    constexpr auto for_each(T&& tp, F&& f) -> std::enable_if_t<!is_reflection<T>::value>
    {
        apply_tuple(std::forward<F>(f), std::forward<T>(tp), std::make_index_sequence<std::tuple_size<std::remove_reference_t <T>>::value>{});
    }

    template<typename T, size_t  I>
    constexpr const char* get_name()
    {
        static_assert(I < get_value<T>(), "out of range");
        return meta_t<T>::_arr[i];
    }

    template<typename T>
    const char* get_name()
    {
        return meta_t<T>::meta_name();
    }

    template<typename T>
    const char* get_name(size_t i)
    {
        return i >= get_value<T>() ? "" : meta_t<T>::_arr[i];
    }

    template<typename T>
    constexpr auto get_value() -> std::enable_if_t<is_reflection<T>::value, size_t>
    {
        return std::tuple_size<typename std::decay_t<T>::meta_type>::value;
    }

    template<typename T>
    constexpr auto get_value() -> std::enable_if_t<!is_reflection<T>::value, size_t>
    {
        return 0;
    }
}

namespace cytx
{
    class enum_factory;

    inline boost::optional<std::pair<std::string, std::string>> split_enum_str(std::string str, std::vector<const char*>&& splits)
    {
        boost::optional<std::pair<std::string, std::string>> result;
        for (auto & s : splits)
        {
            auto pos = str.find(s);
            if (pos != std::string::npos)
            {
                std::pair<std::string, std::string> p;
                p.first = str.substr(0, pos);
                p.second = str.substr(pos + std::string(s).length());
                result = p;
                break;
            }
        }
        return result;
    }

    class enum_helper
    {
        friend enum_factory;
    public:
        using pair_t = std::pair<std::string, uint32_t>;
        using vec_t = std::vector<pair_t>;
        using map_t = std::map<std::string, uint32_t>;

        void init(std::string name, vec_t&& vec)
        {
            name_ = name;
            vec_ = vec;

            for (auto& p : vec_)
            {
                map_.insert(p);
            }
        }

        template<typename T>
        boost::optional<std::string> to_string(T t, const char* split_field = "::")
        {
            boost::optional<std::string> result;

            for (auto& p : vec_)
            {
                if ((uint32_t)t == p.second)
                {
                    if (split_field == nullptr || split_field[0] == 0)
                    {
                        result = p.first;
                    }
                    else
                    {
                        result = fmt::format("{}{}{}", name_, split_field, p.first);
                    }
                    break;
                }
            }
            return result;
        }

        template<typename T>
        boost::optional<T> to_enum(const char* str, bool has_enum_name = false, std::vector<const char*>&& splits = { ".", "::", ":" })
        {
            boost::optional<T> result;
            auto r = to_enum_value<T>(str);
            if (r)
            {
                return r;
            }
            else if (has_enum_name)
            {
                auto r = split_enum_str(str, std::forward<std::vector<const char*>>(splits));
                if (r && r->first == name_)
                {
                    return to_enum_value<T>(str);
                }
            }
            return result;
        }

        template<typename T>
        boost::optional<T> to_enum_value(const char* str)
        {
            boost::optional<T> result;
            auto it = map_.find(std::string(str));
            if (it != map_.end())
                result = (T)it->second;
            return result;
        }

    private:
        enum_helper() {}
        std::string name_;
        vec_t vec_;
        map_t map_;
    };

    template< typename ENUM_T, typename T>
    std::vector<std::pair<const char*, ENUM_T>> get_vec(T& t)
    {
        vector<std::pair<const char*, ENUM_T>> vec;
        for_each(t, [&vec](const auto& p, size_t I, bool is_last)
        {
            vec.push_back(p);
        });
        return std::move(vec);
    }

    namespace detail
    {
        template<typename T>
        int reg_enum()
        {
            enum_factory::instance().reg<T>();
            return 0;
        }
    }

    class enum_factory
    {
    public:

        using pair_t = enum_helper::pair_t;
        using vec_t = enum_helper::vec_t;
        using enum_helper_ptr = std::unique_ptr<enum_helper>;
        using value_t = std::vector<enum_helper_ptr>;
        using value_ptr = std::shared_ptr<value_t>;
        using map_t = std::map<std::string, value_ptr>;

        static enum_factory& instance()
        {
            static enum_factory ef;
            return ef;
        }

        template<typename T>
        void reg()
        {
            auto enum_vec = get_vec<T>(get_meta<T>());
            vec_t vec;
            for (auto& p : enum_vec)
            {
                vec.push_back({ p.first, (uint32_t)p.second });
            }
            reg(enum_meta<T>::name(), enum_meta<T>::alias_name(), std::move(vec));
        }

        void reg(const char* enum_name, const char* alias_name, vec_t&& enum_fields)
        {
            auto helper_ptr = std::unique_ptr<enum_helper>(new enum_helper());
            helper_ptr->init(enum_name, std::forward<vec_t>(enum_fields));

            value_ptr val_ptr = nullptr;
            if (alias_name != nullptr)
            {
                auto it = enums_.find(alias_name);
                if (it != enums_.end())
                {
                    val_ptr = it->second;
                }
                else
                {
                    val_ptr = std::make_shared<value_t>();
                }
            }
            else
            {
                val_ptr = std::make_shared<value_t>();
            }

            val_ptr->emplace_back(std::move(helper_ptr));
            enums_.emplace(enum_name, val_ptr);
            if (alias_name)
                enums_.emplace(alias_name, val_ptr);
        }

        template<typename T>
        boost::optional<std::string> to_string(T t, const char* split_field = "::")
        {
            boost::optional<std::string> result;
            auto name = enum_meta<T>().name();
            auto it = enums_.find(name);
            if (it != enums_.end())
            {
                for (auto& v : *it->second)
                {
                    auto r = v->to_string(t, split_field);
                    if (r)
                        return r;
                }
            }
            return result;
        }

        boost::optional<std::string> to_string(uint32_t t, const char* enum_name, const char* split_field = "::")
        {
            boost::optional<std::string> result;
            auto it = enums_.find(enum_name);
            if (it != enums_.end())
            {
                for (auto& v : *it->second)
                {
                    auto r = v->to_string(t, split_field);
                    if (r)
                        return r;
                }
            }
            return result;
        }

        template<typename T>
        boost::optional<T> to_enum(const char* str, bool has_enum_name = false, std::vector<const char*>&& splits = { ".", "::", ":" })
        {
            boost::optional<T> result;

            auto name = enum_meta<T>().name();
            auto it = enums_.find(name);
            if (it != enums_.end())
            {
                for (auto& v : *it->second)
                {
                    auto r = v->to_enum<T>(str, has_enum_name, std::forward<vector<const char*>>(splits));
                    if (r)
                        return r;
                }
            }
            return result;
        }

    private:
        enum_factory() {}

        map_t enums_;
    };

    template<typename T>
    auto to_string(T t, const char* split_str = "::")  -> std::enable_if_t<enum_meta<T>::value, boost::optional<std::string>>
    {
        return enum_factory::instance().to_string<T>(t, split_str);
    }

    inline auto to_string(uint32_t t, const char* enum_name, const char* split_str = "::")  -> boost::optional<std::string>
    {
        return enum_factory::instance().to_string(t, enum_name, split_str);
    }

    template<typename T>
    auto to_enum(const char* str, bool has_enum_name = false, std::vector<const char*>&& splits = { ".", "::", ":" })
        -> std::enable_if_t<enum_meta<T>::value, boost::optional<T>>
    {
        return enum_factory::instance().to_enum<T>(str, has_enum_name, std::forward<std::vector<const char*>>(splits));
    }

    template<typename T>
    auto to_string(T t, const char* split_str = "::") -> std::enable_if_t<!enum_meta<T>::value, boost::optional<std::string>>
    {
        return boost::optional<std::string>{};
    }

    template<typename T>
    auto to_enum(const char* str, bool has_enum_name = false, std::vector<const char*>&& splits = { ".", "::", ":" })
        -> std::enable_if_t<!enum_meta<T>::value, boost::optional<T>>
    {
        return boost::optional<T>{};
    }

}