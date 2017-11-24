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
        using decay_t = typename std::decay<T>::type;

        //template<typename T> std::false_type static check_tuple_size(...);
        //template<typename T> std::true_type static  check_tuple_size(decltype(std::tuple_size<T>::value)*);
        template < template <typename...> class U, typename T >
        struct is_instantiation_of : std::false_type {};

        template < template <typename...> class U, typename... Args >
        struct is_instantiation_of< U, U<Args...> > : std::true_type {};

        template<typename T>
        struct is_tuple : is_instantiation_of<std::tuple, T>
        {
        };

        //has_begin_end
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

        template<typename T> struct is_poiner_extent : std::false_type {};
        template<typename T> struct is_poiner_extent<std::shared_ptr<T>> : std::true_type {};
        template<typename T> struct is_poiner_extent<std::unique_ptr<T>> : std::true_type {};
        template<typename T> struct is_poiner_extent<std::weak_ptr<T>> : std::true_type {};

        //#define IS_SMART_POINTER(token)
        //	template<typename T> struct is_poiner_extent<std::token##_ptr<T>> : std::true_type{};
        //
        //	IS_SMART_POINTER(shared)
        //	IS_SMART_POINTER(unique)
        //	IS_SMART_POINTER(weak)
    }

    template <typename Arary>
    struct is_std_array : std::false_type {};

    template <typename T, std::size_t N>
    struct is_std_array<std::array<T, N>> : std::true_type {};

    template<typename T>
    struct is_string : std::integral_constant<bool, std::is_same<detail::decay_t<T>, std::string>::value> {};

    template <typename T>
    struct is_container : public std::integral_constant<bool, detail::has_const_iterator<detail::decay_t<T>>::value&&detail::has_begin_end<detail::decay_t<T>>::value && !is_string<T>::value> {};

    template <typename T>
    struct is_singlevalue_container : public std::integral_constant<bool, !is_std_array<T>::value && !std::is_array<detail::decay_t<T>>::value && !detail::is_tuple<detail::decay_t<T>>::value && is_container<detail::decay_t<T>>::value && !detail::has_mapped_type<detail::decay_t<T>>::value> {};

    template <typename T>
    struct is_map_container : public std::integral_constant<bool, is_container<detail::decay_t<T>>::value&&detail::has_mapped_type<detail::decay_t<T>>::value> {};

    template<typename T>
    struct is_normal_class : std::integral_constant<bool, std::is_class<detail::decay_t<T>>::value && !is_string<T>::value>
    {};

    template<typename T>
    struct is_basic_type : std::integral_constant<bool, std::is_arithmetic<detail::decay_t<T>>::value || is_string<T>::value>
    {};

    template<typename T>
    struct is_smart_pointer : detail::is_poiner_extent<detail::decay_t<T>> {};

    template<typename T>
    struct is_pointer_ext : std::integral_constant<bool, std::is_pointer<detail::decay_t<T>>::value || is_smart_pointer<detail::decay_t<T>>::value> {};

    template <typename T, template <typename...> class Template>
    struct is_specialization_of : std::false_type {};

    template <template <typename...> class Template, typename... Args>
    struct is_specialization_of<Template<Args...>, Template> : std::true_type {};


    template <typename T> struct is_variant : is_specialization_of<detail::decay_t<T>, variant> {};
    template <typename T> struct is_optional : is_specialization_of<detail::decay_t<T>, boost::optional> {};

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
    constexpr bool has_##token##_v = has_##token<T>::value;

#define HAS_FUNC(token) \
    template <typename T> \
    struct has_##token \
    {   \
    private:    \
        template <typename P, typename = decltype(std::declval<P>().##token())> \
        static std::true_type test(int); \
        template <typename P> \
        static std::false_type test(...); \
        using result_type = decltype(test<T>(0)); \
    public: \
        static constexpr bool value = result_type::value; \
    }; \
    template<typename T> \
    constexpr bool has_##token##_v = has_##token<T>::value;

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
        static constexpr bool value = result_type::value;
    };
    template<typename T>
    constexpr bool has_meta_macro_v = has_meta_macro<T>::value;

