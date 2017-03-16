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

            router_register()
            {
                init_callback_functions();
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

        protected:
            void init_callback_functions()
            {
                router_.set_on_read([this](connection_ptr conn_ptr)
                {
                    auto& header = conn_ptr->get_read_header();
                    auto read_buffer = conn_ptr->get_read_buffer();
                    router_.apply_invoker(conn_ptr, read_buffer.data(), read_buffer.size());
                });
            }

        protected:
            router_t router_;
        };
    }
}
