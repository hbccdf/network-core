﻿#pragma once
#include <fmt/format.h>
#include "network/base/memory_stream.hpp"
#include "network/util/net.hpp"
#include "deserializer.hpp"
#include "network/base/date_time.hpp"

namespace cytx
{
    class gos_serialize_adapter
    {
    public:
        gos_serialize_adapter(memory_stream& gos, bool is_big_endian = false)
            : gos_(gos)
            , is_big_endian_(is_big_endian) {}
        enum { use_field_separator = 0, use_begin_map = 1 };
    public:
        void begin_serialize(const std::string& key = "") {  }
        void end_serialize() {}
        void begin_object() {  }
        void end_object() {  }
        void begin_array(size_t array_size) { gos_.write_int(gos_hton((int)array_size)); }
        void end_array() {  }
        void begin_fixed_array(size_t array_size) {}
        void end_fixed_array() {}

        void write_is_null(bool is_null)
        {
            write(is_null);
        }

        void write_key_name(const std::string& str) {  }

        template<typename T>
        void write_key(T&& t)
        {
            write(std::forward<T>(t));
        }

        void write(const date_time& t)
        {
            write<int64_t>(t.utc_milliseconds());
        }

        void write(const std::string& str) { gos_.write_string(str); }

        template<typename T>
        auto write(T t) -> std::enable_if_t<is_number_type_v<T>>
        {
            gos_.write_value<T>(t);
        }

    private:
        template<typename T>
        T gos_hton(T t)
        {
            return is_big_endian_ ? cytx::util::hton(t) : t;
        }
    private:
        memory_stream& gos_;
        bool is_big_endian_;
    };

    class gos_deserialize_adapter
    {
    public:
        gos_deserialize_adapter(memory_stream& gos, bool is_big_endian = false)
            : gos_(gos)
            , is_big_endian_(is_big_endian) {}
    public:
        void begin_deserialize() {  }
        void end_deserialize() {}
        void begin_object() {  }
        void end_object() {  }
        void begin_array(size_t& array_size)
        {
            int length = 0;
            gos_.read_int(length);
            length = gos_ntoh(length);
            array_size = length;
        }
        void end_array() {  }

        void begin_fixed_array(size_t array_size) {}
        void end_fixed_array() {}

        void read_null() {  }

        void read_field_name(const std::string& str) {  }

        template<typename T>
        void read_key(T&& t)
        {
            read(std::forward<T>(t));
        }

        void read(date_time& t)
        {
            int64_t v = 0;
            read(v);
            t = date_time::from_utc_milliseconds(v);
        }

        void read(std::string& str) { gos_.read_string(str); }

        template<typename T>
        auto read(T& t) -> std::enable_if_t<is_number_type_v<T>>
        {
            gos_.read_value(t);
        }
    private:
        template<typename T>
        T gos_ntoh(T t)
        {
            return is_big_endian_ ? cytx::util::ntoh(t) : t;
        }
    private:
        memory_stream& gos_;
        bool is_big_endian_;
    };

    template<typename OtherTuple>
    class DeSerializer<gos_deserialize_adapter, OtherTuple> : public BaseDeSerializer<gos_deserialize_adapter, OtherTuple>
    {
        using base_t = BaseDeSerializer<gos_deserialize_adapter, OtherTuple>;
        using adapter_t = gos_deserialize_adapter;
    public:

        template<typename... ARGS>
        DeSerializer(ARGS&&... args)
            : base_t(std::forward<ARGS>(args)...)
        {
        }

        ~DeSerializer()
        {
        }

        template<typename T>
        void DeSerialize(T& t)
        {
            rd_.begin_deserialize();
            ReadObject(t, std::true_type{});
            rd_.end_deserialize();
        }

        template<typename T>
        auto GetTuple()->std::enable_if_t < 0 < std::tuple_size<T>::value, T >
        {
            return make<T, 0>(std::true_type{}, std::tuple<>{});
        }

        template<typename T>
        auto GetTuple()->std::enable_if_t < 0 == std::tuple_size<T>::value, T >
        {
            return std::tuple<>{};
        }

    private:
        template <typename T, size_t Is, typename ReturnT>
        auto make(std::true_type, ReturnT&& t)
        {
            using value_type = std::conditional_t<(Is + 1) == std::tuple_size<T>::value, std::false_type, std::true_type>;
            using elem_t = std::tuple_element_t<Is, T>;
            return make<T, Is + 1>(value_type{}, std::tuple_cat(t, std::tuple<elem_t>(get_tuple_elem<elem_t>())));
        }

        template <typename T, size_t Is, typename ReturnT>
        auto make(std::false_type, ReturnT&& t)
        {
            return t;
        }

        template<typename T>
        auto get_tuple_elem() -> std::enable_if_t<tuple_contains<T, OtherTuple>::value, T>
        {
            return std::get<tuple_index<T, OtherTuple>::value>(tuple_);
        }

        template<typename T>
        auto get_tuple_elem() -> std::enable_if_t<!tuple_contains<T, OtherTuple>::value, T>
        {
            T t{};
            ReadObject(t, std::true_type{});
            return std::move(t);
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, BeginObjec) -> std::enable_if_t<is_user_class_v<T> && !is_date_time_type_v<T>>
        {
            rd_.begin_object();
            auto target_meta = get_meta(t);
            ReadTuple(target_meta);
            rd_.end_object();
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, BeginObjec) -> std::enable_if_t<is_tuple_v<T>>
        {
            ReadTuple(t);
        }

        template<std::size_t I = 0, typename Tuple>
        auto ReadTuple(Tuple& t) -> std::enable_if_t<I == std::tuple_size<Tuple>::value>
        {
        }

