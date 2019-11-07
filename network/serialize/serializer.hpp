#pragma once
#include "network/traits/traits.hpp"
#include "network/meta/meta.hpp"

namespace cytx {

    template<typename ADAPTER_T>
    class Serializer
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
            bool is_null = !static_cast<bool>(t);
            WriteIsNull(is_null, is_last);
            if (!is_null)
            {
                WriteObject(*t, is_last, bj);
            }
        }

        template<typename T, typename BeginObjec>
        auto WriteObject(const T& t, bool is_last, BeginObjec) -> std::enable_if_t<is_reflection<T>::value && !std::is_enum<T>::value>
        {
            adapter_begin_object();
            for_each(get_meta(t), [this](auto& v, size_t I, bool is_last)
            {
                this->WriteObject(v, is_last, std::false_type{});
            });
            adapter_end_object(is_last);
        }

        template<typename T, typename BeginObjec>
        auto WriteObject(T const& t, bool is_last, BeginObjec bo) -> std::enable_if_t<is_tuple<T>::value>
        {
            adapter_begin_fixed_array(std::tuple_size<T>::value);
            for_each(t, [this, bo](auto& v, size_t I, bool is_last)
            {
                this->WriteObject(v, is_last, bo);
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
        void WriteArray(Array const& v, size_t array_size, bool is_last)
        {
            adapter_begin_array(array_size);
            WriteOnlyArray(v, array_size);
            adapter_end_array(is_last);
        }

        template<typename Array>
        void WriteFixdArray(Array const& v, size_t array_size, bool is_last)
        {
            adapter_begin_fixed_array(array_size);
            WriteOnlyArray(v, array_size);
            adapter_end_fixed_array(is_last);
        }

        template<typename Array>
        void WriteOnlyArray(Array const& v, size_t array_size)
        {
            size_t count = 0;
            for (auto const& i : v)
            {
                ++count;
                WriteObject(i, count == array_size, std::false_type{});
            }
        }

        template <typename T, typename BeginObject>
        auto WriteObject(T const& val, bool is_last, BeginObject) -> std::enable_if_t<std::is_enum<std::decay_t<T>>::value>
        {
            using enum_t = std::decay_t<T>;
            using under_type = std::underlying_type_t<enum_t>;

            if (enum_with_str_)
            {
                auto enum_val = write_enum(static_cast<enum_t>(val));
                if (!enum_val)
                {
                    adapter_write_field(static_cast<under_type>(val), is_last);
                }
                else
                {
                    adapter_write_field(enum_val.value(), is_last);
                }
            }
            else
            {
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
            WriteKey(k);
            WriteObject(v, is_last, std::true_type{});
        }

        template<typename T, typename BeginObject>
        auto WriteObject(T const& t, bool is_last, BeginObject) -> std::enable_if_t<is_basic_type<T>::value>
        {
            adapter_write_field(t, is_last);
        }

        void WriteIsNull(bool is_null, bool is_last)
        {
            adapter_write_is_null(is_null, is_last);
        }

        template<typename K>
        auto WriteKey(K& k) -> std::enable_if_t<is_basic_type<K>::value>
        {
            wr_.write_key(k);
        }

        template <typename K>
        auto WriteKey(K& val) -> std::enable_if_t<std::is_enum<std::decay_t<T>>::value>
        {
            using enum_t = std::decay_t<T>;
            using under_type = std::underlying_type_t<enum_t>;

            if (enum_with_str_)
            {
                auto enum_val = write_enum(static_cast<enum_t>(val));
                if (!enum_val)
                {
                    wr_.write_key(static_cast<under_type>(val));
                }
                else
                {
                    wr_.write_key(enum_val.value());
                }
            }
            else
            {
                wr_.write_key(static_cast<under_type>(val));
            }
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
            wr_.write(t, is_last);
        }

        template<typename ADAPTER = adapter_t>
        auto adapter_write_is_null(bool is_null, bool /*is_last*/) -> std::enable_if_t<ADAPTER::use_field_separator == 0>
        {
            wr_.write_is_null(is_null);
        }
        template<typename ADAPTER = adapter_t>
        auto adapter_write_is_null(bool is_null, bool is_last) -> std::enable_if_t<ADAPTER::use_field_separator == 1>
        {
            wr_.write_is_null(is_null, is_last);
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

        template<typename enum_t, typename ADAPTER = adapter_t>
        auto write_enum(enum_t e)->std::enable_if_t<ADAPTER::ralax_check_enum == 0, boost::optional<std::string>>
        {
            return to_string(e, false);
        }
        template<typename enum_t, typename ADAPTER = adapter_t>
        auto write_enum(enum_t e)->std::enable_if_t<ADAPTER::ralax_check_enum == 1, boost::optional<std::string>>
        {
            return ralax_to_string(e, false);
        }

    private:
        adapter_t wr_;
        bool enum_with_str_ = false;
    };
}

