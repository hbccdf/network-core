#include "common_check.hpp"

namespace CytxGame
{
    std::string to_string(const std::set<int>& t)
    {
        fmt::MemoryWriter mr;
        mr.write("[");
        bool is_first = true;
        for (auto i : t)
        {
            if (!is_first)
                mr.write(",{}", i);
            else
                mr.write("{}", i);
            is_first = false;
        }
        mr.write("]");
        return mr.str();
    }

    bool check_room_ready(int& ret, const player_ptr_t& player)
    {
        //teama与teamb人数相等则，认为ready
        size_t teama_size = 0;
        size_t teamb_size = 0;
        size_t teamc_size = 0;
        auto players = player->room()->players();
        if (player->room()->mode_ == room_mode::free && !players.empty())
        {
            ret = 0;
            return is_success(ret);
        }

        for (auto& p : players)
        {
            if (p.second->team_ == 1)
                ++teama_size;
            else if (p.second->team_ == 2)
                ++teamb_size;
            else if (p.second->team_ == 3)
                ++teamc_size;
        }

        bool moba_room = player->room()->mode_ == room_mode::moba;

        ret = (teama_size == teamb_size && teama_size > 0 && (moba_room || teamb_size == teamc_size)) || player->room()->full() ? 0 : 6;
        return is_success(ret);
    }

    bool check_room_size(int& ret, int size, int mode)
    {
        if (mode == (int)room_mode::moba)
        {
            ret = size % 2 == 0 || size == 1 ? 0 : 9;
        }
        else if (mode == (int)room_mode::tripartite)
        {
            ret = size % 3 == 0 ? 0 : 9;
        }
        else
        {
            ret = 0;
        }
        return is_success(ret);
    }

    std::string get_player_info(player_ptr_t& player, int user_id)
    {
        if (player)
            return player->info();
        else
            return std::to_string(user_id);
    }

}
