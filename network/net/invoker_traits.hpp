#pragma once
#include "../traits/traits.hpp"

namespace cytx {
    namespace rpc
    {
        struct handler_return_void {};
        struct handler_return_result {};

        template <typename Handler>
        struct handler_traits
        {
        private:
            using function_traits_type = function_traits<std::remove_reference_t<Handler>>;
        public:
            using return_tag = std::conditional_t<std::is_void<typename function_traits_type::result_type>::value,
                handler_return_void, handler_return_result>;
            using tuple_type = typename function_traits_type::tuple_type;
        };

        template <typename Handler, typename ArgsTuple, size_t ... Is>
        inline auto invoker_call_handler_impl(Handler&& h, ArgsTuple&& args_tuple, std::index_sequence<Is...>)
        {
            using args_tuple_type = std::remove_reference_t<ArgsTuple>;
            return h(std::forward<std::tuple_element_t<Is, args_tuple_type>>(std::get<Is>(args_tuple))...);
        }

        template <typename Handler, typename ArgsTuple>
        inline auto invoker_call_handler(Handler&& h, ArgsTuple&& args_tuple)
        {
            using indices_type = std::make_index_sequence<std::tuple_size<std::remove_reference_t<ArgsTuple>>::value>;
            return invoker_call_handler_impl(std::forward<Handler>(h), std::forward<ArgsTuple>(args_tuple), indices_type{});
        }

        template <typename ReturnTag>
        struct invoker_traits;

        template<typename other_tuple, typename args_tuple>
        struct get_args_tuple_type;

        template<typename other_tuple, typename Arg, class = std::void_t<>>
        struct get_arg_type;

        template<typename other_tuple, typename Arg>
        struct get_arg_type<other_tuple, Arg, std::void_t<std::enable_if_t<tuple_contains<Arg, other_tuple>::value>>>
        {
            using type = std::tuple_element_t <tuple_index<Arg, other_tuple>::value, other_tuple>;
        };

        template<typename other_tuple, typename Arg>
        struct get_arg_type<other_tuple, Arg, std::void_t<std::enable_if_t<!tuple_contains<Arg, other_tuple>::value>>>
        {
            using type = Arg;
        };

        template<typename other_tuple, typename Arg>
        using get_arg_type_t = typename get_arg_type<other_tuple, Arg>::type;

        template<typename other_tuple, typename ... Args>
        struct get_args_tuple_type<other_tuple, std::tuple<Args ...>>
        {
            using type = std::tuple < get_arg_type_t<other_tuple, Args> ...>;
        };

        template<typename other_tuple, typename args_tuple>
        using get_args_tuple_type_t = typename get_args_tuple_type<other_tuple, args_tuple>::type;

        // The return type of the handler is void;
        template <>
        struct invoker_traits<handler_return_void>
        {
            template <typename header_type, typename ConnectCodecPolicy, typename CodecPolicy, typename Handler>
            static auto get(Handler&& handler)
            {
                using connection_t = async_rpc_channel<ConnectCodecPolicy, header_type>;
                using connection_ptr = std::shared_ptr<connection_t>;
                using invoker_t = std::function<void(connection_ptr, char const*, size_t)>;
                using context_t = typename connection_t::context_t;

                using args_tuple_type = typename handler_traits<Handler>::tuple_type;
                invoker_t invoker = [h = std::forward<Handler>(handler)]
                (connection_ptr conn, char const* data, size_t size)
                {
                    CodecPolicy cp{ header_type::big_endian() };
                    auto& header = conn->get_read_header();
                    using tuple_type = std::tuple<connection_ptr, header_type&>;
                    using args_tuple_t = get_args_tuple_type_t<tuple_type, args_tuple_type>;
                    auto args_tuple = cp.template unpack<args_tuple_t, tuple_type>(data, size, { conn, header });
                    invoker_call_handler(h, args_tuple);
                    if (!header.need_reply())
                        return;

                    header.reply(true);
                    auto ctx = context_t::make_message(conn->get_io_service(), conn->get_read_header(), context_t::buffer_t{});
                    conn->response(ctx);
                };
                return invoker;
            }

