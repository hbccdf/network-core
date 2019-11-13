#pragma once
#include <fmt/format.h>
#include <boost/filesystem.hpp>
#include "deserializer.hpp"
#include "detail/csv_parser.hpp"
#include "network/util/cast.hpp"
#include "network/util/string.hpp"

namespace cytx
{
    class csv_runtime_exception : public std::exception
    {
    public:
        csv_runtime_exception(std::string err_msg) : err_msg_(err_msg) {}

#ifdef _WIN32
        char const* what() const override { return err_msg_.c_str(); }
#else
        char const* what() const noexcept override { return err_msg_.c_str(); }
#endif

        std::string message() const { return err_msg_; }

    private:
        std::string err_msg_;
    };

    class csv_deserialize_adapter
    {
    public:
        using row_iterator = size_t;
        using field_iterator = size_t;
        using val_t = const csv_row;

        csv_deserialize_adapter()
        {
        }

        void parse(const std::string& str)
        {
            csv_.parse_string(str);
        }
        void parse_file(const std::string& file_name)
        {
            namespace fs = boost::filesystem;
            file_name_ = fs::path(file_name).filename().string();
            csv_.parse_file(file_name);
        }

        row_iterator begin() const
        {
            return 0;
        }
        row_iterator end() const
        {
            return csv_.row_size();
        }

        int id(row_iterator it) const
        {
            auto& row = csv_[it];
            return cytx::util::cast<int>(row["id"]);
        }

        val_t& get_val(row_iterator it) const
        {
            return csv_[it];
        }

        size_t member_size(val_t& val) const
        {
            return val.size();
        }

        const std::string& get_field_val(val_t& val, const std::string& key) const
        {
            return val[key];
        }

        bool has_field(const std::string& key) const
        {
            auto& headers = csv_.header();
            for (auto& h : headers)
            {
                if (h == key)
                    return true;
            }
            return false;
        }

        csv& get_csv()
        {
            return csv_;
        }

        std::string filename() const
        {
            return file_name_;
        }

        size_t header_lines() const
        {
            return csv_.header_lines();
        }
    private:
        csv csv_;
        std::string file_name_;
    };

    template<typename OtherTuple>
    class DeSerializer<csv_deserialize_adapter, OtherTuple> : public BaseDeSerializer
    {
        using adapter_t = csv_deserialize_adapter;
        using val_t = typename adapter_t::val_t;
    public:

        template<typename... ARGS>
        DeSerializer(ARGS&&... args)
            : rd_(std::forward<ARGS>(args)...)
        {
        }

        template<typename... ARGS>
        DeSerializer(OtherTuple&& t, ARGS&&... args)
            : tuple_(std::move(t))
            , rd_(std::forward<ARGS>(args)...)
        {
        }

        ~DeSerializer()
        {
        }

        adapter_t& get_adapter() { return rd_; }

        void set_tuple(OtherTuple&& t)
        {
            tuple_ = std::move(t);
        }

        template<typename ... ARGS>
        void parse_file(ARGS&&... args)
        {
            try
            {
                rd_.parse_file(std::forward<ARGS>(args)...);
            }
            catch (std::exception& e)
            {
                throw csv_runtime_exception(fmt::format("parse file {} error: {}", rd_.filename(), e.what()));
            }
        }

        void parse(const std::string& str)
        {
            try
            {
                rd_.parse(str);
            }
            catch (std::exception& e)
            {
                throw csv_runtime_exception(fmt::format("parse string({}) error: {}", str.length(), e.what()));
            }
        }

        template<typename T>
        void DeSerialize(T& t)
        {
            try
            {
                ReadMap(std::get<0>(get_meta(t)).second);
            }
            catch(std::exception& e)
            {
                throw csv_runtime_exception(fmt::format("DeSerialize error, file:{}, line:{}, id:{}, key:{}, error: {}", rd_.filename(), current_line_, current_id_, current_key_name_, e.what()));
            }
        }

