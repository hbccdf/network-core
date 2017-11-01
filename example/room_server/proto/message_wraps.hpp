#pragma once
#include <network/gameserver/game_server.hpp>
#include "message_types.h"

 namespace CytxGame
 {
    using namespace cytx::gameserver;
    using namespace cytx;

    using game_server_t = cytx::gameserver::game_server;

    class CLoginGame_Wrap : public Proto
    {
        using this_t = CLoginGame_Wrap;
        using base_t = Proto;
    public:
        CLoginGame_Wrap()
            : base_t(ProtoId()) {}
        CLoginGame_Wrap(const this_t& rhs)
            : base_t(rhs)
            , cLoginGame(rhs.cLoginGame)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(cLoginGame);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, cLoginGame);
        }
        void unpack(msg_ptr& msgp) override
        {
            cLoginGame = unpack_msg<CLoginGame>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::C_LoginGame;
        }

    public:
        CLoginGame cLoginGame;
    };

    class SLoginGame_Wrap : public Proto
    {
        using this_t = SLoginGame_Wrap;
        using base_t = Proto;
    public:
        SLoginGame_Wrap()
            : base_t(ProtoId()) {}
        SLoginGame_Wrap(const this_t& rhs)
            : base_t(rhs)
            , sLoginGame(rhs.sLoginGame)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(sLoginGame);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, sLoginGame);
        }
        void unpack(msg_ptr& msgp) override
        {
            sLoginGame = unpack_msg<SLoginGame>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::S_LoginGame;
        }

    public:
        SLoginGame sLoginGame;
    };

    /*class SCConnect_Wrap : public Proto
    {
        using this_t = SCConnect_Wrap;
        using base_t = Proto;
    public:
        SCConnect_Wrap()
            : base_t(ProtoId()) {}
        SCConnect_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scConnect(rhs.scConnect)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scConnect);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scConnect);
        }
        void unpack(msg_ptr& msgp) override
        {
            scConnect = unpack_msg<SCConnect>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_Connect;
        }

    public:
        SCConnect scConnect;
    };

    class SCDisConnect_Wrap : public Proto
    {
        using this_t = SCDisConnect_Wrap;
        using base_t = Proto;
    public:
        SCDisConnect_Wrap()
            : base_t(ProtoId()) {}
        SCDisConnect_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scDisConnect(rhs.scDisConnect)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scDisConnect);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scDisConnect);
        }
        void unpack(msg_ptr& msgp) override
        {
            scDisConnect = unpack_msg<SCDisConnect>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_DisConnect;
        }

    public:
        SCDisConnect scDisConnect;
    };*/

    class CSCreateRoom_Wrap : public Proto
    {
        using this_t = CSCreateRoom_Wrap;
        using base_t = Proto;
    public:
        CSCreateRoom_Wrap()
            : base_t(ProtoId()) {}
        CSCreateRoom_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csCreateRoom(rhs.csCreateRoom)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csCreateRoom);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csCreateRoom);
        }
        void unpack(msg_ptr& msgp) override
        {
            csCreateRoom = unpack_msg<CSCreateRoom>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_CreateRoom;
        }

    public:
        CSCreateRoom csCreateRoom;
    };

    class SCCreateRoom_Wrap : public Proto
    {
        using this_t = SCCreateRoom_Wrap;
        using base_t = Proto;
    public:
        SCCreateRoom_Wrap()
            : base_t(ProtoId()) {}
        SCCreateRoom_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scCreateRoom(rhs.scCreateRoom)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scCreateRoom);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scCreateRoom);
        }
        void unpack(msg_ptr& msgp) override
        {
            scCreateRoom = unpack_msg<SCCreateRoom>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_CreateRoom;
        }

    public:
        SCCreateRoom scCreateRoom;
    };

    class CSJoinRoom_Wrap : public Proto
    {
        using this_t = CSJoinRoom_Wrap;
        using base_t = Proto;
    public:
        CSJoinRoom_Wrap()
            : base_t(ProtoId()) {}
        CSJoinRoom_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csJoinRoom(rhs.csJoinRoom)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csJoinRoom);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csJoinRoom);
        }
        void unpack(msg_ptr& msgp) override
        {
            csJoinRoom = unpack_msg<CSJoinRoom>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_JoinRoom;
        }

    public:
        CSJoinRoom csJoinRoom;
    };

    class SCJoinRoom_Wrap : public Proto
    {
        using this_t = SCJoinRoom_Wrap;
        using base_t = Proto;
    public:
        SCJoinRoom_Wrap()
            : base_t(ProtoId()) {}
        SCJoinRoom_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scJoinRoom(rhs.scJoinRoom)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scJoinRoom);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scJoinRoom);
        }
        void unpack(msg_ptr& msgp) override
        {
            scJoinRoom = unpack_msg<SCJoinRoom>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_JoinRoom;
        }

    public:
        SCJoinRoom scJoinRoom;
    };

    class SCRoomInfo_Wrap : public Proto
    {
        using this_t = SCRoomInfo_Wrap;
        using base_t = Proto;
    public:
        SCRoomInfo_Wrap()
            : base_t(ProtoId()) {}
        SCRoomInfo_Wrap(const this_t& rhs)
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
            return ::MessageId::SC_RoomInfo;
        }

    public:
        SCRoomInfo scRoomInfo;
    };

    class CSSwitchSeat_Wrap : public Proto
    {
        using this_t = CSSwitchSeat_Wrap;
        using base_t = Proto;
    public:
        CSSwitchSeat_Wrap()
            : base_t(ProtoId()) {}
        CSSwitchSeat_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csSwitchSeat(rhs.csSwitchSeat)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csSwitchSeat);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csSwitchSeat);
        }
        void unpack(msg_ptr& msgp) override
        {
            csSwitchSeat = unpack_msg<CSSwitchSeat>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_SwitchSeat;
        }

    public:
        CSSwitchSeat csSwitchSeat;
    };

    class SCSwitchSeat_Wrap : public Proto
    {
        using this_t = SCSwitchSeat_Wrap;
        using base_t = Proto;
    public:
        SCSwitchSeat_Wrap()
            : base_t(ProtoId()) {}
        SCSwitchSeat_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scSwitchSeat(rhs.scSwitchSeat)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scSwitchSeat);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scSwitchSeat);
        }
        void unpack(msg_ptr& msgp) override
        {
            scSwitchSeat = unpack_msg<SCSwitchSeat>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_SwitchSeat;
        }

    public:
        SCSwitchSeat scSwitchSeat;
    };

    class CSExitRoom_Wrap : public Proto
    {
        using this_t = CSExitRoom_Wrap;
        using base_t = Proto;
    public:
        CSExitRoom_Wrap()
            : base_t(ProtoId()) {}
        CSExitRoom_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csExitRoom(rhs.csExitRoom)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csExitRoom);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csExitRoom);
        }
        void unpack(msg_ptr& msgp) override
        {
            csExitRoom = unpack_msg<CSExitRoom>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_ExitRoom;
        }

    public:
        CSExitRoom csExitRoom;
    };

    class SCExitRoom_Wrap : public Proto
    {
        using this_t = SCExitRoom_Wrap;
        using base_t = Proto;
    public:
        SCExitRoom_Wrap()
            : base_t(ProtoId()) {}
        SCExitRoom_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scExitRoom(rhs.scExitRoom)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scExitRoom);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scExitRoom);
        }
        void unpack(msg_ptr& msgp) override
        {
            scExitRoom = unpack_msg<SCExitRoom>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_ExitRoom;
        }

    public:
        SCExitRoom scExitRoom;
    };

    class SCRoomDestroyed_Wrap : public Proto
    {
        using this_t = SCRoomDestroyed_Wrap;
        using base_t = Proto;
    public:
        SCRoomDestroyed_Wrap()
            : base_t(ProtoId()) {}
        SCRoomDestroyed_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scRoomDestroyed(rhs.scRoomDestroyed)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scRoomDestroyed);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scRoomDestroyed);
        }
        void unpack(msg_ptr& msgp) override
        {
            scRoomDestroyed = unpack_msg<SCRoomDestroyed>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_RoomDestroyed;
        }

    public:
        SCRoomDestroyed scRoomDestroyed;
    };

    class CSStartBattle_Wrap : public Proto
    {
        using this_t = CSStartBattle_Wrap;
        using base_t = Proto;
    public:
        CSStartBattle_Wrap()
            : base_t(ProtoId()) {}
        CSStartBattle_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csStartBattle(rhs.csStartBattle)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csStartBattle);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csStartBattle);
        }
        void unpack(msg_ptr& msgp) override
        {
            csStartBattle = unpack_msg<CSStartBattle>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_StartBattle;
        }

    public:
        CSStartBattle csStartBattle;
    };

    class SCBeginLoadResource_Wrap : public Proto
    {
        using this_t = SCBeginLoadResource_Wrap;
        using base_t = Proto;
    public:
        SCBeginLoadResource_Wrap()
            : base_t(ProtoId()) {}
        SCBeginLoadResource_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scBeginLoadResource(rhs.scBeginLoadResource)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scBeginLoadResource);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scBeginLoadResource);
        }
        void unpack(msg_ptr& msgp) override
        {
            scBeginLoadResource = unpack_msg<SCBeginLoadResource>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_BeginLoadResource;
        }

    public:
        SCBeginLoadResource scBeginLoadResource;
    };

    class CSEndLoadResource_Wrap : public Proto
    {
        using this_t = CSEndLoadResource_Wrap;
        using base_t = Proto;
    public:
        CSEndLoadResource_Wrap()
            : base_t(ProtoId()) {}
        CSEndLoadResource_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csEndLoadResource(rhs.csEndLoadResource)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csEndLoadResource);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csEndLoadResource);
        }
        void unpack(msg_ptr& msgp) override
        {
            csEndLoadResource = unpack_msg<CSEndLoadResource>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_EndLoadResource;
        }

    public:
        CSEndLoadResource csEndLoadResource;
    };

    class SCStartBattle_Wrap : public Proto
    {
        using this_t = SCStartBattle_Wrap;
        using base_t = Proto;
    public:
        SCStartBattle_Wrap()
            : base_t(ProtoId()) {}
        SCStartBattle_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scStartBattle(rhs.scStartBattle)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scStartBattle);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scStartBattle);
        }
        void unpack(msg_ptr& msgp) override
        {
            scStartBattle = unpack_msg<SCStartBattle>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_StartBattle;
        }

    public:
        SCStartBattle scStartBattle;
    };

    class SCBattleResult_Wrap : public Proto
    {
        using this_t = SCBattleResult_Wrap;
        using base_t = Proto;
    public:
        SCBattleResult_Wrap()
            : base_t(ProtoId()) {}
        SCBattleResult_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scBattleResult(rhs.scBattleResult)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scBattleResult);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scBattleResult);
        }
        void unpack(msg_ptr& msgp) override
        {
            scBattleResult = unpack_msg<SCBattleResult>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_BattleResult;
        }

    public:
        SCBattleResult scBattleResult;
    };

    class CSEndBattle_Wrap : public Proto
    {
        using this_t = CSEndBattle_Wrap;
        using base_t = Proto;
    public:
        CSEndBattle_Wrap()
            : base_t(ProtoId()) {}
        CSEndBattle_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csEndBattle(rhs.csEndBattle)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csEndBattle);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csEndBattle);
        }
        void unpack(msg_ptr& msgp) override
        {
            csEndBattle = unpack_msg<CSEndBattle>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_EndBattle;
        }

    public:
        CSEndBattle csEndBattle;
    };

    class SCEndBattle_Wrap : public Proto
    {
        using this_t = SCEndBattle_Wrap;
        using base_t = Proto;
    public:
        SCEndBattle_Wrap()
            : base_t(ProtoId()) {}
        SCEndBattle_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scEndBattle(rhs.scEndBattle)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scEndBattle);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scEndBattle);
        }
        void unpack(msg_ptr& msgp) override
        {
            scEndBattle = unpack_msg<SCEndBattle>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_EndBattle;
        }

    public:
        SCEndBattle scEndBattle;
    };

    class SCServerInfo_Wrap : public Proto
    {
        using this_t = SCServerInfo_Wrap;
        using base_t = Proto;
    public:
        SCServerInfo_Wrap()
            : base_t(ProtoId()) {}
        SCServerInfo_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scServerInfo(rhs.scServerInfo)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scServerInfo);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scServerInfo);
        }
        void unpack(msg_ptr& msgp) override
        {
            scServerInfo = unpack_msg<SCServerInfo>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_ServerInfo;
        }

    public:
        SCServerInfo scServerInfo;
    };

    class CSRequestRoomList_Wrap : public Proto
    {
        using this_t = CSRequestRoomList_Wrap;
        using base_t = Proto;
    public:
        CSRequestRoomList_Wrap()
            : base_t(ProtoId()) {}
        CSRequestRoomList_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csRequestRoomList(rhs.csRequestRoomList)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csRequestRoomList);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csRequestRoomList);
        }
        void unpack(msg_ptr& msgp) override
        {
            csRequestRoomList = unpack_msg<CSRequestRoomList>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_RequestRoomList;
        }

    public:
        CSRequestRoomList csRequestRoomList;
    };

    class SCRequestRoomList_Wrap : public Proto
    {
        using this_t = SCRequestRoomList_Wrap;
        using base_t = Proto;
    public:
        SCRequestRoomList_Wrap()
            : base_t(ProtoId()) {}
        SCRequestRoomList_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scRequestRoomList(rhs.scRequestRoomList)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scRequestRoomList);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scRequestRoomList);
        }
        void unpack(msg_ptr& msgp) override
        {
            scRequestRoomList = unpack_msg<SCRequestRoomList>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_RequestRoomList;
        }

    public:
        SCRequestRoomList scRequestRoomList;
    };

    class CSSwitchShip_Wrap : public Proto
    {
        using this_t = CSSwitchShip_Wrap;
        using base_t = Proto;
    public:
        CSSwitchShip_Wrap()
            : base_t(ProtoId()) {}
        CSSwitchShip_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csSwitchShip(rhs.csSwitchShip)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csSwitchShip);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csSwitchShip);
        }
        void unpack(msg_ptr& msgp) override
        {
            csSwitchShip = unpack_msg<CSSwitchShip>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_SwitchShip;
        }

    public:
        CSSwitchShip csSwitchShip;
    };

    class SCSwitchShip_Wrap : public Proto
    {
        using this_t = SCSwitchShip_Wrap;
        using base_t = Proto;
    public:
        SCSwitchShip_Wrap()
            : base_t(ProtoId()) {}
        SCSwitchShip_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scSwitchShip(rhs.scSwitchShip)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scSwitchShip);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scSwitchShip);
        }
        void unpack(msg_ptr& msgp) override
        {
            scSwitchShip = unpack_msg<SCSwitchShip>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_SwitchShip;
        }

    public:
        SCSwitchShip scSwitchShip;
    };

    class CSSwitchSlot_Wrap : public Proto
    {
        using this_t = CSSwitchSlot_Wrap;
        using base_t = Proto;
    public:
        CSSwitchSlot_Wrap()
            : base_t(ProtoId()) {}
        CSSwitchSlot_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csSwitchSlot(rhs.csSwitchSlot)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csSwitchSlot);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csSwitchSlot);
        }
        void unpack(msg_ptr& msgp) override
        {
            csSwitchSlot = unpack_msg<CSSwitchSlot>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_SwitchSlot;
        }

    public:
        CSSwitchSlot csSwitchSlot;
    };

    class SCSwitchSlot_Wrap : public Proto
    {
        using this_t = SCSwitchSlot_Wrap;
        using base_t = Proto;
    public:
        SCSwitchSlot_Wrap()
            : base_t(ProtoId()) {}
        SCSwitchSlot_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scSwitchSlot(rhs.scSwitchSlot)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scSwitchSlot);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scSwitchSlot);
        }
        void unpack(msg_ptr& msgp) override
        {
            scSwitchSlot = unpack_msg<SCSwitchSlot>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_SwitchSlot;
        }

    public:
        SCSwitchSlot scSwitchSlot;
    };

    class CSPlayerInput_Wrap : public Proto
    {
        using this_t = CSPlayerInput_Wrap;
        using base_t = Proto;
    public:
        CSPlayerInput_Wrap()
            : base_t(ProtoId()) {}
        CSPlayerInput_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csPlayerInput(rhs.csPlayerInput)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csPlayerInput);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csPlayerInput);
        }
        void unpack(msg_ptr& msgp) override
        {
            csPlayerInput = unpack_msg<CSPlayerInput>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_PlayerInput;
        }

    public:
        CSPlayerInput csPlayerInput;
    };

    class SCPlayerInput_Wrap : public Proto
    {
        using this_t = SCPlayerInput_Wrap;
        using base_t = Proto;
    public:
        SCPlayerInput_Wrap()
            : base_t(ProtoId()) {}
        SCPlayerInput_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scPlayerInput(rhs.scPlayerInput)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scPlayerInput);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scPlayerInput);
        }
        void unpack(msg_ptr& msgp) override
        {
            scPlayerInput = unpack_msg<SCPlayerInput>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_PlayerInput;
        }

    public:
        SCPlayerInput scPlayerInput;
    };

    class CSSyncCommandMsg_Wrap : public Proto
    {
        using this_t = CSSyncCommandMsg_Wrap;
        using base_t = Proto;
    public:
        CSSyncCommandMsg_Wrap()
            : base_t(ProtoId()) {}
        CSSyncCommandMsg_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csSyncCommandMsg(rhs.csSyncCommandMsg)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csSyncCommandMsg);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csSyncCommandMsg);
        }
        void unpack(msg_ptr& msgp) override
        {
            csSyncCommandMsg = unpack_msg<CSSyncCommandMsg>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_SyncCommandMsg;
        }

    public:
        CSSyncCommandMsg csSyncCommandMsg;
    };

    class SCSyncCommandMsg_Wrap : public Proto
    {
        using this_t = SCSyncCommandMsg_Wrap;
        using base_t = Proto;
    public:
        SCSyncCommandMsg_Wrap()
            : base_t(ProtoId()) {}
        SCSyncCommandMsg_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scSyncCommandMsg(rhs.scSyncCommandMsg)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scSyncCommandMsg);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scSyncCommandMsg);
        }
        void unpack(msg_ptr& msgp) override
        {
            scSyncCommandMsg = unpack_msg<SCSyncCommandMsg>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_SyncCommandMsg;
        }

    public:
        SCSyncCommandMsg scSyncCommandMsg;
    };

    class SCInstantiate_Wrap : public Proto
    {
        using this_t = SCInstantiate_Wrap;
        using base_t = Proto;
    public:
        SCInstantiate_Wrap()
            : base_t(ProtoId()) {}
        SCInstantiate_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scInstantiate(rhs.scInstantiate)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scInstantiate);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scInstantiate);
        }
        void unpack(msg_ptr& msgp) override
        {
            scInstantiate = unpack_msg<SCInstantiate>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_Instantiate;
        }

    public:
        SCInstantiate scInstantiate;
    };

    class SCFactionMsg_Wrap : public Proto
    {
        using this_t = SCFactionMsg_Wrap;
        using base_t = Proto;
    public:
        SCFactionMsg_Wrap()
            : base_t(ProtoId()) {}
        SCFactionMsg_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scFactionMsg(rhs.scFactionMsg)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scFactionMsg);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scFactionMsg);
        }
        void unpack(msg_ptr& msgp) override
        {
            scFactionMsg = unpack_msg<SCFactionMsg>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_FactionMsg;
        }

    public:
        SCFactionMsg scFactionMsg;
    };

    class CSPing_Wrap : public Proto
    {
        using this_t = CSPing_Wrap;
        using base_t = Proto;
    public:
        CSPing_Wrap()
            : base_t(ProtoId()) {}
        CSPing_Wrap(const this_t& rhs)
            : base_t(rhs)
            , csPing(rhs.csPing)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(csPing);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, csPing);
        }
        void unpack(msg_ptr& msgp) override
        {
            csPing = unpack_msg<CSPing>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_Ping;
        }

    public:
        CSPing csPing;
    };

    class SCPing_Wrap : public Proto
    {
        using this_t = SCPing_Wrap;
        using base_t = Proto;
    public:
        SCPing_Wrap()
            : base_t(ProtoId()) {}
        SCPing_Wrap(const this_t& rhs)
            : base_t(rhs)
            , scPing(rhs.scPing)
        {}
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        msg_ptr pack() const
        {
            return pack_msg(scPing);
        }
        void pack(stream_t& gos) const
        {
            pack_msg(gos, scPing);
        }
        void unpack(msg_ptr& msgp) override
        {
            scPing = unpack_msg<SCPing>(msgp);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_Ping;
        }

    public:
        SCPing scPing;
    };

}
