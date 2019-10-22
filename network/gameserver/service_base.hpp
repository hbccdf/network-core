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
            void set_info(void* info_ptr)
            {
                server_ = static_cast<game_server_t*>(info_ptr);
            }
        protected:
            game_server_t* server_ = nullptr;
        };
    }
}