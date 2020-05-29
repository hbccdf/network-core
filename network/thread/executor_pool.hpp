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
                    executors_[i] = new executor();
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

            executor* get_executor()
            {
                if (executors_.empty())
                    return nullptr;

                auto executor_ptr = executors_[cur_executor_index_];
                ++cur_executor_index_;
                cur_executor_index_ = cur_executor_index_ % (int32_t)executors_.size();
                return executor_ptr;
            }
            executor* get_executor(int32_t id)
            {
                if (executors_.empty() || id < 0)
                    return nullptr;

                int32_t index = id % (int32_t)executors_.size();
                return executors_[index];
            }

            size_t size() const { return executors_.size(); }
            int32_t get_cur_executor_index() const { return executors_.empty() ? -1 : cur_executor_index_; }

        protected:
            std::vector<executor*> executors_;
            std::vector<std::thread> threads_;
            std::vector<work_ptr> works_;
            int32_t cur_executor_index_ = 0;
        };
    }
}