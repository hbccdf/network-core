#pragma once
#include "../traits/traits.hpp"
#include "../meta/meta.hpp"
#include "json_util.hpp"
#include <boost/property_tree/ptree.hpp>
#include "parser.hpp"
#include <boost/program_options.hpp>
#include "../base/cast.hpp"

namespace cytx
{
    using namespace boost::property_tree;
    using namespace std;

    template<typename T>
    class base_deserialize_adapter
    {
    public:
        typedef ptree value_t;
        typedef ptree::iterator array_iterator;
        typedef ptree::iterator member_iterator;

        base_deserialize_adapter() {}
        void parse(const char* str) { parse(string(str)); }
        void parse(const char* str, size_t len) { parse(string(str, len)); }
        void parse(const std::string& str)
        {
            stringstream ss;
            ss << str;
            T::read(ss, pt_);
            init();
        }

        void parse_file(const std::string& file)
        {
            T::read_file(file, pt_);
            init();
        }

        value_t& get_root(const char* key, bool has_root = true)
        {
            if (!has_root || key == nullptr)
            {
                if (pt_.size() > 1)
                    return pt_;
                else
                    return pt_.get_child("+");
            }

            auto child = pt_.get_child_optional(key);
            if(!child)
                throw std::invalid_argument(fmt::format("the key \"{}\" is not exist", key));

            return child.get();
        }

        size_t array_size(value_t& val) { return val.size(); }
        array_iterator array_begin(value_t& val) { return val.begin(); }
        array_iterator array_end(value_t& val) { return val.end(); }
        value_t& it_val(array_iterator it) { return it->second; }
        array_iterator array_it(value_t& val, size_t index)
        {
            auto it = array_begin(val);
            while (index > 0)
            {
                ++it;
                --index;
            }
            return it;
        }

        size_t member_size(value_t& val) { return val.size(); }
        member_iterator member_begin(value_t& val) { return val.begin(); }
        member_iterator member_end(value_t& val) { return val.end(); }
        member_iterator member_it(value_t& val, size_t index)
        {
            auto it = member_begin(val);
            while (index > 0)
            {
                ++it;
                --index;
            }
            return it;
        }

        bool has_member(const char* key, value_t& val) { return val.find(key) != val.not_found(); }
        member_iterator get_member(const char* key, value_t& val)
        {
            auto it = val.find(key);
            if (it == val.not_found())
            {
                auto attr = val.get_child_optional("<xmlattr>");
                if (attr)
                {
                    auto it_key = attr->find(key);
                    if(it_key != attr->not_found())
                        return attr->to_iterator(attr->find(key));
                }
            }
            return val.to_iterator(it);
        }

        template<typename T>
        void read(T& t, value_t& val)
        {
            auto& str = val.data();
            auto pos = val.data().find("$(");
            if (pos != std::string::npos)
            {
                auto end_pos = str.find(")", pos);
                if (end_pos != std::string::npos)
                {
                    std::string str_prop, new_str;
                    auto str_val = str.substr(pos + 2, end_pos - (pos + 2));
                    auto it = properties_.find(str_val);
                    if (it != properties_.end())
                    {
                        str_prop = it->second;
                        new_str = str;
                        new_str.replace(pos, end_pos - pos + 1, str_prop.c_str());
                    }
                    t = util::cast<T>(new_str);
                }
            }
            else
                t = val.get_value<T>();
        }

        std::string first(member_iterator& it)
        {
            if(it->first != "@key")
                return it->first;
            auto name = (it->second).get_optional<std::string>("<xmlattr>.name");
            if (name)
                return name.value();
            return it->first;
        }

        value_t& second(member_iterator& it)
        {
            return it->second;
        }

        void process_array(value_t& val)
        {
            if (val.data().empty())
                return;

            std::string str_val = val.data();
            vector<std::string> values;
            boost::algorithm::split(values, str_val, boost::is_any_of(",;"), boost::algorithm::token_compress_on);
            if (values.empty())
                return;

            val.put_value("");
            for (auto& s : values)
            {
                val.add("+", s);
            }
        }

