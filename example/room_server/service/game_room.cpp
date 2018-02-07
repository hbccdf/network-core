#include "game_player.h"
#include "game_room.h"
#include <network/base/log.hpp>
#include <network/base/utils.hpp>

namespace CytxGame
{
    game_room::game_room(size_t max_size, room_mode mode)
        : room_id_(get_room_id())
        , max_size_(max_size)
        , mode_(mode)
    {
        for (size_t i = 0; i < max_size; ++i)
        {
            pos_list_.insert((int)(i + 1));
        }
    }

    bool game_room::all_ready() const
    {
        return std::all_of(players_.begin(), players_.end(), [](auto& p) {
            return p.second->is_ready_;
        });
    }

    void game_room::join(player_ptr_t& player)
    {
        if (players_.empty())
        {
            master_player_ = player;
        }
        players_.emplace(player->id(), player);

        auto room = this->shared_from_this();
        player->room_ptr_ = room;
        int pos = (int)get_pos();
        set_player_pos(player, pos);
        update_player_info();
    }

    void game_room::remove(player_ptr_t& player)
    {
        if (players_.empty())
            return;

        auto it = players_.find(player->id());
        if (it != players_.end())
        {
            players_.erase(it);
            pos_list_.insert(player->position_);
            player->update_to_login_status();
        }
    }

    void game_room::switch_seat(player_ptr_t& player, int target_pos)
    {
        if (target_pos == player->position_)
            return;

        if (pos_list_.find(target_pos) != pos_list_.end())
        {
            //对方位置没人
            int player_pos = player->position_;
            set_player_pos(player, target_pos);
            pos_list_.erase(target_pos);
            pos_list_.insert(player_pos);
        }
        else
        {
            //对方位置有人
            auto target_player = get_player(target_pos);
            std::swap(player->team_, target_player->team_);
            std::swap(player->type_, target_player->type_);
            std::swap(player->hero_id_, target_player->hero_id_);
            std::swap(player->position_, target_player->position_);
            std::swap(player->player_id_, target_player->player_id_);
        }
    }

    void game_room::destroy()
    {
        room_id_ = 0;
        status_ = room_status::open;
        master_player_ = nullptr;
        max_size_ = 0;

        for (auto& p : players_)
        {
            p.second->update_to_login_status();
        }
        players_.clear();
        pos_list_.clear();
    }

    SCRoomInfo game_room::get_room_info() const
    {
        SCRoomInfo sc_info;
        sc_info.roomId = room_id_;
        sc_info.masterId = master_player_->id();
        sc_info.roomSize = (int)capacity();
        sc_info.mathPattern = (int)mode_;

        for (auto& p : players_)
        {
            sc_info.playerList.emplace_back(p.second->get_player_data());
        }
        std::sort(sc_info.playerList.begin(), sc_info.playerList.end(), [](auto& p1, auto& p2) { return p1.playerId < p2.playerId; });
        return sc_info;
    }

    size_t game_room::get_pos()
    {
        std::map<int, std::vector<int>> pos_map;
        int index = 1;
        for (int i = 0; i < 3; ++i)
        {
            pos_map.emplace(i + 1, std::vector<int>());
        }
        if (mode_ == room_mode::moba)
        {
            int team_size = (int)max_size_ / 2;
            for (auto pos : pos_list_)
            {
                if (pos <= team_size)
                {
                    pos_map[1].push_back(pos);
                }
                else
                {
                    pos_map[2].push_back(pos);
                }
            }

            if (pos_map[1].size() < pos_map[2].size())
            {
                index = 2;
            }
        }
        else if (mode_ == room_mode::tripartite)
        {
            int team_size = (int)max_size_ / 3;
            for (auto pos : pos_list_)
            {
                if (pos <= team_size)
                {
                    pos_map[1].push_back(pos);
                }
                else if (pos > team_size && pos <= (team_size * 2))
                {
                    pos_map[2].push_back(pos);
                }
                else
                {
                    pos_map[3].push_back(pos);
                }
            }
            if (pos_map[1].size() < pos_map[2].size())
            {
                index = 2;
            }
            if (pos_map[index].size() < pos_map[3].size())
            {
                index = 3;
            }
        }
        else
        {
            auto it = pos_list_.begin();
            int pos = *it;
            pos_list_.erase(it);
            return pos;
        }

        int pos = pos_map[index].front();
        pos_list_.erase(pos);
        return pos;
    }

    void game_room::set_player_pos(player_ptr_t& player, int pos)
    {
        if (max_size_ == 2 && mode_ != room_mode::free)
        {
            player->team_ = (pos <= (max_size_ / 2)) ? 1 : 2;
            player->type_ = player_type::DRIVER;
            player->hero_id_ = 20000 + pos;
            player->position_ = (int)pos;
            player->player_id_ = 10000 + pos;
        }
        else if (mode_ == room_mode::moba)
        {
            player->team_ = (pos <= (max_size_ / 2)) ? 1 : 2;
            player->type_ = pos % 2 > 0 ? player_type::DRIVER : player_type::SHOOTER;
            player->hero_id_ = 20000 + (int)((pos + 1) / 2);
            player->position_ = (int)pos;
            player->player_id_ = 10000 + pos;
        }
        else if (mode_ == room_mode::tripartite)
        {
            int size = (int)max_size_ / 3;
            if (pos <= size)
            {
                player->team_ = 1;
            }
            else if (pos > size && pos <= (size * 2))
            {
                player->team_ = 2;
            }
            else
            {
                player->team_ = 3;
            }
            player->type_ = pos % 2 > 0 ? player_type::DRIVER : player_type::SHOOTER;
            player->hero_id_ = 20000 + (int)((pos + 1) / 2);
            player->position_ = (int)pos;
            player->player_id_ = 10000 + pos;
        }
        else if (mode_ == room_mode::free)
        {
            player->team_ = -1;
            player->type_ = pos % 2 > 0 ? player_type::DRIVER : player_type::SHOOTER;
            player->hero_id_ = 20000 + (int)((pos + 1) / 2);
            player->position_ = (int)pos;
            player->player_id_ = 10000 + pos;
        }
    }

    player_ptr_t game_room::get_player(int pos)
    {
        for (auto& p : players_)
        {
            if (p.second->position_ == pos)
                return p.second;
        }
        return nullptr;
    }

    struct hero_info_t
    {
        player_ptr_t move_player;
        player_ptr_t shoot_player;
    };

    void game_room::update_player_info()
    {
        std::map<int, hero_info_t> ships;
        for (auto& p : players_)
        {
            int ship_id = p.second->hero_id_;
            auto& ship_info = ships[ship_id];
            if (p.second->type_ == player_type::DRIVER)
                ship_info.move_player = p.second;
            else
                ship_info.shoot_player = p.second;
        }

        for (auto& p : ships)
        {
            auto& ship_info = p.second;
            if (ship_info.move_player && ship_info.shoot_player)
            {
                ship_info.shoot_player->update_info(ship_info.move_player);
            }
        }
    }
}