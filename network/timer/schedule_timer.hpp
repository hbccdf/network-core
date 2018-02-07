#pragma once
#include <chrono>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace cytx
{
    class schedule_timer : public boost::asio::deadline_timer
    {
        using base_t = boost::asio::deadline_timer;
        using io_service_t = boost::asio::io_service;
    public:
        using duration_t = std::chrono::milliseconds;
    public:
        schedule_timer(io_service_t& ios)
            : base_t(ios)
        {
        }

        ~schedule_timer() { cancel(); }
    public:
        void async_wait_func(int milliseconds, std::function<void()> func)
        {
            expires(milliseconds);
            base_t::async_wait([f = std::move(func)](const boost::system::error_code& ec){
                if (!ec)
                    f();
            });
        }

        void async_wait_func_at_time(int64_t timestamp, std::function<void()> func)
        {
            expires_at_ms(timestamp);
            base_t::async_wait([f = std::move(func)](const boost::system::error_code& ec){
                if (!ec)
                    f();
            });
        }

        void async_wait(int milliseconds, std::function<void(const boost::system::error_code&)> func)
        {
            expires(milliseconds);
            base_t::async_wait(func);
        }

        void async_wait_at_time(int64_t timestamp, std::function<void(const boost::system::error_code&)> func)
        {
            expires_at_ms(timestamp);
            base_t::async_wait(func);
        }

        void async_wait_func(const duration_t& duration, std::function<void()> func)
        {
            expires((int)duration.count());
            base_t::async_wait([f = std::move(func)](const boost::system::error_code& ec){
                if (!ec)
                    f();
            });
        }

        void async_wait(const duration_t& duration, std::function<void(const boost::system::error_code&)> func)
        {
            expires((int)duration.count());
            base_t::async_wait(func);
        }
    protected:
        void expires(int milliseconds)
        {
            expires_from_now(boost::posix_time::milliseconds(milliseconds));
        }
        void expires_at(int timestamp)
        {
            base_t::expires_at(boost::posix_time::from_time_t(timestamp));
        }
        void expires_at_ms(int64_t timestamp)
        {
            auto t = boost::posix_time::from_time_t(timestamp / 1000) + boost::posix_time::milliseconds(timestamp % 1000);
            base_t::expires_at(t);
        }
    };
}