#include "stdafx.h"
#include "utils.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "wedding.h"

#if defined(ENABLE_ASLAN_BUFF_NPC_SYSTEM) && defined(ENABLE_ASLAN_BUFF_EMOTION)
#include "buff_npc_system.h"
#endif

#define NEED_TARGET	(1 << 0)
#define NEED_PC		(1 << 1)
#define WOMAN_ONLY	(1 << 2)
#define OTHER_SEX_ONLY	(1 << 3)
#define SELF_DISARM	(1 << 4)
#define TARGET_DISARM	(1 << 5)
#define BOTH_DISARM	(SELF_DISARM | TARGET_DISARM)

struct emotion_type_s
{
	const char *	command;
	const char *	command_to_client;
	long	flag;
	float	extra_delay;
} emotion_types[] = {
	{ "키스",	"french_kiss",	NEED_PC | OTHER_SEX_ONLY | BOTH_DISARM,		2.0f },
	{ "뽀뽀",	"kiss",		NEED_PC | OTHER_SEX_ONLY | BOTH_DISARM,		1.5f },
	{ "따귀",	"slap",		NEED_PC | SELF_DISARM,				1.5f },
	{ "박수",	"clap",		0,						1.0f },
	{ "와",		"cheer1",	0,						1.0f },
	{ "만세",	"cheer2",	0,						1.0f },

	// DANCE
	{ "댄스1",	"dance1",	0,						1.0f },
	{ "댄스2",	"dance2",	0,						1.0f },
	{ "댄스3",	"dance3",	0,						1.0f },
	{ "댄스4",	"dance4",	0,						1.0f },
	{ "댄스5",	"dance5",	0,						1.0f },
	{ "댄스6",	"dance6",	0,						1.0f },
	// END_OF_DANCE
	{ "축하",	"congratulation",	0,				1.0f	},
	{ "용서",	"forgive",			0,				1.0f	},
	{ "화남",	"angry",			0,				1.0f	},
	{ "유혹",	"attractive",		0,				1.0f	},
	{ "슬픔",	"sad",				0,				1.0f	},
	{ "브끄",	"shy",				0,				1.0f	},
	{ "응원",	"cheerup",			0,				1.0f	},
	{ "질투",	"banter",			0,				1.0f	},
	{ "기쁨",	"joy",				0,				1.0f	},
#if defined(__EXPRESSING_EMOTIONS__)
	{ "dance7", "dance7", 0, 12.0f },
	{ "doze", "doze", 0, 4.666667f },
	{ "exercise", "exercise", 0, 6.333333f },
	{ "pushup", "pushup", 0, 6.666667f },
	{ "selfie", "selfie", 0, 3.000000f },
#endif
	{ "\n",	"\n",		0,						0.0f },
};

std::set<std::pair<DWORD, DWORD> > s_emotion_set;

ACMD(do_emotion_allow)
{
	if ( ch->GetArena() )
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD	val = 0; str_to_number(val, arg1);
	s_emotion_set.emplace(ch->GetVID(), val);
}

#ifdef ENABLE_NEWSTUFF
#include "config.h"
#endif

bool CHARACTER_CanEmotion(CHARACTER& rch)
{
#ifdef ENABLE_NEWSTUFF
	if (g_bDisableEmotionMask)
		return true;
#endif

	if (marriage::WeddingManager::instance().IsWeddingMap(rch.GetMapIndex()))
		return true;

	if (rch.IsEquipUniqueItem(UNIQUE_ITEM_EMOTION_MASK))
		return true;

	if (rch.IsEquipUniqueItem(UNIQUE_ITEM_EMOTION_MASK2))
		return true;

	return false;
}

