#include "proto/all_actions.hpp"
#include "service/player_service.h"
#include "service/room_service.h"
#include "service/common_check.hpp"

namespace CytxGame
{
    class BRStartBattle_Action : public BRStartBattle_Msg
    {
        using this_t = BRStartBattle_Action;
        using base_t = BRStartBattle_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(BRStartBattle_Action);

    void BRStartBattle_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        CHECK_PLAYER_SERVICE(player_svc);
        CHECK_ROOM_SERVICE(room_svc);

        auto& header = msgp->header();

        int32_t user_id = brStartBattle.master_id;
        auto player = player_svc->find_player(user_id);
        int ret = 0;
        if (!check_player(ret, player) || !check_matched(ret, player))
        {
            LOG_DEBUG("player {} start battle resp failed, {}, result:{}", user_id, error_code_str(ret));
            return;
        }

        auto room = player->room();
        SCStartBattle_Msg data_wrap;
        data_wrap.scStartBattle.result = brStartBattle.result ? 0 : 1;
        data_wrap.scStartBattle.startTime = cytx::date_time::now().total_milliseconds();

        //开始战斗了
        if (brStartBattle.result)
        {
            room->status_ = room_status::battle;
            LOG_DEBUG("start battle success");
        }
        //记录日志，开始战斗失败
        else
        {
            room->status_ = room_status::open;
            LOG_DEBUG("start battle failed");
        }
        room_svc->broadcast_msg(room, data_wrap);
    }
}
