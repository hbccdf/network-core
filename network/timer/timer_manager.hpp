﻿#pragma once
#include <boost/asio.hpp>
#include "schedule_timer.hpp"
#include "network/traits/traits.hpp"
#include "network/base/date_time.hpp"
#include "network/base/log.hpp"

#define TIMER_LOG(level, str, ...)              \
if(log_)                                        \
{                                               \
    log_->level(str,  ## __VA_ARGS__);          \
}

#define TIMER_DEBUG(str, ...)                   \
if(log_)                                        \
{                                               \
    log_->debug(str, ## __VA_ARGS__);           \
}

#define TIMER_TRACE(str, ...)                   \
if(log_)                                        \
{                                               \
    log_->trace(str, ## __VA_ARGS__);           \
}

namespace cytx
{
    class timer_manager;

    class timer_proxy
    {
        friend class timer_manager;
        timer_proxy(timer_manager* timer, int id)
            : timer_(timer), id_(id), need_start_(true)
        {
        }

    public:
        timer_proxy()
            : timer_(nullptr), id_(0), need_start_(false)
        {}

        ~timer_proxy()
        {
            auto_start_timer();
        }

        timer_proxy(const timer_proxy& other) = delete;
        timer_proxy& operator=(const timer_proxy& other) = delete;

        timer_proxy(timer_proxy&& other)
            : timer_(other.timer_), id_(other.id_), need_start_(other.need_start_)
        {
            other.need_start_ = false;
        }

        timer_proxy& operator=(timer_proxy&& other)
        {
            timer_ = other.timer_;
            id_ = other.id_;
            need_start_ = other.need_start_;
            other.need_start_ = false;
            return *this;
        }

        bool operator < (const timer_proxy& other) const { return id_ < other.id_; }
        bool operator == (const timer_proxy& other) const { return id_ == other.id_; }

    public:
        void start(bool invoke_first = false);
        void stop();
        void invoke();
        void close();
    private:
        void auto_start_timer();
    private:
        timer_manager* timer_;
        int32_t id_;
        bool need_start_;
    };

    class timer_manager
    {
        using io_service_t = boost::asio::io_service;
        using timer_t = schedule_timer;
        using timer_ptr = std::shared_ptr<timer_t>;
        using this_t = timer_manager;
        using timer_func_t = std::function<bool()>;

        friend timer_proxy;
    protected:
        enum class timer_status { ok, canceled, };
        struct timer_info
        {
            int32_t id;
            timer_status status;
            int milliseconds;
            int64_t next_time;
            timer_ptr timer;
            timer_func_t timer_func;

            bool operator <(const timer_info& other) const { return id < other.id; }
        };

        using timer_info_ptr = std::shared_ptr<timer_info>;
        using timer_map_t = std::map<int32_t, timer_info_ptr>;

    public:
        timer_manager(io_service_t& ios, log_ptr_t log_ptr = nullptr)
            : ios_(ios)
            , timer_id_(0)
            , log_(log_ptr)
        {
            TIMER_DEBUG("create timer manager");
        }

        ~timer_manager() { stop_all_timer(); }
    public:
        timer_proxy set_timer(int32_t milliseconds, timer_func_t func)
        {
            timer_info_ptr ti_ptr = create_timer(milliseconds, func);
            TIMER_DEBUG("create timer {}, millisecond {}, timer count {}", ti_ptr->id, milliseconds, timers_.size());
            return timer_proxy(this, ti_ptr->id);
        }

        timer_proxy set_fix_timer(int32_t milliseconds, timer_func_t func)
        {
            timer_info_ptr ti_ptr = create_timer(milliseconds, func);
            ti_ptr->next_time = date_time::now().utc_milliseconds() + milliseconds;
            TIMER_DEBUG("create fix timer {}, millisecond {}, timer count {}", ti_ptr->id, milliseconds, timers_.size());
            return timer_proxy(this, ti_ptr->id);
        }

        timer_proxy set_fix_timer(int32_t milliseconds, std::function<void()> func)
        {
            return set_fix_timer(milliseconds, (timer_func_t)[f = std::move(func)]()->bool{
                f();
                return true;
            });
        }

        timer_proxy set_once_timer(int32_t milliseconds, std::function<void()> func)
        {
            return set_timer(milliseconds, [f = std::move(func)]()->bool{
                f();
                return false;
            });
        }

        timer_proxy set_auto_timer(int32_t milliseconds, std::function<void()> func)
        {
            return set_timer(milliseconds, [f = std::move(func)]()->bool{
                f();
                return true;
            });
        }

        void stop_all_timer()
        {
            TIMER_DEBUG("stop all timer, size {}", timers_.size());
            for (auto& timer_pair : timers_)
            {
                stop_timer(timer_pair.second);
            }
        }
    protected:
        void auto_start_timer(int32_t id)
        {
            auto it = timers_.find(id);
            if (it != timers_.end() && it->second->status != timer_status::canceled)
            {
                TIMER_DEBUG("auto start timer {}", it->second->id);
                start_timer(it->second);
            }
        }

        void start_timer(int32_t id)
        {
            auto it = timers_.find(id);
            if (it != timers_.end())
            {
                TIMER_DEBUG("start timer {}", it->second->id);
                start_timer(it->second);
            }
        }

        void stop_timer(int32_t id)
        {
            auto it = timers_.find(id);
            if (it != timers_.end())
            {
                TIMER_DEBUG("stop timer {}", it->second->id);
                stop_timer(it->second);
            }
        }

        void invoke_timer(int32_t id)
        {
            auto it = timers_.find(id);
            if (it != timers_.end())
            {
                TIMER_DEBUG("invoke timer {}", it->second->id);
                invoke_timer(it->second);
            }
        }

        void close_timer(int32_t id)
        {
            auto it = timers_.find(id);
            if (it != timers_.end())
            {
                TIMER_DEBUG("close timer {}, current timer count {}", it->second->id, timers_.size());
                stop_timer(it->second);
                timers_.erase(it);
            }
        }

        void start_timer(timer_info_ptr ti_ptr)
        {
            ti_ptr->status = timer_status::ok;
            auto func = boost::bind(&this_t::timer_handler, this, boost::asio::placeholders::error, ti_ptr);
            if (ti_ptr->next_time != 0)
            {
                calc_next_time(ti_ptr);
                ti_ptr->timer->async_wait_at_time(ti_ptr->next_time, func);
            }
            else
            {
                ti_ptr->timer->async_wait(ti_ptr->milliseconds, func);
            }
        }

        void stop_timer(timer_info_ptr ti_ptr)
        {
            boost::system::error_code ec(1, boost::system::system_category());
            ti_ptr->timer->cancel(ec);
            ti_ptr->status = timer_status::canceled;
        }

        void invoke_timer(timer_info_ptr ti_ptr)
        {
            if (ti_ptr->timer_func)
            {
                ti_ptr->timer_func();
            }
        }

        void timer_handler(const boost::system::error_code& ec, timer_info_ptr ti_ptr)
        {
            TIMER_TRACE("timer {}, status {}, ec {}", ti_ptr->id, (int32_t)ti_ptr->status, ec.message());
            if (!ec && timer_status::ok == ti_ptr->status)
            {
                bool is_continue = false;
                if (ti_ptr->timer_func)
                {
                    is_continue = ti_ptr->timer_func();
                }

                if (is_continue || ti_ptr->next_time > 0)
                {
                    TIMER_TRACE("continue timer {}", ti_ptr->id);
                    start_timer(ti_ptr);
                }
            }
        }

        void calc_next_time(timer_info_ptr ti_ptr)
        {
            ti_ptr->next_time += ti_ptr->milliseconds;
            int64_t now_time = date_time::now().utc_milliseconds();
            if (ti_ptr->next_time < now_time)
            {
                ti_ptr->next_time = now_time;
            }
        }

    private:
        timer_info_ptr create_timer(int milliseconds, timer_func_t func)
        {
            auto id = ++timer_id_;
            timer_info_ptr ti_ptr = std::make_shared<timer_info>();
            ti_ptr->id = id;
            ti_ptr->status = timer_status::ok;
            ti_ptr->milliseconds = milliseconds;
            ti_ptr->next_time = 0;
            ti_ptr->timer = std::make_shared<timer_t>(ios_);
            ti_ptr->timer_func = func;

            auto it = timers_.find(id);
            if (it != timers_.end())
            {
                it->second = ti_ptr;
            }
            else
            {
                timers_.emplace(id, ti_ptr);
            }
            return ti_ptr;
        }

    protected:
        io_service_t& ios_;
        timer_map_t timers_;
        int32_t timer_id_;
        cytx::log_ptr_t log_;
    };

    inline void timer_proxy::start(bool invoke_first/* = false*/)
    {
        if (timer_)
        {
            if (invoke_first)
            {
                timer_->invoke_timer(id_);
            }
            need_start_ = false;
            timer_->start_timer(id_);
        }
    }

    inline void timer_proxy::stop()
    {
        if (timer_)
        {
            need_start_ = false;
            timer_->stop_timer(id_);
        }
    }

    inline void timer_proxy::invoke()
    {
        if (timer_)
        {
            timer_->invoke_timer(id_);
        }
    }

    inline void timer_proxy::close()
    {
        if (timer_)
        {
            need_start_ = false;
            timer_->close_timer(id_);
            timer_ = nullptr;
        }
    }

    inline void timer_proxy::auto_start_timer()
    {
        if (need_start_ && timer_)
        {
            timer_->auto_start_timer(id_);
        }
        need_start_ = false;
    }
}