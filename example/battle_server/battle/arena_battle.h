#pragma once
#include "proto/common_msg.h"

namespace CytxGame
{
    struct battle_init_data
    {
        int battle_id;
        SCRoomInfo room_data;
    };

    struct game_info_data
    {
        bool is_running = false;
        room_mode mode = room_mode::moba;
        int frame = 0;
        int battle_id = 0;
        float battle_time = 0.0f;
        float rhythm_scale = 1.0f; //½Ú×àÏµÊý
    };

    class arena_battle
    {
        using this_t = arena_battle;
    public:
        arena_battle();
        virtual ~arena_battle();

        void init(battle_init_data& data);
        void start();
        void update(float delta_time);
        void stop();

    public:
        bool need_destroy() const { return need_destroy_; }
    private:
        bool need_destroy_ = false;
        game_info_data info_;
        SCRoomInfo room_info_;
    };
}