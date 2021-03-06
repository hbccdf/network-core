#pragma once
#include "network/net/exception.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <mutex>

namespace cytx {
    namespace rpc
    {
        static const auto asio_error = boost::asio::placeholders::error;

        class async_connection
        {
            using tcp = boost::asio::ip::tcp;
            using io_service_t = boost::asio::io_service;
        public:
            async_connection(io_service_t& ios, tcp::endpoint const& endpoint)
                : socket_(ios)
                , endpoint_(endpoint)
            {
            }

            async_connection(io_service_t& ios)
                : socket_(ios)
            {
            }

            void start(
                std::function<void()>&& on_success,
                std::function<void(const boost::system::error_code&)>&& on_error)
            {
                on_success_ = std::move(on_success);
                on_error_ = std::move(on_error);
                start_connect();
            }

            tcp::socket& socket()
            {
                return socket_;
            }

            tcp::endpoint const& endpoint() const
            {
                return endpoint_;
            }

        private:
            void start_connect()
            {
                socket_.async_connect(endpoint_, boost::bind(&async_connection::handle_connection, this, asio_error));
            }

            void handle_connection(const boost::system::error_code& error)
            {
                if (!error)
                {
                    if (on_success_)
                        on_success_();
                }
                else
                {
                    if (on_error_)
                        on_error_(error);
                }
                on_error_ = nullptr;
                on_success_ = nullptr;
            }

        private:
            tcp::socket socket_;
            tcp::endpoint endpoint_;
            std::function<void()> on_success_;
            std::function<void(const boost::system::error_code&)> on_error_;
        };
    }
}