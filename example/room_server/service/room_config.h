#pragma once
#include "service_base.hpp"
#include "game_player.h"

namespace CytxGame
{
    using namespace cytx;

    struct room_config_info
    {
        bool end_battle_when_disconnect = true;
        META(end_battle_when_disconnect);
    };

    class room_config : public service_base
    {
    public:
        void init();

    public:
        room_config_info& get_config() { return config_info_; }
        const room_config_info& get_config() const { return config_info_; }
    private:
        room_config_info config_info_;
    };

    REG_SERVICE(room_config);
}