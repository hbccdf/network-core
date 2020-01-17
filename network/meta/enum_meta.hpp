#pragma once
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include <fmt/format.h>
#include <boost/optional.hpp>

#include "meta_common.hpp"
#include "type_meta.hpp"

#define PAIR_ENUM(name, t) std::make_pair(#t, name::t)
#define MAKE_ENUM_TUPLE(enum_name, ...) \
inline auto to_enum_extend(enum_name const*) {  \
struct enum_meta_##enum_name { \
    static auto Meta() { return std::make_tuple(__VA_ARGS__);  } \
    static const char* name() { return #enum_name; }


#define EMMBED_ENUM_TUPLE(name, N, ...) \
REG_TYPE(name); \
MAKE_ENUM_TUPLE(name, MAKE_ARG_LIST(name, N, PAIR_ENUM, COMMA_DELIMITER, __VA_ARGS__))

#define REG_ENUM(name, ...) EMMBED_ENUM_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__) \
    static const char* alias_name() { return nullptr; } \
}; \
return enum_meta_##name{}; \
} \
namespace MACRO_CONCAT(___reg_enum_helper_value___ ## name, __LINE__) { static int val = cytx::detail::reg_enum<name>(); }

#define REG_ALIAS_ENUM(name, alias_name_str, ...) EMMBED_ENUM_TUPLE(name, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__) \
    static const char* alias_name() { return #alias_name_str; } \
}; \
return enum_meta_##name{}; \
} \
namespace MACRO_CONCAT(___reg_enum_helper_value___ ## name, __LINE__) { static int val = cytx::detail::reg_enum<name>(); }


namespace cytx
{
    inline void to_enum_extend(...) {}

    namespace detail
    {
        template<typename T>
        struct get_enum_extend_type
        {
            using type = decltype(to_enum_extend((std::decay_t<T>*)nullptr));
        };
        TEMPLATE_TYPE(get_enum_extend_type);

        template<typename T>
        struct enum_meta : std::integral_constant<bool, !std::is_void_v<get_enum_extend_type_t<T>>> {};

        TEMPLATE_VALUE(enum_meta);

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
        struct is_reflection<T, std::void_t<std::enable_if_t<enum_meta_v<T>>>> : std::true_type {};
    }
}

namespace cytx
{
    namespace detail
    {
        template< typename ENUM_T, typename T>
        std::vector<std::pair<std::string, uint32_t>> get_vec(const T& t)
        {
            std::vector<std::pair<std::string, uint32_t>> vec;
            for_each(t, [&vec](const auto& p, size_t I, bool is_last)
            {
                vec.emplace_back(std::make_pair(p.first, (uint32_t)p.second));
            });
            return std::move(vec);
        }

        class enum_factory;
        class enum_helper
        {
            friend enum_factory;
        public:
            using pair_t = std::pair<std::string, uint32_t>;
            using vec_t = std::vector<pair_t>;
            using map_t = std::unordered_map<std::string, uint32_t>;

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
            boost::optional<std::string> to_string(T t, bool has_enum_name = false) const
            {
                boost::optional<std::string> result;

                for (auto& p : vec_)
                {
                    if ((uint32_t)t == p.second)
                    {
                        if (!has_enum_name)
                        {
                            result = p.first;
                        }
                        else
                        {
                            result = fmt::format("{}::{}", name_, p.first);
                        }
                        break;
                    }
                }
                return result;
            }

            template<typename T>
            boost::optional<T> to_enum(const char* str, bool has_enum_name = false) const
            {
                boost::optional<T> result;
                auto r = to_enum_value<T>(str);
                if (r)
                {
                    return r;
                }
                else if (has_enum_name)
                {
                    auto r = string_util::split_by_string(str, "::");
                    if (r && r->first == name_)
                    {
                        return to_enum_value<T>(str);
                    }
                }
                return result;
            }

            template<typename T>
            boost::optional<T> to_enum_value(const char* str) const
            {
                boost::optional<T> result;
                auto it = map_.find(std::string(str));
                if (it != map_.end())
                    result = (T)it->second;
                return result;
            }

            const vec_t& get_enum_list() const
            {
                return vec_;
            }

            std::string get_name() const
            {
                return name_;
            }

        private:
            enum_helper() {}
            std::string name_;
            vec_t vec_;
            map_t map_;
        };

        class enum_factory
        {
        public:

            using pair_t = enum_helper::pair_t;
            using vec_t = enum_helper::vec_t;
            using enum_helper_ptr = std::unique_ptr<enum_helper>;
            using value_t = std::vector<enum_helper_ptr>;
            using value_ptr = std::shared_ptr<value_t>;
            using map_t = std::unordered_map<std::string, value_ptr>;
            using typeid_map_t = std::unordered_map<type_id_t, value_ptr>;

            static enum_factory& ins()
            {
                static enum_factory ef;
                return ef;
            }

            template<typename T>
            void reg()
            {
                using enum_type = meta_t<T>;
                type_id_t tid = TypeId::id<T>();
                const char* enum_name = enum_type::name();

                auto it = typeid_enums_.find(tid);
                if (it != typeid_enums_.end())
                    return;

                auto meta_tuple = get_meta<T>();
                auto enum_vec = get_vec<T>(meta_tuple);

                inter_reg(tid, enum_name, enum_type::alias_name(), std::move(enum_vec));
            }

            template<typename T>
            boost::optional<std::string> to_string(T t, bool has_enum_name = false) const
            {
                boost::optional<std::string> result;

                type_id_t tid = TypeId::id<T>();

                auto it = typeid_enums_.find(tid);
                if (it != typeid_enums_.end())
                {
                    for (auto& v : *it->second)
                    {
                        result = v->to_string(t, has_enum_name);
                        if (result)
                            return result;
                    }
                }
                return result;
            }

            boost::optional<std::string> to_string(uint32_t t, const char* enum_name, bool has_enum_name = false) const
            {
                boost::optional<std::string> result;
                auto it = enums_.find(enum_name);
                if (it != enums_.end())
                {
                    for (auto& v : *it->second)
                    {
                        result = v->to_string(t, has_enum_name);
                        if (result)
                            return result;
                    }
                }
                return result;
            }

            template<typename T>
            boost::optional<T> to_enum(const char* str, bool has_enum_name = false) const
            {
                boost::optional<T> result;

                type_id_t tid = TypeId::id<T>();

                auto it = typeid_enums_.find(tid);
                if (it != typeid_enums_.end())
                {
                    for (auto& v : *it->second)
                    {
                        result = v->template to_enum<T>(str, has_enum_name);
                        if (result)
                            return result;
                    }
                }
                return result;
            }

            template<typename T>
            vec_t get_enum_info_list() const
            {
                vec_t list;

                type_id_t tid = TypeId::id<T>();

                auto it = typeid_enums_.find(tid);
                if (it != typeid_enums_.end())
                {
                    for (auto& ptr : *(it->second))
                    {
                        list = ptr->get_enum_list();
                        break;
                    }
                }

                return list;
            }

        private:
            enum_factory() {}

            void inter_reg(type_id_t enum_tid, const char* enum_name, const char* alias_name, vec_t&& enum_fields)
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
                }
                if (!val_ptr)
                {
                    val_ptr = std::make_shared<value_t>();
                }

                val_ptr->emplace_back(std::move(helper_ptr));
                typeid_enums_.emplace(enum_tid, val_ptr);
                enums_.emplace(enum_name, val_ptr);
                if (alias_name)
                {
                    enums_.emplace(alias_name, val_ptr);
                }
            }
        private:
            map_t enums_;
            typeid_map_t typeid_enums_;
        };

        template<typename T>
        int reg_enum()
        {
            enum_factory::ins().reg<T>();
            return 0;
        }
    }

