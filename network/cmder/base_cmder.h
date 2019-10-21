#pragma once
#include "icmder.h"
#include "icmder_manager.h"
#include "network/serialize/deserializer.hpp"
#include "network/serialize/bpo_adapter.hpp"

namespace cytx
{
    class base_cmder;
    class add_options_helper
    {
        friend class base_cmder;
    private:
        add_options_helper(const add_options_helper& other)
            : de_(other.de_)
            , op_(other.op_)
            , pod_(other.pod_)
        {

        }

        add_options_helper(DeSerializer<bpo_deserialize_adapter>& de, bpo::options_description* op, bpo::positional_options_description* pod)
            : de_(de)
            , op_(op)
            , pod_(pod)
        {

        }

        ~add_options_helper()
        {
            de_.init(*op_, pod_);
        }
    public:
        add_options_helper& operator()(const char* name, const char* desc)
        {
            boost::shared_ptr<bpo::option_description> d(name, new bpo::untyped_value(true), desc);
            op_->add(d);
            return *this;
        }

        add_options_helper& operator()(const char* name, const bpo::value_semantic* s)
        {
            boost::shared_ptr<bpo::option_description> d(name, s);
            op_->add(d);
            return *this;
        }

        add_options_helper& operator()(const char* name, const bpo::value_semantic* s, const char* desc)
        {
            boost::shared_ptr<bpo::option_description> d(name, s, desc);
            op_->add(d);
            return *this;
        }

        add_options_helper& operator()(const char* name, int max_count)
        {
            pod_->add(name, max_count);
            return *this;
        }

    private:
        DeSerializer<bpo_deserialize_adapter>& de_;
        bpo::options_description* op_;
        bpo::positional_options_description* pod_;
    };

    class base_cmder : public icmder
    {
    public:
        base_cmder()
            : icmder()
        {
            de_.enum_with_str(true);
        }
        virtual void set_cmder_manager(icmder_manager_ptr manager)
        {
            manager_ = manager;
        }

        virtual add_options_helper add_options()
        {
            return add_options_helper(de_, op_.get(), &pd_);
        }

        virtual bool handle_input(int argc, const char* argv[]) override
        {
            de_.parse(argc, argv);
            de_.DeSerialize(*this);

            return execute() == 0;
        }

        virtual int execute()
        {
            dump_help();
            return 1;
        }

    protected:
        icmder_manager_ptr manager_;
        DeSerializer<bpo_deserialize_adapter> de_;
    };
}