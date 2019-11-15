#pragma once
#include <unordered_map>
#include "type_id.hpp"

namespace cytx
{
    template<typename ID>
    class base_type_map
    {
    public:
        template<typename T>
        base_type_map& set(ID id)
        {
            type_id_t tid = TypeId::id<T>();
            id_to_type_map_[id] = tid;
            type_to_id_map_[tid] = id;
            return *this;
        }

        template<typename T>
        ID get() const
        {
            return get_or<T>(-1);
        }

        template<typename T>
        ID get_or(ID id) const
        {
            type_id_t tid = TypeId::id<T>();
            auto it = type_to_id_map_.find(tid);
            if (it != type_to_id_map_.end())
                return it->second;

            return id;
        }

    protected:
        std::unordered_map<ID, type_id_t> id_to_type_map_;
        std::unordered_map<type_id_t, ID> type_to_id_map_;
    };

    using type_map = base_type_map<int>;
}