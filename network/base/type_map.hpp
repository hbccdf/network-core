#pragma once
#include <unordered_map>
#include "type_id.hpp"

namespace cytx
{
    class type_map
    {
    public:
        template<typename T>
        type_map& set(int id)
        {
            type_id_t tid = TypeId::id<T>();
            id_to_type_map_[id] = tid;
            type_to_id_map_[tid] = id;
            return *this;
        }

        template<typename T>
        int get() const
        {
            return get_or<T>(-1);
        }

        template<typename T>
        int get_or(int id) const
        {
            type_id_t tid = TypeId::id<T>();
            auto it = type_to_id_map_.find(tid);
            if (it != type_to_id_map_.end())
                return it->second;

            return id;
        }

    protected:
        std::unordered_map<int, type_id_t> id_to_type_map_;
        std::unordered_map<type_id_t, int> type_to_id_map_;
    };
}