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
}