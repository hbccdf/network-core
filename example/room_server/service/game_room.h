#pragma once
#include "game_common.h"

namespace CytxGame
{
    enum class room_status : uint8_t
    {
        open,
        ready,
        prepare,    //×¼±¸Õ½¶·
        battle,
        close,
    };
    REG_ENUM(room_status, open, ready, prepare, battle, close);

    class game_room : public std::enable_shared_from_this<game_room>
    {
    public:
        game_room(size_t max_size, room_mode mode);
        int id() const { return room_id_; }
        room_status status() const { return status_; }
        const player_map_t& players() const { return players_; }
        size_t capacity() const { return max_size_; }
        bool full() const { return players_.size() == max_size_; }
        bool empty() const { return players_.empty(); }
        bool all_ready() const;

        void join(player_ptr_t& player);
        void remove(player_ptr_t& player);
        void switch_seat(player_ptr_t& player, int target_pos);
        void update_player_info();

        void destroy();

        SCRoomInfo get_room_info() const;
    private:
        size_t get_pos();
        void set_player_pos(player_ptr_t& player, int pos);
        player_ptr_t get_player(int pos);

        static int get_room_id()
        {
            static int current_room_id = 0;
            if (current_room_id >= 65535)
                current_room_id = 0;
            return ++current_room_id;
        }
    public:
        int room_id_ = 0;
        room_status status_ = room_status::open;
        room_mode mode_ = room_mode::moba;
        player_ptr_t master_player_ = nullptr;
        size_t max_size_ = 0;
        player_map_t players_;
        std::set<int> pos_list_;
    };
}