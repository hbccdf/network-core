#include "proto/message_wraps.hpp"
#include "service/common_check.hpp"
#include "service/player_service.h"
#include "service/room_service.h"

namespace CytxGame
{
    REGISTER_PROTOCOL(CSEndLoadResource_Wrap);

    void CSEndLoadResource_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        CHECK_ROOM_SERVICE(room_svc);

        auto& header = msgp->header();
        CSEndLoadResource& data = csEndLoadResource;

        int ret = 0;
        auto player = player_svc->find_player(header.user_id);
        if (!check_player(ret, player) || !check_matched(ret, player) || !check_room_ready(ret, player))
        {
            LOG_DEBUG("player {} end_load_resource failed, {}, player:[{}]", header.user_id, error_code_str(ret), get_player_info(player, header.user_id));
            return;
        }

        //判断是否所有人都准备好
        player->is_ready_ = true;
        auto room = player->room();
        if (room->all_ready())
        {
            SCRoomInfo room_info = room->get_room_info();
            server.send_server_msg(server_unique_id::battle_server, battle_msg_id::start_battle, room_info);
        }
    }
}
