#pragma once
#include "game_server.hpp"

namespace cytx
{
    namespace gameserver
    {
        class service_base
        {
        protected:
            using game_server_t = game_server;
        public:
            void set_world(world_ptr_t world_ptr)
            {
                server_ = world_ptr["game_server"];
            }
        protected:
            game_server_t* server_ = nullptr;
        };
    }
}