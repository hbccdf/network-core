#pragma once
#include "async_client.hpp"

namespace cytx {
    namespace rpc
    {
        template <typename CodecPolicy, typename header_type = msg_header>
        class sync_client
        {
            using codec_policy = CodecPolicy;
            using async_client_t = async_client<codec_policy, header_type>;
            using async_client_ptr = std::shared_ptr<async_client_t>;
            using header_t = header_type;
            using tcp = boost::asio::ip::tcp;

        public:
            sync_client(tcp::endpoint const& endpoint)
                : client_(std::make_shared<async_client_t>(endpoint))
            {
            }

            template <typename Protocol, typename ... Args>
            auto call(Protocol const& protocol, Args&& ... args)
            {
                using result_type = typename Protocol::result_type;
                return call_impl(std::is_void<result_type>{}, protocol, std::forward<Args>(args)...);
            }

        private:
            template <typename Protocol, typename ... Args>
            auto call_impl(std::false_type, Protocol const& protocol, Args&& ... args)
            {
                using result_type = typename Protocol::result_type;
                auto task = client_->call(protocol, std::forward<Args>(args)...);
                return task.get();
            }

            template <typename Protocol, typename ... Args>
            auto call_impl(std::true_type, Protocol const& protocol, Args&& ... args)
            {
                auto task = client_->call(protocol, std::forward<Args>(args)...);
                task.wait();
            }

        private:
            async_client_ptr client_;
        };
    }
}