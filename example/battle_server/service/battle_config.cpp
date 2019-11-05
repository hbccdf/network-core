#include "battle_config.h"
#include <network/gameserver/config_service.hpp>

namespace CytxGame
{
    using namespace cytx::gameserver;

    bool battle_config::init()
    {
        LOG_DEBUG("battle config init");
        config_service* service_ptr = server_->get_service<config_service>();
        if (!service_ptr)
            return false;

        service_ptr->get_config(config_info_, "config.battle");
        if (config_info_.update_time <= 0)
            config_info_.update_time = 1000;

        return true;
    }
}