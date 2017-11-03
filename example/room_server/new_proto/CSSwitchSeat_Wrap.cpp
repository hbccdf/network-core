#include "proto/message_wraps.hpp"
#include "service/common_check.hpp"
#include "service/player_service.h"
#include "service/room_service.h"

namespace CytxGame
{
    REGISTER_PROTOCOL(CSSwitchSeat_Wrap);

    void CSSwitchSeat_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        CHECK_ROOM_SERVICE(room_svc);

        auto& header = msgp->header();

        CSSwitchSeat& data = csSwitchSeat;
        SCSwitchSeat_Wrap switch_seat_wrap;
        auto& ret = switch_seat_wrap.scSwitchSeat.result;

        auto player = player_svc->find_player(header.user_id);
        auto player1 = player_svc->find_player(data.playerId);

        if (!check_player(ret, player) || !check_player(ret, player1) ||
            !check_matched(ret, player) || !check_matched(ret, player1))
        {
            LOG_DEBUG("player {} switch seat failed, {}, player1:{}, pos:{}", header.user_id, error_code_str(ret), data.playerId, data.targetPos);
            server.send_client_msg(header.user_id, switch_seat_wrap);
            return;
        }

        if (player->room() != player1->room() || data.targetPos > player->room()->max_size_ || data.targetPos <= 0)
        {
            ret = 11;
            LOG_DEBUG("player {} switch seat failed, not in same room, player1:{}, pos:{}", header.user_id, data.playerId, data.targetPos);
            server.send_client_msg(header.user_id, switch_seat_wrap);
            return;
        }

        auto room = player->room();
        room->switch_seat(player1, data.targetPos);
        LOG_DEBUG("player {} switch seat success, room:{}, player1:{}, target:{}", header.user_id, room->id(), data.playerId, data.targetPos);

        server.send_client_msg(header.user_id, switch_seat_wrap);

        SCRoomInfo_Wrap room_info;
        room_info.scRoomInfo = room->get_room_info();
        room_svc->broadcast_msg(room, room_info);
    }
}
