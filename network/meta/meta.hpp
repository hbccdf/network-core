#pragma once
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include <fmt/format.h>
#include <boost/optional.hpp>

#include "base_meta.hpp"
#include "enum_meta.hpp"
#include "db_meta.hpp"
#include "struct_meta.hpp"

#include "network/traits/traits.hpp"

#include <fmt/format.h>
#include <boost/optional.hpp>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>

namespace cytx
{
    inline void to_enum_extend(...) {}

    inline void to_db_extend(...) {}

    inline void to_st_extend(...) {}

    struct base_db_meta {};

    namespace detail
    {
        template<typename T>
        struct get_enum_extend_type
        {
            using type = decltype(to_enum_extend((std::decay_t<T>*)nullptr));
        };
        template<typename T>
        using get_enum_extend_type_t = typename get_enum_extend_type<T>::type;

        template<typename T>
        struct get_db_extend_type
        {
            using type = decltype(to_db_extend((std::decay_t<T>*)nullptr));
        };
        template<typename T>
        using get_db_extend_type_t = typename get_db_extend_type<T>::type;

        template<typename T>
        struct get_st_extend_type
        {
            using type = decltype(to_st_extend((std::decay_t<T>*)nullptr));
        };
        template<typename T>
        using get_st_extend_type_t = typename get_st_extend_type<T>::type;

        template<typename T, class = std::void_t<>>
        struct enum_meta : public std::false_type {};

        template<typename T>
        struct enum_meta<T, std::void_t<std::enable_if_t<!std::is_void_v<get_enum_extend_type_t<T>>>>> : std::true_type
        {};

        template<typename T>
        constexpr bool enum_meta_v = enum_meta<std::decay_t<T>>::value;

        template<typename T, class = std::void_t<>>
        struct db_meta : public std::false_type {};

        template<typename T>
        struct db_meta<T, std::void_t<std::enable_if_t<!std::is_void_v<get_db_extend_type_t<T>>>>> : std::true_type
        {};

        template<typename T>
        constexpr bool db_meta_v = db_meta<std::decay_t<T>>::value;

        template<typename T> struct is_db_meta : std::integral_constant<bool, std::is_base_of<base_db_meta, std::decay_t<T>>::value> {};

        template<typename T, class = std::void_t<>>
        struct st_meta : public std::false_type {};

        template<typename T>
        struct st_meta<T, std::void_t<std::enable_if_t<!std::is_void_v<get_st_extend_type_t<T>>>>> : std::true_type
        {};

        template<typename T>
        constexpr bool st_meta_v = st_meta<std::decay_t<T>>::value;


        template <typename T, class = std::void_t<>>
        struct is_reflection : std::false_type
        {
        };

        template <typename T>
        struct is_reflection<T, std::void_t<std::enable_if_t<db_meta_v<T>>>> : std::true_type
        {
        };

        template <typename T>
        struct is_reflection<T, std::void_t<std::enable_if_t<st_meta_v<T>>>> : std::true_type
        {
        };

        template <typename T>
        struct is_reflection<T, std::void_t<std::enable_if_t<has_meta_macro_v<T>>>> : std::true_type
        {
        };

        template<typename T>
        struct is_reflection <T, std::void_t<std::enable_if_t<enum_meta_v<T>>>> : std::true_type
        {
        };
        template<typename T, class = std::void_t<>>
        struct get_meta_impl;

        template<typename T>
        struct get_meta_impl<T, std::void_t<std::enable_if_t<has_meta_macro_v<T>>>>
        {
            using type = std::decay_t<T>;

            static auto meta(T&& t)
            {
                return t.Meta();
            }
        };

        template<typename T>
        struct get_meta_impl<T, std::void_t<std::enable_if_t<enum_meta_v<T>>>>
        {
            using type = get_enum_extend_type_t<T>;

            static auto meta(T&& t)
            {
                return get_enum_extend_type_t<T>::Meta();
            }

            static auto meta()
            {
                return get_enum_extend_type_t<T>::Meta();
            }
        };

