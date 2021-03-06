#include "proto/all_actions.hpp"
#include "service/common_check.hpp"
#include "service/player_service.h"
#include "service/room_service.h"

namespace CytxGame
{
    class CSExitRoom_Action : public CSExitRoom_Msg
    {
        using this_t = CSExitRoom_Action;
        using base_t = CSExitRoom_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSExitRoom_Action);

    void CSExitRoom_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        CHECK_ROOM_SERVICE(room_svc);

        auto& header = msgp->header();

        CSExitRoom& data = csExitRoom;
        SCExitRoom_Msg exit_room_wrap;
        auto& ret = exit_room_wrap.scExitRoom.result;

        auto player = player_svc->find_player(header.user_id);
        if (!check_player(ret, player) || !check_matched(ret, player))
        {
            LOG_DEBUG("player {} exit room failed, {}, player:[{}]", header.user_id, error_code_str(ret), get_player_info(player, header.user_id));

            server.send_client_msg(header.user_id, exit_room_wrap);
            return;
        }

        server.send_client_msg(header.user_id, exit_room_wrap);
        room_svc->exit_room(player);
    }
}
