#pragma once
#include "service_base.hpp"
#include "battle/arena_battle.h"

namespace CytxGame
{
    using namespace cytx;

    class battle_manager : public service_base
    {
        using this_t = battle_manager;
    public:
        void init();
        void stop();
    public:
        void update(float delta);
        bool create_battle(SCRoomInfo& room_info);
        arena_battle* get_battle(int32_t user_id);
    private:
        static int get_battle_id();

    private:
        //std::map<int32_t, arena_battle*> battle_map_;
        std::vector<arena_battle*> battles_;
    };

    REG_SERVICE(battle_manager);
}