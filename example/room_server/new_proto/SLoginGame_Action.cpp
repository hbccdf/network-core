#include "proto/all_actions.hpp"
#include "service/player_service.h"
#include "service/common_check.hpp"

namespace CytxGame
{
    class SLoginGame_Action : public SLoginGame_Msg
    {
        using this_t = SLoginGame_Action;
        using base_t = SLoginGame_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SLoginGame_Action);

    void SLoginGame_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        auto& header = msgp->header();

        //获取player，如果没有，则创建一个player
        auto player = player_svc->get_player(header.user_id);
        //如果player已经在房间中，则把其他玩家踢出
        if (player->room())
        {
            player_svc->handle_player_disconnect(header.user_id);
        }

        if (!player->is_login())
        {
            LOG_INFO("player {} login game, info:[{}]", header.user_id, player->info());
            player->update_to_login_status();

            SCServerInfo_Msg data;
            data.scServerInfo.currentServerTime = cytx::date_time::now().total_milliseconds();
            server.send_client_msg(header.user_id, data);
        }
    }
}
