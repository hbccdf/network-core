#include "proto/message_wraps.hpp"
#include "service/common_check.hpp"
#include "service/player_service.h"
#include "service/room_service.h"

namespace CytxGame
{
    REGISTER_PROTOCOL(CSJoinRoom_Wrap);

    void CSJoinRoom_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        CHECK_ROOM_SERVICE(room_svc);

        auto& header = msgp->header();
        int32_t user_id = header.user_id;

        CSJoinRoom& data = csJoinRoom;
        SCJoinRoom_Wrap join_room_wrap;
        auto& ret = join_room_wrap.scJoinRoom.result;

        auto player = player_svc->find_player(user_id);
        auto room = room_svc->find_room(data.roomId);
        if (!check_player(ret, player) || !check_not_matched(ret, player) ||
            !check_room(ret, room))
        {
            LOG_DEBUG("player {} join room failed, {}, room id:{}, player:[{}]", header.user_id, error_code_str(ret), data.roomId, get_player_info(player, header.user_id));
            server.send_client_msg(user_id, join_room_wrap);
            return;
        }

        room->join(player);
        LOG_DEBUG("player {} join room success, room:{}, size:{}, player:[{}]", header.user_id, room->id(), room->players().size(), player->info());

        server.send_client_msg(user_id, join_room_wrap);

        SCRoomInfo_Wrap room_info;
        room_info.scRoomInfo = room->get_room_info();
        room_svc->broadcast_msg(room, room_info);
    }
}
