#pragma once
#include <network/gameserver/game_server.hpp>
#include "message_types.h"

 namespace CytxGame
 {
    using namespace cytx::gameserver;
    using namespace cytx;

    using game_server_t = cytx::gameserver::game_server;

    class CLoginGame_Msg : public Proto
    {
        using this_t = CLoginGame_Msg;
        using base_t = Proto;
    public:
        CLoginGame_Msg()
            : base_t(ProtoId()) {}
        CLoginGame_Msg(const this_t& rhs)
            : base_t(rhs)
            , cLoginGame(rhs.cLoginGame)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::C_LoginGame;
        }

    public:
        CLoginGame cLoginGame;
    };

    class SLoginGame_Msg : public Proto
    {
        using this_t = SLoginGame_Msg;
        using base_t = Proto;
    public:
        SLoginGame_Msg()
            : base_t(ProtoId()) {}
        SLoginGame_Msg(const this_t& rhs)
            : base_t(rhs)
            , sLoginGame(rhs.sLoginGame)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::S_LoginGame;
        }

    public:
        SLoginGame sLoginGame;
    };

    class SCConnect_Msg : public Proto
    {
        using this_t = SCConnect_Msg;
        using base_t = Proto;
    public:
        SCConnect_Msg()
            : base_t(ProtoId()) {}
        SCConnect_Msg(const this_t& rhs)
            : base_t(rhs)
        {}
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
            return ::MessageId::SC_Connect;
        }

    };

    class SCDisConnect_Msg : public Proto
    {
        using this_t = SCDisConnect_Msg;
        using base_t = Proto;
    public:
        SCDisConnect_Msg()
            : base_t(ProtoId()) {}
        SCDisConnect_Msg(const this_t& rhs)
            : base_t(rhs)
        {}
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
            return ::MessageId::SC_DisConnect;
        }

    };

    class CSCreateRoom_Msg : public Proto
    {
        using this_t = CSCreateRoom_Msg;
        using base_t = Proto;
    public:
        CSCreateRoom_Msg()
            : base_t(ProtoId()) {}
        CSCreateRoom_Msg(const this_t& rhs)
            : base_t(rhs)
            , csCreateRoom(rhs.csCreateRoom)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_CreateRoom;
        }

    public:
        CSCreateRoom csCreateRoom;
    };

    class SCCreateRoom_Msg : public Proto
    {
        using this_t = SCCreateRoom_Msg;
        using base_t = Proto;
    public:
        SCCreateRoom_Msg()
            : base_t(ProtoId()) {}
        SCCreateRoom_Msg(const this_t& rhs)
            : base_t(rhs)
            , scCreateRoom(rhs.scCreateRoom)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_CreateRoom;
        }

    public:
        SCCreateRoom scCreateRoom;
    };

    class CSJoinRoom_Msg : public Proto
    {
        using this_t = CSJoinRoom_Msg;
        using base_t = Proto;
    public:
        CSJoinRoom_Msg()
            : base_t(ProtoId()) {}
        CSJoinRoom_Msg(const this_t& rhs)
            : base_t(rhs)
            , csJoinRoom(rhs.csJoinRoom)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_JoinRoom;
        }

    public:
        CSJoinRoom csJoinRoom;
    };

    class SCJoinRoom_Msg : public Proto
    {
        using this_t = SCJoinRoom_Msg;
        using base_t = Proto;
    public:
        SCJoinRoom_Msg()
            : base_t(ProtoId()) {}
        SCJoinRoom_Msg(const this_t& rhs)
            : base_t(rhs)
            , scJoinRoom(rhs.scJoinRoom)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_JoinRoom;
        }

    public:
        SCJoinRoom scJoinRoom;
    };

    class SCRoomInfo_Msg : public Proto
    {
        using this_t = SCRoomInfo_Msg;
        using base_t = Proto;
    public:
        SCRoomInfo_Msg()
            : base_t(ProtoId()) {}
        SCRoomInfo_Msg(const this_t& rhs)
            : base_t(rhs)
            , scRoomInfo(rhs.scRoomInfo)
        {}
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
            return ::MessageId::SC_RoomInfo;
        }

    public:
        SCRoomInfo scRoomInfo;
    };

    class CSSwitchSeat_Msg : public Proto
    {
        using this_t = CSSwitchSeat_Msg;
        using base_t = Proto;
    public:
        CSSwitchSeat_Msg()
            : base_t(ProtoId()) {}
        CSSwitchSeat_Msg(const this_t& rhs)
            : base_t(rhs)
            , csSwitchSeat(rhs.csSwitchSeat)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_SwitchSeat;
        }

    public:
        CSSwitchSeat csSwitchSeat;
    };

    class SCSwitchSeat_Msg : public Proto
    {
        using this_t = SCSwitchSeat_Msg;
        using base_t = Proto;
    public:
        SCSwitchSeat_Msg()
            : base_t(ProtoId()) {}
        SCSwitchSeat_Msg(const this_t& rhs)
            : base_t(rhs)
            , scSwitchSeat(rhs.scSwitchSeat)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_SwitchSeat;
        }

    public:
        SCSwitchSeat scSwitchSeat;
    };

    class CSExitRoom_Msg : public Proto
    {
        using this_t = CSExitRoom_Msg;
        using base_t = Proto;
    public:
        CSExitRoom_Msg()
            : base_t(ProtoId()) {}
        CSExitRoom_Msg(const this_t& rhs)
            : base_t(rhs)
            , csExitRoom(rhs.csExitRoom)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_ExitRoom;
        }

    public:
        CSExitRoom csExitRoom;
    };

    class SCExitRoom_Msg : public Proto
    {
        using this_t = SCExitRoom_Msg;
        using base_t = Proto;
    public:
        SCExitRoom_Msg()
            : base_t(ProtoId()) {}
        SCExitRoom_Msg(const this_t& rhs)
            : base_t(rhs)
            , scExitRoom(rhs.scExitRoom)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_ExitRoom;
        }

    public:
        SCExitRoom scExitRoom;
    };

    class SCRoomDestroyed_Msg : public Proto
    {
        using this_t = SCRoomDestroyed_Msg;
        using base_t = Proto;
    public:
        SCRoomDestroyed_Msg()
            : base_t(ProtoId()) {}
        SCRoomDestroyed_Msg(const this_t& rhs)
            : base_t(rhs)
            , scRoomDestroyed(rhs.scRoomDestroyed)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_RoomDestroyed;
        }

    public:
        SCRoomDestroyed scRoomDestroyed;
    };

    class CSStartBattle_Msg : public Proto
    {
        using this_t = CSStartBattle_Msg;
        using base_t = Proto;
    public:
        CSStartBattle_Msg()
            : base_t(ProtoId()) {}
        CSStartBattle_Msg(const this_t& rhs)
            : base_t(rhs)
            , csStartBattle(rhs.csStartBattle)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_StartBattle;
        }

    public:
        CSStartBattle csStartBattle;
    };

    class SCBeginLoadResource_Msg : public Proto
    {
        using this_t = SCBeginLoadResource_Msg;
        using base_t = Proto;
    public:
        SCBeginLoadResource_Msg()
            : base_t(ProtoId()) {}
        SCBeginLoadResource_Msg(const this_t& rhs)
            : base_t(rhs)
            , scBeginLoadResource(rhs.scBeginLoadResource)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_BeginLoadResource;
        }

    public:
        SCBeginLoadResource scBeginLoadResource;
    };

    class CSEndLoadResource_Msg : public Proto
    {
        using this_t = CSEndLoadResource_Msg;
        using base_t = Proto;
    public:
        CSEndLoadResource_Msg()
            : base_t(ProtoId()) {}
        CSEndLoadResource_Msg(const this_t& rhs)
            : base_t(rhs)
            , csEndLoadResource(rhs.csEndLoadResource)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_EndLoadResource;
        }

    public:
        CSEndLoadResource csEndLoadResource;
    };

    class SCStartBattle_Msg : public Proto
    {
        using this_t = SCStartBattle_Msg;
        using base_t = Proto;
    public:
        SCStartBattle_Msg()
            : base_t(ProtoId()) {}
        SCStartBattle_Msg(const this_t& rhs)
            : base_t(rhs)
            , scStartBattle(rhs.scStartBattle)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_StartBattle;
        }

    public:
        SCStartBattle scStartBattle;
    };

    class SCBattleResult_Msg : public Proto
    {
        using this_t = SCBattleResult_Msg;
        using base_t = Proto;
    public:
        SCBattleResult_Msg()
            : base_t(ProtoId()) {}
        SCBattleResult_Msg(const this_t& rhs)
            : base_t(rhs)
            , scBattleResult(rhs.scBattleResult)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_BattleResult;
        }

    public:
        SCBattleResult scBattleResult;
    };

    class CSEndBattle_Msg : public Proto
    {
        using this_t = CSEndBattle_Msg;
        using base_t = Proto;
    public:
        CSEndBattle_Msg()
            : base_t(ProtoId()) {}
        CSEndBattle_Msg(const this_t& rhs)
            : base_t(rhs)
            , csEndBattle(rhs.csEndBattle)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_EndBattle;
        }

    public:
        CSEndBattle csEndBattle;
    };

    class SCEndBattle_Msg : public Proto
    {
        using this_t = SCEndBattle_Msg;
        using base_t = Proto;
    public:
        SCEndBattle_Msg()
            : base_t(ProtoId()) {}
        SCEndBattle_Msg(const this_t& rhs)
            : base_t(rhs)
            , scEndBattle(rhs.scEndBattle)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_EndBattle;
        }

    public:
        SCEndBattle scEndBattle;
    };

    class SCServerInfo_Msg : public Proto
    {
        using this_t = SCServerInfo_Msg;
        using base_t = Proto;
    public:
        SCServerInfo_Msg()
            : base_t(ProtoId()) {}
        SCServerInfo_Msg(const this_t& rhs)
            : base_t(rhs)
            , scServerInfo(rhs.scServerInfo)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_ServerInfo;
        }

    public:
        SCServerInfo scServerInfo;
    };

    class CSRequestRoomList_Msg : public Proto
    {
        using this_t = CSRequestRoomList_Msg;
        using base_t = Proto;
    public:
        CSRequestRoomList_Msg()
            : base_t(ProtoId()) {}
        CSRequestRoomList_Msg(const this_t& rhs)
            : base_t(rhs)
            , csRequestRoomList(rhs.csRequestRoomList)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_RequestRoomList;
        }

    public:
        CSRequestRoomList csRequestRoomList;
    };

    class SCRequestRoomList_Msg : public Proto
    {
        using this_t = SCRequestRoomList_Msg;
        using base_t = Proto;
    public:
        SCRequestRoomList_Msg()
            : base_t(ProtoId()) {}
        SCRequestRoomList_Msg(const this_t& rhs)
            : base_t(rhs)
            , scRequestRoomList(rhs.scRequestRoomList)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_RequestRoomList;
        }

    public:
        SCRequestRoomList scRequestRoomList;
    };

    class CSSwitchShip_Msg : public Proto
    {
        using this_t = CSSwitchShip_Msg;
        using base_t = Proto;
    public:
        CSSwitchShip_Msg()
            : base_t(ProtoId()) {}
        CSSwitchShip_Msg(const this_t& rhs)
            : base_t(rhs)
            , csSwitchShip(rhs.csSwitchShip)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_SwitchShip;
        }

    public:
        CSSwitchShip csSwitchShip;
    };

    class SCSwitchShip_Msg : public Proto
    {
        using this_t = SCSwitchShip_Msg;
        using base_t = Proto;
    public:
        SCSwitchShip_Msg()
            : base_t(ProtoId()) {}
        SCSwitchShip_Msg(const this_t& rhs)
            : base_t(rhs)
            , scSwitchShip(rhs.scSwitchShip)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_SwitchShip;
        }

    public:
        SCSwitchShip scSwitchShip;
    };

    class CSSwitchSlot_Msg : public Proto
    {
        using this_t = CSSwitchSlot_Msg;
        using base_t = Proto;
    public:
        CSSwitchSlot_Msg()
            : base_t(ProtoId()) {}
        CSSwitchSlot_Msg(const this_t& rhs)
            : base_t(rhs)
            , csSwitchSlot(rhs.csSwitchSlot)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_SwitchSlot;
        }

    public:
        CSSwitchSlot csSwitchSlot;
    };

    class SCSwitchSlot_Msg : public Proto
    {
        using this_t = SCSwitchSlot_Msg;
        using base_t = Proto;
    public:
        SCSwitchSlot_Msg()
            : base_t(ProtoId()) {}
        SCSwitchSlot_Msg(const this_t& rhs)
            : base_t(rhs)
            , scSwitchSlot(rhs.scSwitchSlot)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_SwitchSlot;
        }

    public:
        SCSwitchSlot scSwitchSlot;
    };

    class CSPlayerInput_Msg : public Proto
    {
        using this_t = CSPlayerInput_Msg;
        using base_t = Proto;
    public:
        CSPlayerInput_Msg()
            : base_t(ProtoId()) {}
        CSPlayerInput_Msg(const this_t& rhs)
            : base_t(rhs)
            , csPlayerInput(rhs.csPlayerInput)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_PlayerInput;
        }

    public:
        CSPlayerInput csPlayerInput;
    };

    class SCPlayerInput_Msg : public Proto
    {
        using this_t = SCPlayerInput_Msg;
        using base_t = Proto;
    public:
        SCPlayerInput_Msg()
            : base_t(ProtoId()) {}
        SCPlayerInput_Msg(const this_t& rhs)
            : base_t(rhs)
            , scPlayerInput(rhs.scPlayerInput)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_PlayerInput;
        }

    public:
        SCPlayerInput scPlayerInput;
    };

    class CSSyncCommandMsg_Msg : public Proto
    {
        using this_t = CSSyncCommandMsg_Msg;
        using base_t = Proto;
    public:
        CSSyncCommandMsg_Msg()
            : base_t(ProtoId()) {}
        CSSyncCommandMsg_Msg(const this_t& rhs)
            : base_t(rhs)
            , csSyncCommandMsg(rhs.csSyncCommandMsg)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_SyncCommandMsg;
        }

    public:
        CSSyncCommandMsg csSyncCommandMsg;
    };

    class SCSyncCommandMsg_Msg : public Proto
    {
        using this_t = SCSyncCommandMsg_Msg;
        using base_t = Proto;
    public:
        SCSyncCommandMsg_Msg()
            : base_t(ProtoId()) {}
        SCSyncCommandMsg_Msg(const this_t& rhs)
            : base_t(rhs)
            , scSyncCommandMsg(rhs.scSyncCommandMsg)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_SyncCommandMsg;
        }

    public:
        SCSyncCommandMsg scSyncCommandMsg;
    };

    class SCInstantiate_Msg : public Proto
    {
        using this_t = SCInstantiate_Msg;
        using base_t = Proto;
    public:
        SCInstantiate_Msg()
            : base_t(ProtoId()) {}
        SCInstantiate_Msg(const this_t& rhs)
            : base_t(rhs)
            , scInstantiate(rhs.scInstantiate)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_Instantiate;
        }

    public:
        SCInstantiate scInstantiate;
    };

    class SCFactionMsg_Msg : public Proto
    {
        using this_t = SCFactionMsg_Msg;
        using base_t = Proto;
    public:
        SCFactionMsg_Msg()
            : base_t(ProtoId()) {}
        SCFactionMsg_Msg(const this_t& rhs)
            : base_t(rhs)
            , scFactionMsg(rhs.scFactionMsg)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_FactionMsg;
        }

    public:
        SCFactionMsg scFactionMsg;
    };

    class CSPing_Msg : public Proto
    {
        using this_t = CSPing_Msg;
        using base_t = Proto;
    public:
        CSPing_Msg()
            : base_t(ProtoId()) {}
        CSPing_Msg(const this_t& rhs)
            : base_t(rhs)
            , csPing(rhs.csPing)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::CS_Ping;
        }

    public:
        CSPing csPing;
    };

    class SCPing_Msg : public Proto
    {
        using this_t = SCPing_Msg;
        using base_t = Proto;
    public:
        SCPing_Msg()
            : base_t(ProtoId()) {}
        SCPing_Msg(const this_t& rhs)
            : base_t(rhs)
            , scPing(rhs.scPing)
        {}
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

    public:
        static uint32_t ProtoId()
        {
            return ::MessageId::SC_Ping;
        }

    public:
        SCPing scPing;
    };

}
