#pragma once
#include <chrono>

namespace cytx
{
    class stop_watch
    {
        using clock_t = std::chrono::high_resolution_clock;
        using microseconds_t = std::chrono::microseconds;
        using milliseconds_t = std::chrono::milliseconds;
        using seconds_t = std::chrono::seconds;
    public:
        stop_watch()
            : elapsed_(0)
        {
        }
        ~stop_watch() {}
    public:
        void start()
        {
            begin_time_ = clock_t::now();
        }
        void stop()
        {
            auto end_time = clock_t::now();
            elapsed_ += std::chrono::duration_cast<microseconds_t>(end_time - begin_time_).count();
        }
        void restart()
        {
            elapsed_ = 0;
            start();
        }
        //Œ¢√Î
        double elapsed() const
        {
            return static_cast<double>(elapsed_);
        }
        //∫¡√Î
        double elapsed_ms() const
        {
            return elapsed_ / 1000.0;
        }
        //√Î
        double elapsed_second() const
        {
            return elapsed_ / 1000000.0;
        }

    private:
        clock_t::time_point begin_time_;
        int64_t elapsed_;
    };
}