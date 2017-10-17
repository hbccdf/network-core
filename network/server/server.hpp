#pragma once
#include "../net/router_register.hpp"
#include "../net/rpc_task.hpp"
#include "../net/ios_wrapper.hpp"

namespace cytx {
    namespace rpc
    {
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
            using ios_t = ios_wrapper;
            using io_service_t = typename ios_t::io_service_t;
            using irouter_t = typename connection_t::irouter_t;
            using irouter_ptr = typename connection_t::irouter_ptr;

        public:
            server(const std::string& ip, uint16_t port, irouter_ptr irouter = nullptr)
                : base_t()
                , ios_()
                , acceptor_(ios_.service(), get_tcp_endpoint(ip, port), true)
                , irouter_(irouter)
            {
            }
            server(uint16_t port, irouter_ptr irouter = nullptr)
                : base_t()
                , ios_()
                , acceptor_(ios_.service(), tcp::endpoint{ tcp::v4(), port }, true)
                , irouter_(irouter)
            {
            }

            server(io_service_t& ios, const std::string& ip, uint16_t port, irouter_ptr irouter = nullptr)
                : base_t()
                , ios_(ios)
                , acceptor_(ios_.service(), get_tcp_endpoint(ip, port), true)
                , irouter_(irouter)
            {
            }
            server(io_service_t& ios, uint16_t port, irouter_ptr irouter = nullptr)
                : base_t()
                , ios_(ios)
                , acceptor_(ios_.service(), tcp::endpoint{ tcp::v4(), port }, true)
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
                ios_.start();
            }

            void stop()
            {
                base_t::stop();
                ios_.stop();
            }

            ios_t& ios()
            {
                return ios_;
            }

          private:
            void do_accept()
            {
                auto new_connection = std::make_shared<connection_t>(ios_, this->router_, irouter_);

                acceptor_.async_accept(new_connection->socket(),
                    [this, new_connection](boost::system::error_code const& error)
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
                        irouter_->connection_incoming(error, new_connection);

                    do_accept();
                });
            }

        private:
            ios_t ios_;
            tcp::acceptor acceptor_;
            mutable std::mutex mutex_;
            irouter_ptr irouter_;
        };
    }
}