#pragma once
#include "game_server.hpp"

namespace cytx
{
    namespace gameserver
    {
        class service_base
        {
        protected:
            using game_server_base_t = cytx::game_server_base_t;
            using game_server_t = game_server;
        public:
            void set_server(game_server_base_t* server_ptr)
            {
                server_ = static_cast<game_server_t*>(server_ptr);
            }
        protected:
            game_server_t* server_ = nullptr;
        };
    }
}