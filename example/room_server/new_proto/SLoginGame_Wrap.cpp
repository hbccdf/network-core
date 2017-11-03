#include "proto/message_wraps.hpp"
#include "service/player_service.h"

namespace CytxGame
{
    REGISTER_PROTOCOL(SLoginGame_Wrap);

    void SLoginGame_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        auto& header = msgp->header();
        //��ȡplayer����
        player_service* player_svc = server.get_service<player_service>();
        if (!player_svc)
        {
            LOG_ERROR("player service is not exist");
            return;
        }

        //��ȡplayer�����û�У��򴴽�һ��player
        auto player = player_svc->get_player(header.user_id);
        //���player�Ѿ��ڷ����У������������߳�
        if (player->room())
        {
            player_svc->handle_player_disconnect(header.user_id);
        }

        if (!player->is_login())
        {
            LOG_INFO("player {} login game, info:[{}]", header.user_id, player->info());
            player->update_to_login_status();

            SCServerInfo_Wrap data;
            data.scServerInfo.currentServerTime = cytx::date_time::now().total_milliseconds();
            server.send_client_msg(header.user_id, data);
        }
    }
}
