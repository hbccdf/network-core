#pragma once
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <functional>
#include <boost/bind.hpp>

namespace cytx {
    namespace net
    {
        class ios_wrapper
        {
        public:
            using io_service_t = boost::asio::io_service;
        private:
            using this_t = ios_wrapper;
            using inter_server_ptr = std::shared_ptr<io_service_t>;
            using work_ptr = std::unique_ptr<io_service_t::work>;
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