    private:
        template<typename T>
        auto ReadMap(T& t)
        {
            current_line_ = rd_.header_lines();
            auto it = rd_.begin();
            auto it_end = rd_.end();
            for (; it != it_end; ++it)
            {
                using element_t = decltype(*t.begin());
                using pair_t = std::decay_t<element_t>;
                using second_type = typename pair_t::second_type;

                ++current_line_;
                int val_id = rd_.id(it);
                current_id_ = val_id;

                second_type s{};
                auto meta_value = get_meta(s);
                ReadTuple(meta_value, rd_.get_val(it));
                t[val_id] = s;
            }
        }

        template<std::size_t I = 0, typename Tuple>
        auto ReadTuple(Tuple& t, val_t& val) -> std::enable_if_t<I == std::tuple_size<Tuple>::value>
        {
        }

        template<std::size_t I = 0, typename Tuple>
        auto ReadTuple(Tuple& t, val_t& val)->std::enable_if_t < I < std::tuple_size<Tuple>::value>
        {
            auto mb_size = rd_.member_size(val);
            if (mb_size <= 0)
                return;

            auto& pair = std::get<I>(t);
            auto& key_name = pair.first;
            auto& pair_val = pair.second;

            current_key_name_ = key_name;
            ReadObject(pair_val, val, key_name);
            ReadTuple<I + 1>(t, val);
        }

        template<typename T>
        auto ReadObject(T& t, val_t& val, const std::string& key_name) ->std::enable_if_t<is_user_class_v<T>>
        {
            auto meta_tuple = get_meta(t);
            auto meta_name = get_name<T>();
            std::string new_key_name = fmt::format("{}(", key_name);

            //find struct start index and end index
            auto& headers = rd_.get_csv().header();

            int start_index = -1;
            int end_index = -1;

            for (size_t i = 0; i < headers.size(); ++i)
            {
                if (headers[i] == new_key_name)
                {
                    start_index = (int)i;
                }

                if (i > start_index && headers[i] == ")")
                {
                    end_index = (int)i;
                    break;
                }
            }

            if (start_index < 0 || end_index < 0 || (start_index + 1) == end_index)
                return;

            int field_count = end_index - (start_index + 1);

            int val_index = start_index + 1;
            int val_count = 0;

            std::map<std::string, int> name_map;
            for (int i = start_index + 1; i < end_index; ++i)
            {
                std::string name = headers[i];
                name_map[name] = i - (start_index + 1);
            }

            while ((val_index + 1) < val.size())
            {
                if (val[val_index + 1] == ")" || val[val_index + 1] == "(")
                {
                    ++val_index;
                    continue;
                }

                cytx::for_each(meta_tuple, [&val_index, &name_map, &val, this](auto& p, size_t I, bool is_last)
                {
                    auto it = name_map.find(p.first);
                    if (it == name_map.end())
                        return;

                    int index = it->second;
                    int field_index = index + val_index;
                    auto& str = val[field_index];
                    this->ReadBase(p.second, str);
                });

                ++val_count;
                val_index += field_count;

                while ((val_index + 1) < val.size())
                {
                    ++val_index;
                    if (val[val_index] == "(")
                    {
                        break;
                    }
                }
            }
        }

