#include "update_timer.h"
#include "battle_config.h"

namespace CytxGame
{
    void update_timer::init()
    {
        config_ = timer_config{};

        battle_config* config_ptr = server_->get_service<battle_config>();
        if (!config_ptr)
            return;

        auto& config = config_ptr->get_config();
        timer_ = server_->set_fix_timer(config.update_time, std::bind(&this_t::update, this));

        config_.custom_delta = config.custom_delta;
        config_.use_custom_delta = config.use_custom_delta;
        last_time_ = date_time::now();
    }

    void update_timer::start()
    {
        timer_.start();
    }

    void update_timer::update()
    {
        date_time current_time = date_time::now();
        if (func_)
        {
            float delta_time = (current_time.total_milliseconds() - last_time_.total_milliseconds()) / 1000.0f;

#if defined(DEBUG) || defined(_DEBUG)
            if (config_.use_custom_delta)
                delta_time = config_.custom_delta;
            if (delta_time <= 0.0005f)
                delta_time = 0.02f;
#endif
            func_(delta_time);
        }

        last_time_ = current_time;
    }

    void update_timer::set_update_func(func_t&& func)
    {
        func_ = func;
    }

}