    private:
        void init()
        {
            if (pt_.empty())
                return;

            parse_using(pt_);

            auto& tmp_pt = pt_.begin()->second;
            for (auto it = tmp_pt.begin(); it != tmp_pt.end();)
            {
                auto& p = *it;
                if (p.first == "property_group")
                {
                    for (auto& prop : p.second)
                    {
                        read(properties_[prop.first], prop.second);
                    }
                }
                else if (p.first == "import")
                {
                    auto& import_pt = p.second;
                    ptree other_pt;
                    auto mb_it = get_member("path", import_pt);
                    if (mb_it != member_end(import_pt))
                    {
                        std::string path;
                        read<std::string>(path, second(mb_it));
                        T::read_file(path, other_pt);
                        it = tmp_pt.erase(it);
                        for (auto& op : other_pt.begin()->second)
                        {
                            it = tmp_pt.push_back(op);
                        }
                        continue;
                    }
                }
                ++it;
            }
        }

        void parse_using(ptree& pt)
        {
            if (pt.empty())
                return;

            auto& tmp_pt = pt;
            for (auto it = tmp_pt.begin(); it != tmp_pt.end();)
            {
                auto& p = *it;
                if (p.first == "using")
                {
                    auto& using_pt = p.second;
                    auto mb_it = get_member("node", using_pt);
                    if (mb_it != member_end(using_pt))
                    {
                        std::string node_path;
                        read<std::string>(node_path, second(mb_it));
                        auto other_pt = pt_.get_child_optional(node_path);
                        if (other_pt)
                        {
                            it = tmp_pt.erase(it);
                            for (auto& op : other_pt.get())
                            {
                                tmp_pt.add_child(fmt::format("<xmlattr>.{}", op.first), op.second);
                            }
                            continue;
                        }
                    }
                    ++it;
                }
                else if (p.first == "<xmlattr>")
                {
                    auto node_path_optional = p.second.get_optional<std::string>("using");
                    if (node_path_optional)
                    {
                        auto other_pt = pt_.get_child_optional(node_path_optional.get());
                        if (other_pt)
                        {
                            p.second.erase("using");
                            for (auto& op : other_pt.get())
                            {
                                p.second.push_back(op);
                            }
                        }
                    }
                    ++it;
                }
                else
                {
                    parse_using(p.second);
                    ++it;
                }
            }
        }

        void debug(const ptree& pt, bool debug_pt = true)
        {
            std::stringstream ss;
            T::write(ss, pt);
            std::string str = ss.str();

            if (debug_pt)
                debug(pt_, false);
        }

        void debug()
        {
            debug(pt_, false);
        }
    protected:
        ptree pt_;
        std::map<std::string, std::string> properties_;
    };

    using xml_deserialize_adapter = base_deserialize_adapter<xml_parser>;
    using ini_deserialize_adapter = base_deserialize_adapter<ini_parser>;
    using info_deserialize_adapter = base_deserialize_adapter<info_parser>;
}

namespace cytx {

    class json_deserialize_adapter
    {
    public:
        typedef json_util::value_type value_t;
        typedef json_util::array_iterator array_iterator;
        typedef json_util::member_iterator member_iterator;

        json_deserialize_adapter(){}
        void parse(const char* str) { json_.parse(str); }
        void parse(const char* str, size_t len) { json_.parse(str, len); }
        void parse(const std::string& str) { json_.parse(str.c_str(), str.length()); }

        value_t& get_root(const char* key, bool has_root = true)
        {
            auto& doc = json_.get_doc();
            if (!has_root)
                return doc;

            if (key == nullptr)
            {
                auto it = doc.MemberBegin();
                return (rapidjson::Value&)it->value;
            }

            if (!doc.HasMember(key))
                throw std::invalid_argument(fmt::format("the key \"{}\" is not exist", key));

            return doc[key];
        }

