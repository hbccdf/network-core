#pragma once
#include <network/gameserver/service_base.hpp>

namespace CytxGame
{
    using namespace cytx;

    struct room_config_info
    {
        bool end_battle_when_disconnect = true;
        META(end_battle_when_disconnect);
    };

    class room_config : public cytx::gameserver::service_base
    {
    public:
        bool init();

    public:
        room_config_info& get_config() { return config_info_; }
        const room_config_info& get_config() const { return config_info_; }
    private:
        room_config_info config_info_;
    };

    REG_SERVICE(room_config);
}