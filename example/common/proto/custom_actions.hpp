#pragma once
#include <network/gameserver/game_server.hpp>

namespace CytxGame
{
    class CSClientClose_Msg : public Proto
    {
        using this_t = CSClientClose_Msg;
        using base_t = Proto;
    public:
        CSClientClose_Msg()
            : base_t(ProtoId())
        {
        }
        msg_ptr pack() const
        {
            return pack_msg();
        }
        void pack(stream_t& gos) const
        {
        }
        void unpack(msg_ptr& msgp) override
        {
        }

    public:
        static uint32_t ProtoId()
        {
            return 0x40a0007;
        }
    };

    class RBStartBattle_Msg : public Proto
    {
        using this_t = RBStartBattle_Msg;
        using base_t = Proto;
    public:
        RBStartBattle_Msg()
            : base_t(ProtoId())
        {
        }
        msg_ptr pack() const
        {
            return pack_msg(scRoomInfo);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scRoomInfo);
        }
        void unpack(msg_ptr& msgp) override
        {
            scRoomInfo = unpack_msg<SCRoomInfo>(msgp);
        }

    public:
        static uint32_t ProtoId()
        {
            return battle_msg_id::start_battle;
        }

    public:
        SCRoomInfo scRoomInfo;
    };

    struct BRStartBattle
    {
        bool result = true;
        int32_t master_id = 0;
        META(result, master_id);
    };
    class BRStartBattle_Msg : public Proto
    {
        using this_t = BRStartBattle_Msg;
        using base_t = Proto;
    public:
        BRStartBattle_Msg()
            : base_t(ProtoId())
        {
        }
        msg_ptr pack() const
        {
            return pack_msg(brStartBattle);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, brStartBattle);
        }
        void unpack(msg_ptr& msgp) override
        {
            brStartBattle = unpack_msg<BRStartBattle>(msgp);
        }

    public:
        static uint32_t ProtoId()
        {
            return battle_msg_id::start_battle_resp;
        }

    public:
        BRStartBattle brStartBattle;
    };
}