        size_t array_size(value_t& val) { return val.Size(); }
        array_iterator array_begin(value_t& val) { return val.Begin(); }
        array_iterator array_end(value_t& val) { return val.End(); }
        value_t& it_val(array_iterator it) { return *it; }
        array_iterator array_it(value_t& val, size_t index)
        {
            return array_begin(val) + index;
        }

        size_t member_size(value_t& val) { return val.MemberCount(); }
        member_iterator member_begin(value_t& val) { return val.MemberBegin(); }
        member_iterator member_end(value_t& val) { return val.MemberEnd(); }
        member_iterator member_it(value_t& val, size_t index)
        {
            return member_begin(val) + index;
        }

        bool has_member(const char* key, value_t& val) { return val.HasMember(key); }
        member_iterator get_member(const char* key, value_t& val) { return val.FindMember(key); }

        template<typename T>
        void read(T& t, value_t& val)
        {
            json_.read_value(t, val);
        }

        std::string first(member_iterator& it)
        {
            return it->name.GetString();
        }

        value_t& second(member_iterator& it)
        {
            return it->value;
        }


    public:
        json_util json_;
    };

    template<typename ADAPTER_T, typename OtherTuple = std::tuple<>>
    class DeSerializer : boost::noncopyable
    {
        typedef ADAPTER_T adapter_t;
        typedef typename adapter_t::value_t val_t;
    public:

        template<typename... ARGS>
        DeSerializer(ARGS&&... args)
            : rd_(std::forward<ARGS>(args)...)
        {
        }

        template<typename... ARGS>
        DeSerializer(OtherTuple&& t, ARGS&&... args)
            : tuple(std::move(t))
            , rd_(std::forward<ARGS>(args)...)
        {
        }

        ~DeSerializer()
        {
        }

        bool enum_with_str() { return enum_with_str_; }
        void enum_with_str(bool val) { enum_with_str_ = val; }

        adapter_t& get_adapter() { return rd_; }

        void set_tuple(OtherTuple&& t)
        {
            tuple = std::move(t);
        }

        template<typename ... ARGS>
        void parse(ARGS&&... args)
        {
            rd_.parse(std::forward<ARGS>(args)...);
        }

        template<typename ... ARGS>
        void parse_file(ARGS&&... args)
        {
            rd_.parse_file(std::forward<ARGS>(args)...);
        }

        template<typename T>
        void DeSerialize(T& t, const std::string& key, bool has_root = true)
        {
            DeSerialize(t, key.c_str(), has_root);
        }

        template<typename T>
        void DeSerialize(T& t, const char* key, bool has_root = true)
        {
            val_t& val = rd_.get_root(key, has_root);
            ReadObject(t, val, std::true_type{});
        }

        template<typename T>
        void DeSerialize(T& t)
        {
            val_t& val = rd_.get_root(nullptr, false);
            ReadObject(t, val, std::true_type{});
        }

        template<typename T>
        auto GetTuple() -> std::enable_if_t< 0 < std::tuple_size<T>::value, T>
        {
            size_t index = 0;
            val_t& val = rd_.get_root(nullptr, false);
            return make<T, 0>(std::true_type{}, std::tuple<>{}, index, val);
        }

        template<typename T>
        auto GetTuple()->std::enable_if_t < 0 == std::tuple_size<T>::value, T >
        {
            return std::tuple<>{};
        }

    private:
        template <typename T, size_t Is, typename ReturnT>
        inline auto make(std::true_type, ReturnT&& t, size_t& index, val_t& val)
        {
            using value_type = std::conditional_t<(Is + 1) == std::tuple_size<T>::value, std::false_type, std::true_type>;
            using elem_t = std::tuple_element_t<Is, T>;
            return make<T, Is + 1>(value_type{}, std::tuple_cat(t, std::tuple<elem_t>(get_tuple_elem<elem_t>(index, val))), index, val);
        }

        template <typename T, size_t Is, typename ReturnT>
        inline auto make(std::false_type, ReturnT&& t, size_t& index, val_t& val)
        {
            return t;
        }

