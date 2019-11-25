#pragma once
#include "network/rpc/net/router_register.hpp"
#include "network/rpc/net/rpc_task.hpp"
#include "network/net/ios_wrapper.hpp"

namespace cytx {
    namespace rpc
    {
        template <typename CodecPolicy, typename header_type = msg_header>
        class async_client : public router_register<CodecPolicy, header_type>, public rpc_call<CodecPolicy, header_type>
        {
        public:
            using base_t = router_register<CodecPolicy, header_type>;
            using codec_policy = CodecPolicy;
            using connection_t = async_rpc_channel<codec_policy, header_type>;
            using connection_ptr = std::shared_ptr<connection_t>;
            using router_t = typename base_t::router_t;
            using header_t = header_type;
            using ios_t = net::ios_wrapper;
            using io_service_t = typename ios_t::io_service_t;
            using irouter_t = typename connection_t::irouter_t;
            using irouter_ptr = typename connection_t::irouter_ptr;
            using rpc_call_t = rpc_call<codec_policy, header_t>;
            using tcp = boost::asio::ip::tcp;

        public:
            async_client(tcp::endpoint endpoint, irouter_ptr irouter = nullptr)
                : base_t()
                , rpc_call_t(nullptr)
                , ios_()
                , client_private_(std::make_shared<connection_t>(ios_, endpoint, this->router_, irouter))
            {
                this->set_conn_ptr(client_private_.get());
                ios_.start();
            }

            async_client(io_service_t& ios, tcp::endpoint endpoint, irouter_ptr irouter = nullptr)
                : base_t()
                , rpc_call_t(nullptr)
                , ios_(ios)
                , client_private_(std::make_shared<connection_t>(ios_, endpoint, this->router_, irouter))
            {
                this->set_conn_ptr(client_private_.get());
                ios_.start();
            }

            ~async_client()
            {
                ios_.stop();
            }

            connection_ptr conn_ptr()
            {
                return client_private_;
            }

            auto connect()
            {
                return client_private_->connect();
            }

            void disconnect()
            {
                client_private_->disconnect();
            }

        private:
            ios_t ios_;
            connection_ptr client_private_;
        };
    }
}
