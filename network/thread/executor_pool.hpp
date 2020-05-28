#pragma once
#include "executor.hpp"

namespace cytx
{
    namespace thread
    {
        class executor_pool
        {
            using io_service_t = boost::asio::io_service;
            using work_ptr = std::shared_ptr<io_service_t::work>;
        public:
            executor_pool() {}
            ~executor_pool()
            {
                stop();

                works_.clear();
                for (auto executor_ptr : executors_)
                {
                    delete executor_ptr;
                }
                executors_.clear();
                threads_.clear();
            }

        public:
            void init(int thread_count)
            {
                executors_.resize(thread_count);

                for (int i = 0; i < thread_count; i++)
                {
                    executors_.push_back(new executor());
                }
            }

            void start()
            {
                if (threads_.empty())
                {
                    for (int i = 0; i < executors_.size(); ++i)
                    {
                        auto executor_ptr = executors_[i];
                        auto work = std::make_shared<io_service_t::work>(executor_ptr->get_service());
                        works_.push_back(work);
                        threads_.emplace_back(std::thread([executor_ptr] { executor_ptr->get_service().run(); }));
                    }
                }
            }
            void stop()
            {
                for (auto executor_ptr : executors_)
                {
                    executor_ptr->stop();
                }

                for (auto& td : threads_)
                {
                    td.join();
                }
            }

        protected:
            std::vector<executor*> executors_;
            std::vector<std::thread> threads_;
            std::vector<work_ptr> works_;
        };
    }
}