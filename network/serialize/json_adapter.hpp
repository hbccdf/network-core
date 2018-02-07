#pragma once
#include <fmt/format.h>
#include "detail/json_util.hpp"

namespace cytx {

    class json_serialize_adapter
    {
    public:
        enum { use_field_separator = 0, use_begin_map = 0, ralax_check_enum = 0 };

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

        void write_is_null(bool is_null)
        {
            if (is_null)
            {
                json_.write_null();
            }
        }

        std::string str() { return json_.str(); }
    protected:
        json_util json_;
    };

    class json_deserialize_adapter
    {
    public:
        typedef json_util::value_type value_t;
        typedef json_util::array_iterator array_iterator;
        typedef json_util::member_iterator member_iterator;

        json_deserialize_adapter() {}
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

        bool is_null(value_t& val)
        {
            return json_.is_null(val);
        }

        std::string first(member_iterator& it)
        {
            return it->name.GetString();
        }

        value_t& second(member_iterator& it)
        {
            return it->value;
        }

        void process_array(value_t& val)
        {

        }
    public:
        json_util json_;
    };
}