#pragma once
#include <fmt/format.h>
#include "network/base/GameObjectStream.h"
#include "network/util/net.hpp"

namespace cytx
{
    class gos_serialize_adapter
    {
    public:
        gos_serialize_adapter(GameObjectStream& gos, bool is_big_endian = false) : gos_(gos), is_big_endian_(is_big_endian) {}
        enum { use_field_separator = 0, use_begin_map = 1, ralax_check_enum = 1 };
    public:
        void begin_serialize(const std::string& key = "") {  }
        void end_serialize() {}
        void begin_object() {  }
        void end_object() {  }
        void begin_array(size_t array_size) { gos_.pushInt(gos_hton((int)array_size)); }
        void end_array() {  }
        void begin_fixed_array(size_t array_size) {}
        void end_fixed_array() {}

        void write(const std::string& str) { gos_.pushString(str); }
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

        template<typename T> std::enable_if_t<std::is_same<T, bool>::value> write(T t)
        {
            gos_.pushBool(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, char>::value> write(T t)
        {
            gos_.pushChar(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, unsigned char>::value> write(T t)
        {
            gos_.pushUChar(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, short>::value> write(T t)
        {
            gos_.pushShort(gos_hton(t));
        }
        template<typename T> std::enable_if_t<std::is_same<T, unsigned short>::value> write(T t)
        {
            gos_.pushUShort(gos_hton(t));
        }
        template<typename T> std::enable_if_t<std::is_same<T, int>::value> write(T t)
        {
            gos_.pushInt(gos_hton(t));
        }
        template<typename T> std::enable_if_t<std::is_same<T, unsigned int>::value> write(T t)
        {
            gos_.pushUInt(gos_hton(t));
        }
        template<typename T> std::enable_if_t<std::is_same<T, boost::uint32_t>::value && !std::is_same<unsigned int, boost::uint32_t>::value> write(T t)
        {
            gos_.pushUint32(gos_hton(t));
        }
        template<typename T> std::enable_if_t<std::is_same<T, float>::value> write(T t)
        {
            gos_.pushFloat(gos_hton(t));
        }
        template<typename T> std::enable_if_t<std::is_same<T, double>::value> write(T t)
        {
            gos_.pushDouble(gos_hton(t));
        }
        template<typename T> std::enable_if_t<std::is_same<T, int64_t>::value> write(T t)
        {
            gos_.pushInt64(gos_hton(t));
        }

    private:
        template<typename T>
        T gos_hton(T t)
        {
            return is_big_endian_ ? cytx::util::hton(t) : t;
        }
    private:
        GameObjectStream& gos_;
        bool is_big_endian_;
    };

    class gos_deserialize_adapter
    {
    public:
        gos_deserialize_adapter(GameObjectStream& gos, bool is_big_endian = false) : gos_(gos), is_big_endian_(is_big_endian) {}
    public:
        void begin_deserialize() {  }
        void end_deserialize() {}
        void begin_object() {  }
        void end_object() {  }
        void begin_array(size_t& array_size)
        {
            int length = 0;
            gos_.getInt(length);
            length = gos_ntoh(length);
            array_size = length;
        }
        void end_array() {  }

        void begin_fixed_array(size_t array_size) {}
        void end_fixed_array() {}

        void read(std::string& str) { gos_.getString(str); }
        void read_null() {  }

        void read_field_name(const std::string& str) {  }

        template<typename T>
        void read_key(T&& t)
        {
            read(std::forward<T>(t));
        }

        template<typename T> std::enable_if_t<std::is_same<T, bool>::value> read(T& t)
        {
            gos_.getBool(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, char>::value> read(T& t)
        {
            gos_.getChar(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, unsigned char>::value> read(T& t)
        {
            gos_.getUChar(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, short>::value> read(T& t)
        {
            gos_.getShort(t);
            t = gos_ntoh(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, unsigned short>::value> read(T& t)
        {
            gos_.getUShort(t);
            t = gos_ntoh(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, int>::value> read(T& t)
        {
            gos_.getInt(t);
            t = gos_ntoh(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, unsigned int>::value> read(T& t)
        {
            gos_.getUInt(t);
            t = gos_ntoh(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, float>::value> read(T& t)
        {
            gos_.getFloat(t);
            t = gos_ntoh(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, double>::value> read(T& t)
        {
            gos_.getDouble(t);
            t = gos_ntoh(t);
        }
        template<typename T> std::enable_if_t<std::is_same<T, int64_t>::value> read(T& t)
        {
            gos_.getInt64(t);
            t = gos_ntoh(t);
        }

    private:
        template<typename T>
        T gos_ntoh(T t)
        {
            return is_big_endian_ ? cytx::util::ntoh(t) : t;
        }
    private:
        GameObjectStream& gos_;
        bool is_big_endian_;
    };

    template<typename OtherTuple>
    class DeSerializer<gos_deserialize_adapter, OtherTuple> : boost::noncopyable
    {
        typedef gos_deserialize_adapter adapter_t;
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

        void set_tuple(OtherTuple&& t)
        {
            tuple = std::move(t);
        }

        adapter_t& get_adapter() { return rd_; }

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
            return std::get<tuple_index<T, OtherTuple>::value>(tuple);
        }

        template<typename T>
        auto get_tuple_elem() -> std::enable_if_t<!tuple_contains<T, OtherTuple>::value, T>
        {
            T t{};
            ReadObject(t, std::true_type{});
            return std::move(t);
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, BeginObjec) -> std::enable_if_t<is_user_class<T>::value>
        {
            rd_.begin_object();
            auto target_meta = get_meta(t);
            ReadTuple(target_meta);
            rd_.end_object();
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, BeginObjec) -> std::enable_if_t<is_tuple<T>::value>
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
            t = std::get<T>(tuple);
        }

        template<typename T>
        auto ReadTupleVal(T& t) -> std::enable_if_t<!tuple_contains<T, OtherTuple>::value>
        {
            ReadObject(t, std::false_type{});
        }

        template<typename T, typename BeginObject>
        auto ReadObject(T& t, BeginObject) -> std::enable_if_t<is_map_container<T>::value>
        {
            rd_.begin_object();
            size_t array_size = 0;
            rd_.begin_array(array_size);
            typedef decltype((*t.begin()).first) first_element_t;
            typedef decltype((*t.begin()).second) second_element_t;
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
        auto ReadObject(T& t, std::true_type) -> std::enable_if_t<is_pair<T>::value>
        {
            rd_.begin_object();
            ReadKV(t.first, t.second, std::true_type{});
            rd_.end_object();
        }

        template<typename T>
        auto ReadObject(T& t, std::false_type) -> std::enable_if_t<is_pair<T>::value>
        {
            ReadKV(t.first, t.second, std::false_type{});
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, BeginObjec) -> std::enable_if_t<has_only_insert<T>::value>
        {
            typedef decltype(*t.begin()) element_t;
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
        auto ReadObject(T& t, BeginObjec) -> std::enable_if_t<has_back_insert<T>::value>
        {
            typedef decltype(*t.begin()) element_t;
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
        auto ReadObject(T& t, BeginObjec) -> std::enable_if_t<is_singlevalue_container<T>::value && !has_back_insert<T>::value && !has_only_insert<T>::value>
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
            typedef decltype((v)[0]) element_t;
            using val_t = std::remove_reference_t<element_t>;

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
        auto ReadObject(T& t, BeginObject) -> std::enable_if_t<is_basic_type<T>::value>
        {
            rd_.read(t);
        }

        template<typename T, typename BeginObject>
        auto ReadObject(T& t, BeginObject) -> std::enable_if_t<is_optional<T>::value>
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
        auto ReadObject(T& val, BeginObject) ->std::enable_if_t<std::is_enum<std::decay_t<T>>::value>
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

    private:
        adapter_t rd_;
        OtherTuple tuple;
    };
}