#pragma once
#include "base_module.hpp"
#include "network/base/world.hpp"

namespace cytx
{
    class module_manager
    {
        using new_module_func_t = std::function<base_module*(world_map* world_ptr)>;
    public:
        static module_manager& ins()
        {
            static module_manager mgr;
            return mgr;
        }

        world_map& world()
        {
            return world_;
        }

    public:
        bool init(std::string module_name)
        {
            return init_module(module_name);
        }

        bool start()
        {
            return std::all_of(std::begin(modules_), std::end(modules_), [](auto& p) {
                return p.second->start();
            });
        }

        bool reload()
        {
            return std::all_of(std::begin(modules_), std::end(modules_), [](auto& p) {
                return p.second->reload();
            });
        }

    public:
        service_manager* get_service_mgr()
        {
            return modules_.begin()->second->get_service_mgr();
        }

    private:
        bool init_module(std::string module_name)
        {
            auto it = reg_modules.find(module_name);
            if (it != reg_modules.end())
            {
                base_module* module_ptr = it->second(&world_);
                modules_[module_name] = module_ptr;
                return true;
            }
            return false;
        }

    public:
        template<typename T>
        auto register_module(const std::string& name, new_module_func_t func) -> std::enable_if_t<std::is_base_of<base_module, T>>
        {
            reg_modules[name] = func;
        }
    private:
        world_map world_;
        std::unordered_map<std::string, new_module_func_t> reg_modules;
        std::unordered_map<std::string, base_module*> modules_;
    };
}

#define REG_SERVICE(type)                                                                       \
namespace __reg_module_ ## type ## __LINE__                                                     \
{                                                                                               \
    static int r = cytx::module_manager::ins().register_module<type>(#type,                     \
                            [](world_map* world_ptr) -> base_cmder* {                           \
                                    return new T(world_ptr);                                    \
                            });                                                                 \
}

