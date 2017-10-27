#pragma once
#include "game_server_base.hpp"
namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            class game_server : public game_server_base
            {
                using this_t = game_server;
                using base_t = game_server_base;
            public:
                game_server(server_unique_id unique_id)
                    : game_server_base(unique_id)
                {

                }

            protected:
                connection_ptr get_connection_ptr(server_unique_id unique_id) const override
                {
                    if (unique_id == server_unique_id::center_server)
                        return center_conn_ptr_;

                    return base_t::get_connection_ptr(unique_id);
                }
            private:
                connection_ptr center_conn_ptr_;
            };
        }
        using game_server = detail::game_server;
    }
}