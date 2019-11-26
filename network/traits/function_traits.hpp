#pragma once
#include <functional>
#include <tuple>
#include <type_traits>
#include <boost/bind.hpp>
#include <boost/any.hpp>

//member function
#define FUNCTION_TRAITS(...)\
template <typename ReturnType, typename ClassType, typename... Args>\
struct function_traits_impl<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits_impl<ReturnType(Args...)>{ \
    constexpr static bool is_class_member_func = true; \
};\

#ifdef LINUX
namespace std
{
    template<typename ... Args>
    struct voider
    {
        using type = void;
    };
    template <typename ... Args>
    using void_t = typename voider<Args...>::type;

    template<typename T>
    constexpr static bool is_void_v = std::is_void<T>::value;
}

#define PLACE_HOLDER std::_Placeholder
#else
#define PLACE_HOLDER std::_Ph
#endif

#define TEMPLATE_VALUE(token) \
    template<typename T> constexpr bool token ##_v = token<T>::value

#define TEMPLATE_TYPE(token) \
    template<typename T> using token ##_t = typename token<T>::type

namespace std
{
    template <int Size>
    struct is_placeholder<boost::arg<Size>>
        : public std::integral_constant<int, Size>
    {
    };
}

namespace cytx
{
    namespace func_detail
    {
        /*
        * 1. function type							==>	Ret(Args...)
        * 2. function pointer						==>	Ret(*)(Args...)
        * 3. function reference						==>	Ret(&)(Args...)
        * 4. pointer to non-static member function	==> Ret(T::*)(Args...)
        * 5. function object and functor				==> &T::operator()
        * 6. function with generic operator call		==> template <typeanme ... Args> &T::operator()
        */
        template <typename T>
        struct function_traits_impl;

        template<typename Ret, typename... Args>
        struct function_traits_impl<Ret(Args...)>
        {
        public:
            enum { arity = sizeof...(Args) };
            typedef Ret function_type(Args...);
            typedef Ret result_type;
            using stl_function_type = std::function<function_type>;
            typedef Ret(*pointer)(Args...);

            template<size_t I>
            struct args
            {
                static_assert(I < arity, "index is out of range, index must less than sizeof Args");
                using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
            };

            using tuple_type = std::tuple<std::decay_t<Args>...>;
            using raw_tuple_type = std::tuple<Args...>;

            constexpr static bool is_class_member_func = false;
            constexpr static bool has_result = !std::is_void_v<result_type>;
        };

        // function pointer
        template<typename Ret, typename... Args>
        struct function_traits_impl<Ret(*)(Args...)> : function_traits_impl<Ret(Args...)> {};

        // std::function
        template <typename Ret, typename... Args>
        struct function_traits_impl<std::function<Ret(Args...)>> : function_traits_impl<Ret(Args...)> {};

        // pointer of non-static member function
        FUNCTION_TRAITS();
        FUNCTION_TRAITS(const);
        FUNCTION_TRAITS(volatile);
        FUNCTION_TRAITS(const volatile);

        // functor
        template<typename Callable>
        struct function_traits_impl : function_traits_impl<decltype(&Callable::operator())> {};

        template<typename T>
        struct function_traits : function_traits_impl<std::decay_t<T>> {};

        template<typename Tuple>
        struct max_arg_index;

        template<>
        struct max_arg_index<std::tuple<>>
        {
            constexpr static int value = 0;
        };

        template<typename Head, typename ... Tail>
        struct max_arg_index<std::tuple<Head, Tail...>>
        {
            constexpr static int head_value = std::is_placeholder<Head>::value;
            constexpr static int tail_value = max_arg_index<std::tuple<Tail...>>::value;
            constexpr static int value = head_value > tail_value ? head_value : tail_value;
        };

        template<typename Index_t, typename FuncArgs, int max_index>
        struct cat_impl;

        template<size_t ... Is, typename FuncArgs, int max_index>
        struct cat_impl<std::index_sequence<Is...>, FuncArgs, max_index>
        {
            using type = std::tuple<PLACE_HOLDER<Is + max_index + 1> ...>;
        };

        template<bool IsMemberFunc, typename TupleArgs, typename FuncArgs>
        struct cat_helper
        {
            constexpr static size_t args_size = std::tuple_size<FuncArgs>::value + (IsMemberFunc ? 1 : 0) - std::tuple_size<TupleArgs>::value;
            using func_args_index_t = std::make_index_sequence<args_size>;
            constexpr static int max_index = max_arg_index<TupleArgs>::value;
            using type = typename cat_impl<func_args_index_t, FuncArgs, max_index>::type;
        };

        template<typename F, typename TupleArgs>
        struct cat
        {
            using type = typename cat_helper<function_traits<F>::is_class_member_func, TupleArgs,
                typename function_traits<F>::raw_tuple_type >::type;
        };

        template<typename T>
        struct bind_help
        {
            template<typename F, typename ... Args>
            static auto bind_impl(F&& f, Args&& ... args)
            {
                return std::bind(std::forward<F>(f), std::forward<Args>(args)...);
            }
        };

        template<typename ... tuple_args>
        struct bind_help<std::tuple<tuple_args ...>>
        {
            template<typename F, typename ... Args>
            static auto bind_impl(F&& f, Args&& ... args)
            {
                return std::bind(std::forward<F>(f), std::forward<Args>(args)..., tuple_args{}...);
            }
        };

