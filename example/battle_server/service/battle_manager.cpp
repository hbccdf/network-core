#include "battle_manager.h"
#include "update_timer.h"

namespace CytxGame
{
    void battle_manager::init()
    {
        LOG_DEBUG("battle manager init");
        update_timer* timer_ptr = server_->get_service<update_timer>();
        if (!timer_ptr)
            return;

        timer_ptr->set_update_func(cytx::bind(&this_t::update, this));
    }

    void battle_manager::stop()
    {

    }

    void battle_manager::update(float delta)
    {

    }

    int battle_manager::get_battle_id()
    {
        static int id = 0;
        return ++id;
    }

}