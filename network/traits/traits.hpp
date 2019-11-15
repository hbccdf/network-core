#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <array>
#include <stack>
#include <set>
#include <map>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/remove.hpp>
#include "function_traits.hpp"

namespace cytx {

#define TEMPLATE_VALUE(token) \
    template<typename T> constexpr bool token ##_v = token<T>::value

#define TEMPLATE_TYPE(token) \
    template<typename T> using token ##_t = typename token<T>::type;

    template <typename ... Args>
    struct variant : boost::variant<boost::blank, Args...>
    {
        using base_type = boost::variant<boost::blank, Args...>;
        using types = typename boost::mpl::remove<typename base_type::types, boost::blank>::type;

        explicit operator bool() const
        {
            return this->which() != 0;
        }

        template <typename T>
        variant& operator= (T&& t)
        {
            return static_cast<variant&>(
                static_cast<base_type&>(*this) = std::forward<T>(t));
        }

        bool operator== (variant const& other) const
        {
            return static_cast<base_type const&>(*this) == static_cast<base_type const&>(other);
        }

        bool operator!= (variant const& other) const
        {
            return !(*this == other);
        }

        bool operator< (variant const& other) const
        {
            return static_cast<base_type const&>(*this) < static_cast<base_type const&>(other);
        }

        bool operator> (variant const& other) const
        {
            return other < *this;
        }

        bool operator>= (variant const& other) const
        {
            return !(*this < other);
        }

        bool operator<= (variant const& other) const
        {
            return !(other < *this);
        }
    };

    namespace detail
    {
        template<typename T>
        struct has_begin_end
        {
        private:
            template<typename U> static auto Check(int) -> decltype(std::declval<U>().begin(), std::declval<U>().end(), std::true_type());
            template<typename U> static std::false_type Check(...);

        public:
            enum
            {
                value = std::is_same<decltype(Check<T>(0)), std::true_type>::value
            };
        };

        TEMPLATE_VALUE(has_begin_end);

        template <typename T>
        struct has_const_iterator
        {
        private:
            template<typename C> static std::true_type Check(typename C::const_iterator*);
            template<typename C> static std::false_type  Check(...);
        public:
            enum
            {
                value = std::is_same<decltype(Check<T>(0)), std::true_type>::value
            };
        };

        TEMPLATE_VALUE(has_const_iterator);

        template <typename T>
        struct has_mapped_type
        {
        private:
            template<typename C> static std::true_type Check(typename C::mapped_type*);
            template<typename C> static std::false_type  Check(...);
        public:
            enum
            {
                value = std::is_same<decltype(Check<T>(0)), std::true_type>::value
            };
        };

        TEMPLATE_VALUE(has_mapped_type);

        template<typename T> struct is_poiner_extent : std::false_type {};
        template<typename T> struct is_poiner_extent<std::shared_ptr<T>> : std::true_type {};
        template<typename T> struct is_poiner_extent<std::unique_ptr<T>> : std::true_type {};
        template<typename T> struct is_poiner_extent<std::weak_ptr<T>> : std::true_type {};

        TEMPLATE_VALUE(is_poiner_extent);
    }

    template <typename T, template <typename...> class Template>
    struct is_specialization_of : std::false_type {};

    template <template <typename...> class Template, typename... Args>
    struct is_specialization_of<Template<Args...>, Template> : std::true_type {};

#define IS_TEMPLATE_CLASS(token) \
    template<typename T> struct is_##token : is_specialization_of<std::decay_t<T>, std::token>{}; \
    template<typename T> constexpr bool is_##token ##_v = is_##token<T>::value

#define IS_SAME_TYPE(token) \
    template<typename T> struct is_##token ##_type : std::integral_constant<bool, std::is_same<token, std::decay_t<T>>::value> {}; \
    template<typename T> constexpr bool is_##token ##_type_v = is_##token ##_type<T>::value


    IS_TEMPLATE_CLASS(vector);
    IS_TEMPLATE_CLASS(tuple);
    IS_TEMPLATE_CLASS(queue);
    IS_TEMPLATE_CLASS(stack);
    IS_TEMPLATE_CLASS(set);
    IS_TEMPLATE_CLASS(map);
    IS_TEMPLATE_CLASS(pair);
    IS_TEMPLATE_CLASS(list);
    IS_TEMPLATE_CLASS(multiset);
    IS_TEMPLATE_CLASS(unordered_set);
    IS_TEMPLATE_CLASS(unordered_map);
    IS_TEMPLATE_CLASS(priority_queue);


    template<typename T>
    using nullable = boost::optional<T>;

