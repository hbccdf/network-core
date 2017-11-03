#include "proto/all_wraps.hpp"
#include "service/player_service.h"
#include "service/common_check.hpp"

namespace CytxGame
{
    REGISTER_PROTOCOL(SLoginGame_Wrap);

    void SLoginGame_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        auto& header = msgp->header();

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
