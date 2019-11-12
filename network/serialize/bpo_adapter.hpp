#pragma once
#include <boost/program_options.hpp>
#include "network/traits/traits.hpp"
#include "network/meta/meta.hpp"
#include "network/util/cast.hpp"
#include "network/util/string.hpp"

namespace boost
{
    namespace program_options
    {
        template<class T>
        auto validate(boost::any& v, const std::vector<std::string>& xs, T*, long) -> std::enable_if_t<std::is_enum<std::decay_t<T>>::value>
        {
            validators::check_first_occurrence(v);
            std::string s(validators::get_single_string(xs));
            try
            {
                //cast only verify the string is valid enum string
                //the val still storage string
                T t = cytx::util::cast<T>(s);
                v = any(s);
            }
            catch (const bad_lexical_cast&) {
                boost::throw_exception(invalid_option_value(s));
            }
        }
    }
}

namespace cytx
{
    namespace bpo = boost::program_options;

    using bpo_option_t = bpo::option_description;
    using bpo_options_t = bpo::options_description;
    using bpo_pos_options_t = bpo::positional_options_description;

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

    struct bpo_deserialize_adapter {};

    template<>
    class DeSerializer<bpo_deserialize_adapter, std::tuple<>> : public BaseDeSerializer
    {
        using val_t = boost::program_options::variable_value;
        using bpo_option_ptr_t = boost::shared_ptr<bpo_option_t>;
    public:
        DeSerializer()
        {
            pod_ptr_ = &pod_;
        }

        ~DeSerializer()
        {
        }

        void init(const bpo_options_t& op, bpo_pos_options_t* pod_ptr = nullptr)
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

