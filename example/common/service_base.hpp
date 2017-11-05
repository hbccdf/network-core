#pragma once
#include <network/service/service_meta.hpp>
#include <network/gameserver/game_server.hpp>

namespace CytxGame
{
    class service_base
    {
        using game_server_base_t = cytx::game_server_base_t;
        using game_server_t = cytx::gameserver::game_server;
    public:
        void set_server(game_server_base_t* server_ptr)
        {
            server_ = static_cast<game_server_t*>(server_ptr);
        }
    protected:
        game_server_t* server_ = nullptr;
    };
}