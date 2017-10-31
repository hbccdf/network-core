#include <network/gameserver/game_server.hpp>
#include "proto/message_types.h"

namespace CytxGame
{
    using namespace cytx;
    using namespace cytx::gameserver;

    using game_server_t = cytx::gameserver::game_server;
    class room_server_t : public game_server_t
    {
        using base_t = game_server_t;
    public:
        room_server_t()
            : game_server_t(server_unique_id::room_server)
        {}
    protected:
        void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
        {
            const uint32_t login_proto = 0x7050005;

            int32_t user_id = msgp->header().user_id;
            uint32_t protocol_id = msgp->header().protocol_id;
            if (protocol_id == login_proto)
            {
                LOG_DEBUG("login {}", user_id);
                SCServerInfo data;
                data.currentServerTime = cytx::date_time::now().total_milliseconds();
                send_client_msg(user_id, ::MessageId::SC_ServerInfo, data);
            }
            else if (protocol_id == ::MessageId::CS_CreateRoom) 
            {
                LOG_DEBUG("create room {}", user_id);

                CSCreateRoom data = unpack_msg<CSCreateRoom>(msgp);

                SCCreateRoom send_data;
                send_data.result = 0;
                send_client_msg(user_id, ::MessageId::SC_CreateRoom, send_data);

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
                send_client_msg(user_id, ::MessageId::SC_RoomInfo, room_info);
            }
            else
            {
                base_t::on_receive(conn_ptr, msgp);
            }
        }
    };
}






int main(int argc, char* argv[])
{
    CytxGame::room_server_t server;
    server.init();
    server.start();
}