        template<typename T>
        struct get_meta_impl<T, std::void_t<std::enable_if_t<db_meta_v<T>>>>
        {
            using type = get_db_extend_type_t<T>;

            static auto meta(T&& t)
            {
                return t.Meta();
            }
        };

        template<typename T>
        struct get_meta_impl<T, std::void_t<std::enable_if_t<st_meta_v<T>>>>
        {
            using type = get_st_extend_type_t<std::decay_t<T>>;

            static auto meta(T&& t)
            {
                return get_st_extend_type_t<std::decay_t<T>>::Meta(t);
            }
        };

        template<typename T, class = std::void_t<>>
        struct get_meta_elem_impl;

        template<typename T>
        struct get_meta_elem_impl<T, std::void_t<std::enable_if_t<has_meta_macro_v<T>>>>
        {
            template<size_t I, typename MetaTuple>
            static auto get(T&& t, MetaTuple&& tp)
            {
                return std::get<I>(tp);
            }

            template<size_t I>
            static auto get(T&& t)
            {
                return get<I>(std::forward<T>(t), get_meta_impl<T>::meta(std::forward<T>(t)));
            }
        };

        template<typename T>
        struct get_meta_elem_impl<T, std::void_t<std::enable_if_t<db_meta_v<T>>>>
        {
            template<size_t I, typename MetaTuple>
            static auto get(T&& t, MetaTuple&& tp)
            {
                return std::forward<T>(t).*(std::get<I>(tp));
            }

            template<size_t I>
            static auto get(T&& t)
            {
                return get<I>(std::forward<T>(t), get_meta_impl<T>::meta(std::forward<T>(t)));
            }
        };

        template<typename T>
        struct get_meta_elem_impl<T, std::void_t<std::enable_if_t<st_meta_v<T>>>>
        {
            template<size_t I, typename MetaTuple>
            static auto get(T&& t, MetaTuple&& tp)
            {
                return std::get<I>(tp);
            }

            template<size_t I>
            static auto get(T&& t)
            {
                return get<I>(std::forward<T>(t), get_meta_impl<T>::meta(std::forward<T>(t)));
            }
        };

        template<typename T>
        struct get_meta_elem_impl<T, std::void_t<std::enable_if_t<is_tuple<T>::value>>>
        {
            template<size_t I>
            static auto get(T&& t)
            {
                return std::get<I>(std::forward<T>(t));
            }
        };

        template<typename T> struct tuple_total_size_impl<T, std::void_t<std::enable_if_t<st_meta_v<T>>>>
        {
            constexpr static int value = tuple_total_size<typename get_meta_impl<T>::type::meta_type>::value;
        };

        template<typename F, typename T>
        struct is_result_bool
        {
            constexpr static bool value = !std::is_void_v<std::result_of_t<F(T, size_t, bool)>>;
        };
    }

    template<typename T>
    using meta_t = typename detail::get_meta_impl<T>::type;

    template<typename T>
    using is_reflection = detail::is_reflection<T>;

    template<typename T>
    constexpr static bool is_reflection_v = is_reflection<T>::value;


    template<typename T>
    auto get_meta()
    {
        return detail::get_meta_impl<T>::meta();
    }

    template<typename T>
    auto get_meta(T&& t)
    {
        return detail::get_meta_impl<T>::meta(std::forward<T>(t));
    }

    template<typename T>
    const char* get_name()
    {
        return meta_t<T>::meta_name();
    }


    template<typename T>
    constexpr auto get_meta_size() -> std::enable_if_t<is_reflection_v<T>, size_t>
    {
        return std::tuple_size<typename meta_t<T>::meta_type>::value;
    }

    template<typename T>
    constexpr auto get_meta_size() -> std::enable_if_t<is_tuple<T>::value, size_t>
    {
        return std::tuple_size<std::decay_t<T>>::value;
    }

    template<typename T>
    constexpr auto get_meta_size() -> std::enable_if_t<!is_reflection<T>::value && !is_tuple<T>::value, size_t>
    {
        return 0;
    }

