#pragma once
#include <network/gameserver/service_base.hpp>

namespace CytxGame
{
    using namespace cytx;

    struct battle_server_info
    {
        int update_time = 1000;
        bool use_custom_delta = false;
        float custom_delta = 0.02f;
        META(update_time, use_custom_delta, custom_delta);
    };

    class battle_config : public cytx::gameserver::service_base
    {
    public:
        bool init();

    public:
        battle_server_info& get_config() { return config_info_; }
        const battle_server_info& get_config() const { return config_info_; }
    private:
        battle_server_info config_info_;
    };

    REG_SERVICE(battle_config);
}