        template<typename T>
        auto get_tuple_elem(size_t& index, val_t& val) -> std::enable_if_t<tuple_contains<T, OtherTuple>::value, T>
        {
            return std::get<T>(tuple);
        }

        template<typename T>
        auto get_tuple_elem(size_t& index, val_t& val) -> std::enable_if_t<!tuple_contains<T, OtherTuple>::value, T>
        {
            if (index == 0)
                process_array<T>(val);

            T t;
            auto array_size = rd_.array_size(val);
            if (array_size <= index)
                return std::move(t);

            auto& ele_val = rd_.it_val(rd_.array_it(val, index));
            ReadObject(t, ele_val, std::true_type{});
            ++index;
            return std::move(t);
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, val_t& val, BeginObjec) -> std::enable_if_t<is_user_class<T>::value>
        {
            ReadTuple(t.Meta(), val, 0, std::false_type{});
        }

        template<typename T, typename BeginObject>
        auto ReadObject(T& t, val_t& val, BeginObject bo) -> std::enable_if_t<is_map_container<T>::value>
        {
            auto it = rd_.member_begin(val);
            auto it_end = rd_.member_end(val);
            for (; it != it_end; ++it)
            {
                typedef decltype(*t.begin()) element_t;
                using pair_t = std::remove_cv_t<std::remove_reference_t<element_t>>;
                using first_type = std::remove_cv_t<typename pair_t::first_type>;
                using second_type = typename pair_t::second_type;

                first_type f = boost::lexical_cast<first_type>(rd_.first(it));
                second_type s;
                ReadObject(s, rd_.second(it), bo);
                t[f] = s;
            }
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, val_t& val, BeginObjec) -> std::enable_if_t<has_only_insert<T>::value>
        {
            typedef decltype(*t.begin()) element_t;
            using ele_t = std::remove_cv_t<std::remove_reference_t<element_t>>;

            process_array<ele_t>(val);
            auto it = rd_.array_begin(val);
            auto it_end = rd_.array_end(val);
            for (; it != it_end; ++it)
            {
                ele_t el;
                ReadObject(el, rd_.it_val(it), std::false_type{});
                std::fill_n(std::inserter(t, t.end()), 1, std::move(el));
            }
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, val_t& val, BeginObjec) -> std::enable_if_t<has_back_insert<T>::value>
        {
            typedef decltype(*t.begin()) element_t;
            using ele_t = std::remove_reference_t<element_t>;

            process_array<ele_t>(val);
            auto it = rd_.array_begin(val);
            auto it_end = rd_.array_end(val);
            for (; it != it_end; ++it)
            {

                ele_t el;
                ReadObject(el, rd_.it_val(it), std::false_type{});
                std::fill_n(std::back_inserter(t), 1, std::move(el));
            }
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, val_t& val, BeginObjec) -> std::enable_if_t<is_singlevalue_container<T>::value && !has_back_insert<T>::value && !has_only_insert<T>::value>
        {
            ReadArray(t, val);
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, val_t& val, BeginObjec bo) -> std::enable_if_t<is_tuple<T>::value>
        {
            ReadTuple(t, val, 0, bo);
        }

        template<std::size_t I = 0, typename Tuple, typename BeginObjec>
        auto ReadTuple(Tuple& t, val_t& val, size_t index, BeginObjec) -> std::enable_if_t<I == std::tuple_size<Tuple>::value>
        {
        }

        template<std::size_t I = 0, typename Tuple>
        auto ReadTuple(Tuple& t, val_t& val, size_t, std::false_type bo)->std::enable_if_t < I < std::tuple_size<Tuple>::value>
        {
            auto mb_size = rd_.member_size(val);
            if (mb_size <= 0)
                return;
            auto it = rd_.member_it(val, I);
            ReadObject(std::get<I>(t), val, bo);
            ReadTuple<I + 1>(t, val, 0, bo);
        }

        template<std::size_t I = 0, typename Tuple>
        auto ReadTuple(Tuple& t, val_t& val, size_t index, std::true_type bo)->std::enable_if_t < I < std::tuple_size<Tuple>::value>
        {
            ReadTupleVal(std::get<I>(t), val, index);
            ReadTuple<I + 1>(t, val, index, bo);
        }

