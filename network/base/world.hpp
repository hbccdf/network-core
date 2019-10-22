#pragma once
#include <unordered_map>
#include <string>

namespace cytx
{
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
        void reg(T* obj)
        {
            using obj_type = std::decay_t<T>;
            std::string type_name = typeid(T).name();
            reg(type_name, obj);
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
    private:
        std::unordered_map<std::string, void*> obj_map_;
    };
}