ACMD(do_emotion)
{
	int i;
	{
		if (ch->IsRiding())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 탄 상태에서 감정표현을 할 수 없습니다."));
			return;
		}
	}

	for (i = 0; *emotion_types[i].command != '\n'; ++i)
	{
		if (!strcmp(cmd_info[cmd].command, emotion_types[i].command))
			break;

		if (!strcmp(cmd_info[cmd].command, emotion_types[i].command_to_client))
			break;
	}

	if (*emotion_types[i].command == '\n')
	{
		sys_err("cannot find emotion");
		return;
	}

	if (!CHARACTER_CanEmotion(*ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("열정의 가면을 착용시에만 할 수 있습니다."));
		return;
	}

	if (IS_SET(emotion_types[i].flag, WOMAN_ONLY) && SEX_MALE==GET_SEX(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("여자만 할 수 있습니다."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	LPCHARACTER victim = NULL;

	if (*arg1)
		victim = ch->FindCharacterInView(arg1, IS_SET(emotion_types[i].flag, NEED_PC));

	if (IS_SET(emotion_types[i].flag, NEED_TARGET | NEED_PC))
	{
		if (!victim)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("그런 사람이 없습니다."));
			return;
		}
	}

	if (victim)
	{
		if (
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
(
#endif
!victim->IsPC()
#if defined(ENABLE_ASLAN_BUFF_NPC_SYSTEM) && defined(ENABLE_ASLAN_BUFF_EMOTION)
			&& !victim->IsBuffNPC())
#endif
		|| victim == ch
		)
			return;

		if (victim->IsRiding())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 탄 상대와 감정표현을 할 수 없습니다."));
			return;
		}

		long distance = DISTANCE_APPROX(ch->GetX() - victim->GetX(), ch->GetY() - victim->GetY());

		if (distance < 10)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("너무 가까이 있습니다."));
			return;
		}

		if (distance > 500)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("너무 멀리 있습니다"));
			return;
		}

		if (IS_SET(emotion_types[i].flag, OTHER_SEX_ONLY))
		{
			if (GET_SEX(ch)==GET_SEX(victim))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이성간에만 할 수 있습니다."));
				return;
			}
		}

		if (IS_SET(emotion_types[i].flag, NEED_PC))
		{
			if (s_emotion_set.find(std::make_pair(victim->GetVID(), ch->GetVID())) == s_emotion_set.end())
			{
				if (true == marriage::CManager::instance().IsMarried( ch->GetPlayerID() ))
				{
					const marriage::TMarriage* marriageInfo = marriage::CManager::instance().Get( ch->GetPlayerID() );

					const DWORD other = marriageInfo->GetOther( ch->GetPlayerID() );

					if (0 == other || other != victim->GetPlayerID())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 행동은 상호동의 하에 가능 합니다."));
						return;
					}
				}
				else
				{
#if defined(ENABLE_ASLAN_BUFF_NPC_SYSTEM) && defined(ENABLE_ASLAN_BUFF_EMOTION)
					if (!(victim->IsBuffNPC() && ch->GetBuffNPCSystem() && victim ==  ch->GetBuffNPCSystem()->GetCharacter()))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "392");
						return;
					}
					if (victim->IsBuffNPC() && ch->GetBuffNPCSystem() && victim ==  ch->GetBuffNPCSystem()->GetCharacter())
					{
						if (ch->GetBuffNPCSystem()->GetSex() == GET_SEX(ch))
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "391");
							return;
						}
					}
#else
					ch->ChatPacket(CHAT_TYPE_INFO, "392");
					return;
#endif
				}
			}

			s_emotion_set.emplace(ch->GetVID(), victim->GetVID());
		}
	}

	char chatbuf[256+1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s %u %u",
			emotion_types[i].command_to_client,
			(DWORD) ch->GetVID(), victim ? (DWORD) victim->GetVID() : 0);

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	++len;

	TPacketGCChat pack_chat;
	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = CHAT_TYPE_COMMAND;
	pack_chat.id = 0;
	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	ch->PacketAround(buf.read_peek(), buf.size());

	if (victim)
		sys_log(1, "ACTION: %s TO %s", emotion_types[i].command, victim->GetName());
	else
		sys_log(1, "ACTION: %s", emotion_types[i].command);
}
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
