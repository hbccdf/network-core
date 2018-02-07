#pragma once
#include "game_player.h"
#include "game_room.h"
#include <network/base/cast.hpp>

namespace CytxGame
{
#define CHECK_PLAYER_SERVICE(name)  \
    player_service* name = server.get_service<player_service>();    \
    if (!name)                                                      \
    {                                                               \
        LOG_ERROR("player service is not exist");                   \
        return;                                                     \
    }

#define CHECK_ROOM_SERVICE(name)  \
    room_service* name = server.get_service<room_service>();        \
    if (!name)                                                      \
    {                                                               \
        LOG_ERROR("room service is not exist");                     \
        return;                                                     \
    }

    inline std::string get_msg_str(uint32_t proto_id)
    {
        return cytx::util::to_str(proto_id, "proto");
    }

    std::string to_string(const std::set<int>& t);

    inline std::string error_code_str(int err)
    {
        return std::to_string(err);
    }

    inline bool is_success(int ret)
    {
        return ret == 0;
    }
    inline bool check_player(int& ret, const player_ptr_t& player)
    {
        ret = player == nullptr ? 1 : 0;
        return is_success(ret);
    }
    inline bool check_not_matched(int& ret, const player_ptr_t& player)
    {
        ret = player->room() ? 2 : 0;
        return is_success(ret);
    }
    inline bool check_room(int& ret, const room_ptr_t& room)
    {
        ret = !room ? 3 : 0;
        if (is_success(ret))
        {
            ret = room->full() ? 10 : 0;
        }
        if (is_success(ret))
        {
            ret = room->status() == room_status::open ? 0 : 11;
        }
        return is_success(ret);
    }
    inline bool check_matched(int& ret, const player_ptr_t& player)
    {
        ret = !player->room() ? 4 : 0;
        return is_success(ret);
    }
    inline bool check_master(int& ret, const player_ptr_t& player)
    {
        ret = player->room()->master_player_ != player ? 5 : 0;
        return is_success(ret);
    }
    bool check_room_ready(int& ret, const player_ptr_t& player);
    inline bool check_room_battle(int& ret, const player_ptr_t& player)
    {
        ret = player->room()->status() == room_status::battle ? 0 : 7;
        return is_success(ret);
    }
    inline bool check_room_mode(int& ret, int mode)
    {
        ret = mode >= (int)room_mode::moba && mode <= (int)room_mode::free ? 0 : 8;
        return is_success(ret);
    }
    bool check_room_size(int& ret, int size, int mode);
    inline bool check_search_room_mode(int& ret, int mode)
    {
        if (mode == -1)
        {
            ret = 0;
            return true;
        }
        return check_room_mode(ret, mode);
    }

    std::string get_player_info(player_ptr_t& player, int user_id);
}