        template<typename T>
        auto ReadTupleVal(T& t, val_t& val, size_t& index) -> std::enable_if_t<tuple_contains<T, OtherTuple>::value>
        {
            t = std::get<T>(tuple);
        }

        template<typename T>
        auto ReadTupleVal(T& t, val_t& val, size_t& index) -> std::enable_if_t<!tuple_contains<T, OtherTuple>::value>
        {
            if (index == 0)
                process_array<T>(val);

            auto array_size = rd_.array_size(val);
            if (array_size <= index)
                return;
            auto& ele_val = rd_.it_val(rd_.array_it(val, index));
            ReadObject(t, ele_val, std::true_type{});
            ++index;
        }

        template<typename T>
        auto ReadObject(T& t, val_t& val, std::true_type bo) -> std::enable_if_t<is_pair<T>::value>
        {
            using pair_t = std::remove_cv_t<std::remove_reference_t<T>>;
            using first_type = typename pair_t::first_type;
            using second_type = typename pair_t::second_type;

            auto size = rd_.member_size(val);
            if (size != 1)
                throw std::invalid_argument(fmt::format("member count {} error", size));

            auto it = rd_.member_begin(val);
            t.first = boost::lexical_cast<first_type>(rd_.first(it));
            ReadObject(t.second, rd_.second(it), bo);
        }

        template<typename T>
        auto ReadObject(T& t, val_t& val, std::false_type) -> std::enable_if_t<is_pair<T>::value>
        {
            auto it = rd_.get_member(t.first, val);
            if(it != rd_.member_end(val))
                ReadObject(t.second, rd_.second(it), std::true_type{});
        }

        template <typename T, size_t N, typename BeginObject>
        void ReadObject(T(&p)[N], val_t& val, BeginObject)
        {
            ReadFixedLengthArray(p, N, val);
        }

        template<typename T, size_t N, typename BeginObject>
        void ReadObject(std::array<T, N>& t, val_t& val, BeginObject)
        {
            ReadFixedLengthArray(t, t.size(), val);
        }

        template <size_t N, typename BeginObject>
        void ReadObject(char(&p)[N], val_t& val, BeginObject)
        {
            std::string str;
            rd_.read(str, val);
            str.copy(p, N);
            p[str.length() >= N ? N - 1 : str.length()] = 0;
        }

        template<typename Array>
        inline void ReadArray(Array & t, val_t& val)
        {
            typedef decltype((t)[0]) element_t;
            using ele_t = std::remove_reference_t<element_t>;

            process_array<ele_t>(val);
            auto it = rd_.array_begin(val);
            auto it_end = rd_.array_end(val);
            for (; it != it_end; ++it)
            {

                ele_t el;
                ReadObject(el, rd_.it_val(it), std::false_type{});
                t.emplace_back(el);
            }
        }

        template<typename Array>
        inline void ReadFixedLengthArray(Array & v, size_t array_size, val_t& val)
        {
            process_array<decltype((v)[0])>(val);
            auto it = rd_.array_begin(val);
            auto it_end = rd_.array_end(val);
            for (size_t i = 0; i < array_size && it != it_end; ++i, ++it)
            {
                ReadObject(v[i], rd_.it_val(it), std::false_type{});
            }
        }

        template<typename T, typename BeginObject>
        auto ReadObject(T& t, val_t& val, BeginObject) -> std::enable_if_t<is_basic_type<T>::value>
        {
            rd_.read(t, val);
        }

        template <typename T, typename BeginObject>
        auto ReadObject(T& t, val_t& val, BeginObject) ->std::enable_if_t<std::is_enum<
            std::remove_reference_t<std::remove_cv_t<T>>>::value>
        {
            if (!enum_with_str_)
            {
                using under_type = std::underlying_type_t<
                    std::remove_reference_t<std::remove_cv_t<T>>>;
                rd_.read(reinterpret_cast<under_type&>(t), val);
            }
            else
            {
                using enum_t = std::remove_reference_t<std::remove_cv_t<T>>;
                std::string str;
                rd_.read(str, val);
                auto enum_val = to_enum<enum_t>(str.c_str());
                if (enum_val)
                    t = enum_val.value();
            }
        }

