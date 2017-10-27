#pragma once
#include "game_server_base.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            class server_manager : public game_server_base
            {
                using this_t = server_manager;
                using base_t = game_server_base;
            public:
                server_manager(server_unique_id unique_id)
                    : base_t(unique_id)
                {

                }
                void init(const std::string& config_file_name = "server_config.xml")
                {
                    base_t::init(config_file_name);
                }

            protected:
                connection_ptr get_connection_ptr(server_unique_id unique_id) const override
                {
                    return base_t::get_connection_ptr(unique_id);
                }

            private:
            };
        }
    }
}