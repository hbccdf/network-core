#include "battle_manager.h"
#include "update_timer.h"
#include "proto/message_types.h"

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
        for (auto battle_ptr : battles_)
        {
            battle_ptr->stop();
            delete battle_ptr;
        }
        battles_.clear();
    }

    void battle_manager::update(float delta)
    {
        for (auto it = battles_.begin(); it != battles_.end();)
        {
            auto battle_ptr = *it;
            battle_ptr->update(delta);
            if (battle_ptr->need_destroy())
            {
                battle_ptr->stop();
                delete battle_ptr;
                it = battles_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    bool battle_manager::create_battle(SCRoomInfo& room_info)
    {
        battle_init_data data;
        data.battle_id = get_battle_id();
        data.room_data = room_info;

        arena_battle* battle_ptr = new arena_battle();
        battle_ptr->init(data);
        battle_ptr->start();
        battles_.push_back(battle_ptr);
        return true;
    }

    arena_battle* battle_manager::get_battle(int32_t user_id)
    {
        return nullptr;
    }

    int battle_manager::get_battle_id()
    {
        static int id = 0;
        return ++id;
    }

}