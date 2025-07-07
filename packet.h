#ifndef __INC_PACKET_H__
#define __INC_PACKET_H__
#include "stdafx.h"

enum
{
	HEADER_CG_HANDSHAKE				= 0xff,
	HEADER_CG_PONG					= 0xfe,
	HEADER_CG_TIME_SYNC				= 0xfc,
	HEADER_CG_KEY_AGREEMENT			= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_

	HEADER_CG_LOGIN					= 1,
	HEADER_CG_ATTACK				= 2,
	HEADER_CG_CHAT					= 3,
	HEADER_CG_CHARACTER_CREATE		= 4,
	HEADER_CG_CHARACTER_DELETE		= 5,
	HEADER_CG_CHARACTER_SELECT		= 6,
	HEADER_CG_MOVE					= 7,
	HEADER_CG_SYNC_POSITION			= 8,
	HEADER_CG_ENTERGAME				= 10,

	HEADER_CG_ITEM_USE				= 11,
	HEADER_CG_ITEM_DROP				= 12,
	HEADER_CG_ITEM_MOVE				= 13,
	HEADER_CG_ITEM_PICKUP			= 15,

	HEADER_CG_QUICKSLOT_ADD			= 16,
	HEADER_CG_QUICKSLOT_DEL			= 17,
	HEADER_CG_QUICKSLOT_SWAP		= 18,
	HEADER_CG_WHISPER				= 19,
	HEADER_CG_ITEM_DROP2			= 20,
	HEADER_CG_ITEM_DESTROY			= 21,
#ifdef ENABLE_SELL_ITEM
	HEADER_CG_ITEM_SELL								= 22,
#endif	
#ifdef ENABLE_SHOW_CHEST_DROP_SYSTEM
	HEADER_CG_CHEST_DROP_INFO = 23,
#endif

	HEADER_CG_ON_CLICK				= 26,
	HEADER_CG_EXCHANGE				= 27,
	HEADER_CG_CHARACTER_POSITION	= 28,
	HEADER_CG_SCRIPT_ANSWER			= 29,
	HEADER_CG_QUEST_INPUT_STRING	= 30,
	HEADER_CG_QUEST_CONFIRM			= 31,

	HEADER_CG_SHOP					= 50,
	HEADER_CG_FLY_TARGETING			= 51,
	HEADER_CG_USE_SKILL				= 52,
	HEADER_CG_ADD_FLY_TARGETING		= 53,
	HEADER_CG_SHOOT					= 54,
	HEADER_CG_MYSHOP				= 55,

#ifdef __SKILL_COLOR_SYSTEM__
	HEADER_CG_SKILL_COLOR			= 56,
#endif

#ifdef ENABLE_OFFLINESHOP_SYSTEM
	HEADER_CG_OFFLINE_SHOP			= 57,
	HEADER_CG_MY_OFFLINE_SHOP		= 58,
#endif

#ifdef __SEND_TARGET_INFO__
	HEADER_CG_TARGET_INFO_LOAD		= 59,
#endif

	HEADER_CG_ITEM_USE_TO_ITEM		= 60,
	HEADER_CG_TARGET			 	= 61,

#ifdef ENABLE_SHOP_SEARCH_SYSTEM
	HEADER_CG_SHOP_SEARCH			= 62,
#endif

	HEADER_CG_TEXT					= 64,
	HEADER_CG_WARP					= 65,
	HEADER_CG_SCRIPT_BUTTON			= 66,
	HEADER_CG_MESSENGER				= 67,

	HEADER_CG_MALL_CHECKOUT			= 69,
	HEADER_CG_SAFEBOX_CHECKIN		= 70,
	HEADER_CG_SAFEBOX_CHECKOUT		= 71,

	HEADER_CG_PARTY_INVITE			= 72,
	HEADER_CG_PARTY_INVITE_ANSWER	= 73,
	HEADER_CG_PARTY_REMOVE			= 74,
	HEADER_CG_PARTY_SET_STATE		= 75,
	HEADER_CG_PARTY_USE_SKILL		= 76,
	HEADER_CG_SAFEBOX_ITEM_MOVE		= 77,
	HEADER_CG_PARTY_PARAMETER		= 78,

	HEADER_CG_GUILD					= 80,
	HEADER_CG_ANSWER_MAKE_GUILD		= 81,

	HEADER_CG_FISHING				= 82,

	HEADER_CG_ITEM_GIVE				= 83,

	HEADER_CG_EMPIRE				= 90,

	HEADER_CG_REFINE				= 96,

#ifdef ENABLE_HUNTING_SYSTEM
	HEADER_CG_SEND_HUNTING_ACTION	= 99,
#endif

	HEADER_CG_MARK_LOGIN			= 100,
	HEADER_CG_MARK_CRCLIST			= 101,
	HEADER_CG_MARK_UPLOAD			= 102,
	HEADER_CG_MARK_IDXLIST			= 104,

	HEADER_CG_HACK					= 105,
	HEADER_CG_CHANGE_NAME			= 106,
	HEADER_CG_LOGIN2				= 109,
	HEADER_CG_DUNGEON				= 110,
	HEADER_CG_LOGIN3				= 111,

	HEADER_CG_GUILD_SYMBOL_UPLOAD	= 112,
	HEADER_CG_SYMBOL_CRC			= 113,

	// SCRIPT_SELECT_ITEM
	HEADER_CG_SCRIPT_SELECT_ITEM	= 114,
	// END_OF_SCRIPT_SELECT_ITEM


#ifdef ENABLE_HWID_BAN
	HEADER_CG_HWID_SYSTEM			= 117,
#endif

#ifdef __EXTENDED_WHISPER_DETAILS__
	HEADER_CG_WHISPER_DETAILS		= 130,
#endif
#ifdef ENABLE_SORT_INVEN
	SORT_INVEN = 140,
	SORT_SPECIAL_INVEN = 141,
#endif

#if defined(__SKILLBOOK_COMB_SYSTEM__)
	HEADER_CG_SKILLBOOK_COMB = 151,
#endif

#ifdef ENABLE_SWITCHBOT
	HEADER_CG_SWITCHBOT				= 171,
#endif

	HEADER_CG_DRAGON_SOUL_REFINE	= 205,
	HEADER_CG_STATE_CHECKER			= 206,

#ifdef __AURA_SYSTEM__
		HEADER_CG_AURA				= 208,
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	HEADER_CG_CUBE_RENEWAL 			= 216,
#endif


#ifdef __MULTI_LANGUAGE_SYSTEM__
	HEADER_CG_CHANGE_LANGUAGE = 221,
#endif

#ifdef ENABLE_EXTENDED_BATTLE_PASS
	HEADER_CG_EXT_BATTLE_PASS_ACTION			= 222,
	HEADER_CG_EXT_SEND_BP_PREMIUM_ITEM		= 223,
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	HEADER_CG_BUFF_NPC_CREATE				= 224,
	HEADER_CG_BUFF_NPC_ACTION				= 225,
#endif

	HEADER_CG_CLIENT_VERSION		= 0xfd,
	HEADER_CG_CLIENT_VERSION2		= 0xf1,

	/********************************************************/
	HEADER_GC_KEY_AGREEMENT_COMPLETED			= 0xfa, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_KEY_AGREEMENT						= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_TIME_SYNC							= 0xfc,
	HEADER_GC_PHASE								= 0xfd,
	HEADER_GC_BINDUDP							= 0xfe,
	HEADER_GC_HANDSHAKE							= 0xff,

	HEADER_GC_CHARACTER_ADD						= 1,
	HEADER_GC_CHARACTER_DEL						= 2,
	HEADER_GC_MOVE								= 3,
	HEADER_GC_CHAT								= 4,
	HEADER_GC_SYNC_POSITION						= 5,

	HEADER_GC_LOGIN_SUCCESS						= 6,
	HEADER_GC_LOGIN_SUCCESS_NEWSLOT				= 32,
	HEADER_GC_LOGIN_FAILURE						= 7,

	HEADER_GC_CHARACTER_CREATE_SUCCESS			= 8,
	HEADER_GC_CHARACTER_CREATE_FAILURE			= 9,
	HEADER_GC_CHARACTER_DELETE_SUCCESS			= 10,
	HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID	= 11,

	HEADER_GC_ATTACK							= 12,
	HEADER_GC_STUN								= 13,
	HEADER_GC_DEAD								= 14,

	HEADER_GC_MAIN_CHARACTER_OLD				= 15,
	HEADER_GC_CHARACTER_POINTS					= 16,
	HEADER_GC_CHARACTER_POINT_CHANGE			= 17,
	HEADER_GC_CHANGE_SPEED						= 18,
	HEADER_GC_CHARACTER_UPDATE					= 19,

	HEADER_GC_ITEM_DEL							= 20,
	HEADER_GC_ITEM_SET							= 21,
	HEADER_GC_ITEM_USE							= 22,
	HEADER_GC_ITEM_DROP							= 23,
	HEADER_GC_ITEM_UPDATE						= 25,

	HEADER_GC_ITEM_GROUND_ADD					= 26,
	HEADER_GC_ITEM_GROUND_DEL					= 27,

	HEADER_GC_QUICKSLOT_ADD						= 28,
	HEADER_GC_QUICKSLOT_DEL						= 29,
	HEADER_GC_QUICKSLOT_SWAP					= 30,

	HEADER_GC_ITEM_OWNERSHIP					= 31,

	HEADER_GC_WHISPER							= 34,

	HEADER_GC_MOTION							= 36,
	HEADER_GC_PARTS								= 37,

	HEADER_GC_SHOP								= 38,
	HEADER_GC_SHOP_SIGN							= 39,

	HEADER_GC_DUEL_START						= 40,
	HEADER_GC_PVP                               = 41,
	HEADER_GC_EXCHANGE							= 42,
	HEADER_GC_CHARACTER_POSITION				= 43,

