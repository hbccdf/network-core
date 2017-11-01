#include "proto/message_wraps.hpp"

namespace CytxGame
{
    REGISTER_PROTOCOL(CSCreateRoom_Wrap);

    void CSCreateRoom_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        int32_t user_id = msgp->header().user_id;
        LOG_DEBUG("create room {}", user_id);

        CSCreateRoom& data = csCreateRoom;

        SCCreateRoom_Wrap send_data;
        send_data.scCreateRoom.result = 0;
        server.send_client_msg(user_id, send_data);

        SCRoomInfo room_info;
        room_info.masterId = user_id;
        room_info.mathPattern = data.matchPattern;
        room_info.roomId = 1;
        room_info.roomSize = data.playerCount;

        PlayerData player;
        player.playerId = 10001;
        player.shipId = 20001;
        player.classType = ClassType::DRIVER;
        player.bulletCount = 0;
        player.faction = 1;
        player.position = 1;
        player.shipLevel = 1;
        player.skillSlotCount = 1;
        player.userId = user_id;

        room_info.playerList.push_back(player);
        SCRoomInfo_Wrap room_data;
        room_data.scRoomInfo = room_info;

        server.send_client_msg(user_id, room_data);
    }
}
