#include "proto/all_wraps.hpp"
#include "service/player_service.h"
#include "service/common_check.hpp"

namespace CytxGame
{
    REGISTER_PROTOCOL(CSClientClose_Wrap);

    void CSClientClose_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        auto& header = msgp->header();
        player_svc->handle_player_disconnect(header.user_id);
    }
}