        template<typename T>
        auto ReadObject(std::vector<T>& t, val_t& val, const std::string& key_name) ->std::enable_if_t<is_user_class_v<T>>
        {
            auto meta_name = get_name<T>();
            std::string new_key_name = fmt::format("{}(", key_name);

            //find struct start index and end index
            auto& headers = rd_.get_csv().header();

            int start_index = -1;
            int end_index = -1;

            for (size_t i = 0; i < headers.size(); ++i)
            {
                if (headers[i] == new_key_name)
                {
                    start_index = (int)i;
                }

                if (i > start_index && headers[i] == ")")
                {
                    end_index = (int)i;
                    break;
                }
            }

            if (start_index < 0 || end_index < 0 || (start_index + 1) == end_index)
                return;

            int field_count = end_index - (start_index + 1);

            int val_index = start_index + 1;
            int val_count = 0;

            std::map<std::string, int> name_map;
            for (int i = start_index + 1; i < end_index; ++i)
            {
                std::string name = headers[i];
                name_map[name] = i - (start_index + 1);
            }

            while ((val_index + 1) < val.size())
            {
                if (val[val_index] == ")" || val[val_index] == "(")
                {
                    ++val_index;
                    continue;
                }

                T tt{};
                auto meta_tuple = get_meta(tt);
                cytx::for_each(meta_tuple, [&val_index, &name_map, &val, this](auto& p, size_t I, bool is_last)
                {
                    auto it = name_map.find(p.first);
                    if (it == name_map.end())
                        return;

                    int index = it->second;
                    int field_index = index + val_index;
                    auto& str = val[field_index];
                    this->ReadBase(p.second, str);
                });

                t.emplace_back(tt);

                ++val_count;
                val_index += field_count;

                while ((val_index + 1) < val.size())
                {
                    ++val_index;
                    if (val[val_index] == "(")
                    {
                        break;
                    }
                }
            }
        }

        template<typename T>
        auto ReadObject(T& t, val_t& val, const std::string& key_name) ->std::enable_if_t<!is_user_class_v<T>>
        {
            if (rd_.has_field(key_name))
            {
                const std::string& val_str = rd_.get_field_val(val, key_name);
                ReadBase(t, val_str);
            }
        }

        template <typename T, size_t N>
        void ReadBase(T(&p)[N], const std::string& val)
        {
            ReadFixedLengthArray(p, N, val);
        }

        template<typename T, size_t N>
        void ReadBase(std::array<T, N>& t, const std::string& val)
        {
            ReadFixedLengthArray(t, t.size(), val);
        }

        template <size_t N>
        void ReadBase(char(&p)[N], const std::string& val)
        {
            val.copy(p, N);
            p[val.length() >= N ? N - 1 : val.length()] = 0;
        }

        template<typename T>
        void ReadBase(std::vector<T>& t, const std::string& val)
        {
            using element_t = decltype((t)[0]);
            using ele_t = std::decay_t<element_t>;

            std::vector<std::string> vals = process_array(val);
            auto it = vals.begin();
            auto it_end = vals.end();
            for (; it != it_end; ++it)
            {
                ele_t el{};
                ReadBase(el, *it);
                t.emplace_back(el);
            }
        }

        template<typename Array>
        void ReadFixedLengthArray(Array & v, size_t array_size, const std::string& val)
        {
            std::vector<std::string> vals = process_array(val);
            auto it = vals.begin();
            auto it_end = vals.end();
            for (size_t i = 0; i < array_size && it != it_end; ++i, ++it)
            {
                ReadBase(v[i], *it);
            }
        }

        template<typename T>
        auto ReadBase(T& t, const std::string& val) -> std::enable_if_t<is_basic_type_v<T> || is_date_time_type_v<T>>
        {
            t = cytx::util::cast<T>(val);
        }

        template<typename T>
        auto ReadBase(T& t, const std::string& val) -> std::enable_if_t<is_optional_v<T>>
        {
            using value_type = typename T::value_type;
            if (val != "null")
            {
                value_type v{};
                ReadBase(v, val);
                t = v;
            }
        }

        template <typename T>
        auto ReadBase(T& t, const std::string& val) ->std::enable_if_t<is_enum_type_v<T>>
        {
            using enum_t = std::decay_t<T>;
            using under_type = std::underlying_type_t<enum_t>;
            if (!enum_with_str_)
            {
                t = (enum_t)cytx::util::cast<under_type>(val);
            }
            else
            {
                t = cytx::util::cast<enum_t>(val);
            }
        }

        auto process_array(const std::string& val)
        {
            std::vector<std::string> result;
            if (!val.empty())
            {
                string_util::split(result, val, ",|");
            }

            for (auto& v : result)
            {
                string_util::trim(v, " \t\r\n");
            }
            return result;
        }


    private:
        adapter_t rd_;
        OtherTuple tuple_;

        size_t current_line_ = 0;
        int32_t current_id_ = 0;
        std::string current_key_name_;
    };
}