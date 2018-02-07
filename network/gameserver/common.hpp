#pragma once
#include "../base/common.h"
#include "../base/log.hpp"

namespace cytx
{
    enum class server_unique_id
    {
        none = 0,
        master = 1,
        slave = 2,
        redis_server = 3,
        db_server = 4,
        login_server = 5,
        res_server = 6,
        lobby_server = 7,
        room_server = 8,
        replay_server = 9,
        gateway_server = 10,
        chat_server = 11,
        push_server = 12,
        battle_server = 13,
        monitor_server = 14,
        rank_server = 15,
        log_server = 16,
        center_server = 17,

        server_num,
    };

    REG_ENUM(server_unique_id, master, slave, redis_server,
        db_server, login_server, res_server, lobby_server, room_server,
        replay_server, gateway_server, chat_server, push_server, battle_server,
        monitor_server, rank_server, log_server, center_server);

    enum class server_thread_mode : uint8_t
    {
        no_io_thread,
        one_io_thread,
        more_io_thread,
    };
    REG_ENUM(server_thread_mode, no_io_thread, one_io_thread, more_io_thread);

    struct server_log_info
    {
        std::string name;
        std::string filename;
        log_level_t level = log_level_t::debug;
        bool console = true;
        META(name, filename, level, console);
    };

    struct server_info
    {
        std::string name;
        std::string ip;
        uint16_t port;
        std::string password;
        server_unique_id unique_id;
        uint32_t connect_interval;
        log_level_t log_level = log_level_t::debug;
        uint32_t disconnect_heartbeat = 0;
        server_thread_mode thread_mode = server_thread_mode::no_io_thread;
        bool batch_send_msg = true;

        std::vector<server_unique_id> depends;
        std::vector<server_log_info> logs;
        boost::optional<std::vector<std::string>> services;

        int32_t flush_log_time = 10;
        META(name, ip, port, password, unique_id, connect_interval, log_level, disconnect_heartbeat, thread_mode, batch_send_msg,
            depends, logs, services, flush_log_time);

        std::string host_ip;
    };
}