﻿#pragma once
#include "network/traits/traits.hpp"
#include "network/meta/meta.hpp"
#include "network/util/cast.hpp"

namespace cytx {

    template<typename ADAPTER_T, typename OtherTuple>
    class BaseDeSerializer : public boost::noncopyable
    {
        using adapter_t = ADAPTER_T;
    public:
        template<typename... ARGS>
        BaseDeSerializer(ARGS&&... args)
            : rd_(std::forward<ARGS>(args)...)
        {}
        template<typename... ARGS>
        BaseDeSerializer(OtherTuple&& t, ARGS&&... args)
            : tuple_(std::move(t))
            , rd_(std::forward<ARGS>(args)...)
        {}

        bool enum_with_str() const { return enum_with_str_; }
        void enum_with_str(bool val) { enum_with_str_ = val; }

        void set_tuple(OtherTuple&& t)
        {
            tuple_ = std::move(t);
        }

        adapter_t& get_adapter() { return rd_; }

        const adapter_t& get_adapter() const { return rd_; }

    protected:
        bool enum_with_str_ = false;
        OtherTuple tuple_;
        adapter_t rd_;
    };

    template<typename ADAPTER_T, typename OtherTuple = std::tuple<>>
    class DeSerializer : public BaseDeSerializer<ADAPTER_T, OtherTuple>
    {
        using base_t = BaseDeSerializer<ADAPTER_T, OtherTuple>;
        using adapter_t = ADAPTER_T;
        using val_t = typename adapter_t::value_t;
    public:

        template<typename... ARGS>
        DeSerializer(ARGS&&... args)
            : base_t(std::forward<ARGS>(args)...)
        {
        }

