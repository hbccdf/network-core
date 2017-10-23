#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#include "../traits/traits.hpp"
#include "../meta/meta.hpp"
#include "../base/cast.hpp"
#include "parser.hpp"

namespace cytx
{
    namespace detail
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
            xml_node(xml_node& node) : key(node.key), val(node.val), type(node.type) {}

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
                while (nodes_.size() > 1)
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
    }

    using xml_serialize_adapter = detail::base_serialize_adapter<xml_parser>;
    using ini_serialize_adapter = detail::base_serialize_adapter<ini_parser>;
    using info_serialize_adapter = detail::base_serialize_adapter<info_parser>;
}

namespace cytx
{
    namespace detail
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
                if (!child)
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
                        if (it_key != attr->not_found())
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
                if (it->first != "@key" && it->first != "+")
                    return it->first;
                if (it->first == "@key")
                {
                    auto name = (it->second).get_optional<std::string>("<xmlattr>.name");
                    if (name)
                        return name.value();
                }
                else
                {
                    auto name = (it->second).get_optional<std::string>("<xmlattr>.key");
                    if (name)
                        return name.value();
                }
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

                            for (auto& op : other_pt.begin()->second)
                            {
                                tmp_pt.push_back(op);
                            }
                            it = tmp_pt.erase(it);
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
    }

    using xml_deserialize_adapter = detail::base_deserialize_adapter<xml_parser>;
    using ini_deserialize_adapter = detail::base_deserialize_adapter<ini_parser>;
    using info_deserialize_adapter = detail::base_deserialize_adapter<info_parser>;
}