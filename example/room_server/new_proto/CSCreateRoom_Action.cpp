#include "proto/all_actions.hpp"
#include "service/common_check.hpp"
#include "service/player_service.h"
#include "service/room_service.h"

namespace CytxGame
{
    class CSCreateRoom_Action : public CSCreateRoom_Msg
    {
        using this_t = CSCreateRoom_Action;
        using base_t = CSCreateRoom_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSCreateRoom_Action);

    void CSCreateRoom_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        auto& header = msgp->header();
        int32_t user_id = header.user_id;

        CHECK_PLAYER_SERVICE(player_svc);
        CHECK_ROOM_SERVICE(room_svc);

        CSCreateRoom& data = csCreateRoom;

        SCCreateRoom_Msg send_data;
        send_data.scCreateRoom.result = 0;
        auto& ret = send_data.scCreateRoom.result;

        auto player = player_svc->find_player(header.user_id);
        if (!check_player(ret, player) || !check_not_matched(ret, player)
            || !check_room_mode(ret, data.matchPattern) || !check_room_size(ret, data.playerCount, data.matchPattern))
        {
            LOG_DEBUG("player {} create room failed, {}, mode {}, player:[{}]", header.user_id, error_code_str(ret), data.matchPattern, get_player_info(player, header.user_id));
            server.send_client_msg(user_id, send_data);
            return;
        }

        auto mode = (room_mode)data.matchPattern;
        auto room = room_svc->create_room(player, data.playerCount, mode);
        LOG_DEBUG("player {} create room success, room size:{}, mode:{}, player:[{}]", header.user_id, data.playerCount, data.matchPattern, player->info());

        server.send_client_msg(user_id, send_data);

        SCRoomInfo_Msg send_wrap;
        send_wrap.scRoomInfo = room->get_room_info();
        room_svc->broadcast_msg(room, send_wrap);
    }
}