#define IS_TEMPLATE_CLASS(token) \
template<typename T> struct is_##token : is_specialization_of<detail::decay_t<T>, std::token>{}; \

    IS_TEMPLATE_CLASS(vector)
    IS_TEMPLATE_CLASS(tuple)
    IS_TEMPLATE_CLASS(queue)
    IS_TEMPLATE_CLASS(stack)
    IS_TEMPLATE_CLASS(set)
    IS_TEMPLATE_CLASS(map)
    IS_TEMPLATE_CLASS(multiset)
    IS_TEMPLATE_CLASS(unordered_set)
    IS_TEMPLATE_CLASS(unordered_map)
    IS_TEMPLATE_CLASS(priority_queue)
    IS_TEMPLATE_CLASS(pair)
    IS_TEMPLATE_CLASS(list)

    template<typename T>
    struct is_container_adapter : std::integral_constant<bool, is_queue<T>::value || is_priority_queue<T>::value>
    {
    };

    template<typename T>
    struct is_user_class : std::integral_constant<bool, is_normal_class<T>::value && !is_container_adapter<T>::value
        && !is_stack<T>::value && !is_container<T>::value && !is_tuple<T>::value && !is_pair<T>::value && !is_optional<T>::value>
    {};

    template<typename T>
    struct has_back_insert : std::integral_constant<bool, is_vector<T>::value || is_list<T>::value || is_queue<T>::value || is_stack<T>::value || is_priority_queue<T>::value>
    {};

    template<typename T>
    struct has_only_insert : std::integral_constant<bool, is_set<T>::value || is_multiset<T>::value || is_unordered_set<T>::value>
    {};


    template < typename T, typename... List >
    struct tuple_contains;

    template < typename T, typename Head, typename... Rest >
    struct tuple_contains<T, Head, Rest...>
        : std::conditional< std::is_same<std::decay_t<T>, std::decay_t<Head>>::value, std::true_type, tuple_contains<T, Rest... >> ::type {};

    template<typename T, typename Arg0, typename ... Args>
    struct tuple_contains<T, std::tuple<Arg0, Args...>>
        : std::conditional< std::is_same<std::decay_t<T>, std::decay_t<Arg0>>::value, std::true_type, tuple_contains<T, Args... >> ::type {};

    template<typename T>
    struct tuple_contains<T, std::tuple<>>
        : std::false_type {};

    template < typename T >
    struct tuple_contains<T> : std::false_type {};

    template <typename T, typename ... List>
    struct tuple_index;

    template<typename T, typename Arg0, typename ... Args>
    struct tuple_index<T, std::tuple<Arg0, Args...>>
    {
        constexpr static int value = std::is_same<std::decay_t<T>, std::decay_t<Arg0>>::value ? 0 :
            (tuple_index<T, std::tuple<Args...>>::value >= 0 ? tuple_index<T, std::tuple<Args...>>::value + 1 : -1);
    };

    template<typename T>
    struct tuple_index<T, std::tuple<>>
    {
        constexpr static int value = -1;
    };

    template<typename T>
    struct tuple_index<T>
    {
        constexpr static int value = -1;
    };


    template<typename T, class = std::void_t<>>
    struct tuple_total_size_impl
    {
        constexpr static int value = -1;
    };

    template<typename T>
    struct tuple_total_size
    {
        constexpr static int value = tuple_total_size_impl<T>::value;
    };

    template<> struct tuple_total_size<bool> { constexpr static int value = 1; };
    template<> struct tuple_total_size<char> { constexpr static int value = 1; };
    template<> struct tuple_total_size<unsigned char> { constexpr static int value = 1; };
    template<> struct tuple_total_size<short> { constexpr static int value = 2; };
    template<> struct tuple_total_size<unsigned short> { constexpr static int value = 2; };
    template<> struct tuple_total_size<int> { constexpr static int value = 4; };
    template<> struct tuple_total_size<unsigned int> { constexpr static int value = 4; };
    template<> struct tuple_total_size<int64_t> { constexpr static int value = 8; };
    template<> struct tuple_total_size<uint64_t> { constexpr static int value = 8; };
    template<> struct tuple_total_size<float> { constexpr static int value = 4; };
    template<> struct tuple_total_size<double> { constexpr static int value = 8; };
    template<typename T, size_t N> struct tuple_total_size<std::array<T, N>> { constexpr static int value = tuple_total_size<T[N]>::value; };
    template<typename T, size_t N> struct tuple_total_size<T[N]> { constexpr static int value = tuple_total_size<T>::value < 0 ? -1 : tuple_total_size<T>::value * (int)N; };

    template<>
    struct tuple_total_size<std::tuple<>>
    {
        constexpr static int value = 0;
    };

    template<typename Arg0, typename ... Args>
    struct tuple_total_size<std::tuple<Arg0, Args ...>>
    {
        constexpr static int first_value = tuple_total_size<Arg0>::value;
        constexpr static int other_value = tuple_total_size<std::tuple<Args...>>::value;
        constexpr static int value = first_value < 0 || other_value < 0 ? -1 : first_value + other_value;
    };

    template<typename T> struct tuple_total_size_impl<T, std::void_t<std::enable_if_t<std::is_enum<std::decay_t<T>>::value>>>
    {
        constexpr static int value = tuple_total_size<std::underlying_type_t<std::decay_t<T>>>::value;
    };

    template<typename T> struct tuple_total_size_impl<T, std::void_t<std::enable_if_t<has_meta_macro<std::decay_t<T>>::value>>>
    {
        constexpr static int value = tuple_total_size<typename std::decay_t<T>::meta_type>::value;
    };

    template<typename K, typename V>
    struct tuple_total_size<std::pair<K, V>>
    {
        constexpr static int value = tuple_total_size<std::tuple<K, V>>::value;
    };
}
