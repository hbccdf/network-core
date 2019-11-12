#pragma once
#include "icmder.hpp"

namespace cytx
{
    using bpo_parser_t = DeSerializer<bpo_deserialize_adapter>;

    using init_parser_func_t = std::function<void(bpo_parser_t&, bpo_options_t&, bpo_pos_options_t*)>;

    class base_cmder;

    template<typename T>
    class base_options_helper
    {
        using real_t = T;
        friend class base_cmder;
    public:
        base_options_helper(bpo_parser_t* de, bpo_options_t* op, bpo_pos_options_t* pod)
            : de_(de)
            , op_(op)
            , pod_(pod)
        {

        }

        base_options_helper(base_options_helper&& other)
            : de_(other.de_)
            , op_(other.op)
            , pod_(other.pod)
        {
        }

        real_t& operator()(const char* name, const char* desc)
        {
            boost::shared_ptr<bpo_option_t> d(new bpo_option_t(name, new bpo::untyped_value(true), desc));
            op_->add(d);
            return get_real_value();
        }

        real_t& operator()(const char* name, const bpo::value_semantic* s)
        {
            boost::shared_ptr<bpo_option_t> d(new bpo_option_t(name, s));
            op_->add(d);
            return get_real_value();
        }

        real_t& operator()(const char* name, const bpo::value_semantic* s, const char* desc)
        {
            boost::shared_ptr<bpo_option_t> d(new bpo_option_t(name, s, desc));
            op_->add(d);
            return get_real_value();
        }

        real_t& operator()(const char* name, const char* desc, int max_count)
        {
            set_pos(name, max_count);
            return operator()(name, desc);
        }

        real_t& operator()(const char* name, const bpo::value_semantic* s, int max_count)
        {
            set_pos(name, max_count);
            return operator()(name, s);
        }

        real_t& operator()(const char* name, const bpo::value_semantic* s, const char* desc, int max_count)
        {
            set_pos(name, max_count);
            return operator()(name, s, desc);
        }

    protected:
        void set_pos(const char* name, int max_count)
        {
            pod_->add(name, max_count);
        }

        T& get_real_value()
        {
            return *reinterpret_cast<T*>(this);
        }

    protected:
        bpo_parser_t* de_;
        bpo_options_t* op_;
        bpo_pos_options_t* pod_;
    };

    class add_options_helper : public base_options_helper<add_options_helper>
    {
        friend class base_cmder;
        using base_t = base_options_helper<add_options_helper>;
    private:
        add_options_helper(bpo_parser_t& de, bpo_options_t* op, bpo_pos_options_t* pod, init_parser_func_t func)
            : base_t(&de, op, pod)
            , func_(func)
        {

        }
    public:
        add_options_helper(add_options_helper&& other)
            : base_t(other.de_, other.op_, other.pod_)
            , func_(std::move(other.func_))
        {

        }

        ~add_options_helper()
        {
            func_(*de_, *op_, pod_);
        }

        int end() const
        {
            return 0;
        }

    private:
        init_parser_func_t func_;
    };

    class cmder_helper : public base_options_helper<cmder_helper>
    {
        using base_t = base_options_helper<cmder_helper>;
    public:
        cmder_helper(const std::string& name, const std::string& desc)
            : base_t(&de_, &op_, &pod_)
            , name_(name)
            , desc_(desc)
        {
            de_.enum_with_str(true);
        }

        cmder_helper(cmder_helper&& other)
            : base_t(&de_, &op_, &pod_)
            , name_(std::move(other.name_))
            , desc_(std::move(other.desc_))
            , op_(std::move(other.op_))
            , pod_(std::move(other.pod_))
        {}

    public:
        template<typename T>
        T parse(int argc, char* argv[])
        {
            T t;

            de_.init<T>(t, op_, &pod_);
            de_.parse(argc, argv);
            de_.DeSerialize(t);

            return t;
        }

    private:
        bpo_parser_t de_;
        std::string name_;
        std::string desc_;
        bpo_options_t op_;
        bpo_pos_options_t pod_;
    };

    inline cmder_helper get_cmder_helper(const std::string& name, const std::string desc)
    {
        return cmder_helper(name, desc);
    }

    inline cmder_helper get_cmder_helper(const std::string& name)
    {
        return get_cmder_helper(name, "");
    }
}