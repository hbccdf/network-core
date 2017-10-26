#pragma once

#include "../serialize/deserializer.hpp"
#include "../serialize/xml_adapter.hpp"
#include "common.hpp"
#include "raw_msg.hpp"

namespace cytx
{
    struct common_info
    {
        bool client_big_endian = false;
        bool server_big_endian = false;
        META(client_big_endian, server_big_endian);
    };

    struct server_cfg
    {
        std::vector<server_info> server;
        common_info common;
        META(server, common);
    };

    class server_config_manager
    {
    public:
        void init(const std::string& config_file, bool modify_endian = true)
        {
            de_.enum_with_str(true);
            de_.parse_file(config_file);
            de_.DeSerialize(config_, "config");
            for (auto& config : config_.server)
            {
                map_.emplace(config.unique_id, &config);
            }
            if (modify_endian)
            {
                gameserver::msg_header::big_endian(config_.common.server_big_endian);
            }
        }

        template<typename T>
        void get_config(T& config, const std::string& key)
        {
            de_.DeSerialize(config, key);
        }

        common_info& common() { return config_.common; }
        const common_info& common() const { return config_.common; }

        server_info& operator[](server_unique_id unique_id)
        {
            return *map_[unique_id];
        }
        const server_info& operator[](server_unique_id unique_id) const
        {
            auto it = map_.find(unique_id);
            return *it->second;
        }

        void parse_real_ip(std::function<std::string(std::string, uint16_t)> func)
        {
            for (auto& config : config_.server)
            {
                config.host_ip = config.ip;
                config.ip = func(config.host_ip, config.port);
            }
        }
    private:
        DeSerializer<xml_deserialize_adapter> de_;
        server_cfg config_;
        std::map<server_unique_id, server_info*> map_;
    };
}