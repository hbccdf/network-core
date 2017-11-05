#pragma once
#include "service_base.hpp"

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
    private:
        static int get_battle_id();
    };
}