    template<size_t I, typename T>
    constexpr auto get(T&& t)
    {
        return detail::get_meta_elem_impl<T>::get<I>(std::forward<T>(t));
    }

    template<size_t I, typename T, typename MetaTuple>
    constexpr auto get(T&& t, MetaTuple&& tp)
    {
        return detail::get_meta_elem_impl<T>::get<I>(std::forward<T>(t), std::forward<MetaTuple>(tp));
    }

    template<size_t I, typename F, typename T>
    auto apply_value(F&& f, T&& t, bool is_last) -> std::enable_if_t<detail::is_result_bool<F, T>::value, bool>
    {
        return std::forward<F>(f)(std::forward<T>(t), I, is_last);
    };

    template<size_t I, typename F, typename T>
    auto apply_value(F&& f, T&& t, bool is_last) -> std::enable_if_t<!detail::is_result_bool<F, T>::value, bool>
    {
        std::forward<F>(f)(std::forward<T>(t), I, is_last);
        return true;
    };

    template<typename ... Args>
    constexpr void apply(Args&& ... args)
    {
    }

    //meta
    template<typename F, typename T, typename Tuple, std::size_t I0, std::size_t... I>
    constexpr void apply(F&& f, T&& t, Tuple&& tp, std::index_sequence<I0, I...>)
    {
        if (apply_value<I0>(std::forward<F>(f), get<I0>(t, tp), sizeof...(I) == 0))
        {
            apply(std::forward<F>(f), std::forward<T>(t), std::forward<Tuple>(tp), std::index_sequence<I...>{});
        }
    }

    //tuple
    template<typename F, typename Tuple, std::size_t I0, std::size_t... I>
    constexpr auto apply(F&& f, Tuple&& tp, std::index_sequence<I0, I...>) -> std::enable_if_t<is_tuple<Tuple>::value>
    {
        if (apply_value<I0>(std::forward<F>(f), std::get<I0>(tp), sizeof...(I) == 0))
        {
            apply(std::forward<F>(f), std::forward<Tuple>(tp), std::index_sequence<I...>{});
        }
    }


    template<typename T, typename F>
    constexpr auto for_each(T&& t, F&& f) -> std::enable_if_t<is_reflection<T>::value>
    {
        apply(std::forward<F>(f), std::forward<T>(t), get_meta(std::forward<T>(t)), std::make_index_sequence<get_meta_size<T>()>{});
    }

    template<typename T, typename F>
    constexpr auto for_each(T&& t, F&& f) -> std::enable_if_t<!is_reflection<T>::value>
    {
        apply(std::forward<F>(f), std::forward<T>(t), std::make_index_sequence<get_meta_size<T>()>{});
    }
}

namespace cytx
{
    namespace detail
    {
        class enum_factory;

        static boost::optional<std::pair<std::string, std::string>> split_enum_str(std::string str, std::vector<const char*>&& splits)
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
            std::vector<std::pair<const char*, ENUM_T>> vec;
            for_each(t, [&vec](const auto& p, size_t I, bool is_last)
            {
                vec.push_back(p);
            });
            return std::move(vec);
        }

        template<typename T>
        int reg_enum()
        {
            enum_factory::instance().reg<T>();
            return 0;
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
                const char* enum_name = get_enum_extend_type_t<T>::name();
                auto it = enums_.find(std::string(enum_name));
                if (it != enums_.end())
                    return;

                auto enum_vec = get_vec<T>(get_meta<T>());
                vec_t vec;
                for (auto& p : enum_vec)
                {
                    vec.push_back({ p.first, (uint32_t)p.second });
                }

                reg(enum_name, get_enum_extend_type_t<T>::alias_name(), std::move(vec));
            }

            void reg(const char* enum_name, const char* alias_name, vec_t&& enum_fields)
            {
                //std::cout << fmt::format("reg enum {}, alias {}, field count {}", enum_name, alias_name ? alias_name : "null", enum_fields.size()) << std::endl;
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
                }
                if(!val_ptr)
                {
                    val_ptr = std::make_shared<value_t>();
                }

