#pragma once
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <thread>
#include <mutex>
#include <functional>
#include <boost/bind.hpp>

namespace cytx {
    namespace rpc
    {
        using io_service_t = boost::asio::io_service;
        using tcp = boost::asio::ip::tcp;
        using deadline_timer_t = boost::asio::deadline_timer;
        using steady_timer_t = boost::asio::steady_timer;
        using duration_t = steady_timer_t::duration;

        class ios_wrapper
        {
            using this_t = ios_wrapper;
            using inter_server_ptr = std::shared_ptr<io_service_t>;
            using work_ptr = std::unique_ptr<io_service_t::work>;
            using lock_t = std::unique_lock<std::mutex>;
        public:
            using io_service_t = boost::asio::io_service;
        public:
            ios_wrapper()
                : is_owner_(true)
                , ios_(std::make_shared<io_service_t>())
                , ios_work_(std::make_unique<io_service_t::work>(*ios_))
            {}

            ios_wrapper(io_service_t& ios)
                : is_owner_(false)
                , ios_(inter_server_ptr(&ios, [](auto* ptr) {}))
                , ios_work_(std::make_unique<io_service_t::work>(*ios_))
            {}

            ~ios_wrapper()
            {
                stop();
                ios_work_.reset();
                ios_.reset();
            }

            void start()
            {
                if (is_owner_)
                {
                    ios_run_thread_ = std::thread(boost::bind(&io_service_t::run, ios_));
                }
            }

            void stop()
            {
                if (is_owner_)
                {
                    ios_work_.reset();
                    if (ios_->stopped())
                        ios_->stop();
                    if (ios_run_thread_.joinable())
                        ios_run_thread_.join();
                }
            }

            bool is_owner() const
            {
                return is_owner_;
            }

            io_service_t& service()
            {
                return *ios_;
            }

            operator io_service_t&()
            {
                return service();
            }

        private:
            bool is_owner_;
            inter_server_ptr ios_;
            work_ptr ios_work_;
            std::thread ios_run_thread_;
        };
    }
}