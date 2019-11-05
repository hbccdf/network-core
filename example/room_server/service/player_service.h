#pragma once
#include <network/gameserver/service_base.hpp>
#include "game_player.h"

namespace CytxGame
{
    class player_service : public cytx::gameserver::service_base
    {
    public:
        bool init();
        void reset();

        player_ptr_t find_player(int user_id, bool is_login = true);
        player_ptr_t get_player(int user_id);
        void handle_player_disconnect(int user_id);
    private:
        player_map_t players_;
    };

    REG_SERVICE(player_service);
}