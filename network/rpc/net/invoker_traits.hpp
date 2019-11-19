#pragma once
#include "network/traits/traits.hpp"

namespace cytx {
    namespace rpc
    {
        struct handler_return_void {};
        struct handler_return_result {};

        template <typename Handler>
        struct handler_traits
        {
        private:
            using function_traits_type = function_traits<std::decay_t<Handler>>;
        public:
            using return_tag = std::conditional_t<std::is_void_v<typename function_traits_type::result_type>,
                handler_return_void, handler_return_result>;
            using tuple_type = typename function_traits_type::tuple_type;
        };

        template <typename ReturnTag>
        struct invoker_traits;

        // The return type of the handler is void;
        template <>
        struct invoker_traits<handler_return_void>
        {
            template <typename header_type, typename ConnectCodecPolicy, typename CodecPolicy, typename Handler>
            static auto get(Handler&& handler)
            {
                using connection_t = async_rpc_channel<ConnectCodecPolicy, header_type>;
                using connection_ptr = std::shared_ptr<connection_t>;
                using invoker_t = std::function<void(connection_ptr, header_type&, const char*)>;
                using context_t = typename connection_t::context_t;
                using args_tuple_t = typename handler_traits<Handler>::tuple_type;
                using tuple_type = std::tuple<connection_ptr, header_type&>;

                invoker_t invoker = [h = std::forward<Handler>(handler)]
                (connection_ptr conn, header_type& header, const char* data)
                {
                    CodecPolicy cp{ header_type::big_endian() };
                    auto recv_proto = header.proto();
                    auto args_tuple = cp.template unpack<args_tuple_t, tuple_type>(data, header.length(), tuple_type{ conn, header });
                    invoke(h, args_tuple);
                    if (header.result() == (uint16_t)result_code::not_reply)
                        return;
                    if (recv_proto == header.proto())
                    {
                        if (!header.need_reply())
                            return;

                        header.reply(true);
                    }
                    auto ctx = context_t::make_message(header, typename context_t::buffer_t{});
                    conn->response(ctx);
                };
                return invoker;
            }

            template <typename header_type, typename ConnectCodecPolicy, typename CodecPolicy, typename Handler, typename PostFunc>
            static auto get(Handler&& handler, PostFunc&& post_func)
            {
                using connection_t = async_rpc_channel<ConnectCodecPolicy, header_type>;
                using connection_ptr = std::shared_ptr<connection_t>;
                using invoker_t = std::function<void(connection_ptr, header_type&, const char*)>;
                using context_t = typename connection_t::context_t;
                using args_tuple_t = typename handler_traits<Handler>::tuple_type;
                using tuple_type = std::tuple<connection_ptr, header_type&>;

                invoker_t invoker = [h = std::forward<Handler>(handler), p = std::forward<PostFunc>(post_func)]
                (connection_ptr conn, header_type& header, const char* data)
                {
                    CodecPolicy cp{ header_type::big_endian() };
                    auto recv_proto = header.proto();
                    auto args_tuple = cp.template unpack<args_tuple_t, tuple_type>(data, header.length(), tuple_type{ conn, header });
                    invoke(h, args_tuple);
                    if (header.result() == (uint16_t)result_code::not_reply)
                        return;
                    if (recv_proto == header.proto())
                    {
                        if (!header.need_reply())
                            return;

                        header.reply(true);
                    }
                    auto ctx = context_t::make_message(header, typename context_t::buffer_t{}, [conn, &p] { p(conn); });
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
                using invoker_t = std::function<void(connection_ptr, header_type&, const char*)>;
                using context_t = typename connection_t::context_t;
                using args_tuple_t = typename handler_traits<Handler>::tuple_type;
                using tuple_type = std::tuple<connection_ptr, header_type&>;

                invoker_t invoker = [h = std::forward<Handler>(handler)]
                (connection_ptr conn, header_type& header, const char* data)
                {
                    CodecPolicy cp{ header_type::big_endian() };
                    auto recv_proto = header.proto();
                    auto args_tuple = cp.template unpack<args_tuple_t, tuple_type>(data, header.length(), tuple_type{ conn, header });
                    auto result = invoke(h, args_tuple);
                    if (header.result() == (uint16_t)result_code::not_reply)
                        return;
                    if (recv_proto == header.proto())
                    {
                        if (!header.need_reply())
                            return;

                        header.reply(true);
                    }
                    auto message = cp.pack(result);
                    auto ctx = context_t::make_message(header, std::move(message));
                    conn->response(ctx);
                };
                return invoker;
            }

            template <typename header_type, typename ConnectCodecPolicy, typename CodecPolicy, typename Handler, typename PostFunc>
            static auto get(Handler&& handler, PostFunc&& post_func)
            {
                using connection_t = async_rpc_channel<ConnectCodecPolicy, header_type>;
                using connection_ptr = std::shared_ptr<connection_t>;
                using invoker_t = std::function<void(connection_ptr, header_type&, const char*)>;
                using context_t = typename connection_t::context_t;
                using args_tuple_t = typename handler_traits<Handler>::tuple_type;
                using tuple_type = std::tuple<connection_ptr, header_type&>;

                invoker_t invoker = [h = std::forward<Handler>(handler), p = std::forward<PostFunc>(post_func)]
                (connection_ptr conn, header_type& header, const char* data)
                {
                    CodecPolicy cp{ header_type::big_endian() };
                    auto recv_proto = header.proto();
                    auto args_tuple = cp.template unpack<args_tuple_t, tuple_type>(data, header.length(), tuple_type{ conn, header });
                    auto result = invoke(h, args_tuple);
                    if (header.result() == (uint16_t)result_code::not_reply)
                        return;
                    if (recv_proto == header.proto())
                    {
                        if (!header.need_reply())
                            return;

                        header.reply(true);
                    }
                    auto message = cp.pack(result);
                    auto ctx = context_t::make_message(header, std::move(message),
                        [conn, r = std::move(result), &p] { p(conn, r); });
                    conn->response(ctx);
                };
                return invoker;
            }
        };
    }
}