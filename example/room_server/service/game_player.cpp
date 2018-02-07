#include "game_player.h"
#include "game_room.h"
#include <network/base/log.hpp>
#include <network/base/utils.hpp>
#include <network/base/cast.hpp>

namespace CytxGame
{
    using namespace cytx::util;
    std::string to_string(const std::vector<int32_t>& values)
    {
        fmt::MemoryWriter mr;
        mr.write("[");
        bool is_first = true;
        for (int v : values)
        {
            if (!is_first)
                mr.write(",{}", v);
            else
                mr.write("{}", v);
            is_first = false;
        }
        mr.write("]");
        return mr.str();
    }

    std::string game_player::info() const
    {
        return fmt::format("id:{}, room_id:{}, type:{}, team:{}, hero_id:{}, pos:{}, ship_level:{}, slot_count:{}, unique_skills:{}",
            id(), room_ptr_ ? room_ptr_->id() : 0, to_str(type_), team_, hero_id_, position_, ship_config_id_, skill_slot_count_, to_string(unique_skills_));
    }

    void game_player::update_info(player_ptr_t& other)
    {
        ship_config_id_ = other->ship_config_id_;
        skill_slot_count_ = other->skill_slot_count_;
        bullet_count_ = other->bullet_count_;
        unique_skills_ = other->unique_skills_;
    }

    PlayerData game_player::get_player_data() const
    {
        PlayerData data;
        data.userId = user_id_;
        data.faction = (int)team_;
        data.classType = type_;
        data.shipId = hero_id_;
        data.position = position_;
        data.playerId = player_id_;
        data.shipLevel = ship_config_id_;
        data.skillSlotCount = skill_slot_count_;
        data.bulletCount = bullet_count_;
        data.uniqueSkills = unique_skills_;
        return data;
    }

    void game_player::update_to_login_status()
    {
        update_to_none_status();
        is_login_ = true;
    }

    void game_player::update_to_none_status()
    {
        room_ptr_ = nullptr;
        is_login_ = false;
        type_ = player_type::DRIVER;
        team_ = -1;
        hero_id_ = 0;
        position_ = 0;
        is_ready_ = false;
        player_id_ = 0;
        ship_config_id_ = 1;
        skill_slot_count_ = 0;
        bullet_count_ = 0;
        unique_skills_.clear();
    }
}