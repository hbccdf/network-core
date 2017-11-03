#include "player_service.h"

namespace CytxGame
{
    player_ptr_t player_service::find_player(int user_id, bool is_login /*= true*/)
    {
        auto it = players_.find(user_id);
        if (it != players_.end())
        {
            if (is_login && !it->second->is_login())
                return nullptr;
            else
                return it->second;
        }
        return nullptr;
    }

    player_ptr_t player_service::get_player(int user_id)
    {
        auto player = find_player(user_id, false);
        if (!player)
        {
            player = std::make_shared<player_t>();
            player->user_id_ = user_id;
            players_.emplace(user_id, player);
        }
        return player;
    }

    void player_service::handle_player_disconnect(int user_id)
    {
        auto player = find_player(user_id);
        if (!player)
        {
            LOG_DEBUG("player {} disconnect", user_id);
            return;
        }

        //exit_room(player);

        player->update_to_none_status();
        LOG_DEBUG("player {} disconnect", user_id);
    }
}