#define ENUM_META_CHECK(name) is_enum_type_v<name>

    using enum_info_list_t = detail::enum_factory::vec_t;

    template<typename T>
    auto to_string(T t, bool has_enum_name = false)  -> std::enable_if_t<ENUM_META_CHECK(T), boost::optional<std::string>>
    {
        return detail::enum_factory::ins().to_string<T>(t, has_enum_name);
    }

    inline auto to_string(uint32_t t, const char* enum_name, bool has_enum_name = false)  -> boost::optional<std::string>
    {
        return detail::enum_factory::ins().to_string(t, enum_name, has_enum_name);
    }

    template<typename T>
    auto to_enum(const char* str, bool has_enum_name = false) -> std::enable_if_t<ENUM_META_CHECK(T), boost::optional<T>>
    {
        return detail::enum_factory::ins().to_enum<T>(str, has_enum_name);
    }

    template<typename T>
    auto get_enum_info_list()  -> std::enable_if_t<ENUM_META_CHECK(T), enum_info_list_t>
    {
        return detail::enum_factory::ins().get_enum_info_list<T>();
    }
}

namespace fmt
{
    template<typename T>
    auto format(fmt::BasicFormatter<char> &f, const char *&format_str, const T& val) -> std::enable_if_t<cytx::is_enum_type_v<T>>
    {
        boost::optional<std::string> v = cytx::to_string(val, false);

        if (v)
        {
            f.writer().write("{}", v.get());
        }
        else
        {
            using under_type = std::underlying_type_t<std::decay_t<T>>;
            f.writer().write("{}", (under_type)val);
        }
    }
}