        template<>
        struct bind_help<std::tuple<>>
        {
            template<typename F, typename ... Args>
            static auto bind_impl(F&& f, Args&& ... args)
            {
                return std::bind(std::forward<F>(f), std::forward<Args>(args)...);
            }
        };


        template<size_t I, typename T>
        T get(std::vector<boost::any>& arg_list)
        {
            boost::any v = arg_list[I];
            return boost::any_cast<T>(v);
        }

        template<size_t I, typename T, typename Tuple>
        T get(Tuple&& t)
        {
            return std::get<I>(std::forward<Tuple>(t));
        }

        template <typename Func, typename ArgsTuple, typename ArgList, size_t ... Is>
        inline auto invoke_impl(Func&& f, ArgList&& arg_list, std::index_sequence<Is...>)
        {
            using args_tuple_t = std::remove_reference_t<ArgsTuple>;
            return f(std::forward<std::tuple_element_t<Is, args_tuple_t>>(get<Is, std::tuple_element_t<Is, args_tuple_t>>(arg_list))...);
        }

        template <typename Func, typename ArgsTuple, size_t ... Is>
        inline auto invoke_impl(Func&& f, ArgsTuple&& args_tuple, std::index_sequence<Is...>)
        {
            using args_tuple_t = std::remove_reference_t<ArgsTuple>;
            return f(std::forward<std::tuple_element_t<Is, args_tuple_t>>(get<Is>(args_tuple))...);
        }
    }

    template<typename T>
    using function_traits = typename func_detail::function_traits<T>;

    template<typename T>
    using indices_t = std::make_index_sequence<std::tuple_size<std::decay_t<T>>::value>;

    template<typename F>
    struct function_result
    {
        using type = typename function_traits<F>::result_type;
    };

    TEMPLATE_TYPE(function_result);

    template<typename F>
    struct function_args
    {
        using type = typename function_traits<F>::raw_tuple_type;
    };

    TEMPLATE_TYPE(function_args);


    template<typename F>
    struct function_decay_args
    {
        using type = typename function_traits<F>::tuple_type;
    };

    TEMPLATE_TYPE(function_decay_args);

    template<typename F>
    struct function_has_result
    {
        constexpr static bool value = function_traits<F>::has_result;
    };

    TEMPLATE_VALUE(function_has_result);

    template<typename F>
    struct function_is_class_method
    {
        constexpr static bool value = function_traits<F>::is_class_member_func;
    };

    TEMPLATE_VALUE(function_is_class_method);

    template<typename T, bool isclass = std::is_class<std::decay_t<T>>::value>
    struct is_callable : std::is_function<std::remove_pointer_t<T>> {};

    template<typename T>
    struct is_callable<T, true> {
        using yes = char;
        using no = struct { char s[2]; };

        struct F { void operator()(); };
        struct Derived : std::decay_t<T>, F {};
        template<typename U, U> struct Check;

        template<typename V>
        static no test(Check<void (F::*)(), &V::operator()>*);

        template<typename>
        static yes test(...);

        constexpr static bool value = sizeof(test<Derived>(0)) == sizeof(yes);
    };

    TEMPLATE_VALUE(is_callable);

    template <typename Func>
    auto to_function(const Func& lambda)
    {
        return static_cast<typename function_traits<Func>::stl_function_type>(lambda);
    }

    template <typename Func>
    auto to_function(Func&& lambda)
    {
        return static_cast<typename function_traits<Func>::stl_function_type>(std::forward<Func>(lambda));
    }

    template <typename Func>
    auto to_function_pointer(const Func& lambda)
    {
        return static_cast<typename function_traits<Func>::pointer>(lambda);
    }

    template<typename ... Args>
    void to_arg_list(std::vector<boost::any>& arg_list, Args&&... args)
    {
        char c[]{ 1, (arg_list.push_back(std::forward<Args>(args)), 0) ... };
    }

    template <typename F, typename ... Args>
    auto bind(F&& f, Args&& ... args)
    {
        using args_tuple_t = typename func_detail::cat<F, std::tuple<Args...>>::type;
        using help_t = func_detail::bind_help<args_tuple_t>;
        using func_t = typename function_traits<F>::stl_function_type;
        return static_cast<func_t>(help_t::template bind_impl<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
    }

    template <typename Func, typename ArgsTuple>
    inline auto invoke(Func&& f, ArgsTuple&& args_tuple)
    {
        return func_detail::invoke_impl(std::forward<Func>(f), std::forward<ArgsTuple>(args_tuple), indices_t<ArgsTuple>{});
    }

    template<typename Func, typename ... Args>
    inline auto invoke(Func&& f, Args&& ... args)
    {
        auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
        return invoke(std::forward<Func>(f), args_tuple);
    }

    template <typename Func>
    inline auto invoke(Func&& f, std::vector<boost::any>& arg_list)
    {
        using args_tuple_t = typename function_traits<Func>::raw_tuple_type;
        return func_detail::invoke_impl<Func, args_tuple_t>(std::forward<Func>(f), arg_list, indices_t<args_tuple_t>{});
    }
}