    template<typename T> struct is_nullable : is_specialization_of<std::decay_t<T>, boost::optional> {};
    template <typename T> struct is_optional : is_specialization_of<std::decay_t<T>, boost::optional> {};
    template <typename T> struct is_variant : is_specialization_of<std::decay_t<T>, variant> {};

    TEMPLATE_VALUE(is_nullable);
    TEMPLATE_VALUE(is_optional);
    TEMPLATE_VALUE(is_variant);


    template <typename Arary>
    struct is_std_array : std::false_type {};

    template <typename T, std::size_t N>
    struct is_std_array<std::array<T, N>> : std::true_type {};

    TEMPLATE_VALUE(is_std_array);

    template<typename T>
    struct is_string : std::integral_constant<bool, std::is_same<std::decay_t<T>, std::string>::value> {};

    TEMPLATE_VALUE(is_string);

    template <typename T>
    struct is_container : public std::integral_constant<bool, detail::has_const_iterator_v<std::decay_t<T>> && detail::has_begin_end_v<std::decay_t<T>> && !is_string_v<T>> {};

    TEMPLATE_VALUE(is_container);

    template <typename T>
    struct is_singlevalue_container : public std::integral_constant<bool, !is_std_array_v<T> && !std::is_array<std::decay_t<T>>::value && !is_tuple_v<T> && is_container_v<T> && !detail::has_mapped_type_v<std::decay_t<T>>> {};

    TEMPLATE_VALUE(is_singlevalue_container);

    template <typename T>
    struct is_map_container : public std::integral_constant<bool, is_container_v<T> && detail::has_mapped_type_v<std::decay_t<T>>> {};

    TEMPLATE_VALUE(is_map_container);

    template<typename T>
    struct is_normal_class : std::integral_constant<bool, std::is_class<std::decay_t<T>>::value && !is_string_v<T>> {};

    TEMPLATE_VALUE(is_normal_class);

    template<typename T>
    struct is_basic_type : std::integral_constant<bool, std::is_arithmetic<std::decay_t<T>>::value || is_string_v<T>> {};

    TEMPLATE_VALUE(is_basic_type);

    template<typename T>
    struct is_enum_type : std::integral_constant<bool, std::is_enum<std::decay_t<T>>::value> {};

    TEMPLATE_VALUE(is_enum_type);

    template<typename T>
    struct is_number_type : std::integral_constant<bool, std::is_arithmetic<std::decay_t<T>>::value> {};

    TEMPLATE_VALUE(is_number_type);

    IS_SAME_TYPE(bool);

    template<typename T>
    struct is_smart_pointer : detail::is_poiner_extent<std::decay_t<T>> {};

    TEMPLATE_VALUE(is_smart_pointer);

    template<typename T>
    struct is_pointer_ext : std::integral_constant<bool, std::is_pointer<std::decay_t<T>>::value || is_smart_pointer_v<std::decay_t<T>>> {};

    TEMPLATE_VALUE(is_pointer_ext);


#define HAS_TYPE(token) \
    template <typename T>struct has_##token{ \
    private: \
    template<typename C> static std::true_type Check(typename C::token##*); \
        template<typename C> static std::false_type  Check(...); \
    public: \
        enum \
        { \
            value = std::is_same<decltype(Check<T>(0)), std::true_type>::value \
        }; \
    };  \
    template<typename T> \
    constexpr bool has_##token##_v = has_##token<std::decay_t<T>>::value;

#define HAS_FUNC(token, ...) \
    template <typename T> \
    struct has_##token \
    {   \
    private:    \
        template <typename P, typename = decltype(std::declval<P>().token(__VA_ARGS__))> \
        static std::true_type test(int); \
        template <typename P> \
        static std::false_type test(...); \
        using result_type = decltype(test<T>(0)); \
    public: \
        constexpr static const bool value = result_type::value; \
    }; \
    template<typename T> \
    constexpr bool has_##token##_v = has_##token<std::decay_t<T>>::value;

    template <typename T>
    struct has_meta_macro
    {
    private:
        template <typename P, typename = decltype(std::declval<P>().Meta())>
        static std::true_type test(int);
        template <typename P>
        static std::false_type test(...);
        using result_type = decltype(test<T>(0));
    public:
        constexpr static const bool value = result_type::value;
    };
    template<typename T>
    constexpr bool has_meta_macro_v = has_meta_macro<std::decay_t<T>>::value;

    template<typename T>
    struct is_container_adapter : std::integral_constant<bool, is_queue_v<T> || is_priority_queue_v<T>> {};

    TEMPLATE_VALUE(is_container_adapter);

    template<typename T>
    struct is_user_class : std::integral_constant<bool, is_normal_class_v<T> && !is_container_adapter_v<T>
        && !is_stack_v<T> && !is_container_v<T> && !is_tuple_v<T> && !is_pair_v<T> && !is_optional_v<T>> {};

