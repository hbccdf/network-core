#pragma once
#include "../traits/traits.hpp"
#include "../meta/meta.hpp"
#include "../base/cast.hpp"

namespace cytx {

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
        auto ReadObject(T& t, val_t& val, BeginObjec) -> std::enable_if_t<is_user_class<T>::value>
        {
            auto m = t.Meta();
            ReadTuple(m, val, 0, std::false_type{});
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

                first_type f = cytx::util::cast<first_type>(rd_.first(it));
                second_type s{};
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
                ele_t el{};
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
                ele_t el{};
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
                ele_t el{};
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

        template<typename T, typename BeginObject>
        auto ReadObject(T& t, val_t& val, BeginObject) -> std::enable_if_t<is_optional<T>::value>
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
        auto ReadObject(T& t, val_t& val, BeginObject) ->std::enable_if_t<std::is_enum<
            std::remove_reference_t<std::remove_cv_t<T>>>::value>
        {
            using enum_t = std::remove_reference_t<std::remove_cv_t<T>>;
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
        auto process_array(val_t& val) -> std::enable_if_t<is_basic_type<std::decay_t<T>>::value
            || std::is_enum<std::decay_t<T>>::value>
        {
            rd_.process_array(val);
        }

        template<typename T>
        auto process_array(val_t& val) -> std::enable_if_t<!is_basic_type<std::decay_t<T>>::value
            && !std::is_enum<std::decay_t<T>>::value>
        {
        }

    private:
        adapter_t rd_;
        OtherTuple tuple;
        bool enum_with_str_ = false;
    };
}