	HEADER_GC_PING								= 44,
	HEADER_GC_SCRIPT							= 45,
	HEADER_GC_QUEST_CONFIRM						= 46,

#ifdef ENABLE_EVENT_MANAGER
	HEADER_GC_EVENT_MANAGER = 47,
#endif

#ifdef ENABLE_OFFLINESHOP_SYSTEM
	HEADER_GC_OFFLINE_SHOP						= 52,
	HEADER_GC_OFFLINE_SHOP_SIGN					= 53,
#endif


#ifdef __SEND_TARGET_INFO__
	HEADER_GC_TARGET_INFO					= 58,
#endif
	HEADER_GC_MOUNT								= 61,
	HEADER_GC_OWNERSHIP							= 62,
	HEADER_GC_TARGET			 				= 63,

	HEADER_GC_WARP								= 65,

#ifdef ENABLE_NEW_DETAILS_GUI
	HEADER_GC_KILL_LOG							= 68,
#endif

	HEADER_GC_ADD_FLY_TARGETING					= 69,
	HEADER_GC_CREATE_FLY						= 70,
	HEADER_GC_FLY_TARGETING						= 71,
	HEADER_GC_SKILL_LEVEL_OLD					= 72,
	HEADER_GC_SKILL_LEVEL						= 76,

	HEADER_GC_MESSENGER							= 74,
	HEADER_GC_GUILD								= 75,

	HEADER_GC_PARTY_INVITE						= 77,
	HEADER_GC_PARTY_ADD							= 78,
	HEADER_GC_PARTY_UPDATE						= 79,
	HEADER_GC_PARTY_REMOVE						= 80,
	HEADER_GC_QUEST_INFO						= 81,
	HEADER_GC_REQUEST_MAKE_GUILD				= 82,
	HEADER_GC_PARTY_PARAMETER					= 83,

	HEADER_GC_SAFEBOX_SET						= 85,
	HEADER_GC_SAFEBOX_DEL						= 86,
	HEADER_GC_SAFEBOX_WRONG_PASSWORD			= 87,
	HEADER_GC_SAFEBOX_SIZE						= 88,

	HEADER_GC_FISHING							= 89,

	HEADER_GC_EMPIRE							= 90,

	HEADER_GC_PARTY_LINK						= 91,
	HEADER_GC_PARTY_UNLINK						= 92,

#ifdef ENABLE_ITEMSHOP
	HEADER_GC_ITEMSHOP							= 93,
#endif

#ifdef __MAINTENANCE__
	HEADER_GC_MAINTENANCE						= 94,
#endif

	HEADER_GC_REFINE_INFORMATION_OLD			= 95,

	HEADER_GC_VIEW_EQUIP						= 99,

	HEADER_GC_MARK_BLOCK						= 100,
	HEADER_GC_MARK_IDXLIST						= 102,

	HEADER_GC_TIME								= 106,
	HEADER_GC_CHANGE_NAME						= 107,

	HEADER_GC_DUNGEON							= 110,

	HEADER_GC_WALK_MODE							= 111,
	HEADER_GC_SKILL_GROUP						= 112,
	HEADER_GC_MAIN_CHARACTER					= 113,

	HEADER_GC_SEPCIAL_EFFECT					= 114,

	HEADER_GC_NPC_POSITION						= 115,

	HEADER_GC_LOGIN_KEY							= 118,
	HEADER_GC_REFINE_INFORMATION				= 119,
	HEADER_GC_CHANNEL							= 121,

	HEADER_GC_TARGET_UPDATE						= 123,
	HEADER_GC_TARGET_DELETE						= 124,
	HEADER_GC_TARGET_CREATE						= 125,

	HEADER_GC_AFFECT_ADD						= 126,
	HEADER_GC_AFFECT_REMOVE						= 127,

	HEADER_GC_MALL_OPEN							= 122,
	HEADER_GC_MALL_SET							= 128,
	HEADER_GC_MALL_DEL							= 129,

	HEADER_GC_LAND_LIST							= 130,
	HEADER_GC_LOVER_INFO						= 131,
	HEADER_GC_LOVE_POINT_UPDATE					= 132,

	HEADER_GC_SYMBOL_DATA						= 133,

	// MINING
	HEADER_GC_DIG_MOTION						= 134,
	// END_OF_MINING

	HEADER_GC_DAMAGE_INFO						= 135,
	HEADER_GC_CHAR_ADDITIONAL_INFO				= 136,

	// SUPPORT_BGM
	HEADER_GC_MAIN_CHARACTER3_BGM				= 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL			= 138,
	// END_OF_SUPPORT_BGM

	HEADER_GC_AUTH_SUCCESS						= 150,

	HEADER_GC_PANAMA_PACK						= 151,

	//HYBRID CRYPT
	HEADER_GC_HYBRIDCRYPT_KEYS					= 152,
	HEADER_GC_HYBRIDCRYPT_SDB					= 153, // SDB means Supplmentary Data Blocks
	//HYBRID CRYPT
#ifdef __RANKING_SYSTEM__
	HEADER_GC_RANK_INFO = 154,
#endif
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
	HEADER_GC_SHOPSEARCH_SET					= 156,
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	HEADER_GC_BUFF_NPC_ACTION = 163,
	HEADER_GC_BUFF_NPC_USE_SKILL = 164,
#endif

#ifdef ENABLE_SHOW_CHEST_DROP_SYSTEM
		HEADER_GC_CHEST_DROP_INFO = 165,
#endif

#ifdef __EXTENDED_WHISPER_DETAILS__
	HEADER_GC_WHISPER_DETAILS = 169,
#endif

#ifdef ENABLE_SWITCHBOT
	HEADER_GC_SWITCHBOT = 171,
#endif

	// ROULETTE
	HEADER_GC_ROULETTE							= 200,
	// END_ROULETTE

	HEADER_GC_SPECIFIC_EFFECT					= 208,

	HEADER_GC_DRAGON_SOUL_REFINE				= 209,
	HEADER_GC_RESPOND_CHANNELSTATUS				= 210,
#ifdef u1x
	HEADER_GC_RANKING_UPDATE				= 211, 
#endif
#ifdef ENABLE_ENTITY_PRELOADING
	HEADER_GC_PRELOAD_ENTITIES 					= 212,
#endif
#ifdef __AURA_SYSTEM__
		HEADER_GC_AURA							= 216,
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	HEADER_GC_CUBE_RENEWAL 					= 217,
#endif

#ifdef ENABLE_PREMIUM_SYSTEM
	HEADER_GC_REFRESH_PREMIUM				= 218,
#endif

#ifdef __MULTI_LANGUAGE_SYSTEM__
	HEADER_GC_REQUEST_CHANGE_LANGUAGE = 219,
#endif

#if defined(__BL_KILL_BAR__)
	HEADER_GC_KILLBAR 						= 220,
#endif


#ifdef ENABLE_EXTENDED_BATTLE_PASS
	HEADER_GC_EXT_BATTLE_PASS_OPEN 					= 235,
	HEADER_GC_EXT_BATTLE_PASS_GENERAL_INFO 		= 236,
	HEADER_GC_EXT_BATTLE_PASS_MISSION_INFO 		= 237,
	HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE 	= 238,
	HEADER_GC_EXT_BATTLE_PASS_SEND_RANKING 		= 239,
#endif

#ifdef ENABLE_ATLAS_BOSS
	HEADER_GC_BOSS_POSITION						= 240,
#endif

#ifdef ENABLE_EXCHANGE_LOG
	HEADER_GC_EXCHANGE_LOG = 241,
#endif

#ifdef ENABLE_HUNTING_SYSTEM
	HEADER_GC_HUNTING_OPEN_MAIN					= 242,
	HEADER_GC_HUNTING_OPEN_SELECT				= 243,
	HEADER_GC_HUNTING_OPEN_REWARD				= 244,
	HEADER_GC_HUNTING_UPDATE					= 245,
	HEADER_GC_HUNTING_RECIVE_RAND_ITEMS			= 246,
#endif


	/////////////////////////////////////////////////////////////////////////////

	HEADER_GG_LOGIN				= 1,
	HEADER_GG_LOGOUT				= 2,
	HEADER_GG_RELAY				= 3,
	HEADER_GG_NOTICE				= 4,
	HEADER_GG_SHUTDOWN				= 5,
	HEADER_GG_GUILD				= 6,
	HEADER_GG_DISCONNECT			= 7,
	HEADER_GG_SHOUT				= 8,
	HEADER_GG_SETUP				= 9,
	HEADER_GG_MESSENGER_ADD                     = 10,
	HEADER_GG_MESSENGER_REMOVE                  = 11,
	HEADER_GG_FIND_POSITION			= 12,
	HEADER_GG_WARP_CHARACTER			= 13,
	HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX		= 15,
	HEADER_GG_TRANSFER				= 16,
	HEADER_GG_XMAS_WARP_SANTA			= 17,
	HEADER_GG_XMAS_WARP_SANTA_REPLY		= 18,
	HEADER_GG_RELOAD_CRC_LIST			= 19,
	HEADER_GG_LOGIN_PING			= 20,
	HEADER_GG_CHECK_CLIENT_VERSION		= 21,
	HEADER_GG_BLOCK_CHAT			= 22,
#ifdef __WORLD_BOSS_YUMA__
	HEADER_GG_NEW_NOTICE				= 23,
#endif


	HEADER_GG_SIEGE					= 25,
	HEADER_GG_MONARCH_NOTICE		= 26,
	HEADER_GG_MONARCH_TRANSFER		= 27,

	HEADER_GG_CHECK_AWAKENESS		= 29,
#ifdef ENABLE_FULL_NOTICE
	HEADER_GG_BIG_NOTICE			= 30,
#endif
#ifdef ENABLE_SWITCHBOT
	HEADER_GG_SWITCHBOT				= 31,
#endif
#ifdef ENABLE_MULTI_FARM_BLOCK
	HEADER_GG_MULTI_FARM			= 32,
#endif
#ifdef __MULTI_LANGUAGE_SYSTEM__
	HEADER_GG_LOCALE_NOTICE = 34,
#endif
#ifdef __RANKING_SYSTEM__
	HEADER_GG_PLAYER_RANK = 36,
	HEADER_GG_RANK_RESET = 37,
#endif
#ifdef ENABLE_ULTIMATE_REGEN
	HEADER_GG_NEW_REGEN = 40,
#endif

};

#pragma pack(1)
typedef struct SPacketGGSetup
{
	BYTE	bHeader;
	WORD	wPort;
	BYTE	bChannel;
} TPacketGGSetup;

