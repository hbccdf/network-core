#include "proto/all_actions.hpp"
#include "service/battle_manager.h"
#include "service/service_check.hpp"

namespace CytxGame
{
    class RBStartBattle_Action : public RBStartBattle_Msg
    {
        using this_t = RBStartBattle_Action;
        using base_t = RBStartBattle_Msg;
    public:
        RBStartBattle_Action()
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(RBStartBattle_Action);

    void RBStartBattle_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_BATTLE_MANAGER(battle_svc);

        bool result = battle_svc->create_battle(scRoomInfo);

        BRStartBattle_Msg data_wrap;
        data_wrap.brStartBattle.result = result;
        data_wrap.brStartBattle.master_id = scRoomInfo.masterId;
        server.send_server_msg(server_unique_id::room_server, data_wrap);
    }
}