        template<typename T>
        auto init(T& t, const bpo_options_t& op, bpo_pos_options_t* pod_ptr = nullptr) -> std::enable_if_t<is_user_class<T>::value>
        {
            std::unordered_map<std::string, bpo_option_ptr_t> option_map;

            for (auto& p : op.options())
            {
                option_map.emplace(p->long_name(), p);
            }

            auto meta_val = get_meta(t);
            for_each(meta_val, [this, &option_map](auto& v, size_t I, bool is_last)
            {
                auto it = option_map.find(v.first);
                if (it == option_map.end())
                    return;

                auto option_ptr = it->second;

                auto untyped_value_ptr = dynamic_cast<const bpo::untyped_value*>(option_ptr->semantic().get());
                bool is_untyped_value = untyped_value_ptr != nullptr;
                if (!is_untyped_value)
                    return;

                using val_t = std::decay_t<decltype(v.second)>;
                ops_.add(get_option_ptr<val_t>(option_ptr));

                option_map.erase(it);
            });

            for (auto& p : option_map)
            {
                ops_.add(p.second);
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

        void parse(size_t argc, const char* const argv[], const bpo_options_t& op, bpo_pos_options_t* pod_ptr = nullptr)
        {
            init(op, pod_ptr);
            parse(argc, argv);
        }

        void parse(const char* cmd_line, const bpo_options_t& op, bpo_pos_options_t* pod_ptr = nullptr)
        {
            init(op, pod_ptr);
            parse(cmd_line);
        }

        void parse(const std::vector<std::string>& args, const bpo_options_t& op, bpo_pos_options_t* pod_ptr = nullptr)
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
        auto ReadObject(T& t, val_t& val) -> std::enable_if_t<is_container<T>::value>
        {
            if (val.empty())
                return;

            std::vector<std::string> strs = val.as<std::vector<std::string>>();
            std::vector<std::string> vals = process_array(strs);

            ReadContainer(t, vals);
        }

        template<typename T>
        auto ReadObject(T& t, val_t& val) -> std::enable_if_t<is_pair<T>::value>
        {
            if (val.empty())
                return;

            std::string vals = val.as<std::string>();
            ReadContainerElement(t, vals);
        }

        template<typename T>
        auto ReadObject(T& t, val_t& val)->std::enable_if_t<is_basic_type<T>::value
            && !std::is_same<T, bool>::value>
        {
            if (!val.empty())
                t = val.as<std::decay_t<T>>();
        }

        template <typename T>
        auto ReadObject(T& t, val_t& val) ->std::enable_if_t<std::is_enum<T>::value>
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

        template<typename T>
        auto ReadContainer(T& t, std::vector<std::string>& vals) -> std::enable_if_t<has_only_insert<T>::value>
        {
            using element_t = decltype(*t.begin());
            using ele_t = std::decay_t<element_t>;
            for (auto& str : vals)
            {
                ele_t el{};
                ReadContainerElement(el, str);
                std::fill_n(std::inserter(t, t.end()), 1, std::move(el));
            }
        }

        template<typename T>
        auto ReadContainer(T& t, std::vector<std::string>& vals) -> std::enable_if_t<has_back_insert<T>::value>
        {
            using element_t = decltype(*t.begin());
            using ele_t = std::decay_t<element_t>;
            for (auto& str : vals)
            {
                ele_t el{};
                ReadContainerElement(el, str);
                std::fill_n(std::back_inserter(t), 1, std::move(el));
            }
        }

        template<typename T>
        auto ReadContainer(T& t, std::vector<std::string>& vals) -> std::enable_if_t<is_map_container<T>::value>
        {
            using element_t = decltype(*t.begin());
            using pair_t = std::decay_t<element_t>;
            using first_type = std::decay_t<typename pair_t::first_type>;
            using second_type = typename pair_t::second_type;

            using ele_t = std::pair<first_type, second_type>;

            for (auto& str : vals)
            {
                ele_t el{};
                ReadContainerElement(el, str);
                t[el.first] = el.second;
            }
        }

        template<typename T>
        auto ReadContainerElement(T& t, const std::string& val) -> std::enable_if_t<is_basic_type<T>::value>
        {
            t = util::cast<T>(val);
        }

        template<typename T>
        auto ReadContainerElement(T& t, const std::string& val) -> std::enable_if_t<is_pair<T>::value>
        {
            std::vector<std::string> list = string_util::split(val, "=");
            ReadContainerElement(t.first, list[0]);
            ReadContainerElement(t.second, list[1]);
        }

    private:
        template<typename T>
        auto get_option_ptr(bpo_option_ptr_t option_ptr) const -> std::enable_if_t<std::is_same<bool, T>::value, bpo_option_ptr_t>
        {
            return option_ptr;
        }

        template<typename T>
        auto get_option_ptr(bpo_option_ptr_t option_ptr) const -> std::enable_if_t<!std::is_same<bool, T>::value &&
            (is_basic_type<T>::value || std::is_enum<T>::value), bpo_option_ptr_t>
        {

            bpo_option_ptr_t d(new bpo_option_t(get_option_name(option_ptr).c_str(), value<T>(), option_ptr->description().c_str()));
            return d;
        }

        template<typename T>
        auto get_option_ptr(bpo_option_ptr_t option_ptr) const -> std::enable_if_t<is_container<T>::value, bpo_option_ptr_t>
        {
            bpo_option_ptr_t d(new bpo_option_t(get_option_name(option_ptr).c_str(), value<std::vector<std::string>>(), option_ptr->description().c_str()));
            return d;
        }

        template<typename T>
        auto get_option_ptr(bpo_option_ptr_t option_ptr) const -> std::enable_if_t<is_pair<T>::value, bpo_option_ptr_t>
        {
            bpo_option_ptr_t d(new bpo_option_t(get_option_name(option_ptr).c_str(), value<std::string>(), option_ptr->description().c_str()));
            return d;
        }

        std::string get_option_name(bpo_option_ptr_t option_ptr) const
        {
            std::string option_format_name = option_ptr->format_name();
            char short_name = option_format_name[1] == '-' ? (char)0 : option_format_name[1];

            fmt::MemoryWriter mr;
            mr << option_ptr->long_name();
            if (short_name != (char)0)
            {
                mr << "," << short_name;
            }
            return mr.str();
        }

        std::vector<std::string> process_array(std::vector<std::string>& strs)
        {
            std::vector<std::string> vals;
            for (auto& str : strs)
            {
                auto strs = string_util::split(str, ",;");
                std::copy(strs.begin(), strs.end(), std::back_inserter(vals));
            }
            return vals;
        }

    private:
        bpo::variables_map vm_;
        bpo_options_t ops_;
        bpo_pos_options_t pod_;
        bpo_pos_options_t* pod_ptr_;
    };
}
