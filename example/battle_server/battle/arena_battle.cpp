#include "arena_battle.h"
#include <network/base/log.hpp>

namespace CytxGame
{
    arena_battle::arena_battle()
    {

    }

    arena_battle::~arena_battle()
    {

    }

    void arena_battle::init(battle_init_data& data)
    {
        room_info_ = data.room_data;
        info_.battle_id = data.battle_id;
    }

    void arena_battle::start()
    {
        LOG_DEBUG("battle {} start", info_.battle_id);
    }

    void arena_battle::update(float delta_time)
    {
        if (!info_.is_running)
            return;

        ++info_.frame;
        info_.battle_time += delta_time;
    }

    void arena_battle::stop()
    {
        LOG_DEBUG("battle {} stop", info_.battle_id);
    }

}