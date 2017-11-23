#pragma once
#include <network/gameserver/game_server.hpp>

namespace CytxGame
{
    class CSClientClose_Wrap : public Proto
    {
        using this_t = CSClientClose_Wrap;
        using base_t = Proto;
    public:
        CSClientClose_Wrap()
            : base_t(ProtoId()) {}
        CSClientClose_Wrap(const this_t& rhs)
            : base_t(rhs)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
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
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return 0x40a0007;
        }
    };

    class RBStartBattle_Wrap : public Proto
    {
        using this_t = RBStartBattle_Wrap;
        using base_t = Proto;
    public:
        RBStartBattle_Wrap()
            : base_t(ProtoId()) {}
        RBStartBattle_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scRoomInfo(rhs.scRoomInfo)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
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
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

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
    class BRStartBattle_Wrap : public Proto
    {
        using this_t = BRStartBattle_Wrap;
        using base_t = Proto;
    public:
        BRStartBattle_Wrap()
            : base_t(ProtoId()) {}
        BRStartBattle_Wrap(const this_t& rhs)
            : base_t(rhs)
            , brStartBattle(rhs.brStartBattle)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
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
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return battle_msg_id::start_battle_resp;
        }

    public:
        BRStartBattle brStartBattle;
    };
}