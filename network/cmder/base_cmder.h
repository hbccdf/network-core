#pragma once
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

    class icmder_parser
    {
    public:
        icmder_parser(bpo_parser_t& de)
            : de_(de)
            , cmder_(nullptr)
        {}

        void set_cmder(base_cmder* cmder)
        {
            cmder_ = cmder;
        }

        virtual void parse() = 0;

    protected:
        bpo_parser_t& de_;
        base_cmder* cmder_;
    };

    template<typename T>
    class icmder_meta_parser : public icmder_parser
    {
        using cmder_t = T;
    public:
        icmder_meta_parser(bpo_parser_t& de)
            : icmder_parser(de)
        {}

        virtual void parse() override
        {
            cmder_t* cmder_ptr = (cmder_t*)cmder_;
            de_.DeSerialize(*cmder_ptr);
        }

    };

    class base_cmder : public icmder
    {
    public:
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

        virtual void set_parser(icmder_parser* parser)
        {
            parser_ = parser;
        }

        virtual add_options_helper add_options()
        {
            return add_options_helper(de_, op_.get(), &pd_);
        }

        virtual int handle_input(int argc, const char* argv[]) override
        {
            init_value();

            de_.parse(argc, argv);
            parser_->parse();
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
        icmder_parser* parser_;
    };

    template<typename T>
    class base_meta_cmder : public base_cmder
    {
        using cmder_t = T;
        using parser_t = icmder_meta_parser<cmder_t>;
    public:
        base_meta_cmder()
            : base_cmder()
            , parser_(de_)
        {
            parser_.set_cmder(this);
            set_parser(&parser_);
        }

    protected:
        parser_t parser_;
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

            cmder->set_cmder_manager(&icmder_manager::ins());
            cmder->init(name, desc);
            return cmder->add_options();
        }
    };

#define REGISTER_CMDER(type, name, desc) \
int __REGISTER_CMDER_VAL__ ## __LINE__ ## type = (cmder_factory::ins().register_cmder<type>(name, desc))
}