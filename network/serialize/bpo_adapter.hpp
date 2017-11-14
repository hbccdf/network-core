#pragma once
#include <boost/program_options.hpp>
#include "../traits/traits.hpp"
#include "../meta/meta.hpp"
#include "../base/cast.hpp"
#include "deserializer.hpp"

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
        }

        ~DeSerializer()
        {
        }

        bool enum_with_str() { return enum_with_str_; }
        void enum_with_str(bool val) { enum_with_str_ = val; }

        void init(const bpo::options_description& op)
        {
            for (auto& o : op.options())
            {
                ops_.add(o);
            }
        }

        void parse(size_t argc, const char* const argv[])
        {
            vm_.clear();
            bpo::store(bpo::parse_command_line((int)argc, argv, ops_), vm_);
            bpo::notify(vm_);
        }

        void parse(const char* cmd_line)
        {
            vm_.clear();
            std::vector<std::string> args = bpo::split_winmain(cmd_line);
            bpo::store(bpo::command_line_parser(args).options(ops_).run(), vm_);
            bpo::notify(vm_);
        }

        void parse(const std::vector<std::string>& args)
        {
            vm_.clear();
            bpo::store(bpo::command_line_parser(args).options(ops_).run(), vm_);
            bpo::notify(vm_);
        }

        void parse(size_t argc, const char* const argv[], const bpo::options_description& op)
        {
            vm_.clear();
            for (auto& o : op.options())
            {
                ops_.add(o);
            }
            bpo::store(bpo::parse_command_line((int)argc, argv, ops_), vm_);
            bpo::notify(vm_);
        }

        void parse(const char* cmd_line, const bpo::options_description& op)
        {
            vm_.clear();
            for (auto& o : op.options())
            {
                ops_.add(o);
            }
            std::vector<std::string> args = bpo::split_winmain(cmd_line);
            bpo::store(bpo::command_line_parser(args).options(ops_).run(), vm_);
            bpo::notify(vm_);
        }

        void parse(const std::vector<std::string>& args, const bpo::options_description& op)
        {
            vm_.clear();
            for (auto& o : op.options())
            {
                ops_.add(o);
            }
            bpo::store(bpo::command_line_parser(args).options(ops_).run(), vm_);
            bpo::notify(vm_);
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
            for_each(get_meta(t), [this](auto& v, size_t I, bool is_last)
            {
                ReadTuplePair(v);
            });
        }

        bool empty()
        {
            return vm_.empty();
        }

        const bpo::variables_map& vm() const { return vm_; }

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
                return;
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
        auto ReadObject(T& t, val_t& val)->std::enable_if_t<!std::is_enum<std::remove_reference_t<std::remove_cv_t<T>>>::value
            && !std::is_same<T, bool>::value>
        {
            if (!val.empty())
                t = val.as<T>();
        }

        template <typename T>
        auto ReadObject(T& t, val_t& val) ->std::enable_if_t<std::is_enum<
            std::remove_reference_t<std::remove_cv_t<T>>>::value>
        {
            using enum_t = std::remove_reference_t<std::remove_cv_t<T>>;
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
        bool enum_with_str_ = false;
    };
}
