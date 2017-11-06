#pragma once
#include <network/meta/meta.hpp>
#include "message_types.h"

namespace CytxGame
{
    enum battle_msg_id
    {
        start_battle = 0x110d0001, //request, need response, ��溽, BattleServer, Э��� 0x1
        stop_battle = 0x100d0002, //request, not need response, ��溽, BattleServer, Э��� 0x2

        start_battle_resp = 0x120d0001, //response, not need response, ��溽, BattleServer, Э��� 0x1
    };
    REG_ALIAS_ENUM(battle_msg_id, proto, start_battle, stop_battle, start_battle_resp);

    enum class room_mode : uint8_t
    {
        moba,           //mobaģʽ
        tripartite,     //����ģʽ
        free,           //����ģʽ
    };
    REG_ENUM(room_mode, moba, tripartite, free);

    using player_type = ::ClassType::type;
    REG_ENUM(player_type, DRIVER, SHOOTER);

}