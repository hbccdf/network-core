#pragma once
#include "game_player.h"
#include "service_base.hpp"

namespace CytxGame
{
    class player_service : public service_base
    {
    public:
        void reset() {}

        player_ptr_t find_player(int user_id, bool is_login = true);
        player_ptr_t get_player(int user_id);
        void handle_player_disconnect(int user_id);
    private:
        player_map_t players_;
    };

    REG_SERVICE(player_service);
}