        template<std::size_t I = 0, typename Tuple>
        auto ReadTuple(Tuple& t)->std::enable_if_t < I < std::tuple_size<Tuple>::value>
        {
            ReadTupleVal(std::get<I>(t));
            ReadTuple<I + 1>(t);
        }

        template<typename T>
        auto ReadTupleVal(T& t) -> std::enable_if_t<tuple_contains<T, OtherTuple>::value>
        {
            t = std::get<T>(tuple_);
        }

        template<typename T>
        auto ReadTupleVal(T& t) -> std::enable_if_t<!tuple_contains<T, OtherTuple>::value>
        {
            ReadObject(t, std::false_type{});
        }

        template<typename T, typename BeginObject>
        auto ReadObject(T& t, BeginObject) -> std::enable_if_t<is_map_container_v<T>>
        {
            rd_.begin_object();
            size_t array_size = 0;
            rd_.begin_array(array_size);
            using first_element_t = decltype((*t.begin()).first) ;
            using second_element_t = decltype((*t.begin()).second);
            using first_val_t = std::decay_t<first_element_t>;
            using second_val_t = std::decay_t<second_element_t>;

            using val_t = std::pair<first_val_t, second_val_t>;

            for (size_t i = 0; i < array_size; ++i)
            {
                val_t pair{};
                ReadKV(pair.first, pair.second, BeginObject{});
                t.insert(pair);
            }
            rd_.end_object();
        }

        template<typename T>
        auto ReadObject(T& t, std::true_type) -> std::enable_if_t<is_pair_v<T>>
        {
            rd_.begin_object();
            ReadKV(t.first, t.second, std::true_type{});
            rd_.end_object();
        }

        template<typename T>
        auto ReadObject(T& t, std::false_type) -> std::enable_if_t<is_pair_v<T>>
        {
            ReadKV(t.first, t.second, std::false_type{});
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, BeginObjec) -> std::enable_if_t<has_only_insert_v<T>>
        {
            using element_t = decltype(*t.begin());
            using val_t = std::decay_t<element_t>;

            size_t array_size = 0;
            rd_.begin_array(array_size);
            for (size_t i = 0; i < array_size; ++i)
            {
                val_t val{};
                ReadObject(val, std::false_type{});
                std::fill_n(std::inserter(t, t.end()), 1, val);
            }
            rd_.end_array();
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, BeginObjec) -> std::enable_if_t<has_back_insert_v<T>>
        {
            using element_t = decltype(*t.begin());
            using val_t = std::decay_t<element_t>;

            size_t array_size = 0;
            rd_.begin_array(array_size);
            for (size_t i = 0; i < array_size; ++i)
            {
                val_t val{};
                ReadObject(val, std::false_type{});
                std::fill_n(std::back_inserter(t), 1, val);
            }
            rd_.end_array();
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, BeginObjec) -> std::enable_if_t<is_singlevalue_container_v<T> && !has_back_insert_v<T> && !has_only_insert_v<T>>
        {
            ReadArray(t, t.size());
        }

        template <typename T, size_t N, typename BeginObject>
        void ReadObject(T(&p)[N], BeginObject)
        {
            ReadFixedLengthArray(p, N);
        }

        template<typename T, size_t N, typename BeginObject>
        void ReadObject(std::array<T, N>& t, BeginObject)
        {
            ReadFixedLengthArray(t, t.size());
        }

        template <size_t N, typename BeginObject>
        void ReadObject(char(&p)[N], BeginObject)
        {
            std::string str;
            rd_.read(str);
            str.copy(p, N);
            p[str.length() >= N ? N - 1 : str.length()] = 0;
        }

        template<typename Array>
        void ReadArray(Array & v, size_t array_size)
        {
            using element_t = decltype(*t.begin());
            using val_t = std::decay_t<element_t>;

            rd_.begin_array(array_size);
            for (size_t i = 0; i < array_size; ++i)
            {
                val_t val{};
                ReadObject(val, std::false_type{});
                v.emplace_back(val);
            }
            rd_.end_array();
        }

        template<typename Array>
        void ReadFixedLengthArray(Array & v, size_t array_size)
        {
            rd_.begin_fixed_array(array_size);
            for (size_t i = 0; i < array_size; ++i)
            {
                ReadObject(v[i], std::false_type{});
            }
            rd_.end_fixed_array();
        }

        template<typename T, typename BeginObject>
        auto ReadObject(T& t, BeginObject) -> std::enable_if_t<is_basic_type_v<T>>
        {
            rd_.read(t);
        }

        template<typename BeginObject>
        auto ReadObject(date_time& t, BeginObject)
        {
            int64_t utc_millisecond = 0;
            rd_.read(utc_millisecond);
            t = date_time::from_utc_milliseconds(utc_millisecond);
        }

        template<typename T, typename BeginObject>
        auto ReadObject(T& t, BeginObject) -> std::enable_if_t<is_optional_v<T>>
        {
            using value_type = typename T::value_type;
            bool is_null = false;
            rd_.read(is_null);
            if (!is_null)
            {
                value_type v{};
                ReadObject(v, std::true_type{});
                t = v;
            }
        }

        template <typename T, typename BeginObject>
        auto ReadObject(T& val, BeginObject) ->std::enable_if_t<is_enum_type_v<T>>
        {
            using under_type = std::underlying_type_t<std::decay_t<T>>;

            under_type tmp_val{};
            rd_.read(tmp_val);
            val = static_cast<T>(tmp_val);
        }

        template<typename K, typename V>
        void ReadKV(K& k, V& v, std::false_type)
        {
            rd_.read_field_name(k);
            ReadObject(v, std::true_type{});
        }

        template<typename K, typename V>
        void ReadKV(K& k, V& v, std::true_type)
        {
            ReadObject(k, std::true_type{});
            ReadObject(v, std::true_type{});
        }
    };
}