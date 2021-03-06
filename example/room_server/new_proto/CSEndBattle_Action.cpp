#include "proto/all_actions.hpp"
#include "service/common_check.hpp"
#include "service/player_service.h"
#include "service/room_service.h"

namespace CytxGame
{
    class CSEndBattle_Action : public CSEndBattle_Msg
    {
        using this_t = CSEndBattle_Action;
        using base_t = CSEndBattle_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSEndBattle_Action);

    void CSEndBattle_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        CHECK_ROOM_SERVICE(room_svc);

        auto& header = msgp->header();
        CSEndBattle& data = csEndBattle;

        SCEndBattle_Msg data_wrap;
        auto& ret = data_wrap.scEndBattle.result;

        auto player = player_svc->find_player(header.user_id);
        if (!check_player(ret, player) || !check_matched(ret, player) || !check_room_battle(ret, player))
        {
            LOG_DEBUG("player {} end battle failed, {}, player:[{}]", header.user_id, error_code_str(ret), get_player_info(player, header.user_id));
            server.send_client_msg(header.user_id, data_wrap);
        }
        room_svc->exit_room(player);
        server.send_client_msg(header.user_id, data_wrap);
    }
}
