#pragma once
#include "../traits/traits.hpp"
#include "../meta/meta.hpp"
#include <boost/lexical_cast.hpp>
#include "json_util.hpp"
#include <fmt/format.h>
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "parser.hpp"

namespace cytx
{
    using namespace boost::property_tree;

    enum class node_type { object, array, value };

    struct xml_node
    {
        node_type type;
        std::string key;
        ptree val;

        xml_node(xml_node&& node) : key(std::move(node.key)),
            val(std::move(node.val)), type(node.type) {}
        xml_node(node_type t) : type(t) {}
        xml_node(xml_node& node) : key(node.key),  val(node.val), type(node.type) {}

        xml_node& operator=(xml_node&& node)
        {
            key = std::move(node.key);
            val = std::move(node.val);
            type = node.type;
            return *this;
        }

        void reset(node_type t)
        {
            type = t;
            key = "";
            val.clear();
        }
    };

    inline bool find(std::string str, std::string chars)
    {
        return std::any_of(str.begin(), str.end(), [chs = std::move(chars)](auto c) -> bool { return chs.find(c) != std::string::npos; });
    }

    template<typename T>
    class base_serialize_adapter
    {
    public:
        enum { use_field_separator = 0, use_begin_map = 0 };

        base_serialize_adapter() {}

        void begin_serialize(const std::string& key = "")
        {
            nodes_.clear();
            nodes_.emplace_back(xml_node(node_type::object));
            cur_node_.reset(node_type::value);
            cur_node_.key = key.empty() ? "+" : key;
        }
        void end_serialize()
        {
            if (!(cur_node_.key.empty() && cur_node_.val.data().empty() && cur_node_.val.empty()))
            {
                if (cur_node_.key.empty())
                    cur_node_.key = "+";
                nodes_.emplace_back(cur_node_);
            }
            while(nodes_.size() > 1)
            {
                auto& pt = nodes_[nodes_.size() - 2];
                auto& node = nodes_.back();
                pt.val.add_child(node.key.empty() ? "+" : node.key, node.val);
                nodes_.pop_back();
            }

            auto node = nodes_.back();
            std::stringstream ss;
            T::write(ss, node.val);
            buf_ = ss.str();
        }
        void begin_object()
        {
            if (cur_node_.type == node_type::value)
            {
                if (!cur_node_.val.data().empty())
                {
                    nodes_.back().val.add_child(cur_node_.key.empty() ? "+" : cur_node_.key, cur_node_.val);
                    cur_node_.reset(node_type::object);
                }
                else
                    cur_node_.type = node_type::object;
            }
            nodes_.emplace_back(cur_node_);
            cur_node_.reset(node_type::value);
        }
        void end_object()
        {
            auto node = nodes_.back();
            nodes_.pop_back();
            if (!cur_node_.key.empty() || !cur_node_.val.data().empty())
            {
                node.val.add_child(cur_node_.key, cur_node_.val);
            }
            nodes_.back().val.add_child(node.key.empty() ? "+" : node.key, node.val);
            cur_node_.reset(node_type::value);
        }
        void begin_array(size_t)
        {
            if (cur_node_.type == node_type::value)
            {
                if (!cur_node_.val.data().empty())
                {
                    nodes_.back().val.add_child(cur_node_.key.empty() ? "+" : cur_node_.key, cur_node_.val);
                    cur_node_.reset(node_type::object);
                }
                else
                    cur_node_.type = node_type::object;
            }
            nodes_.emplace_back(cur_node_);
            cur_node_.reset(node_type::array);
            nodes_.emplace_back(cur_node_);
            cur_node_.reset(node_type::value);
        }
        void end_array()
        {
            auto node = nodes_.back();
            nodes_.pop_back();
            if (cur_node_.type == node_type::value)
            {
                if (!cur_node_.key.empty() || !cur_node_.val.data().empty())
                {
                    if (node.val.empty() && node.type != node_type::array)
                        node.val = cur_node_.val;
                    else
                        node.val.add_child(cur_node_.key.empty() ? "+" : cur_node_.key, cur_node_.val);
                }
            }
            else
            {
                if (node.val.empty() && node.type != node_type::array)
                    node.val = cur_node_.val;
                else
                    node.val.add_child(cur_node_.key.empty() ? "+" : cur_node_.key, cur_node_.val);
            }

            auto obj_node = nodes_.back();
            nodes_.pop_back();
            if (obj_node.val.empty() && obj_node.type != node_type::array)
                obj_node.val = node.val;
            else
                obj_node.val.add_child(node.key.empty() ? "+" : node.key, node.val);

            nodes_.back().val.add_child(obj_node.key.empty() ? "+" : obj_node.key, obj_node.val);
            cur_node_.reset(node_type::value);
        }
        void begin_fixed_array(size_t s) { begin_array(s); }
        void end_fixed_array() { end_array(); }

