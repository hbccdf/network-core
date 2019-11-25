#pragma once
#include "net_common.hpp"
#include "tcp_connect.hpp"
#include "ios_pool.hpp"

#define NET_LOG(level, str, ...)                \
if(log_)                                        \
{                                               \
    log_->level(str, ## __VA_ARGS__);           \
}

#define NET_DEBUG(str, ...)                     \
if(log_)                                        \
{                                               \
    log_->debug(str, ## __VA_ARGS__);           \
}

#define NET_TRACE(str, ...)                     \
if(log_)                                        \
{                                               \
    log_->trace(str, ## __VA_ARGS__);           \
}

namespace cytx
{
    namespace net
    {
        struct server_options
        {
            server_thread_mode thread_mode;
            int32_t disconnect_interval;
            bool batch_send_msg;
        };

        template<typename MSG = server_msg<msg_body>>
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
                , acceptor_(ios_pool_.get_main_service(), util::to_tcp_endpoint(port), true)
                , router_ptr_(router_ptr)
                , options_(options)
                , cur_conn_id_(0)
                , log_(cytx::log::get_log("net"))
            {
                NET_DEBUG("create server tcp://{}:{}, thread_mode {}, io thread count {}", ip, port, (int32_t)options.thread_mode, ios_pool_.size());
            }

            void start()
            {
                NET_DEBUG("tcp server start");
                do_accept();
                ios_pool_.start();
            }

            void stop()
            {
                NET_DEBUG("tcp server begin stop");
                ios_pool_.stop();
            }

            io_service_t& get_io_service()
            {
                return ios_pool_.get_main_service();
            }

            const server_options& get_options() const { return options_; }

            connection_ptr create_connection()
            {
                ++cur_conn_id_;
                connection_options conn_options;
                conn_options.batch_send_msg = options_.batch_send_msg;
                conn_options.disconnect_interval = options_.disconnect_interval;

                NET_DEBUG("tcp server create connection {}, ios index {}, batch_send_msg {}, disconnect_interval {}", cur_conn_id_,
                    ios_pool_.get_cur_ios_index(), conn_options.batch_send_msg, conn_options.disconnect_interval);

                return std::make_shared<connection_t>(ios_pool_.get_io_service(), router_ptr_, cur_conn_id_, conn_options);
            }
        private:
            void do_accept()
            {
                auto new_connection = create_connection();
                NET_DEBUG("tcp server do accept");

                acceptor_.async_accept(new_connection->socket(),
                    [this, new_connection](const ec_t& err) mutable
                {
                    if (!err)
                    {
                        NET_DEBUG("tcp server accepted on connection {}", new_connection->get_conn_id());
                        if (router_ptr_)
                            router_ptr_->on_connect(new_connection, err);

                        new_connection->start();
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
            cytx::log_ptr_t log_;
        };
    }
}