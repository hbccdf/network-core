#pragma once

#include <boost/type_traits.hpp>

#define DEFINE_STR_PROTOCOL(handler, ...) static const ::cytx::rpc::rpc_protocol<__VA_ARGS__> handler{ #handler }
#define DEFINE_PROTOCOL(name, handler, ...) static const ::cytx::rpc::rpc_protocol<__VA_ARGS__> name{ (uint32_t)handler }

namespace cytx {
    namespace rpc
    {
        template <typename Func, typename ... Args>
        struct is_argument_match
        {
        private:
            template <typename T>
            static std::false_type test(...);

            template <typename T, typename =
                decltype(std::declval<T>()(std::declval<Args>()...))>
                static std::true_type test(int);

            using result_type = decltype(test<Func>(0));
        public:
            static constexpr bool value = result_type::value;
        };

        template <typename Func>
        struct rpc_protocol;

        template <typename Func>
        struct rpc_protocol_base;

        template <typename Ret, typename ... Args>
        struct rpc_protocol_base<Ret(Args...)>
        {
            using result_type = typename boost::function_traits<Ret(Args...)>::result_type;
            using signature_type = Ret(Args...);

            explicit rpc_protocol_base(std::string const& name)
                : name_(static_cast<uint64_t>(std::hash<std::string>{}(name)))
                , is_inter_protocl_(false)
            {
            }

            explicit rpc_protocol_base(uint32_t protocol)
                : name_(protocol)
                , is_inter_protocl_(true)
            {
            }

            uint64_t proto() const noexcept
            {
                return name_;
            }

            bool inter_protocol() const
            {
                return is_inter_protocl_;
            }

            template <typename CodecPolicy, typename ... TArgs>
            auto pack_args(CodecPolicy const& cp, TArgs&& ... args) const
            {
                static_assert(is_argument_match<signature_type, TArgs...>::value, "Arguments` types don`t match the protocol!");
                return cp.pack_args(std::forward<Args>(static_cast<Args>(args))...);
            }

        private:
            uint64_t name_;
            bool is_inter_protocl_;
        };

        template <typename Ret, typename ... Args>
        struct rpc_protocol<Ret(Args...)> : rpc_protocol_base<Ret(Args...)>
        {
            using base_type = rpc_protocol_base<Ret(Args...)>;
            using result_type = typename base_type::result_type;
            using signature_type = typename base_type::signature_type;

            using base_type::base_type;

            template <typename CodecPolicy, typename = std::enable_if_t<!std::is_void<result_type>::value>>
            auto pack_result(CodecPolicy const& cp, result_type&& ret) const
            {
                return cp.pack(std::forward<result_type>(ret));
            }

            template <typename CodecPolicy, typename = std::enable_if_t<!std::is_void<result_type>::value>>
            result_type unpack(CodecPolicy& cp, char const* data, size_t length) const
            {
                return cp.template unpack<result_type>(data, length);
            }
        };

        template <typename ... Args>
        struct rpc_protocol<void(Args...)> : rpc_protocol_base<void(Args...)>
        {
            using base_type = rpc_protocol_base<void(Args...)>;
            using result_type = typename base_type::result_type;
            using signature_type = typename base_type::signature_type;

            using base_type::base_type;
        };

        template <typename>
        struct is_rpc_protocol : std::false_type {};

        template <typename Func>
        struct is_rpc_protocol<rpc_protocol<Func>> : std::true_type {};
    }
}