typedef struct SPacketGGLogin
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD	dwPID;
	BYTE	bEmpire;
	long	lMapIndex;
	BYTE	bChannel;
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif
} TPacketGGLogin;

typedef struct SPacketGGLogout
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGLogout;

typedef struct SPacketGGRelay
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lSize;
} TPacketGGRelay;

typedef struct SPacketGGNotice
{
	BYTE	bHeader;
	long	lSize;
} TPacketGGNotice;

#ifdef __WORLD_BOSS_YUMA__
typedef struct SPacketGGNewNotice
{
	BYTE	bHeader;
	long	lSize;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	int		iSecondsToSpawn;
} TPacketGGNewNotice;
#endif

typedef struct SPacketGGMonarchNotice
{
	BYTE	bHeader;
	BYTE	bEmpire;
	long	lSize;
} TPacketGGMonarchNotice;

//FORKED_ROAD
typedef struct SPacketGGForkedMapInfo
{
	BYTE	bHeader;
	BYTE	bPass;
	BYTE	bSungzi;
} TPacketGGForkedMapInfo;
//END_FORKED_ROAD
typedef struct SPacketGGShutdown
{
	BYTE	bHeader;
} TPacketGGShutdown;

typedef struct SPacketGGGuild
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
} TPacketGGGuild;

enum
{
	GUILD_SUBHEADER_GG_CHAT,
	GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS,
};

typedef struct SPacketGGGuildChat
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
	char	szText[CHAT_MAX_LEN + 1];
} TPacketGGGuildChat;

typedef struct SPacketGGParty
{
	BYTE	header;
	BYTE	subheader;
	DWORD	pid;
	DWORD	leaderpid;
} TPacketGGParty;

enum
{
	PARTY_SUBHEADER_GG_CREATE,
	PARTY_SUBHEADER_GG_DESTROY,
	PARTY_SUBHEADER_GG_JOIN,
	PARTY_SUBHEADER_GG_QUIT,
};

typedef struct SPacketGGDisconnect
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGDisconnect;

typedef struct SPacketGGShout
{
	BYTE	bHeader;
	BYTE	bEmpire;
	char	szText[CHAT_MAX_LEN + 1];
} TPacketGGShout;

typedef struct SPacketGGXmasWarpSanta
{
	BYTE	bHeader;
	BYTE	bChannel;
	long	lMapIndex;
} TPacketGGXmasWarpSanta;

typedef struct SPacketGGXmasWarpSantaReply
{
	BYTE	bHeader;
	BYTE	bChannel;
} TPacketGGXmasWarpSantaReply;

typedef struct SPacketGGMessenger
{
	BYTE        bHeader;
	char        szAccount[CHARACTER_NAME_MAX_LEN + 1];
	char        szCompanion[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessenger;

typedef struct SPacketGGFindPosition
{
	BYTE header;
	DWORD dwFromPID;
	DWORD dwTargetPID;
} TPacketGGFindPosition;

typedef struct SPacketGGWarpCharacter
{
	BYTE header;
	DWORD pid;
	long x;
	long y;
#ifdef ENABLE_CMD_WARP_IN_DUNGEON
	int mapIndex;
#endif
} TPacketGGWarpCharacter;

//  HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX	    = 15,

typedef struct SPacketGGGuildWarMapIndex
{
	BYTE bHeader;
	DWORD dwGuildID1;
	DWORD dwGuildID2;
	long lMapIndex;
} TPacketGGGuildWarMapIndex;

typedef struct SPacketGGTransfer
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lX, lY;
} TPacketGGTransfer;

typedef struct SPacketGGLoginPing
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGLoginPing;

typedef struct SPacketGGBlockChat
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lBlockDuration;
} TPacketGGBlockChat;

typedef struct command_text
{
	BYTE	bHeader;
} TPacketCGText;

typedef struct command_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketCGHandshake;

typedef struct command_login
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	DWORD	dwLoginKey;
	DWORD	adwClientKey[4];
	int		NewCheckVersion;
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
#ifdef ENABLE_HWID_BAN
	char	hwid[HWID_MAX_LEN + 1];
#endif
	DWORD	adwClientKey[4];
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif
} TPacketCGLogin3;

#ifdef ENABLE_HWID_BAN
typedef struct SPacketCGHwidBan
{
	BYTE header;
	int mode;
	char szPlayer[24 + 1];
	char szReason[512 + 1];
} TPacketCGHwidBan;
#endif

typedef struct packet_login_key
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
} TPacketGCLoginKey;

typedef struct command_player_select
{
	BYTE	header;
	BYTE	index;
} TPacketCGPlayerSelect;

typedef struct command_player_delete
{
	BYTE	header;
	BYTE	index;
	char	private_code[8];
} TPacketCGPlayerDelete;

typedef struct command_player_create
{
	BYTE        header;
	BYTE        index;
	char        name[CHARACTER_NAME_MAX_LEN + 1];
	WORD        job;
	BYTE	shape;
	BYTE	Con;
	BYTE	Int;
	BYTE	Str;
	BYTE	Dex;
} TPacketCGPlayerCreate;

typedef struct command_player_create_success
{
	BYTE		header;
	BYTE		bAccountCharacterIndex;
	TSimplePlayer	player;
} TPacketGCPlayerCreateSuccess;

typedef struct command_attack
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwVID;
	BYTE	bCRCMagicCubeProcPiece;
	BYTE	bCRCMagicCubeFilePiece;
} TPacketCGAttack;

enum EMoveFuncType
{
	FUNC_WAIT,
	FUNC_MOVE,
	FUNC_ATTACK,
	FUNC_COMBO,
	FUNC_MOB_SKILL,
	_FUNC_SKILL,
	FUNC_MAX_NUM,
	FUNC_SKILL = 0x80,
};

typedef struct command_move
{
	BYTE	bHeader;
	BYTE	bFunc;
	BYTE	bArg;
	BYTE	bRot;
	long	lX;
	long	lY;
	DWORD	dwTime;
} TPacketCGMove;

typedef struct command_sync_position_element
{
	DWORD	dwVID;
	long	lX;
	long	lY;
} TPacketCGSyncPositionElement;

typedef struct command_sync_position
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketCGSyncPosition;

typedef struct command_chat
{
	BYTE	header;
	WORD	size;
	BYTE	type;
} TPacketCGChat;

typedef struct command_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	char 	szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

typedef struct command_entergame
{
	BYTE	header;
} TPacketCGEnterGame;

typedef struct command_item_use
{
	BYTE 	header;
	TItemPos 	Cell;
} TPacketCGItemUse;

typedef struct command_item_use_to_item
{
	BYTE	header;
	TItemPos	Cell;
	TItemPos	TargetCell;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE 	header;
	TItemPos 	Cell;
	DWORD	gold;
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE 	header;
	TItemPos 	Cell;
	DWORD	gold;
	uint16_t	count;
} TPacketCGItemDrop2;

typedef struct command_item_destroy
{
	BYTE header;
	TItemPos Cell;
} TPacketCGItemDestroy;

#ifdef ENABLE_SELL_ITEM
typedef struct command_item_sell
{
    BYTE        header;
    TItemPos    Cell;
	DWORD		gold;
} TPacketCGItemSell;
#endif

typedef struct command_item_move
{
	BYTE 	header;
	TItemPos	Cell;
	TItemPos	CellTo;
	uint16_t	count;
} TPacketCGItemMove;

#ifdef ENABLE_SORT_INVEN
typedef struct sort_inven
{
	BYTE	header;
	BYTE 	option;
} TPacketCGSortInven;

typedef struct sort_special_inven
{
	BYTE	header;
	BYTE 	option;
} TPacketCGSortSpecialInven;
#endif	

typedef struct command_item_pickup
{
	BYTE 	header;
	DWORD	vid;
} TPacketCGItemPickup;

typedef struct command_quickslot_add
{
	BYTE	header;
	BYTE	pos;
	TQuickslot	slot;
} TPacketCGQuickslotAdd;

typedef struct command_quickslot_del
{
	BYTE	header;
	BYTE	pos;
} TPacketCGQuickslotDel;

typedef struct command_quickslot_swap
{
	BYTE	header;
	BYTE	pos;
	BYTE	change_pos;
} TPacketCGQuickslotSwap;

enum
{
	SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2,

#ifdef ENABLE_OFFLINESHOP_SYSTEM
	SHOP_SUBHEADER_CG_ADD_ITEM,
	SHOP_SUBHEADER_CG_REMOVE_ITEM,
	SHOP_SUBHEADER_CG_DESTROY_OFFLINE_SHOP,
	SHOP_SUBHEADER_CG_CHECK,
	SHOP_SUBHEADER_CG_BUTTON,
	SHOP_SUBHEADER_CG_CHANGE_TITLE,
	SHOP_SUBHEADER_CG_TAKE_MONEY,
	SHOP_SUBHEADER_CG_LOG_REMOVE,
	SHOP_SUBHEADER_CG_CHANGE_DECORATION,
	SHOP_SUBHEADER_CG_OPEN_SLOT,
	SHOP_SUBHEADER_CG_GET_ITEM,
	SHOP_SUBHEADER_CG_ADD_TIME,
	SHOP_SUBHEADER_CG_OPEN_WITH_VID
#endif

};

typedef struct command_shop_buy
{
	BYTE	count;
} TPacketCGShopBuy;

typedef struct command_shop_sell
{
	BYTE	pos;
	uint16_t	count;
#ifdef ENABLE_SPECIAL_STORAGE
	BYTE	byType;
#endif
} TPacketCGShopSell;

typedef struct command_shop
{
	BYTE	header;
	BYTE	subheader;

#ifdef ENABLE_OFFLINESHOP_SYSTEM
	char	title[50 + 1];
	DWORD	vid;
	WORD	pos;
#endif

} TPacketCGShop;

typedef struct command_on_click
{
	BYTE	header;
	DWORD	vid;
} TPacketCGOnClick;

enum
{
	EXCHANGE_SUBHEADER_CG_START,	/* arg1 == vid of target character */
	EXCHANGE_SUBHEADER_CG_ITEM_ADD,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ITEM_DEL,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ELK_ADD,	/* arg1 == amount of gold */
	EXCHANGE_SUBHEADER_CG_ACCEPT,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_CG_CANCEL,	/* arg1 == not used */
};

