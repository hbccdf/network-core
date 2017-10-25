#pragma once
#include <thread>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
namespace cytx
{
    namespace gameserver
    {
        class ios_pool
        {
            using io_service_t = boost::asio::io_service;
            using ios_ptr = std::shared_ptr<io_service_t>;
            using work_ptr = std::shared_ptr<io_service_t::work>;
        public:
            explicit ios_pool(uint32_t io_service_count)
                : main_ios_ptr_(std::make_shared<io_service_t>())
            {
                init_io_services(io_service_count);
            }
            explicit ios_pool(io_service_t& main_ios, uint32_t io_service_count)
                : main_ios_ptr_(ios_ptr(&main_ios, [](auto* ptr) {}))
            {
                init_io_services(io_service_count);
            }
            explicit ios_pool()
                : ios_pool(std::thread::hardware_concurrency())
            {
            }
            explicit ios_pool(io_service_t& main_ios)
                : ios_pool(main_ios, std::thread::hardware_concurrency())
            {
            }

        public:
            void run()
            {
                for (auto& ios : ios_ptrs_)
                {
                    std::thread td(boost::bind(&io_service_t::run, ios));
                    td.detach();
                }

                main_ios_ptr_->run();
            }

            void stop()
            {
                for (auto& ios : ios_ptrs_)
                {
                    ios->stop();
                }

                main_ios_ptr_->stop();
            }

            io_service_t& get_main_service()
            {
                return *main_ios_ptr_;
            }
            io_service_t& get_io_service()
            {
                if (ios_ptrs_.empty())
                    return *main_ios_ptr_;

                auto& ios = *(ios_ptrs_[cur_ios_index_]);
                ++cur_ios_index_;
                cur_ios_index_ = cur_ios_index_ % (int32_t)ios_ptrs_.size();
                return ios;
            }
            io_service_t& get_io_service(int32_t id)
            {
                if (ios_ptrs_.empty())
                    return *main_ios_ptr_;

                int32_t index = id % (int32_t)ios_ptrs_.size();
                return *(ios_ptrs_[index]);
            }

        private:
            void init_io_services(uint32_t io_service_count)
            {
                works_.push_back(std::make_shared<io_service_t::work>(*main_ios_ptr_));

                if (io_service_count <= 0)
                    return;

                for (uint32_t i = 0; i < io_service_count; ++i)
                {
                    auto ios = std::make_shared<io_service_t>();
                    auto work = std::make_shared<io_service_t::work>(*ios);
                    ios_ptrs_.push_back(ios);
                    works_.push_back(work);
                }
            }

        private:
            ios_pool(ios_pool&) = delete;
            ios_pool& operator= (ios_pool&) = delete;

        private:
            ios_ptr main_ios_ptr_;
            std::vector<ios_ptr> ios_ptrs_;
            std::vector<work_ptr> works_;
            int32_t cur_ios_index_ = 0;
        };
    }
}