﻿#pragma once
#include "icmder.h"
#include "icmder_manager.h"
#include "network/serialize/deserializer.hpp"
#include "network/serialize/bpo_adapter.hpp"

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

        add_options_helper& operator()(const char* name, int max_count)
        {
            pod_->add(name, max_count);
            return *this;
        }

        int end() const
        {
            return 1;
        }

    private:
        bpo_parser_t& de_;
        bpo_options_t* op_;
        bpo_pos_options_t* pod_;
    };

    class base_cmder : public icmder
    {
    public:
        using parser_func_t = std::function<void(bpo_parser_t&)>;

        base_cmder()
            : icmder()
        {
            de_.enum_with_str(true);
        }

        virtual void init_value() = 0;

        virtual void set_cmder_manager(icmder_manager_ptr manager)
        {
            manager_ = manager;
        }

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
            init_value();

            de_.parse(argc, argv);
            parser_func_(de_);

            return execute();
        }

        virtual int execute()
        {
            dump_help();
            return 1;
        }

    protected:
        icmder_manager_ptr manager_;
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
            auto cmder = std::make_shared<T>();
            icmder_manager::ins().register_cmder(name, cmder);

            cmder->set_parser([cmder](auto& de) {
                de.DeSerialize(*cmder);
            });
            cmder->set_cmder_manager(&icmder_manager::ins());
            cmder->init(name, desc);
            return cmder->add_options();
        }
    };

#define REGISTER_CMDER(type, name, desc) \
int __REGISTER_CMDER_VAL__ ## __LINE__ ## type = (cmder_factory::ins().register_cmder<type>(name, desc))
}