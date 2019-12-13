#pragma once
#include "base_cmder.hpp"

namespace cytx
{
    class cmder_factory
    {
    public:
        static cmder_factory& ins()
        {
            static cmder_factory factory;
            return factory;
        }

        template<typename T>
        auto register_cmder(const char* name, const char* desc) -> std::enable_if_t<std::is_base_of<base_cmder, T>::value, add_options_helper>
        {
            auto cmder = instance_factory::ins().get<T>();
            icmder_manager::ins().register_cmder(name, cmder);

            cmder->set_parser([cmder](auto& de) {
                de.DeSerialize(*cmder);
            });

            cmder->set_init_parser([cmder](auto& de, auto& ops, auto pos_ops) {
                de.init<T>(*cmder, ops, pos_ops);
            });

            cmder->init_options(name, desc);
            return cmder->add_options();
        }
    };
}

#define REG_CMDER(type, name, desc) \
REG_TYPE(type); \
static int MACRO_CONCAT(__REGISTER_CMDER_VAL__ ## type, __LINE__) = (cytx::cmder_factory::ins().register_cmder<type>(name, desc))