    TEMPLATE_VALUE(is_user_class);

    template<typename T>
    struct has_back_insert : std::integral_constant<bool, is_vector_v<T> || is_list_v<T> || is_queue_v<T> || is_stack_v<T> || is_priority_queue_v<T>> {};

    TEMPLATE_VALUE(has_back_insert);

    template<typename T>
    struct has_only_insert : std::integral_constant<bool, is_set_v<T> || is_multiset_v<T> || is_unordered_set_v<T>> {};

    TEMPLATE_VALUE(has_only_insert);


    template < typename T, typename... List >
    struct tuple_contains;

    template < typename T, typename Head, typename... Rest >
    struct tuple_contains<T, Head, Rest...>
        : std::conditional< std::is_same<std::decay_t<T>, std::decay_t<Head>>::value, std::true_type, tuple_contains<T, Rest... >> ::type {};

    template<typename T, typename Arg0, typename ... Args>
    struct tuple_contains<T, std::tuple<Arg0, Args...>>
        : std::conditional< std::is_same<std::decay_t<T>, std::decay_t<Arg0>>::value, std::true_type, tuple_contains<T, Args... >> ::type {};

    template<typename T>
    struct tuple_contains<T, std::tuple<>> : std::false_type {};

    template < typename T >
    struct tuple_contains<T> : std::false_type {};

    template <typename T, typename ... List>
    struct tuple_index;

    template<typename T, typename Arg0, typename ... Args>
    struct tuple_index<T, std::tuple<Arg0, Args...>>
    {
        enum {
            value = std::is_same<std::decay_t<T>, std::decay_t<Arg0>>::value ? 0 :
            (tuple_index<T, std::tuple<Args...>>::value >= 0 ? tuple_index<T, std::tuple<Args...>>::value + 1 : -1)
        };
    };

    template<typename T>
    struct tuple_index<T, std::tuple<>>
    {
        enum { value = -1 };
    };

    template<typename T>
    struct tuple_index<T>
    {
        enum { value = -1 };
    };

    namespace detail
    {
        template<typename T, class = std::void_t<>>
        struct tuple_total_size
        {
            enum { value = -1 };
        };

        template<> struct tuple_total_size<bool> { enum { value = 1 }; };
        template<> struct tuple_total_size<char> { enum { value = 1 }; };
        template<> struct tuple_total_size<unsigned char> { enum { value = 1 }; };
        template<> struct tuple_total_size<short> { enum { value = 2 }; };
        template<> struct tuple_total_size<unsigned short> { enum { value = 2 }; };
        template<> struct tuple_total_size<int> { enum { value = 4 }; };
        template<> struct tuple_total_size<unsigned int> { enum { value = 4 }; };
        template<> struct tuple_total_size<int64_t> { enum { value = 8 }; };
        template<> struct tuple_total_size<uint64_t> { enum { value = 8 }; };
        template<> struct tuple_total_size<float> { enum { value = 4 }; };
        template<> struct tuple_total_size<double> { enum { value = 8 }; };
        template<typename T, size_t N> struct tuple_total_size<std::array<T, N>> { enum { value = tuple_total_size<T[N]>::value }; };
        template<typename T, size_t N> struct tuple_total_size<T[N]> { enum { value = tuple_total_size<T>::value < 0 ? -1 : tuple_total_size<T>::value * (int)N }; };

        template<>
        struct tuple_total_size<std::tuple<>>
        {
            enum { value = 0 };
        };

        template<typename Arg0, typename ... Args>
        struct tuple_total_size<std::tuple<Arg0, Args ...>>
        {
            constexpr static const int first_value = tuple_total_size<Arg0>::value;
            constexpr static const int other_value = tuple_total_size<std::tuple<Args...>>::value;
            enum { value = first_value < 0 || other_value < 0 ? -1 : first_value + other_value };
        };

        template<typename T> struct tuple_total_size<T, std::void_t<std::enable_if_t<std::is_enum<std::decay_t<T>>::value>>>
        {
            enum { value = tuple_total_size<std::underlying_type_t<std::decay_t<T>>>::value };
        };

        template<typename K, typename V>
        struct tuple_total_size<std::pair<K, V>>
        {
            enum { value = tuple_total_size<std::tuple<K, V>>::value };
        };
    }

    template<typename ... Args>
    using tuple_total_size = detail::tuple_total_size<std::tuple<Args ...>>;

    template<typename ... Args>
    constexpr int tuple_total_size_v = detail::tuple_total_size<std::tuple<Args ...>>::value;
}
