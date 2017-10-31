#pragma once
#include <network/gameserver/game_server.hpp>
#include "proto/message_types.h"

namespace CytxGame
{
    using namespace cytx::gameserver;
    using namespace cytx;

    class SLoginGame_Wrap : public Proto
    {
        using this_t = SLoginGame_Wrap;
        using base_t = Proto;
    public:
        SLoginGame_Wrap()
            : base_t()
        {}
        SLoginGame_Wrap(const this_t& rhs)
            : base_t(rhs)
            , sLoginGame_(rhs.sLoginGame_)
        {
        }
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() override
        {
            return pack_msg(sLoginGame_);
        }
        void unpack(msg_ptr& msgp) override
        {
            sLoginGame_ = unpack_msg<SLoginGame>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr) override;

    public:
        static uint32_t ProtoId()
        {
            return 0x7050005;
        }

    public:
        SLoginGame sLoginGame_;
    };

    class SCServerInfo_Wrap : public Proto
    {
        using this_t = SCServerInfo_Wrap;
        using base_t = Proto;
    public:
        SCServerInfo_Wrap()
            : base_t()
        {}
        SCServerInfo_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scServerInfo_(rhs.scServerInfo_)
        {
        }
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() override
        {
            return pack_msg(scServerInfo_);
        }
        void unpack(msg_ptr& msgp) override
        {
            scServerInfo_ = unpack_msg<SCServerInfo>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr) override
        {}
    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_ServerInfo;
        }
    public:
        SCServerInfo scServerInfo_;
    };
}