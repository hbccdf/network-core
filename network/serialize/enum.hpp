#pragma once
#include "common.hpp"
#include <fmt/format.h>
#include <boost/optional.hpp>
#include <vector>
#include <map>
#include <memory>
#include "../traits/traits.hpp"
#include <cstdint>

template<typename T>
struct enum_meta : public std::false_type {};

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
            else if(has_enum_name)
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

    template<typename T>
    void reg_vec_pair(std::vector<std::pair<const char*, T>>& vec, const std::pair<const char*, T>& p)
    {
        vec.push_back(p);
    }

    template<typename ENUM_T, size_t I, typename T>
    auto reg_vec(std::vector<std::pair<const char*, ENUM_T>>& vec, const T& t)->std::enable_if_t < I < std::tuple_size<T>::value>
    {
        reg_vec_pair<ENUM_T>(vec, std::get<I>(t));
        reg_vec<ENUM_T, I + 1, T>(vec, t);
    }

    template<typename ENUM_T, size_t I, typename T>
    auto reg_vec(std::vector<std::pair<const char*, ENUM_T>>& vec, const T& t)->std::enable_if_t < I == std::tuple_size<T>::value>
    {
    }


    template< typename ENUM_T, typename T>
    std::vector<std::pair<const char*, ENUM_T>> get_vec(const T& t)
    {
        vector<std::pair<const char*, ENUM_T>> vec;
        reg_vec<ENUM_T, 0, T>(vec, t);
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
        using map_t = std::map<const char*, value_ptr>;

        static enum_factory& instance()
        {
            static enum_factory ef;
            return ef;
        }

        template<typename T>
        void reg()
        {
            enum_meta<T> meta;
            auto enum_vec = get_vec<T>(meta());
            vec_t vec;
            for (auto& p : enum_vec)
            {
                vec.push_back({ p.first, (uint32_t)p.second });
            }
            reg(meta.name(), meta.alias_name(), std::move(vec));
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

#define PAIR_ENUM(name, t) std::make_pair(#t, name##::t)
#define MAKE_ENUM_TUPLE(enum_name, ...) template<> struct enum_meta<enum_name>  : public std::true_type \
    { auto operator() () { return std::make_tuple(__VA_ARGS__);  } \
    const char* name() { return #enum_name; }

#define MAKE_ENUM_ARG_LIST_1(name, op, arg, ...)   op(name, arg)
#define MAKE_ENUM_ARG_LIST_2(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_1(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_3(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_2(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_4(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_3(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_5(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_4(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_6(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_5(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_7(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_6(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_8(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_7(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_9(name, op, arg, ...)   op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_8(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_10(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_9(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_11(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_10(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_12(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_11(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_13(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_12(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_14(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_13(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_15(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_14(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_16(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_15(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_17(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_16(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_18(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_17(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_19(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_18(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_20(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_19(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_21(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_20(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_22(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_21(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_23(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_22(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_24(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_23(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_25(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_24(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_26(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_25(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_27(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_26(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_28(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_27(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_29(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_28(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_30(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_29(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_31(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_30(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_32(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_31(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_33(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_32(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_34(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_33(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_35(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_34(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_36(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_35(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_37(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_36(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_38(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_37(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_39(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_38(name, op, __VA_ARGS__))
#define MAKE_ENUM_ARG_LIST_40(name, op, arg, ...)  op(name, arg), MARCO_EXPAND(MAKE_ENUM_ARG_LIST_39(name, op, __VA_ARGS__))


#define MAKE_ENUM_ARG_LIST(name, N, op, arg, ...) \
        MACRO_CONCAT(MAKE_ENUM_ARG_LIST, N)(name, op, arg, __VA_ARGS__)

#define EMMBED_ENUM_TUPLE(name, N, ...) \
MAKE_ENUM_TUPLE(name, MAKE_ENUM_ARG_LIST(name, N, PAIR_ENUM, __VA_ARGS__))

#define REG_ENUM(name, ...) EMMBED_ENUM_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__) \
    const char* alias_name() { return nullptr; } \
}; \
namespace ___reg_enum_helper_value___ ## name ## __LINE__ { static int val = cytx::detail::reg_enum<name>(); }

#define REG_ALIAS_ENUM(name, alias_name_str, ...) EMMBED_ENUM_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__) \
    const char* alias_name() { return #alias_name_str; } \
}; \
namespace ___reg_enum_helper_value___ ## name ## __LINE__ { static int val = cytx::detail::reg_enum<name>(); }

}