        template<typename T>
        auto process_array(val_t& val) -> std::enable_if_t<(is_basic_type<std::decay_t<T>>::value && !std::is_same<std::decay_t<T>, std::string>::value)
                                            || std::is_enum<std::decay_t<T>>::value>
        {
            rd_.process_array(val);
        }

        template<typename T>
        auto process_array(val_t& val) -> std::enable_if_t<(!is_basic_type<std::decay_t<T>>::value && !std::is_enum<std::decay_t<T>>::value) || std::is_same<std::decay_t<T>, std::string>::value>
        {
        }

    private:
        adapter_t rd_;
        OtherTuple tuple;
        bool enum_with_str_ = false;
    };
}

namespace cytx
{
    namespace bpo = boost::program_options;
    using namespace std;
    using namespace boost::program_options;

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

        void init(const options_description& op)
        {
            for (auto& o : op.options())
            {
                ops_.add(o);
            }
        }

        void parse(size_t argc, const char* const argv[])
        {
            vm_.clear();
            store(parse_command_line((int)argc, argv, ops_), vm_);
            notify(vm_);
        }

        void parse(const char* cmd_line)
        {
            vm_.clear();
            vector<string> args = split_winmain(cmd_line);
            store(command_line_parser(args).options(ops_).run(), vm_);
            notify(vm_);
        }

        void parse(const vector<string>& args)
        {
            vm_.clear();
            store(command_line_parser(args).options(ops_).run(), vm_);
            notify(vm_);
        }

        void parse(size_t argc, const char* const argv[], const options_description& op)
        {
            vm_.clear();
            store(parse_command_line((int)argc, argv, ops_), vm_);
            notify(vm_);
        }

        void parse(const char* cmd_line, const options_description& op)
        {
            vm_.clear();
            for (auto& o : op.options())
            {
                ops_.add(o);
            }
            vector<string> args = split_winmain(cmd_line);
            store(command_line_parser(args).options(ops_).run(), vm_);
            notify(vm_);
        }

        void parse(const vector<string>& args, const options_description& op)
        {
            vm_.clear();
            store(command_line_parser(args).options(ops_).run(), vm_);
            notify(vm_);
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

        const variables_map& vm() const { return vm_; }

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
            auto typed_value_ptr = dynamic_cast<const typed_value<bool>*>(op->semantic().get());
            bool is_typed_value = typed_value_ptr != nullptr;

            if (!is_typed_value)
            {
                v = vm_.count(key) != 0;
            }
            else if(vm_.count(key))
            {
                val_t val = vm_[key];
                v = val.empty() ? false : val.as<bool>();
            }
        }

        template<typename T>
        auto ReadObject(T& t, val_t& val)->std::enable_if_t<!std::is_enum<std::remove_reference_t<std::remove_cv_t<T>>>::value
            && !std::is_same<T, bool>::value>
        {
            if(!val.empty())
                t = val.as<T>();
        }

        template <typename T>
        auto ReadObject(T& t, val_t& val) ->std::enable_if_t<std::is_enum<
            std::remove_reference_t<std::remove_cv_t<T>>>::value>
        {
            if (!enum_with_str_)
            {
                using under_type = std::underlying_type_t<
                    std::remove_reference_t<std::remove_cv_t<T>>>;
                t = (T)val.as<under_type>();
            }
            else
            {
                using enum_t = std::remove_reference_t<std::remove_cv_t<T>>;
                std::string str = val.as<std::string>();
                auto enum_val = to_enum<enum_t>(str.c_str());
                if (enum_val)
                    t = enum_val.value();
            }
        }

    private:
        variables_map vm_;
        options_description ops_;
        bool enum_with_str_ = false;
    };
}

