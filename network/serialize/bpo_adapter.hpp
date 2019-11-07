#pragma once
#include <boost/program_options.hpp>
#include "network/traits/traits.hpp"
#include "network/meta/meta.hpp"
#include "network/util/cast.hpp"

namespace cytx
{
    namespace bpo = boost::program_options;

    struct bpo_deserialize_adapter {};

    template<>
    class DeSerializer<bpo_deserialize_adapter, std::tuple<>> : boost::noncopyable
    {
        using val_t = boost::program_options::variable_value;
    public:
        DeSerializer()
        {
            pod_ptr_ = &pod_;
        }

        ~DeSerializer()
        {
        }

        bool enum_with_str() { return enum_with_str_; }
        void enum_with_str(bool val) { enum_with_str_ = val; }

        void init(const bpo::options_description& op, bpo::positional_options_description* pod_ptr = nullptr)
        {
            for (auto& o : op.options())
            {
                ops_.add(o);
            }

            if (pod_ptr)
            {
                pod_ptr_ = pod_ptr;
            }
        }

        void parse(size_t argc, const char* const argv[])
        {
            clear();
            bpo::store(bpo::command_line_parser((int)argc, argv).options(ops_).positional(*pod_ptr_).run(), vm_);
            bpo::notify(vm_);
        }

        void parse(const char* cmd_line)
        {
            clear();
#ifdef LINUX
            std::vector<std::string> args = bpo::split_unix(cmd_line);
#else
            std::vector<std::string> args = bpo::split_winmain(cmd_line);
#endif
            bpo::store(bpo::command_line_parser(args).options(ops_).positional(*pod_ptr_).run(), vm_);
            bpo::notify(vm_);
        }

        void parse(const std::vector<std::string>& args)
        {
            clear();
            bpo::store(bpo::command_line_parser(args).options(ops_).positional(*pod_ptr_).run(), vm_);
            bpo::notify(vm_);
        }

        void parse(size_t argc, const char* const argv[], const bpo::options_description& op, bpo::positional_options_description* pod_ptr = nullptr)
        {
            init(op, pod_ptr);
            parse(argc, argv);
        }

        void parse(const char* cmd_line, const bpo::options_description& op, bpo::positional_options_description* pod_ptr = nullptr)
        {
            init(op, pod_ptr);
            parse(cmd_line);
        }

        void parse(const std::vector<std::string>& args, const bpo::options_description& op, bpo::positional_options_description* pod_ptr = nullptr)
        {
            init(op, pod_ptr);
            parse(args);
        }

        template<typename T>
        auto DeSerialize(T& t, const std::string& key) -> std::enable_if_t<!is_user_class<T>::value>
        {
            DeSerialize(t, key.c_str());
        }

        template<typename T>
        auto DeSerialize(T& t, const char* key) -> std::enable_if_t<!is_user_class<T>::value>
        {
            if (vm_.count(key))
            {
                val_t val = vm_[key];
                ReadObject(t, val);
            }
        }

        void DeSerialize(bool& t, const char* key)
        {
            ReadBool(t, key);
        }

        template<typename T>
        auto DeSerialize(T& t) -> std::enable_if_t<is_user_class<T>::value>
        {
            auto meta_val = get_meta(t);
            for_each(meta_val, [this](auto& v, size_t I, bool is_last)
            {
                this->ReadTuplePair(v);
            });
        }

        bool empty() const
        {
            return vm_.empty();
        }

        const bpo::variables_map& vm() const { return vm_; }

    private:
        void clear()
        {
            vm_.clear();
        }

    private:
        template<typename T>
        auto ReadTuplePair(T& p) -> std::enable_if_t<!std::is_same<bool, std::decay_t<decltype(p.second)>>::value>
        {
            if (vm_.count(p.first))
            {
                val_t val = vm_[p.first];
                ReadObject(p.second, val);
            }
        }

        template<typename T>
        auto ReadTuplePair(T& p) -> std::enable_if_t<std::is_same<bool, std::decay_t<decltype(p.second)>>::value>
        {
            ReadBool(p.second, p.first);
        }

        void ReadBool(bool& v, const char* key)
        {
            auto op = ops_.find_nothrow(key, true);
            if (op == nullptr)
            {
                v = false;
                return;
            }
            auto typed_value_ptr = dynamic_cast<const bpo::typed_value<bool>*>(op->semantic().get());
            bool is_typed_value = typed_value_ptr != nullptr;

            if (!is_typed_value)
            {
                v = vm_.count(key) != 0;
            }
            else if (vm_.count(key))
            {
                val_t val = vm_[key];
                v = val.empty() ? false : val.as<bool>();
            }
        }

        template<typename T>
        auto ReadObject(T& t, val_t& val)->std::enable_if_t<!std::is_enum<std::decay_t<T>>::value
            && !std::is_same<T, bool>::value>
        {
            if (!val.empty())
                t = val.as<T>();
        }

        template <typename T>
        auto ReadObject(T& t, val_t& val) ->std::enable_if_t<std::is_enum<std::decay_t<T>>::value>
        {
            using enum_t = std::decay_t<T>;
            using under_type = std::underlying_type_t<enum_t>;

            if (!enum_with_str_)
            {
                t = (T)val.as<under_type>();
            }
            else
            {
                std::string str = val.as<std::string>();
                t = cytx::util::cast<enum_t>(str);
            }
        }

    private:
        bpo::variables_map vm_;
        bpo::options_description ops_;
        bpo::positional_options_description pod_;
        bpo::positional_options_description* pod_ptr_;
        bool enum_with_str_ = false;
    };
}
