#pragma once
#include "base_module.hpp"
#include "network/base/world.hpp"
#include "network/meta/type_meta.hpp"

namespace cytx
{
    class module_manager
    {
        using new_module_func_t = std::function<base_module*(world_ptr_t world_ptr)>;
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

        void stop()
        {
            std::for_each(std::begin(modules_), std::end(modules_), [](auto& p) {
                p.second->stop();
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
            auto it = reg_modules_.find(module_name);
            if (it != reg_modules_.end())
            {
                base_module* module_ptr = it->second(&world_);
                modules_[module_name] = module_ptr;
                return module_ptr->init();
            }
            return false;
        }

    public:
        template<typename T>
        auto register_module(const std::string& name) -> std::enable_if_t<std::is_base_of<base_module, T>::value>
        {
            reg_modules_[name] = [](world_ptr_t world_ptr) -> base_module* {
                T* module = world_ptr->factory().get<T>();
                module->set_world(world_ptr);
                return module;
            };
        }
    private:
        world_t world_;
        std::unordered_map<std::string, new_module_func_t> reg_modules_;
        std::unordered_map<std::string, base_module*> modules_;
    };
}

#define REG_MODULE(type)                                                                        \
REG_TYPE(type)                                                                                  \
namespace __reg_module_ ## type ## __LINE__                                                     \
{                                                                                               \
    static int r = (cytx::module_manager::ins().register_module<type>(#type), 0);               \
}

