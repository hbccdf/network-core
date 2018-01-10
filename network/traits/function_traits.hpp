#pragma once
#include <functional>
#include <tuple>
#include <type_traits>
#include <boost/bind.hpp>

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
}
#endif

namespace cytx
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

    template<typename T>
    struct function_traits : function_traits_impl<std::remove_cv_t<std::remove_reference_t<T>>>
    {};

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

        using tuple_type = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...> ;
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

    template <typename Function>
    typename function_traits<Function>::stl_function_type to_function(const Function& lambda)
    {
        return static_cast<typename function_traits<Function>::stl_function_type>(lambda);
    }

    template <typename Function>
    typename function_traits<Function>::stl_function_type to_function(Function&& lambda)
    {
        return static_cast<typename function_traits<Function>::stl_function_type>(std::forward<Function>(lambda));
    }

    template <typename Function>
    typename function_traits<Function>::pointer to_function_pointer(const Function& lambda)
    {
        return static_cast<typename function_traits<Function>::pointer>(lambda);
    }
}


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

#ifdef LINUX
    template<size_t ... Is, typename FuncArgs, int max_index>
    struct cat_impl<std::index_sequence<Is...>, FuncArgs, max_index>
    {
        using type = std::tuple<std::_Placeholder<Is + max_index + 1> ...>;
    };
#else
    template<size_t ... Is, typename FuncArgs, int max_index>
    struct cat_impl<std::index_sequence<Is...>, FuncArgs, max_index>
    {
        using type = std::tuple<std::_Ph<Is + max_index + 1> ...>;
    };
#endif

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

    template <typename F, typename ... Args>
    auto bind(F&& f, Args&& ... args)
    {
        using args_tuple_t = typename cat<F, std::tuple<Args...>>::type;
        using help_t = bind_help<args_tuple_t>;
        using func_t = typename function_traits<F>::stl_function_type;
        return static_cast<func_t>(help_t::template bind_impl<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
    }
}