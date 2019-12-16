#pragma once
#include <unordered_map>
#include "type_id.hpp"
#include "network/traits/traits.hpp"

namespace cytx
{
    template<typename ID>
    class base_type_map
    {
    public:
        base_type_map()
            : default_id_(get_default<ID>())
        {
        }

        template<typename T>
        base_type_map& set(const ID& id)
        {
            type_id_t tid = TypeId::id<T>();
            id_to_type_map_[id] = tid;
            type_to_id_map_[tid] = id;
            return *this;
        }

        template<typename T>
        ID get() const
        {
            return get_or<T>(default_id_);
        }

        template<typename T>
        ID get_or(const ID& id) const
        {
            type_id_t tid = TypeId::id<T>();
            auto it = type_to_id_map_.find(tid);
            if (it != type_to_id_map_.end())
                return it->second;

            return id;
        }

        template<typename T>
        bool has() const
        {
            type_id_t tid = TypeId::id<T>();
            return type_to_id_map_.find(tid) != type_to_id_map_.end();
        }

    protected:

        template<typename T>
        auto get_default() const -> std::enable_if_t<is_number_type_v<T>, T>
        {
            return -1;
        }

        template<typename T>
        auto get_default() const -> std::enable_if_t<is_string_v<T>, T>
        {
            return "";
        }

    protected:
        std::unordered_map<ID, type_id_t> id_to_type_map_;
        std::unordered_map<type_id_t, ID> type_to_id_map_;
        ID default_id_;
    };

    using type_map = base_type_map<int>;
}