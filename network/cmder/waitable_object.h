#pragma once
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <boost/optional.hpp>

namespace cytx
{
    class waitable_object
    {
        using lock_t = std::unique_lock<std::mutex>;

    public:
        waitable_object() {}

        void wait()
        {
            lock_t locker{ mutex_ };
            cond_var_.wait(locker, [this] { return is_over_; });
        }

        void wait(int milliseconds)
        {
            if (milliseconds <= 0)
            {
                wait();
                return;
            }

            lock_t locker{ mutex_ };
            cond_var_.wait_for(locker, std::chrono::milliseconds(milliseconds), [this] { return is_over_; });
        }

        void notify()
        {
            lock_t lock{ mutex_ };
            is_over_ = true;
            cond_var_.notify_one();
        }

        void reset()
        {
            if (is_over_)
            {
                is_over_ = false;
            }
        }

        bool is_over() const
        {
            return is_over_;
        }

    protected:
        bool is_over_ = false;
        std::mutex mutex_;
        std::condition_variable cond_var_;
    };

    template<typename T>
    class waitable
    {
        using result_t = boost::optional<T>;
    public:
        void wait()
        {
            object_.wait();
        }

        void wait(int milliseconds)
        {
            object_.wait(milliseconds);
        }

        result_t get()
        {
            wait();
            return result_;
        }

        result_t get(int milliseconds)
        {
            object_.wait(milliseconds);
            return result_;
        }

        void set(const T& v)
        {
            result_ = v;
            object_.notify();
        }

        void reset()
        {
            if (is_over())
            {
                result_ = boost::optional<T>();
                object_.reset();
            }
        }

        bool is_over() const
        {
            return object_.is_over();
        }

    protected:
        boost::optional<T> result_;
        waitable_object object_;
    };

    template<>
    class waitable<void>
    {
    public:
        void wait()
        {
            object_.wait();
        }

        void wait(int milliseconds)
        {
            object_.wait(milliseconds);
        }

        void reset()
        {
            if (is_over())
            {
                object_.reset();
            }
        }

        bool is_over() const
        {
            return object_.is_over();
        }

    protected:
        waitable_object object_;
    };
}