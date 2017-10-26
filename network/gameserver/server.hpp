#pragma once
#include "tcp_connect.hpp"
#include "ios_pool.hpp"

namespace cytx
{
    namespace gameserver
    {
        enum class server_thread_mode : uint8_t
        {
            no_io_thread,
            one_io_thread,
            more_io_thread,
        };

        struct server_options
        {
            server_thread_mode thread_mode;
            int32_t disconnect_interval;
            bool batch_send_msg;
        };

        inline uint32_t get_thread_count(server_thread_mode thread_mode)
        {
            switch (thread_mode)
            {
            case server_thread_mode::no_io_thread:
                return 0;
            case server_thread_mode::one_io_thread:
                return 1;
            case server_thread_mode::more_io_thread:
                return std::thread::hardware_concurrency();
            default:
                return 0;
            }
        }

        template<typename MSG = server_msg>
        class tcp_server
        {
        public:
            using msg_t = MSG;
            using acceptor_t = boost::asio::ip::tcp::acceptor;
            using connection_t = tcp_connection<msg_t>;
            using connection_ptr = std::shared_ptr<connection_t>;
            using msg_ptr = typename connection_t::msg_ptr;
            using irouter_t = typename connection_t::irouter_t;
            using irouter_ptr = typename connection_t::irouter_ptr;
            using io_service_t = boost::asio::io_service;
            using ec_t = boost::system::error_code;

            tcp_server(const std::string& ip, uint16_t port, const server_options& options, irouter_ptr router_ptr = nullptr)
                : ios_pool_(get_thread_count(options.thread_mode))
                , acceptor_(ios_pool_.get_main_service(), cytx::util::get_tcp_endpoint(ip, port), true)
                , router_ptr_(router_ptr)
                , options_(options)
                , cur_conn_id_(0)
            {
            }

            void start()
            {
                do_accept();
                ios_pool_.run();
            }

            void stop()
            {
                ios_pool_.stop();
            }

            io_service_t& get_io_service()
            {
                return ios_pool_.get_main_service();
            }

            const server_options& get_options() const { return options_; }

        private:
            void do_accept()
            {
                ++cur_conn_id_;
                connection_options conn_options;
                conn_options.batch_send_msg = options_.batch_send_msg;
                conn_options.disconnect_interval = options_.disconnect_interval;

                auto new_connection = std::make_shared<connection_t>(ios_pool_.get_io_service(), router_ptr_, cur_conn_id_, conn_options);

                acceptor_.async_accept(new_connection->socket(),
                    [this, new_connection](const ec_t& err) mutable
                {
                    if (!err)
                    {
                        new_connection->start();
                        if (router_ptr_)
                            router_ptr_->on_connect(new_connection, err);
                    }
                    else
                    {
                        LOG_DEBUG("accept error, {}", err.message());
                    }

                    do_accept();
                });
            }
        private:
            ios_pool ios_pool_;
            acceptor_t acceptor_;
            irouter_ptr router_ptr_;
            server_options options_;
            int32_t cur_conn_id_;
        };
    }
}