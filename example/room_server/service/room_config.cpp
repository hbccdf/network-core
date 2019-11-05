#include "room_config.h"
#include <network/gameserver/config_service.hpp>

namespace CytxGame
{
    using namespace cytx::gameserver;

    bool room_config::init()
    {
        LOG_DEBUG("room config init");
        config_service* service_ptr = server_->get_service<config_service>();
        if (!service_ptr)
            return false;

        service_ptr->get_config(config_info_, "config.room");
        return true;
    }
}