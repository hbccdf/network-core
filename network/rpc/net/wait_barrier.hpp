#pragma once
#include <mutex>
#include <condition_variable>
#include "ios_wrapper.hpp"

namespace cytx {
    namespace rpc
    {
        class ios_result_barrier
        {
            using lock_t = std::unique_lock<std::mutex>;
        public:
            using ios_t = ios_wrapper;
            ios_result_barrier(ios_t& ios)
                : ios_(ios)
                , is_over_(false)
            {

            }
            void wait()
            {
                if (ios_.is_owner())
                {
                    using namespace std::chrono_literals;
                    lock_t locker{ mutex_ };
                    cond_var_.wait(locker, [this] { return is_over_; });
                }
                else
                {
                    while (!is_over_)
                        ios_.service().run_one();
                }
            }

            void notify()
            {
                if (ios_.is_owner())
                {
                    lock_t lock{ mutex_ };
                    is_over_ = true;
                    cond_var_.notify_one();
                }
                else
                {
                    is_over_ = true;
                }
            }

            bool is_over() const
            {
                return is_over_;
            }

        protected:
            ios_t& ios_;
            bool is_over_ = false;
            std::mutex mutex_;
            std::condition_variable cond_var_;
        };
    }
}