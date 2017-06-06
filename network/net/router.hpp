#pragma once
// boost libraries
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/algorithm/string.hpp>

namespace cytx {
    namespace rpc
    {
        template<typename ConnectType>
        class router_base : boost::noncopyable
        {
        public:
            using connection_t = ConnectType;
            using connection_ptr = std::shared_ptr<ConnectType>;
            using header_t = typename connection_t::header_t;
            using invoker_t = std::function<void(connection_ptr, header_t&, const char*)>;
            using invoker_container = std::unordered_map<uint64_t, invoker_t>;
            using on_read_func = std::function<void(connection_ptr, header_t&)>;
            using on_error_func = std::function<void(connection_ptr, rpc_result const& error)>;
            using before_invoke_func = std::function<bool(connection_ptr, const header_t&, const char*, size_t)>;
            using before_send_func = std::function<bool(connection_ptr, const header_t&)>;
            using after_send_func = std::function<void(connection_ptr, const header_t&)>;
            using proto_func = std::function<bool(const header_t&)>;

        protected:
            struct proto_func_invoker
            {
                proto_func func;
                invoker_t invoker;
            };

        public:
            inline bool register_invoker(uint64_t name, invoker_t&& invoker);
            inline bool has_invoker(uint64_t name) noexcept;

            inline void on_read(connection_ptr const& conn_ptr, header_t& header);
            inline void on_error(connection_ptr const& conn_ptr, rpc_result const& error);
            inline void set_on_read(on_read_func&& on_read);
            inline void set_on_error(on_error_func&& on_error);
            inline void set_before_invoker(before_invoke_func&& before_invoker);
            void set_before_send_func(before_send_func&& before_send)
            {
                before_send_func_ = std::forward<before_send_func>(before_send);
            }
            void set_after_send_func(after_send_func&& after_send)
            {
                after_send_func_ = std::forward<after_send_func>(after_send);
            }
        protected:
            invoker_container invokers_;
            on_read_func on_read_;
            on_error_func on_error_;
            before_invoke_func before_invoker_;
            std::vector<proto_func_invoker> proto_invokers_;
        public:
            before_send_func before_send_func_;
            after_send_func after_send_func_;
        };

        template<typename T, typename H>
        class async_rpc_channel;

        template <typename CodecPolicy, typename header_type>
        class router : public router_base<async_rpc_channel<CodecPolicy, header_type>>
        {
        public:
            using codec_policy = CodecPolicy;
            using message_t = typename codec_policy::buffer_type;
            using connection_t = typename router_base::connection_t;
            using connection_ptr = typename router_base::connection_ptr;
            using header_t = typename router_base::header_t;
            using invoker_t = typename router_base::invoker_t;
            using invoker_container = typename router_base::invoker_container;
            using context_t = typename router_base::connection_t::context_t;
            using on_read_func = typename router_base::on_read_func;
            using on_error_func = typename router_base::on_error_func;
            using proto_func = typename router_base::proto_func;

        public:
            template <typename Handler, typename PostFunc>
            inline bool register_invoker(std::string const& name, Handler&& handler, PostFunc&& post_func);

            template <typename CallCodecPolicy, typename Handler, typename PostFunc>
            inline bool register_codec_invoker(std::string const& name, Handler&& handler, PostFunc&& post_func);

            template <typename Handler, typename PostFunc>
            inline bool register_invoker(uint32_t protocol, Handler&& handler, PostFunc&& post_func);

            template <typename CallCodecPolicy, typename Handler, typename PostFunc>
            inline bool register_codec_invoker(uint32_t protocol, Handler&& handler, PostFunc&& post_func);

            template <typename Handler>
            inline bool register_invoker(std::string const& name, Handler&& handler);

            template <typename CallCodecPolicy, typename Handler>
            inline bool register_codec_invoker(std::string const& name, Handler&& handler);

            template <typename Handler>
            inline bool register_invoker(uint32_t protocol, Handler&& handler);

            template <typename CallCodecPolicy, typename Handler>
            inline bool register_codec_invoker(uint32_t protocol, Handler&& handler);

            template <typename Handler>
            inline bool register_raw_invoker(std::string const& name, Handler&& handler);

            template <typename Handler>
            inline bool register_raw_invoker(uint32_t protocol, Handler&& handler);

            template <typename Handler>
            inline bool register_invoker(proto_func func, Handler&& handler);

            inline bool has_invoker(std::string const& name) const;

            inline void apply_invoker(connection_ptr conn, header_t& header, char const* data, size_t size) const;

        private:
            template <typename CallCodecPolicy, typename Handler, typename ... Handlers>
            inline bool register_codec_invoker_impl(uint64_t proto_hash, Handlers&& ... handlers);

            template <typename Handler>
            inline bool register_raw_invoker_impl(uint64_t proto_hash, Handler&& handler);

        private:
            std::hash<std::string> hash_;
        };
    }
}