        ~DeSerializer()
        {
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
        auto make(std::true_type, ReturnT&& t, size_t& index, val_t& val)
        {
            using value_type = std::conditional_t<(Is + 1) == std::tuple_size<T>::value, std::false_type, std::true_type>;
            using elem_t = std::tuple_element_t<Is, T>;
            return make<T, Is + 1>(value_type{}, std::tuple_cat(t, std::tuple<elem_t>(get_tuple_elem<elem_t>(index, val))), index, val);
        }

        template <typename T, size_t Is, typename ReturnT>
        auto make(std::false_type, ReturnT&& t, size_t& index, val_t& val)
        {
            return t;
        }

        template<typename T>
        auto get_tuple_elem(size_t& index, val_t& val) -> std::enable_if_t<tuple_contains<T, OtherTuple>::value, T>
        {
            return std::get<T>(tuple_);
        }

        template<typename T>
        auto get_tuple_elem(size_t& index, val_t& val) -> std::enable_if_t<!tuple_contains<T, OtherTuple>::value, T>
        {
            if (index == 0)
                process_array<T>(val);

            T t{};
            auto array_size = rd_.array_size(val);
            if (array_size <= index)
                return t;

            auto& ele_val = rd_.it_val(rd_.array_it(val, index));
            ReadObject(t, ele_val, std::true_type{});
            ++index;
            return t;
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, val_t& val, BeginObjec) -> std::enable_if_t<is_user_class_v<T> && !is_date_time_type_v<T>>
        {
            auto m = get_meta(t);
            ReadTuple(m, val, 0, std::false_type{});
        }

        template<typename T, typename BeginObject>
        auto ReadObject(T& t, val_t& val, BeginObject bo) -> std::enable_if_t<is_map_container_v<T>>
        {
            auto it = rd_.member_begin(val);
            auto it_end = rd_.member_end(val);
            for (; it != it_end; ++it)
            {
                using element_t = decltype(*t.begin());
                using pair_t = std::decay_t<element_t>;
                using first_type = std::decay_t<typename pair_t::first_type>;
                using second_type = typename pair_t::second_type;

                first_type f = cytx::util::cast<first_type>(rd_.first(it));
                second_type s{};
                ReadObject(s, rd_.second(it), bo);
                t[f] = s;
            }
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, val_t& val, BeginObjec) -> std::enable_if_t<has_only_insert_v<T>>
        {
            using element_t = decltype(*t.begin());
            using ele_t = std::decay_t<element_t>;

            process_array<ele_t>(val);
            auto it = rd_.array_begin(val);
            auto it_end = rd_.array_end(val);
            for (; it != it_end; ++it)
            {
                if (!rd_.is_valid(it))
                    continue;

                ele_t el{};
                ReadObject(el, rd_.it_val(it), std::false_type{});
                std::fill_n(std::inserter(t, t.end()), 1, std::move(el));
            }
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, val_t& val, BeginObjec) -> std::enable_if_t<has_back_insert_v<T>>
        {
            using element_t = decltype(*t.begin());
            using ele_t = std::decay_t<element_t>;

            process_array<ele_t>(val);
            auto it = rd_.array_begin(val);
            auto it_end = rd_.array_end(val);
            for (; it != it_end; ++it)
            {
                if(!rd_.is_valid(it))
                    continue;

                ele_t el{};
                ReadObject(el, rd_.it_val(it), std::false_type{});
                std::fill_n(std::back_inserter(t), 1, std::move(el));
            }
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, val_t& val, BeginObjec) -> std::enable_if_t<is_singlevalue_container_v<T> && !has_back_insert_v<T> && !has_only_insert_v<T>>
        {
            ReadArray(t, val);
        }

        template<typename T, typename BeginObjec>
        auto ReadObject(T& t, val_t& val, BeginObjec bo) -> std::enable_if_t<is_tuple_v<T>>
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
            t = std::get<T>(tuple_);
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
        auto ReadObject(T& t, val_t& val, std::true_type bo) -> std::enable_if_t<is_pair_v<T>>
        {
            using pair_t = std::decay_t<T> ;
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
        auto ReadObject(T& t, val_t& val, std::false_type) -> std::enable_if_t<is_pair_v<T>>
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
        void ReadArray(Array & t, val_t& val)
        {
            using element_t = decltype((t)[0]);
            using ele_t = std::decay_t<element_t>;

            process_array<ele_t>(val);
            auto it = rd_.array_begin(val);
            auto it_end = rd_.array_end(val);
            for (; it != it_end; ++it)
            {
                if (!rd_.is_valid(it))
                    continue;

                ele_t el{};
                ReadObject(el, rd_.it_val(it), std::false_type{});
                t.emplace_back(el);
            }
        }

        template<typename Array>
        void ReadFixedLengthArray(Array & v, size_t array_size, val_t& val)
        {
            process_array<decltype((v)[0])>(val);
            auto it = rd_.array_begin(val);
            auto it_end = rd_.array_end(val);
            for (size_t i = 0; i < array_size && it != it_end; ++it)
            {
                if (!rd_.is_valid(it))
                    continue;

                ReadObject(v[i], rd_.it_val(it), std::false_type{});
                ++i;
            }
        }

        template<typename T, typename BeginObject>
        auto ReadObject(T& t, val_t& val, BeginObject) -> std::enable_if_t<is_basic_type_v<T>>
        {
            rd_.read(t, val);
        }

        template<typename BeginObject>
        void ReadObject(date_time& t, val_t& val, BeginObject)
        {
            std::string str;
            rd_.read(str, val);
            t = util::cast<date_time>(str);
        }


        template<typename T, typename BeginObject>
        auto ReadObject(T& t, val_t& val, BeginObject) -> std::enable_if_t<is_optional_v<T>>
        {
            using value_type = typename T::value_type;
            if (!rd_.is_null(val))
            {
                value_type v{};
                ReadObject(v, val, std::true_type{});
                t = v;
            }
        }

        template <typename T, typename BeginObject>
        auto ReadObject(T& t, val_t& val, BeginObject) ->std::enable_if_t<is_enum_type_v<T>>
        {
            using enum_t = std::decay_t<T>;
            using under_type = std::underlying_type_t<enum_t>;
            if (!enum_with_str_)
            {
                rd_.read(reinterpret_cast<under_type&>(t), val);
            }
            else
            {
                std::string str;
                rd_.read(str, val);
                t = cytx::util::cast<enum_t>(str);
            }
        }

        template<typename T>
        auto process_array(val_t& val) -> std::enable_if_t<is_basic_type_v<T> || is_enum_type_v<T> || is_date_time_type_v<T>>
        {
            rd_.process_array(val);
        }

        template<typename T>
        auto process_array(val_t& val) -> std::enable_if_t<!(is_basic_type_v<T> || is_enum_type_v<T> || is_date_time_type_v<T>)>
        {
        }
    };
}

