#pragma once
#include "proto/common_msg.h"

namespace CytxGame
{
    class game_room;
    class game_player;
    using room_t = game_room;
    using room_ptr_t = std::shared_ptr<room_t>;

    using player_t = game_player;
    using player_ptr_t = std::shared_ptr<player_t>;

    using player_map_t = std::map<int, player_ptr_t>;
    using room_map_t = std::map<int, room_ptr_t>;
}