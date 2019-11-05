#pragma once
#include <network/gameserver/service_base.hpp>
#include <network/base/date_time.hpp>

namespace CytxGame
{
    using namespace cytx;

    struct timer_config
    {
        bool use_custom_delta;
        float custom_delta;
    };
    class update_timer : public cytx::gameserver::service_base
    {
        using this_t = update_timer;
        using timer_t = cytx::timer_proxy;
    public:
        bool init();
        bool start();

    public:
        using func_t = std::function<void(float)>;
        void set_update_func(func_t&& func);
    private:
        void update();
    private:
        timer_t timer_;
        date_time last_time_;
        func_t func_;
        timer_config config_;
    };

    REG_SERVICE(update_timer);
}