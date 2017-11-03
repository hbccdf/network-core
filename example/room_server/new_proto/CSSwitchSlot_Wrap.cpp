#include "proto/message_wraps.hpp"
#include "service/common_check.hpp"
#include "service/player_service.h"
#include "service/room_service.h"

namespace CytxGame
{
    REGISTER_PROTOCOL(CSSwitchSlot_Wrap);

    void CSSwitchSlot_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        CHECK_ROOM_SERVICE(room_svc);

        auto& header = msgp->header();
        CSSwitchSlot& data = csSwitchSlot;

        SCSwitchShip_Wrap data_wrap;
        auto& ret = data_wrap.scSwitchShip.result;
        auto player = player_svc->find_player(header.user_id);
        if (!check_player(ret, player) || !check_matched(ret, player))
        {
            LOG_DEBUG("player {} switch slot failed, {}, player:[{}]", header.user_id, error_code_str(ret), get_player_info(player, header.user_id));
            server.send_client_msg(header.user_id, data_wrap);
            return;
        }

        //server.send_client_msg(header.user_id, data_wrap);

        player->skill_slot_count_ = data.slotCount;
        player->unique_skills_ = data.uniqueSkills;
        player->bullet_count_ = data.bulletCount;
        auto room = player->room();
        room->update_player_info();

        SCRoomInfo_Wrap room_info_wrap;
        room_info_wrap.scRoomInfo = room->get_room_info();
        room_svc->broadcast_msg(room, room_info_wrap);
    }
}