        void write_key_name(const std::string& str) { write_key(str); }

        template<typename T>
        void write_key(T&& t)
        {
            if (cur_node_.key.empty())
            {
                auto str = fmt::format("{}", t);
                if (find(str, "@,.<>\"'/&; \t\r\n"))
                {
                    cur_node_.key = "@key";
                    cur_node_.val.add("<xmlattr>.name", str);
                }
                else
                    cur_node_.key = str;
            }
            else
            {
                auto& node = nodes_.back();
                node.val.add_child(cur_node_.key, cur_node_.val);
                auto type = cur_node_.type;
                cur_node_.reset(type);
                cur_node_.key = fmt::format("{}", t);
            }
        }

        template<typename T>
        void write(T&& t)
        {
            if (cur_node_.type == node_type::array)
            {
                ptree p;
                p.put_value(t);
                cur_node_.val.add_child(cur_node_.key.empty() ? "+" : cur_node_.key, p);
            }
            else if (cur_node_.type == node_type::value)
            {
                if (cur_node_.val.data().empty())
                    cur_node_.val.put_value(t);
                else
                {
                    nodes_.back().val.add_child(cur_node_.key.empty() ? "+" : cur_node_.key, cur_node_.val);
                    cur_node_.val.put_value(t);
                }
            }
            else
            {
                cur_node_.val.put_value(t);
            }
        }

        void write_null()
        {
            /*auto& node = cur_node();
            node.val.put_value("null");*/
        }

        std::string str() { return buf_; }
    private:
        xml_node cur_node_{ node_type::value };
        std::vector<xml_node> nodes_;
        std::string buf_;
    };

    using xml_serialize_adapter = base_serialize_adapter<xml_parser>;
    using ini_serialize_adapter = base_serialize_adapter<ini_parser>;
    using info_serialize_adapter = base_serialize_adapter<info_parser>;
}

namespace cytx {

    class json_serialize_adapter
    {
    public:
        enum { use_field_separator = 0, use_begin_map = 0 };

        void begin_serialize(const std::string& key = "") { json_.reset(); }
        void end_serialize() {}
        void begin_object() { json_.start_object(); }
        void end_object() { json_.end_object(); }
        void begin_array(size_t) { json_.start_array(); }
        void end_array() { json_.end_array(); }

        void begin_fixed_array(size_t) { json_.start_array(); }
        void end_fixed_array() { json_.end_array(); }

        template<typename T>
        void write_key(T&& t)
        {
            json_.write_value(fmt::format("{}", t));
        }

        void write_key_name(const std::string& str) { write_key(str); }

        template<typename T>
        void write(T&& t)
        {
            json_.write_value(std::forward<T>(t));
        }

        void write_null() { json_.write_null(); }

        std::string str() { return json_.str(); }
    protected:
        json_util json_;
    };

    template<typename ADAPTER_T>
    class Serializer : boost::noncopyable
    {
        typedef ADAPTER_T adapter_t;
    public:

        template<typename... ARGS>
        Serializer(ARGS&&... args)
            : wr_(std::forward<ARGS>(args)...)
        {
        }

        ~Serializer()
        {
        }

        adapter_t& get_adapter() { return wr_; }

        bool enum_with_str() { return enum_with_str_; }
        void enum_with_str(bool val) { enum_with_str_ = val; }

        template<typename T>
        void Serialize(const T& t, const char* key = nullptr)
        {
            wr_.begin_serialize(key == nullptr ? "" : key);
            WriteObject(t, true, std::true_type{});
            wr_.end_serialize();
        }

    private:
        template <typename T, typename BeginObjec>
        auto WriteObject(T const& t, bool is_last, BeginObjec bj) -> std::enable_if_t<is_optional<T>::value>
        {
            if (static_cast<bool>(t))
            {
                WriteObject(*t, is_last, bj);
            }
            else
            {
                WriteNull(is_last);
            }
        }

