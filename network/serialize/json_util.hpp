#pragma once
#include <string>
#include <cstdint>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>

#include "../traits/traits.hpp"

namespace cytx {
    class json_util : boost::noncopyable
    {
        typedef rapidjson::Writer<rapidjson::StringBuffer> json_writer_t;
    public:
        typedef rapidjson::Value value_type;
        typedef value_type::ValueIterator array_iterator;
        typedef value_type::MemberIterator member_iterator;

        json_util() : writer_(buf_)
        {
        }

        template<typename T>
        void write_json(const char* key, T&& value)
        {
            writer_.String(key);
            write_value(std::forward<T>(value));
        }

        template<typename T>
        void write_json(const std::string& key, T&& value)
        {
            writer_.String(key.c_str());
            write_value(std::forward<T>(value));
        }

        void reset()
        {
            writer_.Reset(buf_);
            buf_.Clear();
        }

        void start_object()
        {
            writer_.StartObject();
        }

        void end_object()
        {
            writer_.EndObject();
        }

        void start_array()
        {
            writer_.StartArray();
        }

        void end_array()
        {
            writer_.EndArray();
        }

        void write_value(const char* val)
        {
            if (val == nullptr)
                writer_.Null();
            else
                writer_.String(val);
        }

        void read_value(char*& t, rapidjson::Value& val)
        {
            if (val.IsString())
                t = (char*)val.GetString();
        }

        void write_value(const std::string& val)
        {
            writer_.String(val.c_str());
        }

        void read_value(std::string& t, rapidjson::Value& val)
        {
            if (val.IsString())
                t = val.GetString();
        }

        void parse(const char* json)
        {
            auto& r = doc_.Parse<0>(json);
            if (r.HasParseError())
            {
                throw std::invalid_argument("json string parse failed");
            }
        }

        void parse(const char* json, std::size_t length)
        {
            auto& r = doc_.Parse<0>(json, length);
            if (r.HasParseError())
            {
                throw std::invalid_argument("json string parse failed");
            }
        }

        rapidjson::Document& get_doc()
        {
            return doc_;
        }

        void write_value(uint8_t val)
        {
            writer_.Int(val);
        }

        void read_value(uint8_t& t, rapidjson::Value& val)
        {
            if (val.IsInt())
                t = (uint8_t)val.GetInt();
        }

        void write_value(int8_t val)
        {
            writer_.Int(val);
        }

        void read_value(int8_t& t, rapidjson::Value& val)
        {
            if (val.IsInt())
                t = (int8_t)val.GetInt();
        }

        void write_value(int16_t val)
        {
            writer_.Int(val);
        }

        void read_value(int16_t& t, rapidjson::Value& val)
        {
            if (val.IsInt())
                t = (int16_t)val.GetInt();
        }

        void write_value(uint16_t val)
        {
            writer_.Int(val);
        }

        void read_value(uint16_t& t, rapidjson::Value& val)
        {
            if (val.IsInt())
                t = (uint16_t)val.GetInt();
        }

        void write_value(int val)
        {
            writer_.Int(val);
        }

        void read_value(int& t, rapidjson::Value& val)
        {
            if (val.IsInt())
                t = val.GetInt();
        }

        void write_value(char val)
        {
            writer_.Int(val);
        }

        void read_value(char& t, rapidjson::Value& val)
        {
            if (val.IsInt())
                t = (char)val.GetInt();
        }

        void write_value(uint32_t val)
        {
            writer_.Uint(val);
        }

        void read_value(uint32_t& t, rapidjson::Value& val)
        {
            if (val.IsUint())
                t = val.GetUint();
        }

        void write_value(int64_t val)
        {
            writer_.Int64(val);
        }

        void read_value(int64_t& t, rapidjson::Value& val)
        {
            if (val.IsInt64())
                t = val.GetInt64();
        }

        void write_value(uint64_t val)
        {
            writer_.Uint64(val);
        }

        void read_value(uint64_t& t, rapidjson::Value& val)
        {
            if (val.IsUint64())
                t = val.GetUint64();
        }

        void write_value(double val)
        {
            writer_.Double(val);
        }

        void read_value(double& t, rapidjson::Value& val)
        {
            if (val.IsNumber())
                t = val.GetDouble();
        }

        void write_value(float val)
        {
            writer_.Double(static_cast<double>(val));
        }

        void read_value(float& t, rapidjson::Value& val)
        {
            if (val.IsNumber())
                t = static_cast<float>(val.GetDouble());
        }

        template<typename T>
        typename std::enable_if<std::is_same<T, bool>::value>::type write_value(T val)
        {
            writer_.Bool(val);
        }

        //还要过滤智能指针的情况.
        template<typename T>
        typename std::enable_if<is_pointer_ext<T>::value>::type write_value(T val)
        {
            //不支持动态指针的原因是反序列化的时候涉及到指针的内存管理，反序列化不应该考虑为对象分配内存.
            throw std::invalid_argument("not surpport dynamic pointer");
        }

        void write_null()
        {
            writer_.Null();
        }

        void read_value(bool& t, rapidjson::Value& val)
        {
            if (val.IsBool())
                t = val.GetBool();
        }

        const char* c_str()
        {
            return buf_.GetString();
        }

        std::string str()
        {
            return buf_.GetString();
        }

    private:
        rapidjson::StringBuffer buf_; //json字符串的buf.
        json_writer_t writer_; //json写入器.
        rapidjson::Document doc_;
    };
}

