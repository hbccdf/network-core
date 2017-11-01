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
namespace csharp AutoGen

struct TVector3{
	10: i32 x
	20: i32 y
	30: i32 z
}

struct TRotation{
	10: i32 x
	20: i32 y
	30: i32 z
	40: i32 w
}

struct Bounds{
	10: TVector3 center
	20: TVector3 size
}

enum BattlePattern{
	None = -1,
	Moba,
	ThreeParty,
	Royale,
}

enum OperateCmdType{
	MOVE,
	MOVE_STOP,
	DRAW_LINE_START,
	DRAW_LINE,
	DRAW_LINE_END,
	CREATE_CAGE,// moveDir is center,argv1 is radius(int * 1000)
	DRAW_WALL_ENERGY_START,
	DRAW_WALL_ENERGY_CANCEL,
	DRAW_WALL_START,
	DRAW_WALL,
	DRAW_WALL_END,
	USE_SKILL_START = 20,
	USE_SKILL_STOP,
	USE_SKILL_ONCE,
	//ATTACK_START,
	//ATTACK_END,
	DROP_SKILL,
	USE_ULT_SKILL_START
	USE_ULT_SKILL_END
	USE_ULT_SKILL_TRIGGER
	ULT_CONTROL_MOVE
	USE_TAG
}

struct PlayerOperateCommand{
	10: i32 uid
	20: OperateCmdType type
	30: TVector3 moveDir
	40: i32 argv1
	50: i32 argv2
	60: i32 argv3
}

struct MovementData{
	10: TVector3 moveDir
	20: TVector3 facingDir
}

struct StateMaps{
	10: map<i32, bool> bools
	20: map<i32, i32> ints
	30: map<i32, double> decimals
	40: map<i32, TVector3> vectors
}

struct UnitSyncData{
	10:	StateMaps states
}


enum SyncDataType{
	STATUS = 0,
	MOVE = 1,
	MOVE_END = 2,
	USE_SKILL = 3,
	BE_KILLED = 4,
	//ATTACK_START = 5,
	//ATTACK_END = 6,
	BULLET = 7,
	LINE = 8,
	CAGE = 9,
	REVIVE = 10,
	GET_ITEM = 11,
	ADD_BUFF = 12,
	DEL_BUFF = 13,
	TRIGGER_MAGIC = 14,
	DROP_SKILL,
	USE_ULT_SKILL_START,
	USE_ULT_SKILL_SUCCESS,
	USE_ULT_SKILL_FAILED,
	USE_ULT_SKILL_CANCEL,
	USE_ULT_SKILL_TRIGGER,
	TAG_SUCCESS
}

struct Move{
	10: TVector3 pos
	20: TVector3 dir
	30: TRotation rot
}

struct MoveStop{
	10: TVector3 pos
	20: TRotation rot
}

struct UseSkill{
	10: i32 skillId
	20: list<i32> targetIds
}

struct KillDead{
	10: i32 killerId
}

struct Bullet{
	10: i32 uid
	20: i32 casterId
	30: TVector3 pos
	40: TVector3 dir
}

struct SyncBullet{
	10: list<Bullet> bulletList
}

enum LineType{
	DrawLine = 1,
	DrawWall = 2,
}

struct Line{
	10: i32 uid
	20: i32 casterId
	30: list<TVector3> points
	40: bool isEnd
	50: LineType lineType
}



struct Cage{
	10: i32 uid
	20: TVector3 center
	30: i32 radius
	40: i32 captureTime
}

struct Revive{
	10: i32 bornId
	20: TVector3 bornPos
}

struct SyncLine{
	10: list<Line> lineList
}

struct GetItem{
	10: i32 itemUid
}

struct AddBuff{
	10: i32 id
}

struct DelBuff{
	10: i32 id
}

struct TriggerMagic{
	10: i32 magicId
}

struct DropSkill{
	10: i32 skillId
	20: bool isA
}

struct UseUltSkillStart{
	10: i32 id
}

struct UseUltSkillSuccess{
	10: i32 id
	20: i32 power
	30: TRotation rot
}

struct UseUltSkillFailed{
	10: i32 id
}

struct UseUltSkillCancel{
	10: i32 id
}

struct UseUltSkillTrigger{
	10: i32 id
	20: i32 stage
}

struct TagSuccess{
	10: i32 type
	20: TVector3 pos
}

struct SyncData{
	10: i32 uid
	20: SyncDataType syncType
	30: optional UnitSyncData unitStatus
	40: optional Move move
	50: optional MoveStop moveStop
	60: optional UseSkill userSkill
	70: optional KillDead killDead
	80: optional SyncBullet bullet
	90: optional SyncLine line
	100: optional Cage cage
	110: optional Revive revive
	120: optional GetItem getItem
	130: optional AddBuff addBuff
	140: optional DelBuff delBuff
	150: optional TriggerMagic triggerMagic
	160: optional DropSkill dropSkill
	170: optional UseUltSkillStart useUltSkillStart
	175: optional UseUltSkillSuccess useUltSkillSuccess
	180: optional UseUltSkillFailed useUltSkillFailed
	190: optional UseUltSkillCancel useUltSkillCancel
	200: optional UseUltSkillTrigger useUltSkillTrigger 
	210: optional TagSuccess useTag
}

enum ClassType{
	DRIVER = 1,//数值的设置用于位运算
	SHOOTER = 2
}

struct PlayerData{
	10: i32 userId
	20: string nickname
	30: i32 faction
	40: ClassType classType
	50: i32 shipId
	60: i32 position
	70: i32 playerId
	80: i32 shipLevel
	90: i32 skillSlotCount
	100: list<i32> uniqueSkills
	110: i32 bulletCount
}

struct RoleData{
	10: i32 shipId
	20: i32 shipConfigId
	
}

struct UserData{
	10: i32 userId
	20: string userAccount
	30: string userPassword
	40: RoleData roleData
}

struct AccountData{
	10: i32 id
	20: string sessionKey
	30: string channel
	40: string openId
	50: i64 lastLoginTime
}

enum ObjType{
	PLAYER,
	MONSTER,//怪物
	ITEM,
	MAGIC,
	WALL_PALYER,//墙
	BASECENTER,//基地

}

struct InstantiateData{
	10: i32 uid
	15: i32 creator
	20: ObjType type
	30: TVector3 pos
	40: TRotation rot
	50: list<i32> argvs
	60: TVector3 casterPos
}

enum MonsterType{
	None,
        ResShark = 1,
        Whale = 2,
       	GuardShark = 3,
	FaceFish = 5,
	BottleItem = 6
	BottleItem2 = 7
}

enum MagicType{
	None,
        ShuiLei = 1,
	PaoDan,
}

enum FactionType{
	None = -1,
	FactionA = 1,
	FactionB = 2,
	FactionC = 3,
}


struct RoomData{
	10: i32 id
	20: string name
	30: i32 pattern
	40: i32 playerCount
	50: i32 playerMaxCount
}