typedef struct command_exchange
{
	BYTE	header;
	BYTE	sub_header;
	int64_t	arg1;
	BYTE	arg2;
	TItemPos	Pos;
} TPacketCGExchange;

typedef struct command_position
{
	BYTE	header;
	BYTE	position;
} TPacketCGPosition;

typedef struct command_script_answer
{
	BYTE	header;
	BYTE	answer;
	//char	file[32 + 1];
	//BYTE	answer[16 + 1];
} TPacketCGScriptAnswer;

typedef struct command_script_button
{
	BYTE        header;
	unsigned int	idx;
} TPacketCGScriptButton;

typedef struct command_quest_input_string
{
	BYTE header;
	char msg[64+1];
} TPacketCGQuestInputString;

typedef struct command_quest_confirm
{
	BYTE header;
	BYTE answer;
	DWORD requestPID;
} TPacketCGQuestConfirm;

typedef struct packet_quest_confirm
{
	BYTE header;
	char msg[64+1];
	long timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketGCHandshake;

enum EPhase
{
	PHASE_CLOSE,
	PHASE_HANDSHAKE,
	PHASE_LOGIN,
	PHASE_SELECT,
	PHASE_LOADING,
	PHASE_GAME,
	PHASE_DEAD,

	PHASE_CLIENT_CONNECTING,
	PHASE_DBCLIENT,
	PHASE_P2P,
	PHASE_AUTH,
};

typedef struct packet_phase
{
	BYTE	header;
	BYTE	phase;
} TPacketGCPhase;

typedef struct packet_bindudp
{
	BYTE	header;
	DWORD	addr;
	WORD	port;
} TPacketGCBindUDP;

enum
{
	LOGIN_FAILURE_ALREADY	= 1,
	LOGIN_FAILURE_ID_NOT_EXIST	= 2,
	LOGIN_FAILURE_WRONG_PASS	= 3,
	LOGIN_FAILURE_FALSE		= 4,
	LOGIN_FAILURE_NOT_TESTOR	= 5,
	LOGIN_FAILURE_NOT_TEST_TIME	= 6,
	LOGIN_FAILURE_FULL		= 7
};

typedef struct packet_login_success
{
	BYTE		bHeader;
	TSimplePlayer	players[PLAYER_PER_ACCOUNT];
	DWORD		guild_id[PLAYER_PER_ACCOUNT];
	char		guild_name[PLAYER_PER_ACCOUNT][GUILD_NAME_MAX_LEN+1];

	DWORD		handle;
	DWORD		random_key;
} TPacketGCLoginSuccess;

typedef struct packet_auth_success
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
	BYTE	bResult;
} TPacketGCAuthSuccess;

typedef struct packet_login_failure
{
	BYTE	header;
	char	szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct packet_create_failure
{
	BYTE	header;
	BYTE	bType;
} TPacketGCCreateFailure;

enum
{
	ADD_CHARACTER_STATE_DEAD		= (1 << 0),
	ADD_CHARACTER_STATE_SPAWN		= (1 << 1),
	ADD_CHARACTER_STATE_GUNGON		= (1 << 2),
	ADD_CHARACTER_STATE_KILLER		= (1 << 3),
	ADD_CHARACTER_STATE_PARTY		= (1 << 4),
};

enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CHR_EQUIPPART_ACCE,
#endif
#ifdef __AURA_SYSTEM__
	CHR_EQUIPPART_AURA,
#endif
	CHR_EQUIPPART_NUM,
};

#ifdef __ITEM_SHINING__
enum EShiningParts
{
	CHR_SHINING_WEAPON_1,
//	CHR_SHINING_WEAPON_2,
//	CHR_SHINING_WEAPON_3,
	CHR_SHINING_ARMOR_1,
//	CHR_SHINING_ARMOR_2,
	CHR_SHINING_SPECIAL,
	CHR_SHINING_NUM,
};
#endif

typedef struct packet_add_char
{
	BYTE	header;
	DWORD	dwVID;

	float	angle;
	long	x;
	long	y;
	long	z;

	BYTE	bType;
	DWORD	wRaceNum; // @fixme501
	WORD	bMovingSpeed;
	BYTE	bAttackSpeed;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];
} TPacketGCCharacterAdd;

typedef struct packet_char_additional_info
{
	BYTE    header;
	DWORD   dwVID;
	char    name[CHARACTER_NAME_MAX_LEN + 1];
	DWORD   awPart[CHR_EQUIPPART_NUM]; // @fixme502
#ifdef __ITEM_SHINING__
	DWORD	adwShining[CHR_SHINING_NUM];
#endif
	BYTE	bEmpire;
	DWORD   dwGuildID;
#ifdef ENABLE_SHOW_GUILD_LEADER
	uint8_t dwNewIsGuildName;
#endif
	DWORD   dwLevel;

#if defined(__CONQUEROR_LEVEL__)
	DWORD dwConquerorLevel;
#endif

	short	sAlignment;
#ifdef ENABLE_TITLE_SYSTEM
	short	sPrestige;
#endif
	BYTE	bPKMode;
	DWORD	dwMountVnum;
#ifdef __SKILL_COLOR_SYSTEM__
	DWORD	dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_update_char
{
	BYTE	header;
	DWORD	dwVID;

	DWORD	awPart[CHR_EQUIPPART_NUM]; // @fixme502
#ifdef __ITEM_SHINING__
	DWORD	adwShining[CHR_SHINING_NUM];
#endif
	WORD	bMovingSpeed;
	BYTE	bAttackSpeed;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];

	DWORD	dwGuildID;
#ifdef ENABLE_SHOW_GUILD_LEADER
	uint8_t dwNewIsGuildName;
#endif
	short	sAlignment;

	DWORD	dwLevel;

#if defined(__CONQUEROR_LEVEL__)
	DWORD dwConquerorLevel;
#endif

#ifdef ENABLE_TITLE_SYSTEM
	short	sPrestige;
#endif
	BYTE	bPKMode;
	DWORD	dwMountVnum;
#ifdef __SKILL_COLOR_SYSTEM__
	DWORD	dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE	header;
	DWORD	id;
} TPacketGCCharacterDelete;

typedef struct packet_chat
{
	BYTE	header;
	WORD	size;
	BYTE	type;
	DWORD	id;
	BYTE	bEmpire;
} TPacketGCChat;

typedef struct packet_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bType;
	char	szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCWhisper;

