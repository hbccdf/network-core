#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include "base/date_time.hpp"
#include "ios_wrapper.hpp"
namespace cytx
{
    using namespace rpc;
    class schedule_timer : public boost::asio::deadline_timer
    {
        using base_t = boost::asio::deadline_timer;
        using ios_t = ios_wrapper;
    public:
        using duration_t = std::chrono::milliseconds;
    public:
        schedule_timer(ios_t& ios)
            : base_t(ios.service())
            , ios_(ios)
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
            base_t::expires_at(date_time::from_local(timestamp));
        }
        void expires_at_ms(int64_t timestamp)
        {
            base_t::expires_at(date_time::from_local_milliseconds(timestamp));
        }
    protected:
        ios_t& ios_;
    };
}