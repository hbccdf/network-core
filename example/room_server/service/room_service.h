#pragma once
#include "service_base.hpp"
#include "game_room.h"

namespace CytxGame
{
    class room_service : public service_base
    {
    public:
        void init();

        room_ptr_t create_room(player_ptr_t& master, size_t max_size, room_mode mode);
        room_ptr_t find_room(int room_id);
        void exit_room(player_ptr_t& player);
        void destroy_room(room_ptr_t& room);
        void deal_destroy_room(room_ptr_t& room);
        std::vector<room_ptr_t> get_room_list(room_mode mode, bool all_mode);

    public:
        template<typename T>
        void broadcast_msg(room_ptr_t& room, T& msg)
        {
            for (const auto& p : room->players())
            {
                server_->send_client_msg(p.first, msg);
            }
        }
    private:
        room_map_t rooms_;
    };

    REG_SERVICE(room_service);
}