typedef struct packet_main_character
{
	BYTE        header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter;

// SUPPORT_BGM
typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	float	fBGMVol;
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter4_BGM_VOL;
// END_OF_SUPPORT_BGM

typedef struct packet_points
{
	BYTE	header;
	int64_t		points[POINT_MAX_NUM];
} TPacketGCPoints;

typedef struct packet_skill_level
{
	BYTE		bHeader;
	TPlayerSkill	skills[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct packet_point_change
{
	int		header;
	DWORD	dwVID;
	BYTE	type;
	int64_t	amount;
	int64_t	value;
} TPacketGCPointChange;

typedef struct packet_stun
{
	BYTE	header;
	DWORD	vid;
} TPacketGCStun;

typedef struct packet_dead
{
	BYTE	header;
	DWORD	vid;
} TPacketGCDead;

struct TPacketGCItemDelDeprecated
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
	BYTE	count;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

typedef struct packet_item_set
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
	uint16_t	count;
	DWORD	flags;
	DWORD	anti_flags;
	bool	highlight;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TPacketGCItemSet;

typedef struct packet_item_del
{
	BYTE	header;
	BYTE	pos;
} TPacketGCItemDel;

struct packet_item_use
{
	BYTE	header;
	TItemPos Cell;
	DWORD	ch_vid;
	DWORD	victim_vid;
	DWORD	vnum;
};

struct packet_item_move
{
	BYTE	header;
	TItemPos Cell;
	TItemPos CellTo;
};

typedef struct packet_item_update
{
	BYTE	header;
	TItemPos Cell;
	uint16_t	count;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TPacketGCItemUpdate;

typedef struct packet_item_ground_add
{
	BYTE	bHeader;
	long 	x, y, z;
	DWORD	dwVID;
	DWORD	dwVnum;
} TPacketGCItemGroundAdd;

typedef struct packet_item_ownership
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_item_ground_del
{
	BYTE	bHeader;
	DWORD	dwVID;
} TPacketGCItemGroundDel;

struct packet_quickslot_add
{
	BYTE	header;
	BYTE	pos;
	TQuickslot	slot;
};

struct packet_quickslot_del
{
	BYTE	header;
	BYTE	pos;
};

struct packet_quickslot_swap
{
	BYTE	header;
	BYTE	pos;
	BYTE	pos_to;
};

struct packet_motion
{
	BYTE	header;
	DWORD	vid;
	DWORD	victim_vid;
	WORD	motion;
};

enum EPacketShopSubHeaders
{
	SHOP_SUBHEADER_GC_START,
	SHOP_SUBHEADER_GC_END,
	SHOP_SUBHEADER_GC_UPDATE_ITEM,
	SHOP_SUBHEADER_GC_UPDATE_PRICE,
	SHOP_SUBHEADER_GC_OK,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY,

#ifdef ENABLE_MULTISHOP
	SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM,
#endif

	SHOP_SUBHEADER_GC_SOLDOUT,
	SHOP_SUBHEADER_GC_INVENTORY_FULL,
	SHOP_SUBHEADER_GC_INVALID_POS,
	SHOP_SUBHEADER_GC_SOLD_OUT,
	SHOP_SUBHEADER_GC_START_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX,

#ifdef ENABLE_OFFLINESHOP_SYSTEM
	SHOP_SUBHEADER_GC_REFRESH_MONEY,
	SHOP_SUBHEADER_GC_CHECK_RESULT,
	SHOP_SUBHEADER_GC_REFRESH_DISPLAY_COUNT,
	SHOP_SUBHEADER_GC_REALWATCHER_COUNT,
	SHOP_SUBHEADER_GC_REFRESH_COUNT,
	SHOP_SUBHEADER_GC_CHANGE_TITLE,
#endif

};

struct packet_shop_item
{
	DWORD       vnum;
	int64_t       price;
	uint16_t        count;
#ifdef ENABLE_MULTISHOP
	DWORD		wPriceVnum;
	DWORD		wPrice;
#endif
	BYTE		display_pos;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

typedef struct packet_shop_start
{
	DWORD   owner_vid;
	struct packet_shop_item	items[SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCShopStart;

typedef struct packet_shop_start_ex
{
	typedef struct sub_packet_shop_tab
	{
		char name[SHOP_TAB_NAME_MAX];
		BYTE coin_type;
		packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
	} TSubPacketShopTab;
	DWORD owner_vid;
	BYTE shop_tab_count;
} TPacketGCShopStartEx;

typedef struct packet_shop_update_item
{
	BYTE			pos;
	struct packet_shop_item	item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
	int64_t				iPrice;
} TPacketGCShopUpdatePrice;

typedef struct packet_shop
{
	BYTE        header;
	WORD	size;
	BYTE        subheader;
} TPacketGCShop;

struct packet_exchange
{
	BYTE	header;
	BYTE	sub_header;
	BYTE	is_me;
	int64_t	arg1;	// vnum
	TItemPos	arg2;	// cell
	DWORD	arg3;	// count
#ifdef WJ_ENABLE_TRADABLE_ICON
	TItemPos	arg4;	// srccell
#endif
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

enum EPacketTradeSubHeaders
{
	EXCHANGE_SUBHEADER_GC_START,	/* arg1 == vid */
	EXCHANGE_SUBHEADER_GC_ITEM_ADD,	/* arg1 == vnum  arg2 == pos  arg3 == count */
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,
	EXCHANGE_SUBHEADER_GC_GOLD_ADD,	/* arg1 == gold */
	EXCHANGE_SUBHEADER_GC_ACCEPT,	/* arg1 == accept */
	EXCHANGE_SUBHEADER_GC_END,		/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_ALREADY,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_LESS_GOLD,	/* arg1 == not used */
};

struct packet_position
{
	BYTE	header;
	DWORD	vid;
	BYTE	position;
};

typedef struct packet_ping
{
	BYTE	header;
} TPacketGCPing;

struct packet_script
{
	BYTE	header;
	WORD	size;
	BYTE	skin;
	WORD	src_size;
};

typedef struct packet_change_speed
{
	BYTE		header;
	DWORD		vid;
	WORD		moving_speed;
} TPacketGCChangeSpeed;

struct packet_mount
{
	BYTE	header;
	DWORD	vid;
	DWORD	mount_vid;
	BYTE	pos;
	DWORD	x, y;
};

typedef struct packet_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	DWORD		dwVID;
	long		lX;
	long		lY;
	DWORD		dwTime;
	DWORD		dwDuration;
} TPacketGCMove;

typedef struct packet_ownership
{
	BYTE		bHeader;
	DWORD		dwOwnerVID;
	DWORD		dwVictimVID;
} TPacketGCOwnership;

typedef struct packet_sync_position_element
{
	DWORD	dwVID;
	long	lX;
	long	lY;
} TPacketGCSyncPositionElement;

typedef struct packet_sync_position
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketGCSyncPosition;

typedef struct packet_fly
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwStartVID;
	DWORD	dwEndVID;
} TPacketGCCreateFly;

typedef struct command_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwTargetVID;
	long		x, y;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwShooterVID;
	DWORD		dwTargetVID;
	long		x, y;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE		bHeader;
	BYTE		bType;
} TPacketCGShoot;

typedef struct packet_duel_start
{
	BYTE	header;
	WORD	wSize;
} TPacketGCDuelStart;

enum EPVPModes
{
	PVP_MODE_NONE,
	PVP_MODE_AGREE,
	PVP_MODE_FIGHT,
	PVP_MODE_REVENGE
};

typedef struct packet_pvp
{
	BYTE        bHeader;
	DWORD       dwVIDSrc;
	DWORD       dwVIDDst;
	BYTE        bMode;
} TPacketGCPVP;

typedef struct command_use_skill
{
	BYTE	bHeader;
	DWORD	dwVnum;
	DWORD	dwVID;
} TPacketCGUseSkill;

typedef struct command_target
{
	BYTE	header;
	DWORD	dwVID;
} TPacketCGTarget;

typedef struct packet_target
{
	BYTE	header;
	DWORD	dwVID;
	BYTE	bHPPercent;
#ifdef __VIEW_TARGET_DECIMAL_HP__
	int64_t iMinHP;
	int64_t iMaxHP;
#endif

#if defined(__ELEMENT_ADD__)
	BYTE	bElement[MOB_ELEMENT_MAX_NUM];
#endif

} TPacketGCTarget;

typedef struct packet_warp
{
	BYTE	bHeader;
	long	lX;
	long	lY;
	long	lAddr;
	WORD	wPort;
} TPacketGCWarp;

typedef struct command_warp
{
	BYTE	bHeader;
} TPacketCGWarp;

struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
	BYTE flag;
#if defined(__QUEST_RENEWAL__)
	BYTE type;
	bool is_confirmed;
#endif

};

enum
{
#ifdef ENABLE_MESSENGER_TEAM
	MESSENGER_SUBHEADER_GC_TEAM_LIST,
	MESSENGER_SUBHEADER_GC_TEAM_LOGIN,
	MESSENGER_SUBHEADER_GC_TEAM_LOGOUT,
#endif
	MESSENGER_SUBHEADER_GC_LIST,
	MESSENGER_SUBHEADER_GC_LOGIN,
	MESSENGER_SUBHEADER_GC_LOGOUT,
	MESSENGER_SUBHEADER_GC_INVITE,
};

typedef struct packet_messenger
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCMessenger;

typedef struct packet_messenger_guild_list
{
	BYTE connected;
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildList;

typedef struct packet_messenger_guild_login
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogin;

typedef struct packet_messenger_guild_logout
{
	BYTE length;

	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogout;

typedef struct packet_messenger_list_offline
{
	BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerListOffline;

typedef struct packet_messenger_list_online
{
	BYTE connected; // always 1
	BYTE length;
} TPacketGCMessengerListOnline;

#ifdef ENABLE_MESSENGER_TEAM
typedef struct packet_messenger_team_list_offline
{
	BYTE connected;
	BYTE length;
} TPacketGCMessengerTeamListOffline;

typedef struct packet_messenger_team_list_online
{
	BYTE connected;
	BYTE length;
} TPacketGCMessengerTeamListOnline;
#endif

enum
{
	MESSENGER_SUBHEADER_CG_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE,
	MESSENGER_SUBHEADER_CG_INVITE_ANSWER,
};

typedef struct command_messenger
{
	BYTE header;
	BYTE subheader;
} TPacketCGMessenger;

typedef struct command_messenger_add_by_vid
{
	DWORD vid;
} TPacketCGMessengerAddByVID;

typedef struct command_messenger_add_by_name
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketCGMessengerAddByName;

typedef struct command_messenger_remove
{
	char login[LOGIN_MAX_LEN+1];
	//DWORD account;
} TPacketCGMessengerRemove;

typedef struct command_safebox_checkout
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
} TPacketCGSafeboxCheckin;

///////////////////////////////////////////////////////////////////////////////////
// Party

typedef struct command_party_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketCGPartyParameter;

typedef struct paryt_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketGCPartyParameter;

typedef struct packet_party_add
{
	BYTE	header;
	DWORD	pid;
	char	name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCPartyAdd;

typedef struct command_party_invite
{
	BYTE	header;
	DWORD	vid;
} TPacketCGPartyInvite;

typedef struct packet_party_invite
{
	BYTE	header;
	DWORD	leader_vid;
} TPacketGCPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE	header;
	DWORD	leader_vid;
	BYTE	accept;
} TPacketCGPartyInviteAnswer;

typedef struct packet_party_update
{
	BYTE	header;
	DWORD	pid;
	BYTE	role;
	BYTE	percent_hp;
	short	affects[7];
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketGCPartyRemove;

typedef struct packet_party_link
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyLink;

typedef struct packet_party_unlink
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyUnlink;

typedef struct command_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
	BYTE header;
	DWORD pid;
	BYTE byRole;
	BYTE flag;
} TPacketCGPartySetState;

enum
{
	PARTY_SKILL_HEAL = 1,
	PARTY_SKILL_WARP = 2
};

typedef struct command_party_use_skill
{
	BYTE header;
	BYTE bySkillIndex;
	DWORD vid;
} TPacketCGPartyUseSkill;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketCGSafeboxSize;

typedef struct packet_safebox_wrong_password
{
	BYTE	bHeader;
} TPacketCGSafeboxWrongPassword;

typedef struct command_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketGCEmpire;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE        bHeader;
	BYTE        bState;
	long	lMoney;
} TPacketCGSafeboxMoney;

typedef struct packet_safebox_money_change
{
	BYTE	bHeader;
	long	lMoney;
} TPacketGCSafeboxMoneyChange;

// Guild

enum
{
	GUILD_SUBHEADER_GC_LOGIN,
	GUILD_SUBHEADER_GC_LOGOUT,
	GUILD_SUBHEADER_GC_LIST,
	GUILD_SUBHEADER_GC_GRADE,
	GUILD_SUBHEADER_GC_ADD,
	GUILD_SUBHEADER_GC_REMOVE,
	GUILD_SUBHEADER_GC_GRADE_NAME,
	GUILD_SUBHEADER_GC_GRADE_AUTH,
	GUILD_SUBHEADER_GC_INFO,
	GUILD_SUBHEADER_GC_COMMENTS,
	GUILD_SUBHEADER_GC_CHANGE_EXP,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_GC_SKILL_INFO,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_GC_GUILD_INVITE,
	GUILD_SUBHEADER_GC_WAR,
	GUILD_SUBHEADER_GC_GUILD_NAME,
	GUILD_SUBHEADER_GC_GUILD_WAR_LIST,
	GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST,
	GUILD_SUBHEADER_GC_WAR_SCORE,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
#if defined(ENABLE_GUILD_LAND_INFO)
	GUILD_SUBHEADER_GC_LAND_INFO,
#endif
};

enum GUILD_SUBHEADER_CG
{
	GUILD_SUBHEADER_CG_ADD_MEMBER,
	GUILD_SUBHEADER_CG_REMOVE_MEMBER,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY,
	GUILD_SUBHEADER_CG_OFFER,
	GUILD_SUBHEADER_CG_POST_COMMENT,
	GUILD_SUBHEADER_CG_DELETE_COMMENT,
	GUILD_SUBHEADER_CG_REFRESH_COMMENT,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_CG_USE_SKILL,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER,
	GUILD_SUBHEADER_CG_CHARGE_GSP,
	GUILD_SUBHEADER_CG_DEPOSIT_MONEY,
	GUILD_SUBHEADER_CG_WITHDRAW_MONEY,
};

typedef struct packet_guild
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCGuild;

typedef struct packet_guild_name_t
{
	BYTE header;
	WORD size;
	BYTE subheader;
	DWORD	guildID;
	char	guildName[GUILD_NAME_MAX_LEN];
} TPacketGCGuildName;

typedef struct packet_guild_war
{
	DWORD	dwGuildSelf;
	DWORD	dwGuildOpp;
	BYTE	bType;
	BYTE 	bWarState;
#if defined(ENABLE_MAP_PVP_TACT) && defined(ENABLE_WAR_MAP_PVP_TACT)
	bool	isTactic;
#endif
} TPacketGCGuildWar;

typedef struct command_guild
{
	BYTE header;
	BYTE subheader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN+1];
} TPacketCGAnswerMakeGuild;

typedef struct command_guild_use_skill
{
	DWORD	dwVnum;
	DWORD	dwPID;
} TPacketCGGuildUseSkill;

// Guild Mark
typedef struct command_mark_login
{
	BYTE    header;
	DWORD   handle;
	DWORD   random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
	BYTE	header;
	DWORD	gid;
	BYTE	image[16*12*4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
	BYTE	header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
	BYTE	header;
	BYTE	imgIdx;
	DWORD	crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
	BYTE    header;
	DWORD	bufSize;
	WORD	count;
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
	BYTE	header;
	DWORD	bufSize;
	BYTE	imgIdx;
	DWORD	count;
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE	header;
	WORD	size;
	DWORD	guild_id;
} TPacketCGGuildSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE header;
	DWORD guild_id;
	DWORD crc;
	DWORD size;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

// Fishing

typedef struct command_fishing
{
	BYTE header;
	BYTE dir;
} TPacketCGFishing;

typedef struct packet_fishing
{
	BYTE header;
	BYTE subheader;
	DWORD info;
	BYTE dir;
} TPacketGCFishing;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
	FISHING_SUBHEADER_GC_FISH,
};

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
	BYTE byItemCount;
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
	BYTE	bHeader;
	char	szBuf[255 + 1];
} TPacketCGHack;

// SubHeader - Dungeon
enum
{
	DUNGEON_SUBHEADER_GC_TIME_ATTACK_START = 0,
	DUNGEON_SUBHEADER_GC_DESTINATION_POSITION = 1,
};

typedef struct packet_dungeon
{
	BYTE bHeader;
	WORD size;
	BYTE subheader;
} TPacketGCDungeon;

typedef struct packet_dungeon_dest_position
{
	long x;
	long y;
} TPacketGCDungeonDestPosition;

//typedef struct SPacketGCShopSign
//{
//	BYTE	bHeader;
//	DWORD	dwVID;
//	char	szSign[SHOP_SIGN_MAX_LEN + 1];
//} TPacketGCShopSign;

typedef struct SPacketCGMyShop
{
	BYTE	bHeader;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
	BYTE	bCount;
} TPacketCGMyShop;

typedef struct SPacketGCTime
{
	BYTE	bHeader;
	time_t	time;
} TPacketGCTime;

enum
{
	WALKMODE_RUN,
	WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
	BYTE	header;
	DWORD	vid;
	BYTE	mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
	BYTE        header;
	BYTE        skill_group;
} TPacketGCChangeSkillGroup;

typedef struct SPacketCGRefine
{
	BYTE	header;
	BYTE	pos;
	BYTE	type;
} TPacketCGRefine;

typedef struct SPacketCGRequestRefineInfo
{
	BYTE	header;
	BYTE	pos;
} TPacketCGRequestRefineInfo;

typedef struct SPacketGCRefineInformaion
{
	BYTE	header;
	BYTE	type;
	BYTE	pos;
	DWORD	src_vnum;
	DWORD	result_vnum;
	BYTE	material_count;
	int		cost;
	int		prob;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TPacketGCRefineInformation;

#ifdef ENABLE_DUNGEON_BOSS_ICON_IN_MAP
struct TNPCPosition
{
	BYTE bType;
	DWORD vnum;
	long x;
	long y;
	DWORD time;
};
#else
struct TNPCPosition
{
	BYTE bType;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	long x;
	long y;
};
#endif

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;

	// array of TNPCPosition
} TPacketGCNPCPosition;

typedef struct SPacketGCSpecialEffect
{
	BYTE header;
	BYTE type;
	DWORD vid;
} TPacketGCSpecialEffect;

typedef struct SPacketCGChangeName
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketCGChangeName;

typedef struct SPacketGCChangeName
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCChangeName;

typedef struct command_client_version
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion;

typedef struct command_client_version2
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion2;

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
#ifdef ENABLE_ANTI_EXP
	bool anti_exp;
#endif
} TPacketGCChannel;

#if defined(__BL_KILL_BAR__)
typedef struct command_kill_bar
{
	BYTE	bHeader;
	BYTE	bKillerRace;
	BYTE	bKillerWeaponType;
	BYTE	bVictimRace;
	char	szKiller[CHARACTER_NAME_MAX_LEN + 1];
	char	szVictim[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCKillBar;
#endif

typedef struct SEquipmentItemSet
{
	DWORD   vnum;
	uint16_t    count;
	long    alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TEquipmentItemSet;

typedef struct pakcet_view_equip
{
	BYTE	header;
	DWORD	vid;
	struct {
		DWORD	vnum;
		uint16_t	count;
		long	alSockets[ITEM_SOCKET_MAX_NUM];
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	} equips[23];
} TPacketViewEquip;

typedef struct SLandPacketElement
{
	DWORD	dwID;
	long	x, y;
	long	width, height;
	DWORD	dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
	BYTE	header;
	WORD	size;
} TPacketGCLandList;

typedef struct SPacketGCTargetCreate
{
	BYTE	bHeader;
	long	lID;
	char	szName[32+1];
	DWORD	dwVID;
	BYTE	bType;
} TPacketGCTargetCreate;

typedef struct SPacketGCTargetUpdate
{
	BYTE	bHeader;
	long	lID;
	long	lX, lY;
} TPacketGCTargetUpdate;

typedef struct SPacketGCTargetDelete
{
	BYTE	bHeader;
	long	lID;
} TPacketGCTargetDelete;

typedef struct SPacketGCAffectAdd
{
	BYTE		bHeader;
	TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct SPacketGCAffectRemove
{
	BYTE	bHeader;
	DWORD	dwType;
	BYTE	bApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_lover_info
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE love_point;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE header;
	BYTE love_point;
} TPacketGCLovePointUpdate;

// MINING
typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;
// END_OF_MINING

// SCRIPT_SELECT_ITEM
typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;
// END_OF_SCRIPT_SELECT_ITEM

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int damage;
} TPacketGCDamageInfo;

typedef struct tag_GGSiege
{
	BYTE	bHeader;
	BYTE	bEmpire;
	BYTE	bTowerCount;
} TPacketGGSiege;

typedef struct SPacketGGMonarchTransfer
{
	BYTE	bHeader;
	DWORD	dwTargetPID;
	long	x;
	long	y;
} TPacketMonarchGGTransfer;

typedef struct SPacketGGCheckAwakeness
{
	BYTE bHeader;
} TPacketGGCheckAwakeness;

typedef struct SPacketGCPanamaPack
{
	BYTE	bHeader;
	char	szPackName[256];
	BYTE	abIV[32];
} TPacketGCPanamaPack;

typedef struct SPacketGCHybridCryptKeys
{
	SPacketGCHybridCryptKeys() : m_pStream(NULL) {}
	~SPacketGCHybridCryptKeys()
	{
		if( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + KeyStreamLen;
	}

	BYTE* GetStreamData()
	{
		if( m_pStream )
			delete[] m_pStream;

		uDynamicPacketSize = (WORD)GetStreamSize();

		m_pStream = new BYTE[ uDynamicPacketSize ];

		memcpy( m_pStream, &bHeader, 1 );
		memcpy( m_pStream+1, &uDynamicPacketSize, 2 );
		memcpy( m_pStream+3, &KeyStreamLen, 4 );

		if( KeyStreamLen > 0 )
			memcpy( m_pStream+7, pDataKeyStream, KeyStreamLen );

		return m_pStream;
	}

	BYTE	bHeader;
	WORD    uDynamicPacketSize;
	int		KeyStreamLen;
	BYTE*   pDataKeyStream;

private:
	BYTE* m_pStream;
} TPacketGCHybridCryptKeys;

typedef struct SPacketGCPackageSDB
{
	SPacketGCPackageSDB() : m_pDataSDBStream(NULL), m_pStream(NULL) {}
	~SPacketGCPackageSDB()
	{
		if( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + iStreamLen;
	}

	BYTE* GetStreamData()
	{
		if( m_pStream )
			delete[] m_pStream;

		uDynamicPacketSize =  GetStreamSize();

		m_pStream = new BYTE[ uDynamicPacketSize ];

		memcpy( m_pStream, &bHeader, 1 );
		memcpy( m_pStream+1, &uDynamicPacketSize, 2 );
		memcpy( m_pStream+3, &iStreamLen, 4 );

		if( iStreamLen > 0 )
			memcpy( m_pStream+7, m_pDataSDBStream, iStreamLen );

		return m_pStream;
	}

	BYTE	bHeader;
	WORD    uDynamicPacketSize;
	int		iStreamLen;
	BYTE*   m_pDataSDBStream;

private:
	BYTE* m_pStream;
} TPacketGCPackageSDB;

#ifdef _IMPROVED_PACKET_ENCRYPTION_
struct TPacketKeyAgreement
{
	static const int MAX_DATA_LEN = 256;
	BYTE bHeader;
	WORD wAgreedLength;
	WORD wDataLength;
	BYTE data[MAX_DATA_LEN];
};

struct TPacketKeyAgreementCompleted
{
	BYTE bHeader;
	BYTE data[3]; // dummy (not used)
};

#endif // _IMPROVED_PACKET_ENCRYPTION_

#define MAX_EFFECT_FILE_NAME 128
typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[MAX_EFFECT_FILE_NAME];
} TPacketGCSpecificEffect;

enum EDragonSoulRefineWindowRefineType
{
	DragonSoulRefineWindow_UPGRADE,
	DragonSoulRefineWindow_IMPROVEMENT,
	DragonSoulRefineWindow_REFINE,
};

enum EPacketCGDragonSoulSubHeaderType
{
	DS_SUB_HEADER_OPEN,
	DS_SUB_HEADER_CLOSE,
	DS_SUB_HEADER_DO_REFINE_GRADE,
	DS_SUB_HEADER_DO_REFINE_STEP,
	DS_SUB_HEADER_DO_REFINE_STRENGTH,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
};
typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header (HEADER_CG_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DRAGON_SOUL_REFINE_GRID_SIZE];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SPacketCGStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
} TPacketCGStateCheck;

typedef struct SPacketGCStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
	unsigned char state;
} TPacketGCStateCheck;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
enum
{
	HEADER_CG_ACCE = 211,
	HEADER_GC_ACCE = 215,
	ACCE_SUBHEADER_GC_OPEN = 0,
	ACCE_SUBHEADER_GC_CLOSE,
	ACCE_SUBHEADER_GC_ADDED,
	ACCE_SUBHEADER_GC_REMOVED,
	ACCE_SUBHEADER_CG_REFINED,
	ACCE_SUBHEADER_CG_CLOSE = 0,
	ACCE_SUBHEADER_CG_ADD,
	ACCE_SUBHEADER_CG_REMOVE,
	ACCE_SUBHEADER_CG_REFINE,
};

typedef struct SPacketAcce
{
	BYTE	header;
	BYTE	subheader;
	bool	bWindow;
	uint64_t	dwPrice;
	BYTE	bPos;
	TItemPos	tPos;
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TPacketAcce;
#endif

#ifdef ENABLE_SWITCHBOT
struct TPacketGGSwitchbot
{
	BYTE bHeader;
	WORD wPort;
	TSwitchbotTable table;

	TPacketGGSwitchbot() : bHeader(HEADER_GG_SWITCHBOT), wPort(0)
	{
		table = {};
	}
};

enum ECGSwitchbotSubheader
{
	SUBHEADER_CG_SWITCHBOT_START,
	SUBHEADER_CG_SWITCHBOT_STOP,
};

struct TPacketCGSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

enum EGCSwitchbotSubheader
{
	SUBHEADER_GC_SWITCHBOT_UPDATE,
	SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM,
	SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION,
};

struct TPacketGCSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

struct TSwitchbotUpdateItem
{
	BYTE	slot;
	BYTE	vnum;
	BYTE	count;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};
#endif

#ifdef __SEND_TARGET_INFO__
typedef struct packet_target_info
{
	BYTE	header;
	DWORD	dwVID;
	DWORD	race;
	DWORD	dwVnum;
	BYTE	count;
#ifdef ENABLE_SEND_TARGET_INFO_EXTENDED
	DWORD	rarity;
#endif
} TPacketGCTargetInfo;

typedef struct packet_target_info_load
{
	BYTE header;
	DWORD dwVID;
} TPacketCGTargetInfoLoad;
#endif

#ifdef __SKILL_COLOR_SYSTEM__
typedef struct packet_skill_color
{
	BYTE		bheader;
	BYTE		skill;
	DWORD		col1;
	DWORD		col2;
	DWORD		col3;
	DWORD		col4;
	DWORD		col5;
} TPacketCGSkillColor;
#endif

#ifdef ENABLE_EXTENDED_BATTLE_PASS
typedef struct SPacketCGExtBattlePassAction
{
	BYTE bHeader;
	BYTE bAction;
} TPacketCGExtBattlePassAction;

typedef struct SPacketCGExtBattlePassSendPremiumItem
{
	BYTE bHeader;
	int iSlotIndex;
} TPacketCGExtBattlePassSendPremiumItem;

typedef struct SPacketGCExtBattlePassOpen
{
	BYTE bHeader;
} TPacketGCExtBattlePassOpen;

typedef struct SPacketGCExtBattlePassGeneralInfo
{
	BYTE bHeader;
	BYTE bBattlePassType;
	char	szSeasonName[64+1];
	DWORD dwBattlePassID;
	DWORD dwBattlePassStartTime;
	DWORD dwBattlePassEndTime;
} TPacketGCExtBattlePassGeneralInfo;

typedef struct SPacketGCExtBattlePassMissionInfo
{
	BYTE bHeader;
	WORD wSize;
	WORD wRewardSize;
	BYTE bBattlePassType;
	DWORD dwBattlePassID;
} TPacketGCExtBattlePassMissionInfo;

typedef struct SPacketGCExtBattlePassMissionUpdate
{
	BYTE bHeader;
	BYTE bBattlePassType;
	BYTE bMissionIndex;
	BYTE bMissionType;
	DWORD dwNewProgress;
} TPacketGCExtBattlePassMissionUpdate;

typedef struct SPacketGCExtBattlePassRanking
{
	BYTE bHeader;
	char	szPlayerName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE bBattlePassType;
	BYTE	bBattlePassID;
	DWORD	dwStartTime;
	DWORD	dwEndTime;
} TPacketGCExtBattlePassRanking;
#endif


#ifdef ENABLE_CUBE_RENEWAL_WORLDARD

enum
{
	CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_DATES_LOADING,

	CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM,
	CUBE_RENEWAL_SUB_HEADER_CLOSE,
};

typedef struct  packet_send_cube_renewal
{
	BYTE header;
	BYTE subheader;
	DWORD index_item;
	DWORD count_item;
	DWORD index_item_improve;
}TPacketCGCubeRenewalSend;


typedef struct dates_cube_renewal
{
	DWORD npc_vnum;
	DWORD index;

	DWORD vnum_reward;
	int count_reward;

	bool item_reward_stackable;

	DWORD vnum_material_1;
	int count_material_1;

	DWORD vnum_material_2;
	int count_material_2;

	DWORD vnum_material_3;
	int count_material_3;

	DWORD vnum_material_4;
	int count_material_4;

	DWORD vnum_material_5;
	int count_material_5;

	int gold;
	int percent;

#ifdef ENABLE_GAYA_SYSTEM
	int gaya;
#endif

#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
	DWORD allowCopy;
#endif

	char category[100];
}TInfoDateCubeRenewal;

typedef struct packet_receive_cube_renewal
{
	packet_receive_cube_renewal(): header(HEADER_GC_CUBE_RENEWAL)
	{}

	BYTE header;
	BYTE subheader;
	TInfoDateCubeRenewal date_cube_renewal;
}TPacketGCCubeRenewalReceive;
#endif


#ifdef ENABLE_ENTITY_PRELOADING
/*** HEADER_GC_PRELOAD_ENTITIES ***/
typedef struct packet_preload_entities
{
	uint8_t header;
	uint16_t size;
	uint16_t count;
} TPacketGCPreloadEntities;
#endif


#ifdef ENABLE_OFFLINESHOP_SYSTEM
typedef struct packet_offline_shop_item
{
	DWORD		vnum;
	long long	price;
	int			count;
	BYTE		display_pos;
	long		alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	BYTE		status;
	char		szBuyerName[CHARACTER_NAME_MAX_LEN+1];
#ifdef __CHANGELOOK_SYSTEM__
	DWORD		transmutation;
#endif
	DWORD		owner_id;
} TOfflineShopItem;
typedef struct SPacketGCShopSign
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;
typedef struct SPacketCGMyOfflineShop
{
	BYTE	bHeader;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
	BYTE	bCount;
	DWORD	shopVnum;
	BYTE	shopTitle;
} TPacketCGMyOfflineShop;
typedef struct packet_offline_shop_start
{
	DWORD	owner_vid;
	TOfflineShopItem items[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
	DWORD	m_dwDisplayedCount;
	char			title[SHOP_SIGN_MAX_LEN + 1];
	long long	price;
	DWORD	m_dwRealWatcherCount;
	ShopLog			log[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
	long long		flag;
	int						time;
	DWORD			type;
	bool			IsOwner;
} TPacketGCOfflineShopStart;
typedef struct packet_offline_shop_update_item
{
	BYTE						pos;
	TOfflineShopItem			 item;
	DWORD						m_dwDisplayedCount;
	DWORD			m_dwRealWatcherCount;
	char			title[SHOP_SIGN_MAX_LEN + 1];
	long long	price;
	ShopLog			log;
	long long		flag;
	int					time;
	DWORD			type;
} TPacketGCOfflineShopUpdateItem;
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
typedef struct packet_priv_shop_item_set
{
	BYTE header;
	BYTE count;
#ifdef ENABLE_OFFLINESHOP_SYSTEM
	TOfflineShopItem items[251];
#endif
} TPacketGCShopSearchItemSet;

typedef struct command_shop_search
{
	BYTE	header;
	int		ItemCat;
	BYTE	Value;
	DWORD	itemVnum;
	int		SubCat;
	BYTE	avg[2];
	BYTE	skill[2];
	BYTE	abs[2];
	BYTE	level[2];
	BYTE	refine[2];
	BYTE	sex;
	BYTE	enchant;
	BYTE	alchemy;
	BYTE	character;
} TPacketCGShopSearch;
#endif
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
typedef struct SPacketCGBuffNPCCreate
{
	BYTE bHeader;
	BYTE	bAction;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	bSex;
}TPacketCGBuffNPCCreate;

typedef struct SPacketCGBuffNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
	DWORD	dValue1;
	DWORD	dValue2;
	DWORD	dValue3;
} TPacketCGBuffNPCAction;

typedef struct SPacketGCBuffNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
	DWORD	dValue1;
	DWORD	dValue2;
	DWORD	dValue3;
} TPacketGCBuffNPCAction;

typedef struct SPacketGCBuffNPCUseSkill
{
	BYTE	bHeader;
	DWORD	dSkillVnum;
	DWORD	dVid;
	DWORD	dSkillLevel;
}TPacketGCBuffNPCUseSkill;
#endif

#ifdef ENABLE_ITEMSHOP
typedef struct SPacketGCItemShop
{
	BYTE	header;
	DWORD	size;
} TPacketGCItemShop;
#endif

#ifdef ENABLE_ULTIMATE_REGEN
typedef struct SGGPacketNewRegen
{
	BYTE header;
	BYTE subHeader;
	WORD id;
	bool isAlive;
} TGGPacketNewRegen;
enum
{
	NEW_REGEN_LOAD,
	NEW_REGEN_REFRESH,
};
#endif

#ifdef ENABLE_EVENT_MANAGER
typedef struct SPacketGCEventManager
{
	BYTE	header;
	DWORD	size;
} TPacketGCEventManager;
#endif

#ifdef ENABLE_NEW_DETAILS_GUI
typedef struct SPacketGCKillLOG
{
	BYTE	header;
	long	kill_log[KILL_MAX_NUM];
} TPacketGCKillLOG;
#endif

#ifdef ENABLE_PREMIUM_SYSTEM
typedef struct packet_refresh_premium_packet
{
	BYTE bHeader;
} TPacketGCRefreshPremium;
#endif

typedef struct SItemData
{
	DWORD	vnum;
	BYTE	count;
	DWORD	flags;
	DWORD	anti_flags;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TItemData;

#ifdef __AURA_SYSTEM__
enum EPacketGCAuraSubHeader
{
	AURA_SUBHEADER_GC_OPEN,
	AURA_SUBHEADER_GC_CLOSE,
	AURA_SUBHEADER_GC_SET_ITEM,
	AURA_SUBHEADER_GC_CLEAR_SLOT,
	AURA_SUBHEADER_GC_CLEAR_ALL,
	AURA_SUBHEADER_GC_CLEAR_RIGHT,
	AURA_SUBHEADER_GC_REFINE_INFO,
};

typedef struct SSubPacketGCAuraOpenClose
{
	BYTE	bAuraWindowType;
} TSubPacketGCAuraOpenClose;

typedef struct SSubPacketGCAuraSetItem
{
	TItemPos	Cell;
	TItemPos	AuraCell;
	TItemData	pItem;
} TSubPacketGCAuraSetItem;

typedef struct SSubPacketGCAuraClearSlot
{
	BYTE	bAuraSlotPos;
} TSubPacketGCAuraClearSlot;

typedef struct SSubPacketGCAuraRefineInfo
{
	BYTE	bAuraRefineInfoType;
	BYTE	bAuraRefineInfoLevel;
	BYTE	bAuraRefineInfoExpPercent;
} TSubPacketGCAuraRefineInfo;

enum EPacketCGAuraSubHeader
{
	AURA_SUBHEADER_CG_REFINE_CHECKIN,
	AURA_SUBHEADER_CG_REFINE_CHECKOUT,
	AURA_SUBHEADER_CG_REFINE_ACCEPT,
	AURA_SUBHEADER_CG_REFINE_CANCEL,
};

typedef struct SSubPacketCGAuraRefineCheckIn
{
	TItemPos	ItemCell;
	TItemPos	AuraCell;
	BYTE		byAuraRefineWindowType;
} TSubPacketCGAuraRefineCheckIn;

typedef struct SSubPacketCGAuraRefineCheckOut
{
	TItemPos	AuraCell;
	BYTE		byAuraRefineWindowType;
} TSubPacketCGAuraRefineCheckOut;

typedef struct SSubPacketCGAuraRefineAccept
{
	BYTE		byAuraRefineWindowType;
} TSubPacketCGAuraRefineAccept;

typedef struct SPacketGCAura
{
	SPacketGCAura() : bHeader(HEADER_GC_AURA) {}
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCAura;

typedef struct SPacketCGAura
{
	SPacketCGAura() : bHeader(HEADER_CG_AURA) {}
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketCGAura;
#endif

#ifdef ENABLE_SHOW_CHEST_DROP_SYSTEM
typedef struct SPacketCGChestDropInfo
{
	BYTE	header;
	WORD	wInventoryCell;
} TPacketCGChestDropInfo;

typedef struct SChestDropInfoTable
{
	BYTE	bPageIndex;
	BYTE	bSlotIndex;
	DWORD	dwItemVnum;
	BYTE	bItemCount;
} TChestDropInfoTable;

typedef struct SPacketGCChestDropInfo
{
	BYTE	bHeader;
	WORD	wSize;
	DWORD	dwChestVnum;
} TPacketGCChestDropInfo;
#endif

#ifdef __MAINTENANCE__
typedef struct SPacketGCMaintenance
{
	BYTE header;
	TMainteance maintenance;
}TPacketGCMaintenance;
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
enum
{
	MULTI_FARM_SET,
	MULTI_FARM_REMOVE,
};
typedef struct SPacketGGMultiFarm
{
	BYTE	header;
	DWORD	size;
	BYTE	subHeader;
	char	playerIP[IP_ADDRESS_LENGTH + 1];
	DWORD	playerID;
	char	playerName[CHARACTER_NAME_MAX_LEN+1];
	bool	farmStatus;
	BYTE	affectType;
	int		affectTime;
} TPacketGGMultiFarm;
#endif

#ifdef __MULTI_LANGUAGE_SYSTEM__
typedef struct SPacketGGLocaleNotice
{
	BYTE bHeader;
	bool bBigFont;
	char szNotice[MAX_QUEST_NOTICE_ARGS + 1][CHAT_MAX_LEN + 1];
} TPacketGGLocaleNotice;

typedef struct SPacketChangeLanguage
{
	BYTE bHeader;
	BYTE bLanguage;
} TPacketChangeLanguage;
#endif

#ifdef __EXTENDED_WHISPER_DETAILS__
typedef struct SPacketCGWhisperDetails
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisperDetails;

typedef struct SPacketGCWhisperDetails
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif
} TPacketGCWhisperDetails;
#endif

#ifdef ENABLE_ATLAS_BOSS
typedef struct SPacketGCBossPosition
{
	BYTE	bHeader;
	WORD	wSize;
	WORD	wCount;
} TPacketGCBossPosition;

struct TBossPosition
{
	BYTE	bType;
#ifdef ENABLE_MULTI_NAMES
	DWORD	szName;
#else
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
#endif
	long	lX;
	long	lY;
	long	lTime;
};
#endif

#ifdef ENABLE_HUNTING_SYSTEM
typedef struct SPacketCGHuntingAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue;
} TPacketGCHuntingAction;

typedef struct SPacketCGOpenWindowHuntingMain
{
	BYTE	bHeader;
	DWORD	dLevel;
	DWORD	dMonster;
	DWORD	dCurCount;
	DWORD	dDestCount;
	DWORD	dMoneyMin;
	DWORD	dMoneyMax;
	DWORD	dExpMin;
	DWORD	dExpMax;
	DWORD	dRaceItem;
	DWORD	dRaceItemCount;
} TPacketGCOpenWindowHuntingMain;

typedef struct SPacketCGOpenWindowHuntingSelect
{
	BYTE	bHeader;
	DWORD	dLevel;
	BYTE	bType;
	DWORD	dMonster;
	DWORD	dCount;
	DWORD	dMoneyMin;
	DWORD	dMoneyMax;
	DWORD	dExpMin;
	DWORD	dExpMax;
	DWORD	dRaceItem;
	DWORD	dRaceItemCount;
} TPacketGCOpenWindowHuntingSelect;

typedef struct SPacketGCOpenWindowReward
{
	BYTE	bHeader;
	DWORD	dLevel;
	DWORD	dReward;
	DWORD	dRewardCount;
	DWORD	dRandomReward;
	DWORD	dRandomRewardCount;
	DWORD	dMoney;
	BYTE	bExp;
} TPacketGCOpenWindowReward;

typedef struct SPacketGCUpdateHunting
{
	BYTE	bHeader;
	DWORD	dCount;
} TPacketGCUpdateHunting;

typedef struct SPacketGCReciveRandomItems
{
	BYTE	bHeader;
	BYTE	bWindow;
	DWORD	dItemVnum;
	DWORD	dItemCount;
} TPacketGCReciveRandomItems;
#endif

#ifdef ENABLE_EXCHANGE_LOG
typedef struct SPacketGCExchangeLog
{
	BYTE	header;
	DWORD	size;
} TPacketGCExchangeLog;
enum
{
	SUB_EXCHANGELOG_LOAD,
	SUB_EXCHANGELOG_LOAD_ITEM,
};
#endif

#ifdef u1x
typedef struct SPacketRank
{
	BYTE	header;
	TRanking	m_ranking_l[10];
	TRanking	m_ranking_d_s[10];
	TRanking	m_ranking_k_m[10];
	TRanking	m_ranking_k_b[10];
	TRanking	m_ranking_c_d[10];
	TRanking	m_ranking_p[10];
	TRanking	m_ranking_y[10];
	TRanking	m_ranking_m_o[10];
	TRanking	m_ranking_c_f[10];
	TRanking	m_ranking_o_c[10];
} TPacketRank;
#endif

#ifdef __RANKING_SYSTEM__
typedef struct SPacketGGPlayerRank
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	bEmpire;
	BYTE	bRace;
	long long lValue;
	int		iMode;
	DWORD	dwPid;
	char	szAccount[LOGIN_MAX_LEN + 1];
	long	lSize;
} TPacketGGPlayerRank;

typedef struct SGCRankInfo
{
	BYTE	bHeader;
	char 	szName[CHARACTER_NAME_MAX_LEN + 1];
	int		iMode, iPos, bEmpire, bRace, iMyPos;
	long long	lValue;
} TPacketGCRankInfo;
#endif


#if defined(__SKILLBOOK_COMB_SYSTEM__)
typedef struct SPacketCGSkillBookCombination
{
	BYTE bHeader;
	BYTE bAction;
	TItemPos CombItemGrid[SKILLBOOK_COMB_SLOT_MAX];
} TPacketCGSkillBookCombination;
#endif


#pragma pack()
#endif
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
