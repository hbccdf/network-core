#pragma once
#include "../service/service_meta.hpp"
#include "server_config.hpp"

namespace cytx
{
    namespace gameserver
    {
        class config_service
        {
        public:
            config_service(server_config_manager& config_mgr)
                : config_mgr_(config_mgr)
            {}

            template<typename T>
            void get_config(T& config, const std::string& key)
            {
                return config_mgr_.get_config(config, key);
            }
        private:
            server_config_manager& config_mgr_;
        };
    }
}