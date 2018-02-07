/**
 * The first thing to know about are types. The available types in Thrift are:
 *
 *  bool        Boolean, one byte
 *  byte        Signed byte
 *  i16         Signed 16-bit integer
 *  i32         Signed 32-bit integer
 *  i64         Signed 64-bit integer
 *  double      64-bit floating point value
 *  string      String
 *  map<t1,t2>  Map from one type to another
 *  list<t1>    Ordered list of one type
 *  set<t1>     Set of unique elements of one type
 *
 * Did you also notice that Thrift supports C style comments?
 */

 include "struct.thrift"

namespace csharp AutoGen

enum MessageId{
	C_LoginGame					=		0x5050005,
	S_LoginGame					=		0x7050005,

	SC_Connect 					= 		0x40a0006,
	SC_DisConnect				=		0x40a0008,

	CS_CreateRoom				=		0x1d080001,
	SC_CreateRoom				=		0x1e080001,
	CS_JoinRoom					=		0x1d080002,
	SC_JoinRoom					=		0x1e080002,
	SC_RoomInfo					=		0x1e080003,
	CS_SwitchSeat				=		0x1d080004,
	SC_SwitchSeat				=		0x1e080004,
	CS_ExitRoom					=		0x1d080005,
	SC_ExitRoom					=		0x1e080005,
	SC_RoomDestroyed			=		0x1e080006,

	CS_StartBattle				=		0x1d080007,
	SC_BeginLoadResource		=		0x1e080007,
	CS_EndLoadResource			=		0x1d080008,
	SC_StartBattle				=		0x1e080008,
	SC_BattleResult				=		0x1e080009,
	CS_EndBattle				=		0x1d08000a,
	SC_EndBattle				=		0x1e08000a,
	SC_ServerInfo				=		0x1e08000b,
	CS_RequestRoomList			=		0x1d08000c,
	SC_RequestRoomList			=		0x1e08000c,
	CS_SwitchShip				=		0x1d08000d,
	SC_SwitchShip				=		0x1e08000d,
	CS_SwitchSlot 				= 		0x1d08000e,
	SC_SwitchSlot 				= 		0x1e08000e,
	
	CS_PlayerInput				=		0x1d0d0001,
	SC_PlayerInput				=		0x1e0d0001,
	CS_SyncCommandMsg		    =		0x1d0d0002,
	SC_SyncCommandMsg		    =		0x1e0d0002,
	SC_Instantiate				=		0x1e0d0003,
	SC_FactionMsg				=		0x1e0d0004,
    
	CS_Ping						=		0x1d0d0005,
	SC_Ping						=		0x1e0d0005,
}

// general login server
struct CLoginGame {
	10: i32 accountId
	20: string sessionKey
}

struct SLoginGame {
	10: bool state
	20: optional string error
	30: optional struct.AccountData account
}

struct CSCreateRoom{
	10: i32 playerCount
	20: i32 matchPattern
}

struct SCCreateRoom{
	10: i32 result
}

struct CSRequestRoomList{
	10: i32 pattern
	20: i32 pageNum
	30: i32 countPerPage
}

struct SCRequestRoomList{
	10: i32 result
	20: i32 roomCount
	30: list<struct.RoomData> roomList
}

struct CSJoinRoom{
	10: i32 roomId
}

struct SCJoinRoom{
	10: i32 result
}

struct SCRoomInfo{
	10: i32 roomId
	20: i32 masterId
	30: list<struct.PlayerData> playerList
	40: i32 roomSize
	50: i32 mathPattern
}

struct CSSwitchSeat{
	10: i32 playerId
	20: i32 targetPos
}

struct SCSwitchSeat{
	10: i32 result
}

struct CSSwitchShip{
	10: i32 shipId
}

struct SCSwitchShip{
	10: i32 result
}

struct CSSwitchSlot{
	10: i32 slotCount
	20: list<i32> uniqueSkills
	30: i32 bulletCount
}

struct SCSwitchSlot{
	10: i32 result
}

struct CSExitRoom{}

struct SCExitRoom{
	10: i32 result
}

struct SCRoomDestroyed{
	10: i32 result
}

struct CSStartBattle{}

struct SCBeginLoadResource{
	10: i32 result
}

struct CSEndLoadResource{}

struct SCStartBattle{
	10: i32 result
	20: i64 startTime
}

struct SCBattleResult{
	10: i32 result
	20: map<i32,i32> fectionScore
}

struct CSEndBattle{}

struct SCEndBattle{
	10: i32 result
}

struct	CSPlayerInput{
	10: list<struct.PlayerOperateCommand> commandList
}

struct SCPlayerInput{
	10: list<struct.PlayerOperateCommand> commandList
}

struct CSSyncCommandMsg{
	10:	list<struct.SyncData> commandList
}

struct SCSyncCommandMsg{
	10: i32 time
	20: list<struct.SyncData> commandList
}

struct SCInstantiate{
	10: list<struct.InstantiateData> objList
}

struct CSPing
{
}

struct SCPing
{
	10: i32 result
}

struct SCServerInfo{
	10: i64 currentServerTime
}

struct SCFactionMsg{
	10: i32 shipId		//占领该区域的船的id
	20: i32 regionId		//占领区域id
}