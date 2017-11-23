#include "proto/all_wraps.hpp"
#include "service/battle_manager.h"
#include "service/service_check.hpp"

namespace CytxGame
{
    REGISTER_PROTOCOL(RBStartBattle_Wrap);

    void RBStartBattle_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_BATTLE_MANAGER(battle_svc);

        bool result = battle_svc->create_battle(scRoomInfo);

        BRStartBattle_Wrap data_wrap;
        data_wrap.brStartBattle.result = result;
        data_wrap.brStartBattle.master_id = scRoomInfo.masterId;
        server.send_server_msg(server_unique_id::room_server, data_wrap);
    }
}