            template <typename header_type, typename ConnectCodecPolicy, typename CodecPolicy, typename Handler, typename PostFunc>
            static auto get(Handler&& handler, PostFunc&& post_func)
            {
                using connection_t = async_rpc_channel<ConnectCodecPolicy, header_type>;
                using connection_ptr = std::shared_ptr<connection_t>;
                using invoker_t = std::function<void(connection_ptr, char const*, size_t)>;
                using context_t = typename connection_t::context_t;

                using args_tuple_type = typename handler_traits<Handler>::tuple_type;
                invoker_t invoker = [h = std::forward<Handler>(handler), p = std::forward<PostFunc>(post_func)]
                (connection_ptr conn, char const* data, size_t size)
                {
                    CodecPolicy cp{ header_type::big_endian() };
                    auto& header = conn->get_read_header();
                    using tuple_type = std::tuple<connection_ptr, header_type&>;
                    using args_tuple_t = get_args_tuple_type_t<tuple_type, args_tuple_type>;
                    auto args_tuple = cp.template unpack<args_tuple_t, tuple_type>(data, size, { conn, header });
                    invoker_call_handler(h, args_tuple);
                    if (!header.need_reply())
                        return;

                    header.reply(true);
                    auto ctx = context_t::make_message(conn->get_io_service(), conn->get_read_header(), context_t::buffer_t{}, [conn, &p] { p(conn); });
                    conn->response(ctx);
                };
                return invoker;
            }
        };

        // The return type of the handler is non void;
        // The handler runs synchronously with io thread;
        template <>
        struct invoker_traits<handler_return_result>
        {
            template <typename header_type, typename ConnectCodecPolicy, typename CodecPolicy, typename Handler>
            static auto get(Handler&& handler)
            {
                using connection_t = async_rpc_channel<ConnectCodecPolicy, header_type>;
                using connection_ptr = std::shared_ptr<connection_t>;
                using invoker_t = std::function<void(connection_ptr, char const*, size_t)>;
                using context_t = typename connection_t::context_t;

                using args_tuple_type = typename handler_traits<Handler>::tuple_type;
                invoker_t invoker = [h = std::forward<Handler>(handler)]
                (connection_ptr conn, char const* data, size_t size)
                {
                    CodecPolicy cp{ header_type::big_endian() };
                    auto& header = conn->get_read_header();
                    using tuple_type = std::tuple<connection_ptr, header_type&>;
                    using args_tuple_t = get_args_tuple_type_t<tuple_type, args_tuple_type>;
                    auto args_tuple = cp.template unpack<args_tuple_t, tuple_type>(data, size, { conn, header });
                    auto result = invoker_call_handler(h, args_tuple);
                    if (!header.need_reply())
                        return;

                    header.reply(true);
                    auto message = cp.pack(result);
                    auto ctx = context_t::make_message(conn->get_io_service(), conn->get_read_header(), std::move(message));
                    conn->response(ctx);
                };
                return invoker;
            }

            template <typename header_type, typename ConnectCodecPolicy, typename CodecPolicy, typename Handler, typename PostFunc>
            static auto get(Handler&& handler, PostFunc&& post_func)
            {
                using connection_t = async_rpc_channel<ConnectCodecPolicy, header_type>;
                using connection_ptr = std::shared_ptr<connection_t>;
                using invoker_t = std::function<void(connection_ptr, char const*, size_t)>;
                using context_t = typename connection_t::context_t;

                using args_tuple_type = typename handler_traits<Handler>::tuple_type;
                invoker_t invoker = [h = std::forward<Handler>(handler), p = std::forward<PostFunc>(post_func)]
                (connection_ptr conn, char const* data, size_t size)
                {
                    CodecPolicy cp{ header_type::big_endian() };
                    auto& header = conn->get_read_header();
                    using tuple_type = std::tuple<connection_ptr, header_type&>;
                    using args_tuple_t = get_args_tuple_type_t<tuple_type, args_tuple_type>;
                    auto args_tuple = cp.template unpack<args_tuple_t, tuple_type>(data, size, { conn, header });
                    auto result = invoker_call_handler(h, args_tuple);
                    if (!header.need_reply())
                        return;

                    header.reply(true);
                    auto message = cp.pack(result);
                    auto ctx = context_t::make_message(conn->get_io_service(), conn->get_read_header(), std::move(message),
                        [conn, r = std::move(result), &p] { p(conn, r); });
                    conn->response(ctx);
                };
                return invoker;
            }
        };
    }
}