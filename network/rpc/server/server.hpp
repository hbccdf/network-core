#pragma once
#include "network/net/ios_wrapper.hpp"
#include "network/net/ios_pool.hpp"
#include "network/net/net_common.hpp"
#include "network/rpc/net/router_register.hpp"
#include "network/rpc/net/rpc_task.hpp"

namespace cytx {
    namespace rpc
    {
        struct server_options
        {
            net::server_thread_mode thread_mode;
        };

        template <typename CodecPolicy, typename header_type = msg_header>
        class server : public router_register<CodecPolicy, header_type>
        {
        public:
            using base_t = router_register<CodecPolicy, header_type>;
            using codec_policy = CodecPolicy;
            using router_t = typename base_t::router_t;
            using invoker_t = typename router_t::invoker_t;
            using connection_t = typename router_t::connection_t;
            using connection_ptr = typename router_t::connection_ptr;
            using header_t = header_type;
            using ios_t = net::ios_wrapper;
            using io_service_t = typename ios_t::io_service_t;
            using irouter_t = typename connection_t::irouter_t;
            using irouter_ptr = typename connection_t::irouter_ptr;
            using tcp = boost::asio::ip::tcp;

        public:
            server(const std::string& ip, uint16_t port, irouter_ptr irouter = nullptr)
                : base_t()
                , ios_pool_(0)
                , ios_(ios_pool_.get_main_service())
                , acceptor_(ios_.service(), util::to_tcp_endpoint(ip, port), true)
                , irouter_(irouter)
            {
            }
            server(uint16_t port, irouter_ptr irouter = nullptr)
                : base_t()
                , ios_pool_(0)
                , ios_(ios_pool_.get_main_service())
                , acceptor_(ios_.service(), util::to_tcp_endpoint(port), true)
                , irouter_(irouter)
            {
            }

            server(io_service_t& ios, const std::string& ip, uint16_t port, irouter_ptr irouter = nullptr)
                : base_t()
                , ios_pool_(ios, 0)
                , ios_(ios_pool_.get_main_service())
                , acceptor_(ios_.service(), util::to_tcp_endpoint(ip, port), true)
                , irouter_(irouter)
            {
            }
            server(io_service_t& ios, uint16_t port, irouter_ptr irouter = nullptr)
                : base_t()
                , ios_pool_(ios, 0)
                , ios_(ios_pool_.get_main_service())
                , acceptor_(ios_.service(), util::to_tcp_endpoint(port), true)
                , irouter_(irouter)
            {
            }

            server(const std::string& ip, uint16_t port, const server_options& options, irouter_ptr irouter = nullptr)
                : base_t()
                , ios_pool_(net::get_thread_count(options.thread_mode))
                , ios_(ios_pool_.get_main_service())
                , acceptor_(ios_.service(), util::to_tcp_endpoint(ip, port), true)
                , irouter_(irouter)
            {
            }

            server(io_service_t& ios, const std::string& ip, uint16_t port, const server_options& options, irouter_ptr irouter = nullptr)
                : base_t()
                , ios_pool_(ios, net::get_thread_count(options.thread_mode))
                , ios_(ios_pool_.get_main_service())
                , acceptor_(ios_.service(), util::to_tcp_endpoint(ip, port), true)
                , irouter_(irouter)
            {
            }

            ~server()
            {
                stop();
            }

            void start()
            {
                do_accept();
                ios_pool_.start();
            }

            void stop()
            {
                base_t::stop();
                ios_pool_.stop();
            }

            ios_t& ios()
            {
                return ios_;
            }

            io_service_t& get_io_service()
            {
                return ios_pool_.get_main_service();
            }

          private:
            void do_accept()
            {
                auto new_connection = std::make_shared<connection_t>(ios_pool_.get_io_service(), this->router_, irouter_);

                acceptor_.async_accept(new_connection->socket(),
                    [this, new_connection](boost::system::error_code const& error) mutable
                {
                    if (!error)
                    {
                        new_connection->start();
                    }
                    else
                    {
                        // TODO log error
                    }
                    if (irouter_)
                        irouter_->on_connect(new_connection, net_result(error));

                    do_accept();
                });
            }

        private:
            net::ios_pool ios_pool_;
            ios_t ios_;
            tcp::acceptor acceptor_;
            mutable std::mutex mutex_;
            irouter_ptr irouter_;
        };
    }
}