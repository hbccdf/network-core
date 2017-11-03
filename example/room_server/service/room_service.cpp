#include "room_service.h"
#include "room_config.h"
#include "proto/message_wraps.hpp"

namespace CytxGame
{
    room_ptr_t room_service::create_room(player_ptr_t& master, size_t max_size, room_mode mode)
    {
        auto room = std::make_shared<room_t>(max_size, mode);
        room->join(master);
        rooms_.emplace(room->id(), room);
        return room;
    }

    room_ptr_t room_service::find_room(int room_id)
    {
        if (room_id == -1)
        {
            //找到一个有空闲位置的房间
            for (auto& p : rooms_)
            {
                room_ptr_t& room = p.second;
                if (room->status() == room_status::open && !room->full())
                {
                    return room;
                }
            }
            return nullptr;
        }
        auto it = rooms_.find(room_id);
        if (it != rooms_.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void room_service::exit_room(player_ptr_t& player)
    {
        if (!player->room())
            return;

        auto room = player->room();

        bool end_battle_when_disconnect = true;
        auto config_ptr = server_->get_service<room_config>();
        if (config_ptr)
        {
            end_battle_when_disconnect = config_ptr->get_config().end_battle_when_disconnect;
        }
        if (end_battle_when_disconnect)
        {
            deal_destroy_room(room);
            return;
        }

        room->remove(player);
        auto status = room->status();
        if (status != room_status::battle)
        {
            deal_destroy_room(room);
        }
        else if (room->players().empty())
        {
            destroy_room(room);
        }
    }

    void room_service::destroy_room(room_ptr_t& room)
    {
        auto it = rooms_.find(room->id());
        room->destroy();
        if (it != rooms_.end())
        {
            rooms_.erase(it);
        }
    }

    void room_service::deal_destroy_room(room_ptr_t& room)
    {
        SCRoomDestroyed_Wrap data;
        data.scRoomDestroyed.result = 0;
        broadcast_msg(room, data);
        destroy_room(room);
    }

    std::vector<room_ptr_t> room_service::get_room_list(room_mode mode, bool all_mode)
    {
        std::vector<room_ptr_t> list;
        for (auto& p : rooms_)
        {
            room_ptr_t& room = p.second;
            if ((all_mode || room->mode_ == mode) && room->status() == room_status::open)
            {
                list.push_back(room);
            }
        }
        return list;
    }
}