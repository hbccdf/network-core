#include "proto/all_wraps.hpp"
#include "service/common_check.hpp"
#include "service/player_service.h"
#include "service/room_service.h"

namespace CytxGame
{
    REGISTER_PROTOCOL(CSStartBattle_Wrap);

    void CSStartBattle_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        CHECK_ROOM_SERVICE(room_svc);

        auto& header = msgp->header();
        CSStartBattle& data = csStartBattle;

        SCBeginLoadResource_Wrap data_wrap;

        int32_t ret = 0;
        auto player = player_svc->find_player(header.user_id);
        if (!check_player(ret, player) || !check_matched(ret, player)
            || !check_master(ret, player) || !check_room_ready(ret, player))
        {
            LOG_DEBUG("player {} start battle failed, {}, player:[{}]", header.user_id, error_code_str(ret), get_player_info(player, header.user_id));
            return;
        }

        auto room = player->room();
        room->status_ = room_status::prepare;
        room_svc->broadcast_msg(room, data_wrap);
    }
}
