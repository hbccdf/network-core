#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <boost/asio.hpp>
#include "network/timer/timer_manager.hpp"
#include "network/util/time.hpp"

namespace cytx
{
    namespace thread
    {
        class executor
        {
            using io_service_t = boost::asio::io_service;
        public:
            executor() {}
            ~executor() {}

        public:
            io_service_t& get_service() { return io_service_; }

            bool is_stoped() const
            {
                return io_service_.stopped();
            }

            void stop()
            {
                io_service_.stop();
            }
        public:
            template<typename F, typename... Args>
            void execute(F&& f, Args&&... args)
            {
                if (io_service_.stopped())
                    throw std::runtime_error("enqueue on stoped thread_pool");

                io_service_.post(cytx::bind(f, std::forward<Args>(args)...));
            }

            template<typename F, typename... Args>
            auto invoke(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
            {
                if (io_service_.stopped())
                    throw std::runtime_error("enqueue on stoped thread_pool");

                using return_type = typename std::result_of<F(Args...)>::type;
                auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...));
                std::future<return_type> res = task->get_future();
                io_service_.post([task]() {(*task)(); });
                return res;
            }

            template<typename F, typename T>
            auto invoke(F&& f, T&& result) -> std::future<T>
            {
                return invoke([r = std::forward<T>(result)](){ f(); return r; });
            }

            template<typename F>
            auto invoke_all(const std::vector<F>& tasks) -> std::vector<std::future<typename std::result_of<F()>::type>>
            {
                if (io_service_.stopped())
                    throw std::runtime_error("enqueue on stoped thread_pool");

                using return_t = std::future<typename std::result_of<F()>::type>;
                std::vector<return_t> futures;
                futures.reserve(tasks.size());

                for (auto& task : tasks)
                {
                    futures.emplace_back(invoke(task));
                }

                for (auto& future : futures)
                {
                    future.get();
                }

                return futures;
            }

            template<typename F>
            auto schedule(F&& f, int64_t delay_time, int64_t time_unit = time_util::millisecond) -> std::future<typename std::result_of<F()>::type>
            {
                if (io_service_.stopped())
                    throw std::runtime_error("enqueue on stoped thread_pool");

                using return_type = typename std::result_of<F()>::type;
                auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));
                std::future<return_type> res = task->get_future();

                timer_mgr_.set_once_timer((int32_t)(delay_time * time_unit), [task]() {(*task)(); });
                return res;
            }

            template<typename F>
            timer_proxy schedule_at_fixed_rate(F&& f, int64_t period, int64_t time_unit = time_util::millisecond)
            {
                if (io_service_.stopped())
                    throw std::runtime_error("enqueue on stoped thread_pool");

                timer_proxy result = timer_mgr_.set_fix_timer((int32_t)(period * time_unit), std::forward<F>(f));
                result.start();
                return result;
            }

            template<typename F>
            timer_proxy schedule_at_fixed_delay(F&& f, int64_t delay_time, int64_t time_unit = time_util::millisecond)
            {
                if (io_service_.stopped())
                    throw std::runtime_error("enqueue on stoped thread_pool");

                timer_proxy result = timer_mgr_.set_auto_timer((int32_t)(delay_time * time_unit), std::forward<F>(f));
                result.start();
                return result;
            }
        protected:
            io_service_t io_service_;
            timer_manager timer_mgr_{ io_service_ };
        };
    }
}