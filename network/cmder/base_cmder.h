#pragma once
#include "icmder.h"
#include "network/serialize/deserializer.hpp"
#include "network/serialize/bpo_adapter.hpp"
#include "network/base/world.hpp"

namespace cytx
{
    using bpo_parser_t = DeSerializer<bpo_deserialize_adapter>;

    template<class T>
    bpo::typed_value<T>* value()
    {
        return bpo::value<T>(0);
    }

    template<class T>
    bpo::typed_value<T>* value(T* v)
    {
        bpo::typed_value<T>* r = new bpo::typed_value<T>(v);
        return r;
    }

    class base_cmder;
    class add_options_helper
    {
        friend class base_cmder;
    private:
        add_options_helper(bpo_parser_t& de, bpo_options_t* op, bpo_pos_options_t* pod)
            : de_(de)
            , op_(op)
            , pod_(pod)
        {

        }
    public:
        add_options_helper(const add_options_helper& other)
            : de_(other.de_)
            , op_(other.op_)
            , pod_(other.pod_)
        {

        }

        ~add_options_helper()
        {
            de_.init(*op_, pod_);
        }

        add_options_helper& operator()(const char* name, const char* desc)
        {
            boost::shared_ptr<bpo_option_t> d(new bpo_option_t(name, new bpo::untyped_value(true), desc));
            op_->add(d);
            return *this;
        }

        add_options_helper& operator()(const char* name, const bpo::value_semantic* s)
        {
            boost::shared_ptr<bpo_option_t> d(new bpo_option_t(name, s));
            op_->add(d);
            return *this;
        }

        add_options_helper& operator()(const char* name, const bpo::value_semantic* s, const char* desc)
        {
            boost::shared_ptr<bpo_option_t> d(new bpo_option_t(name, s, desc));
            op_->add(d);
            return *this;
        }

        add_options_helper& operator()(const char* name, const char* desc, int max_count)
        {
            set_pos(name, max_count);
            return operator()(name, desc);
        }

        add_options_helper& operator()(const char* name, const bpo::value_semantic* s, int max_count)
        {
            set_pos(name, max_count);
            return operator()(name, s);
        }

        add_options_helper& operator()(const char* name, const bpo::value_semantic* s, const char* desc, int max_count)
        {
            set_pos(name, max_count);
            return operator()(name, s, desc);
        }

        int end() const
        {
            return 0;
        }

    private:
        void set_pos(const char* name, int max_count)
        {
            pod_->add(name, max_count);
        }

    private:
        bpo_parser_t& de_;
        bpo_options_t* op_;
        bpo_pos_options_t* pod_;
    };


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