        template<typename T, typename BeginObjec>
        auto WriteObject(const T& t, bool is_last, BeginObjec) -> std::enable_if_t<is_reflection<T>::value && !enum_meta<T>::value>
        {
            adapter_begin_object();
            for_each(get_meta(t), [this](auto& v, size_t I, bool is_last)
            {
                WriteObject(v, is_last, std::false_type{});
            });
            adapter_end_object(is_last);
        }

        template<typename T, typename BeginObjec>
        auto WriteObject(T const& t, bool is_last, BeginObjec bo) -> std::enable_if_t<is_tuple<T>::value>
        {
            adapter_begin_fixed_array(std::tuple_size<T>::value);
            for_each(t, [this, bo](auto& v, size_t I, bool is_last)
            {
                WriteObject(v, is_last, bo);
            });
            adapter_end_fixed_array(is_last);
        }

        template<typename T, typename BeginObjec>
        auto WriteObject(T const& t, bool is_last, BeginObjec) -> std::enable_if_t<is_singlevalue_container<T>::value>
        {
            WriteArray(t, t.size(), is_last);
        }

        template<typename T, typename BeginObject>
        auto WriteObject(T const& t, bool is_last, BeginObject) -> std::enable_if_t<is_map_container<T>::value>
        {
            auto array_size = t.size();
            adapter_begin_object();
            adapter_begin_map(array_size);
            size_t count = 0;
            for (auto const& pair : t)
            {
                ++count;
                WriteKV(pair.first, pair.second, count == array_size, BeginObject{});
            }
            adapter_end_map(true);
            adapter_end_object(is_last);
        }

        template<typename T>
        auto WriteObject(T const& t, bool is_last, std::true_type) -> std::enable_if_t<is_pair<T>::value>
        {
            adapter_begin_object();
            WriteKV(t.first, t.second, is_last, std::true_type{});
            adapter_end_object(is_last);
        }

        template<typename T>
        auto WriteObject(T const& t, bool is_last, std::false_type) -> std::enable_if_t<is_pair<T>::value>
        {
            WriteKV(t.first, t.second, is_last, std::false_type{});
        }

        template<typename T, size_t N, typename BeginObject>
        void WriteObject(std::array<T, N> const& t, bool is_last, BeginObject)
        {
            WriteFixdArray(t, t.size(), is_last);
        }

        template <typename T, size_t N, typename BeginObject>
        void WriteObject(T const(&p)[N], bool is_last, BeginObject)
        {
            WriteFixdArray(p, N, is_last);
        }

        template <size_t N, typename BeginObject>
        void WriteObject(char const(&p)[N], bool is_last, BeginObject bj)
        {
            WriteObject((const char*)p, is_last, bj);
        }

        template<typename Array>
        inline void WriteArray(Array const& v, size_t array_size, bool is_last)
        {
            adapter_begin_array(array_size);
            WriteOnlyArray(v, array_size);
            adapter_end_array(is_last);
        }

        template<typename Array>
        inline void WriteFixdArray(Array const& v, size_t array_size, bool is_last)
        {
            adapter_begin_fixed_array(array_size);
            WriteOnlyArray(v, array_size);
            adapter_end_fixed_array(is_last);
        }

        template<typename Array>
        inline void WriteOnlyArray(Array const& v, size_t array_size)
        {
            size_t count = 0;
            for (auto const& i : v)
            {
                ++count;
                WriteObject(i, count == array_size, std::false_type{});
            }
        }

        template <typename T, typename BeginObject>
        auto WriteObject(T const& val, bool is_last, BeginObject) -> std::enable_if_t<std::is_enum<
            std::remove_reference_t<std::remove_cv_t<T>>>::value>
        {
            if (enum_with_str_)
            {
                using enum_t = std::remove_reference_t<std::remove_cv_t<T>>;
                auto enum_val = to_string(static_cast<enum_t>(val), nullptr);
                if (!enum_val)
                {
                    adapter_write_field(std::string(""), is_last);
                }
                else
                {
                    adapter_write_field(enum_val.value(), is_last);
                }
            }
            else
            {
                using under_type = std::underlying_type_t<
                    std::remove_reference_t<std::remove_cv_t<T>>>;
                adapter_write_field(static_cast<under_type>(val), is_last);
            }
        }

