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
            manager_ = world_ptr_["icmder_mgr"];
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

        virtual void set_init_parser(init_parser_func_t func)
        {
            init_parser_func_ = func;
        }

        virtual add_options_helper add_options()
        {
            return add_options_helper(de_, op_.get(), &pd_, init_parser_func_);
        }

        virtual int handle_input(const std::vector<std::string>& args) override
        {
            reset_value();

            de_.parse(args);
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

            before_process();
            int ret = process();
            after_process();

            return ret;
        }

        virtual void before_process()
        {
        }

        virtual int process() = 0;

        virtual void after_process()
        {
        }

        virtual void notify_waiting_result(int result)
        {

        }

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
        init_parser_func_t init_parser_func_;
    };
}