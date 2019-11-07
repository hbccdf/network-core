#pragma once
#include "cmder_helper.hpp"
#include "network/base/world.hpp"

namespace cytx
{
    class icmder_manager;
    class base_cmder : public icmder
    {
    public:
        using parser_func_t = std::function<void(bpo_parser_t&)>;

        base_cmder()
            : icmder()
        {
            de_.enum_with_str(true);
        }

        void set_world(world_ptr_t world_ptr)
        {
            world_ptr_ = world_ptr;

            manager_ = world_ptr_->get<icmder_manager>("icmder_mgr");
        }

    public:
        virtual void init()
        {

        }

        virtual void reset_value() = 0;

        virtual void set_parser(parser_func_t func)
        {
            parser_func_ = func;
        }

        virtual add_options_helper add_options()
        {
            return add_options_helper(de_, op_.get(), &pd_);
        }

        virtual int handle_input(int argc, const char* argv[]) override
        {
            reset_value();

            de_.parse(argc, argv);
            parser_func_(de_);

            return execute();
        }

        virtual int execute()
        {
            if (need_help())
            {
                show_help();
                return 0;
            }
            return process();
        }

        virtual int process() = 0;

    protected:
        bool need_help() const
        {
            return de_.vm().count("help") > 0;
        }

    protected:
        world_ptr_t world_ptr_ = nullptr;
        icmder_manager* manager_ = nullptr;
        bpo_parser_t de_;
        parser_func_t parser_func_;
    };

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

            cmder->init_options(name, desc);
            return cmder->add_options();
        }
    };

#define REG_CMDER(type, name, desc) \
static int __REGISTER_CMDER_VAL__ ## type ## __LINE__ = (cmder_factory::ins().register_cmder<type>(name, desc))
}