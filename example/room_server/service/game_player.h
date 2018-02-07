#pragma once
#include "game_common.h"

namespace CytxGame
{
    class game_player
    {
    public:
        int id() const { return user_id_; }
        room_ptr_t room() const { return room_ptr_; }
        bool is_login() const { return is_login_; }
        std::string info() const;

        void update_info(player_ptr_t& other);
        PlayerData get_player_data() const;
    public:
        void update_to_login_status();
        void update_to_none_status();
    public:
        int user_id_ = -1;
        room_ptr_t room_ptr_ = nullptr;
        bool is_login_ = false;
        player_type type_ = player_type::DRIVER;
        int32_t team_ = -1;
        int32_t hero_id_ = 0;
        int32_t position_ = 0;
        int32_t player_id_ = 0;
        int32_t ship_config_id_ = 1;
        int32_t skill_slot_count_ = 0;
        int32_t bullet_count_ = 0;
        std::vector<int32_t> unique_skills_;
        bool is_ready_ = false;
    };
}