                val_ptr->emplace_back(std::move(helper_ptr));
                enums_.emplace(enum_name, val_ptr);
                if (alias_name)
                {
                    enums_.emplace(alias_name, val_ptr);
                }
            }

            template<typename T>
            boost::optional<std::string> to_string(T t, const char* split_field = "::")
            {
                boost::optional<std::string> result;
            auto name = get_enum_extend_type_t<T>::name();
                auto it = enums_.find(name);
                if (it != enums_.end())
                {
                    for (auto& v : *it->second)
                    {
                        result = v->to_string(t, split_field);
                        if (result)
                            return result;
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
                        result = v->to_string(t, split_field);
                        if (result)
                            return result;
                    }
                }
                return result;
            }

            template<typename T>
            boost::optional<T> to_enum(const char* str, bool has_enum_name = false, std::vector<const char*>&& splits = { ".", "::", ":" })
            {
                boost::optional<T> result;

            auto name = get_enum_extend_type_t<T>::name();
                auto it = enums_.find(name);
                if (it != enums_.end())
                {
                    for (auto& v : *it->second)
                    {
                        result = v->to_enum<T>(str, has_enum_name, std::forward<std::vector<const char*>>(splits));
                        if (result)
                            return result;
                    }
                }
                return result;
            }

        private:
            enum_factory() {}

            map_t enums_;
        };
    }

#ifdef ENUM_META_RALAX_CHECK
#define ENUM_META_CHECK(name) detail::enum_meta<name>::value
#else
#define ENUM_META_CHECK(name) std::is_enum<name>::value
#endif

    template<typename T>
    auto to_string(T t, const char* split_str = "::")  -> std::enable_if_t<ENUM_META_CHECK(T), boost::optional<std::string>>
    {
        return detail::enum_factory::instance().to_string<T>(t, split_str);
    }

    template<typename T>
    auto to_string(T t, const char* split_str = "::") -> std::enable_if_t<!ENUM_META_CHECK(T), boost::optional<std::string>>
    {
        return boost::optional<std::string>{};
    }

    inline auto to_string(uint32_t t, const char* enum_name, const char* split_str = "::")  -> boost::optional<std::string>
    {
        return detail::enum_factory::instance().to_string(t, enum_name, split_str);
    }

    template<typename T>
    auto to_enum(const char* str, bool has_enum_name = false, std::vector<const char*>&& splits = { ".", "::", ":" })
        -> std::enable_if_t<ENUM_META_CHECK(T), boost::optional<T>>
    {
        return detail::enum_factory::instance().to_enum<T>(str, has_enum_name, std::forward<std::vector<const char*>>(splits));
    }

    template<typename T>
    auto to_enum(const char* str, bool has_enum_name = false, std::vector<const char*>&& splits = { ".", "::", ":" })
        -> std::enable_if_t<!ENUM_META_CHECK(T), boost::optional<T>>
    {
        return boost::optional<T>{};
    }


    template<typename T>
    auto ralax_to_string(T t, const char* split_str = "::")  -> std::enable_if_t<detail::enum_meta<T>::value, boost::optional<std::string>>
    {
        return detail::enum_factory::instance().to_string<T>(t, split_str);
    }

    template<typename T>
    auto ralax_to_string(T t, const char* split_str = "::") -> std::enable_if_t<!detail::enum_meta<T>::value, boost::optional<std::string>>
    {
        return boost::optional<std::string>{};
    }

    template<typename T>
    auto ralax_to_enum(const char* str, bool has_enum_name = false, std::vector<const char*>&& splits = { ".", "::", ":" })
        -> std::enable_if_t<detail::enum_meta<T>::value, boost::optional<T>>
    {
        return detail::enum_factory::instance().to_enum<T>(str, has_enum_name, std::forward<std::vector<const char*>>(splits));
    }

    template<typename T>
    auto ralax_to_enum(const char* str, bool has_enum_name = false, std::vector<const char*>&& splits = { ".", "::", ":" })
        -> std::enable_if_t<!detail::enum_meta<T>::value, boost::optional<T>>
    {
        return boost::optional<T>{};
    }
}