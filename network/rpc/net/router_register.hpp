#pragma once
#include "async_rpc_channel.hpp"
#include "protocol.hpp"
#include "router_impl.hpp"

namespace cytx {
    namespace rpc
    {
        template <typename CodecPolicy, typename header_type = msg_header>
        class router_register
        {
        public:
            using codec_policy = CodecPolicy;
            using connection_t = async_rpc_channel<codec_policy, header_type>;
            using connection_ptr = std::shared_ptr<connection_t>;
            using router_t = router<codec_policy, header_type>;
            using header_t = header_type;
            using ios_t = ios_wrapper;
            using before_invoke_func = typename router_t::before_invoke_func;
            using before_send_func = typename router_t::before_send_func;
            using after_send_func = typename router_t::after_send_func;

            router_register()
            {
                init_callback_functions();
                msg_ios_.start();
            }

            void stop()
            {
                msg_ios_.stop();
            }
        public:
            router_t& router()
            {
                return router_;
            }

            template <typename Handler, typename PostFunc>
            bool register_handler(std::string const& name, Handler&& handler, PostFunc&& post_func)
            {
                return router_.register_invoker(name, std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
            }

            template <typename CallCodecPolicy, typename Handler, typename PostFunc>
            bool register_codec_handler(std::string const& name, Handler&& handler, PostFunc&& post_func)
            {
                return router_.register_codec_invoker<CallCodecPolicy>(name, std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
            }

            template <typename Handler>
            bool register_handler(std::string const& name, Handler&& handler)
            {
                return router_.register_invoker(name, std::forward<Handler>(handler));
            }

            template <typename CallCodecPolicy, typename Handler>
            bool register_codec_handler(std::string const& name, Handler&& handler)
            {
                return router_.register_codec_invoker<CallCodecPolicy>(name, std::forward<Handler>(handler));
            }

            template <typename Handler, typename PostFunc>
            bool register_handler(uint32_t protocol, Handler&& handler, PostFunc&& post_func)
            {
                return router_.register_invoker(protocol, std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
            }

            template <typename CallCodecPolicy, typename Handler, typename PostFunc>
            bool register_codec_handler(uint32_t protocol, Handler&& handler, PostFunc&& post_func)
            {
                return router_.register_codec_invoker<CallCodecPolicy>(protocol, std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
            }

            template <typename Handler>
            bool register_handler(uint32_t protocol, Handler&& handler)
            {
                return router_.register_invoker(protocol, std::forward<Handler>(handler));
            }

            template <typename CallCodecPolicy, typename Handler>
            bool register_codec_handler(uint32_t protocol, Handler&& handler)
            {
                return router_.register_codec_invoker<CallCodecPolicy>(protocol, std::forward<Handler>(handler));
            }

            template <typename proto_type, typename Handler, typename PostFunc>
            auto register_handler(proto_type protocol, Handler&& handler, PostFunc&& post_func)
                -> std::enable_if_t<std::is_enum<proto_type>::value, bool>
            {
                return router_.register_invoker((uint32_t)protocol, std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
            }

            template <typename CallCodecPolicy, typename proto_type, typename Handler, typename PostFunc>
            auto register_codec_handler(proto_type protocol, Handler&& handler, PostFunc&& post_func)
                -> std::enable_if_t<std::is_enum<proto_type>::value, bool>
            {
                return router_.register_codec_invoker<CallCodecPolicy>((uint32_t)protocol, std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
            }

            template <typename proto_type, typename Handler>
            auto register_handler(proto_type protocol, Handler&& handler)
                -> std::enable_if_t<std::is_enum<proto_type>::value, bool>
            {
                return router_.register_invoker((uint32_t)protocol, std::forward<Handler>(handler));
            }

            template <typename CallCodecPolicy, typename proto_type, typename Handler>
            auto register_codec_handler(proto_type protocol, Handler&& handler)
                -> std::enable_if_t<std::is_enum<proto_type>::value, bool>
            {
                return router_.register_codec_invoker<CallCodecPolicy>((uint32_t)protocol, std::forward<Handler>(handler));
            }

            template <typename Handler>
            bool register_raw_handler(std::string const& name, Handler&& handler)
            {
                return router_.register_raw_invoker(name, std::forward<Handler>(handler));
            }

            template <typename Handler>
            bool register_raw_handler(uint32_t protocol, Handler&& handler)
            {
                return router_.register_raw_invoker(protocol, std::forward<Handler>(handler));
            }

            template <typename proto_type, typename Handler>
            auto register_raw_handler(proto_type protocol, Handler&& handler)
                ->std::enable_if_t<std::is_enum<proto_type>::value, bool>
            {
                return router_.register_raw_invoker((uint32_t)protocol, std::forward<Handler>(handler));
            }

            inline void set_before_invoker(before_invoke_func&& before_invoker)
            {
                router_.set_before_invoker(std::forward<before_invoke_func>(before_invoker));
            }

            inline void set_before_send_func(before_send_func&& before_send)
            {
                router_.set_before_send_func(std::forward<before_send_func>(before_send));
            }
            inline void set_after_send_func(after_send_func&& after_send)
            {
                router_.set_after_send_func(std::forward<after_send_func>(after_send));
            }

        protected:
            void init_callback_functions()
            {
                router_.set_on_read([this](connection_ptr conn_ptr, header_t& header)
                {
                    auto read_buffer = conn_ptr->get_read_buffer();
                    auto gos_ptr = std::make_shared<cytx::codec::gos_buffer>(read_buffer.data(), read_buffer.data() + read_buffer.size());

                    msg_ios_.service().post([conn_ptr, header, this, gos_ptr] () mutable
                    {
                        router_.apply_invoker(conn_ptr, header, gos_ptr->data(), gos_ptr->size());
                    });
                });
            }

        protected:
            router_t router_;
            ios_t msg_ios_;
        };
    }
}