        void WriteObject(const char* t, bool is_last, std::true_type)
        {
            adapter_write_field(t, is_last);
        }

        void WriteObject(const char* t, bool is_last, std::false_type)
        {
            adapter_write_field(t, is_last);
        }

        template<typename K, typename V>
        void WriteKV(K& k, V& v, bool is_last, std::false_type)
        {
            wr_.write_key_name(k);
            WriteObject(v, is_last, std::true_type{});
        }

        template<typename K, typename V>
        void WriteKV(K& k, V& v, bool is_last, std::true_type)
        {
            wr_.write_key(k);
            WriteObject(v, is_last, std::true_type{});
        }

        template<typename T, typename BeginObject>
        auto WriteObject(T const& t, bool is_last, BeginObject) -> std::enable_if_t<is_basic_type<T>::value>
        {
            adapter_write_field(t, is_last);
        }

        void WriteNull(bool is_last)
        {
            adapter_write_null(is_last);
        }

    private:
        template<typename T, typename ADAPTER = adapter_t>
        auto adapter_write_field(T const& t, bool /*is_last*/) -> std::enable_if_t<ADAPTER::use_field_separator == 0>
        {
            wr_.write(t);
        }
        template<typename T, typename ADAPTER = adapter_t>
        auto adapter_write_field(T const& t, bool is_last) -> std::enable_if_t<ADAPTER::use_field_separator == 1>
        {
            wr_.write_field(t, is_last);
        }

        template<typename ADAPTER = adapter_t>
        auto adapter_write_null(bool /*is_last*/) -> std::enable_if_t<ADAPTER::use_field_separator == 0>
        {
            wr_.write_null();
        }
        template<typename ADAPTER = adapter_t>
        auto adapter_write_null(bool is_last) -> std::enable_if_t<ADAPTER::use_field_separator == 1>
        {
            wr_.write_null(is_last);
        }

        void adapter_begin_object()
        {
            wr_.begin_object();
        }

        template<typename ADAPTER = adapter_t>
        auto adapter_end_object(bool /*is_last*/)->std::enable_if_t<ADAPTER::use_field_separator == 0>
        {
            wr_.end_object();
        }
        template<typename ADAPTER = adapter_t>
        auto adapter_end_object(bool is_last)->std::enable_if_t<ADAPTER::use_field_separator == 1>
        {
            wr_.end_object(is_last);
        }

        void adapter_begin_array(size_t array_size)
        {
            wr_.begin_array(array_size);
        }

        template<typename ADAPTER = adapter_t>
        auto adapter_end_array(bool /*is_last*/)->std::enable_if_t<ADAPTER::use_field_separator == 0>
        {
            wr_.end_array();
        }
        template<typename ADAPTER = adapter_t>
        auto adapter_end_array(bool is_last)->std::enable_if_t<ADAPTER::use_field_separator == 1>
        {
            wr_.end_array(is_last);
        }

        void adapter_begin_fixed_array(size_t array_size)
        {
            wr_.begin_fixed_array(array_size);
        }

        template<typename ADAPTER = adapter_t>
        auto adapter_end_fixed_array(bool /*is_last*/)->std::enable_if_t<ADAPTER::use_field_separator == 0>
        {
            wr_.end_fixed_array();
        }
        template<typename ADAPTER = adapter_t>
        auto adapter_end_fixed_array(bool is_last)->std::enable_if_t<ADAPTER::use_field_separator == 1>
        {
            wr_.end_fixed_array(is_last);
        }

        template<typename ADAPTER = adapter_t>
        auto adapter_begin_map(size_t /*array_size*/)->std::enable_if_t<ADAPTER::use_begin_map == 0>
        {
        }
        template<typename ADAPTER = adapter_t>
        auto adapter_begin_map(size_t array_size)->std::enable_if_t<ADAPTER::use_begin_map == 1>
        {
            adapter_begin_array(array_size);
        }

        template<typename ADAPTER = adapter_t>
        auto adapter_end_map(bool /*is_last*/)->std::enable_if_t<ADAPTER::use_begin_map == 0>
        {
        }
        template<typename ADAPTER = adapter_t>
        auto adapter_end_map(bool is_last)->std::enable_if_t<ADAPTER::use_begin_map == 1>
        {
            adapter_end_array(is_last);
        }

    private:
        adapter_t wr_;
        bool enum_with_str_ = false;
    };
}

