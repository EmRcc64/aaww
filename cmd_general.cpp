#include "stdafx.h"
#ifdef __FreeBSD__
#include <md5.h>
#else
#include "../../libthecore/include/xmd5.h"
#endif
#ifdef ENABLE_CHANNEL_SWITCHER
	#include "map_location.h"
	#include "cmd.h"
	#include "../../common/CommonDefines.h"
	int channel_index;
#endif
#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "item.h"
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "threeway_war.h"
#include "log.h"
#include "../../common/VnumHelper.h"
#include "shop_manager.h"
#ifdef ENABLE_TITLE_SYSTEM	
	#include "title.h"
#endif
#ifdef ENABLE_PREMIUM_SYSTEM
	#include "premium_system.h"
#endif

#ifdef __MAINTENANCE__
#include "maintenance.h"
#endif

#ifdef __WORLD_BOSS_YUMA__
#include "worldboss.h"
#endif
#ifdef __RANKING_SYSTEM__
#include "RankPlayer.h"
#endif

#ifdef PET_ATTACK
	#include "PetSystem.h"
#endif
DWORD shopvid;
int shopvnum;
struct NPCBul
	{
		NPCBul() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;
				if (ch->IsNPC())
				{
					shopvid = ch->GetVID();
				}
			}
		}
	};

ACMD(do_user_horse_ride)
{
#ifdef __RENEWAL_MOUNT__
	if (ch->MountBlockMap())
		return;
	time_t now = get_global_time();
	if (ch->GetProtectTime("mount.ride") > now)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d"), ch->GetProtectTime("mount.ride") - now);
		return;
	}
	ch->SetProtectTime("mount.ride", now + 2);
#endif

	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 탈것을 이용중입니다."));
			return;
		}

		if (ch->GetHorse() == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 먼저 소환해주세요."));
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}


#ifdef __PROMO_CODE__
ACMD(do_promo_code)
{
    char arg1[33]; // 32+1 for null, limit to code size
    one_argument(argument, arg1, sizeof(arg1));

    if (!*arg1)
        return;

    if (strpbrk(arg1, "'\";\\"))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("promo sqli msg"));
        return;
    }

    if (!ch->CanActPromoCode())
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("promo wait a few seconds"));
        return;
    }

    ch->SetActPromoCode();

    // Escape promo code for SQL
    char promo_escaped[65];
    DBManager::instance().EscapeString(promo_escaped, sizeof(promo_escaped), arg1, strlen(arg1));

    DBManager::instance().ReturnQuery(QID_CHECK_PROMO, ch->GetPlayerID(), promo_escaped,
        "SELECT `promo_id`, `code`, `item_vnum`, `item_count`, `receiver` FROM `promo_table` WHERE `code` = '%s'", promo_escaped);
}
#endif



#ifdef ENABLE_TELEPORT_WINDOW
#include "teleport_window.h"
ACMD(do_teleportwindowcall)
{
	if(ch == NULL)
		return;
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, "teleportwindowcall");
}

ACMD(do_teleportwindow)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	int index = 0;
	
	if(ch == NULL)
		return;
	
	if(!*arg1 || !isdigit(*arg1))
		return;
	
	str_to_number(index, arg1);
	
	
	CTeleportWindow::instance().Process(ch, index);
}
#endif


ACMD(do_user_horse_back)
{
#ifdef __RENEWAL_MOUNT__
	if (ch->MountBlockMap())
		return;
#endif

	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 돌려보냈습니다."));
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말에서 먼저 내려야 합니다."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 먼저 소환해주세요."));
	}
}

ACMD(do_user_horse_feed)
{
#ifdef __RENEWAL_MOUNT__
	if (ch->MountBlockMap())
		return;
#endif

	if (ch->GetMyShop())
		return;

	if (!ch->GetHorse())
	{
		if (ch->IsHorseRiding() == false)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 먼저 소환해주세요."));
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 탄 상태에서는 먹이를 줄 수 없습니다."));
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;
	BYTE bLocale = ch->GetLanguage();

	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말에게 %s%s 주었습니다."),
			LC_ITEM_NAME(dwFood, bLocale),
			g_iUseLocale ? "" : under_han(LC_ITEM_NAME(dwFood, bLocale)) ? LC_TEXT("을") : LC_TEXT("를"));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 아이템이 필요합니다"), LC_ITEM_NAME(dwFood, bLocale));
	}
}

#define MAX_REASON_LEN		128

#ifdef ENABLE_CHANGE_CHANNEL
EVENTINFO(ChangeChannelEventInfo)
{
	DynamicCharacterPtr ch;
	int				channel_number;
	int         	left_second;

	ChangeChannelEventInfo()
	: ch()
	, channel_number( 0 )
	, left_second( 0 )
	{
	}
};
#endif

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo()
	: ch()
	, subcmd( 0 )
	, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

struct SendDisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetCharacter())
		{
			if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
	: seconds( 0 )
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int * pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
			return 0;

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
		char buf[64];
		snprintf(buf, sizeof(buf), "Server shut down in %d seconds.", *pSec);
		SendNotice(buf);

		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	char buf[64];
	snprintf(buf, sizeof(buf), "Server will close in %d seconds.", iSec);

	SendNotice(buf);

	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

#ifdef ENABLE_SPECIAL_STORAGE
//upgrade
ACMD (do_sort_special_storage_upp)
{
    if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || 
        ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->IsAcceOpened(true) || 
        ch->IsAcceOpened(false))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
        return;
    }

    int lastSortInventoryPulse = ch->GetSortSpecialStoragePulse();
    if (lastSortInventoryPulse > get_global_time())
    {
        int remainingTime = (lastSortInventoryPulse - get_global_time());
        ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", remainingTime);
        return;
    }

    ch->SetNextSortSpecialStoragePulse(get_global_time() + 15);

    std::unordered_map<int, LPITEM> stackableItems;

    for (int i = 0; i < SPECIAL_INVENTORY_MAX_NUM; ++i)
    {
        LPITEM item = ch->GetUpgradeInventoryItem(i);
        if (!item || item->isLocked() || item->GetCount() == ITEM_MAX_COUNT)
            continue;

        if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
        {
            int vnum = item->GetVnum();
            if (stackableItems.find(vnum) == stackableItems.end())
            {
                stackableItems[vnum] = item;
            }
            else
            {
                LPITEM existingItem = stackableItems[vnum];
                bool matchingSockets = true;
                
                for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k)
                {
                    if (existingItem->GetSocket(k) != item->GetSocket(k))
                    {
                        matchingSockets = false;
                        break;
                    }
                }
                
                if (matchingSockets)
                {
                    BYTE bAddCount = MIN(ITEM_MAX_COUNT - existingItem->GetCount(), item->GetCount());
                    existingItem->SetCount(existingItem->GetCount() + bAddCount);
                    item->SetCount(item->GetCount() - bAddCount);
                }
            }
        }
    }
}

//books
ACMD (do_sort_special_storage_book)
{
    if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || 
        ch->GetShopOwner() || ch->IsOpenSafebox() || 
        ch->IsCubeOpen() || ch->IsAcceOpened(true) || 
        ch->IsAcceOpened(false))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
        return;
    }

    int lastSortInventoryPulse = ch->GetSortSpecialStoragePulse();
    if (lastSortInventoryPulse > get_global_time())
    {
        int remainingTime = (lastSortInventoryPulse - get_global_time());
        ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", remainingTime);
        return;
    }

    ch->SetNextSortSpecialStoragePulse(get_global_time() + 15);

    std::unordered_map<int, LPITEM> stackableItems;

    for (int i = 0; i < SPECIAL_INVENTORY_MAX_NUM; ++i)
    {
        LPITEM item = ch->GetBookInventoryItem(i);
        if (!item || item->isLocked() || item->GetCount() == ITEM_MAX_COUNT)
            continue;

        if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
        {
            int vnum = item->GetVnum();
            if (stackableItems.find(vnum) == stackableItems.end())
            {
                stackableItems[vnum] = item;
            }
            else
            {
                LPITEM existingItem = stackableItems[vnum];
                bool matchingSockets = true;

                for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k)
                {
                    if (existingItem->GetSocket(k) != item->GetSocket(k))
                    {
                        matchingSockets = false;
                        break;
                    }
                }

                if (matchingSockets)
                {
                    BYTE bAddCount = MIN(ITEM_MAX_COUNT - existingItem->GetCount(), item->GetCount());
                    existingItem->SetCount(existingItem->GetCount() + bAddCount);
                    item->SetCount(item->GetCount() - bAddCount);
                }
            }
        }
    }
}

//stones
ACMD (do_sort_special_storage_stone)
{
    if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || 
        ch->GetShopOwner() || ch->IsOpenSafebox() || 
        ch->IsCubeOpen() || ch->IsAcceOpened(true) || 
        ch->IsAcceOpened(false))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
        return;
    }

    int lastSortInventoryPulse = ch->GetSortSpecialStoragePulse();
    if (lastSortInventoryPulse > get_global_time())
    {
        int remainingTime = (lastSortInventoryPulse - get_global_time());
        ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", remainingTime);
        return;
    }

    ch->SetNextSortSpecialStoragePulse(get_global_time() + 15);

    std::unordered_map<int, LPITEM> stackableItems;

    for (int i = 0; i < SPECIAL_INVENTORY_MAX_NUM; ++i)
    {
        LPITEM item = ch->GetStoneInventoryItem(i);
        if (!item || item->isLocked() || item->GetCount() == ITEM_MAX_COUNT)
            continue;

        if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
        {
            int vnum = item->GetVnum();
            if (stackableItems.find(vnum) == stackableItems.end())
            {
                stackableItems[vnum] = item;
            }
            else
            {
                LPITEM existingItem = stackableItems[vnum];
                bool matchingSockets = true;

                for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k)
                {
                    if (existingItem->GetSocket(k) != item->GetSocket(k))
                    {
                        matchingSockets = false;
                        break;
                    }
                }

                if (matchingSockets)
                {
                    BYTE bAddCount = MIN(ITEM_MAX_COUNT - existingItem->GetCount(), item->GetCount());
                    existingItem->SetCount(existingItem->GetCount() + bAddCount);
                    item->SetCount(item->GetCount() - bAddCount);
                }
            }
        }
    }
}

//chests
ACMD (do_sort_special_storage_chest)
{
    if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || 
        ch->GetShopOwner() || ch->IsOpenSafebox() || 
        ch->IsCubeOpen() || ch->IsAcceOpened(true) || 
        ch->IsAcceOpened(false))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
        return;
    }

    int lastSortInventoryPulse = ch->GetSortSpecialStoragePulse();
    if (lastSortInventoryPulse > get_global_time())
    {
        int remainingTime = (lastSortInventoryPulse - get_global_time());
        ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", remainingTime);
        return;
    }

    ch->SetNextSortSpecialStoragePulse(get_global_time() + 15);

    std::unordered_map<int, LPITEM> stackableItems;

    for (int i = 0; i < SPECIAL_INVENTORY_MAX_NUM; ++i)
    {
        LPITEM item = ch->GetChestInventoryItem(i);
        if (!item || item->isLocked() || item->GetCount() == ITEM_MAX_COUNT)
            continue;

        if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
        {
            int vnum = item->GetVnum();
            if (stackableItems.find(vnum) == stackableItems.end())
            {
                stackableItems[vnum] = item;
            }
            else
            {
                LPITEM existingItem = stackableItems[vnum];
                bool matchingSockets = true;

                for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k)
                {
                    if (existingItem->GetSocket(k) != item->GetSocket(k))
                    {
                        matchingSockets = false;
                        break;
                    }
                }

                if (matchingSockets)
                {
                    BYTE bAddCount = MIN(ITEM_MAX_COUNT - existingItem->GetCount(), item->GetCount());
                    existingItem->SetCount(existingItem->GetCount() + bAddCount);
                    item->SetCount(item->GetCount() - bAddCount);
                }
            }
        }
    }
}



/////////////////////////////////////old for all
ACMD (do_sort_special_storage)
{
	if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->IsAcceOpened(true) || ch->IsAcceOpened(false))
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
        return;
	}
    int lastSortInventoryPulse = ch->GetSortSpecialStoragePulse();

    if (lastSortInventoryPulse > get_global_time()) {
        int remainingTime = (lastSortInventoryPulse - get_global_time());
        ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", remainingTime);
        return;
    }
    ch->SetNextSortSpecialStoragePulse(get_global_time() + 15);
	for (int m = 0; m < 4; ++m)
	{
		for (int i = 0; i < SPECIAL_INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item;

            switch(m)
            {
                case 0:
                    item = ch->GetUpgradeInventoryItem(i);
                    break;
                case 1:
                    item = ch->GetBookInventoryItem(i);
                    break;
                case 2:
                    item = ch->GetStoneInventoryItem(i);
                    break;
                default:
                    item = ch->GetChestInventoryItem(i);
                    break;
            }
           
            if(!item)
                continue;
           
            if(item->isLocked())
                continue;

			if (item->isLocked())
				continue;

			if (item->GetCount() == ITEM_MAX_COUNT)
				continue;

			if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
			{
				for (int j = i; j < SPECIAL_INVENTORY_MAX_NUM; ++j)
				{
					
					LPITEM item2;
					
					switch (m)
					{
						case 0:
							item2 = ch->GetUpgradeInventoryItem(j);
							break;
						case 1:
							item2 = ch->GetBookInventoryItem(j);
							break;
						case 2:
							item2 = ch->GetStoneInventoryItem(j);
							break;
						default:
							item2 = ch->GetChestInventoryItem(j);
							break;
					}

					if (!item2)
						continue;

					if (item2->isLocked())
						continue;

					if (item2->GetVnum() == item->GetVnum())
					{
						bool bStopSockets = false;

						for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k)
						{
							if (item2->GetSocket(k) != item->GetSocket(k))
							{
								bStopSockets = true;
								break;
							}
						}

						if (bStopSockets)
							continue;

						// #ifdef ENABLE_NEW_MAX_COUNT
						// WORD bAddCount = MIN(ITEM_MAX_COUNT - item->GetCount(), item2->GetCount());
						// #else
						BYTE bAddCount = MIN(ITEM_MAX_COUNT - item->GetCount(), item2->GetCount());
						// #endif

						item->SetCount(item->GetCount() + bAddCount);
						item2->SetCount(item2->GetCount() - bAddCount);

						continue;
					}
				}
			}
		}
	}
}///////////////////////////////old for all(commented)

ACMD(do_sort_items)
{
    if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || 
        ch->GetShopOwner() || ch->IsOpenSafebox() || 
        ch->IsCubeOpen() || ch->IsAcceOpened(true) || 
        ch->IsAcceOpened(false))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
        return;
    }

    int lastSortInventoryPulse = ch->GetSortInventoryPulse();
    if (lastSortInventoryPulse > get_global_time()) 
    {
        int remainingTime = (lastSortInventoryPulse - get_global_time());
        ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", remainingTime);
        return;
    }
    ch->SetNextSortInventoryPulse(get_global_time() + 15);

    std::unordered_map<int, LPITEM> stackableItems;

    for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
    {
        LPITEM item = ch->GetInventoryItem(i);
        
        // Check for null, locked, or full count items
        if (!item || item->isLocked() || item->GetCount() == ITEM_MAX_COUNT)
            continue;

        if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
        {
            int vnum = item->GetVnum();
            if (stackableItems.find(vnum) == stackableItems.end())
            {
                stackableItems[vnum] = item;
            }
            else
            {
                LPITEM existingItem = stackableItems[vnum];
                bool matchingSockets = true;

                for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k)
                {
                    if (existingItem->GetSocket(k) != item->GetSocket(k))
                    {
                        matchingSockets = false;
                        break;
                    }
                }

                if (matchingSockets)
                {
                    BYTE bAddCount = MIN(ITEM_MAX_COUNT - existingItem->GetCount(), item->GetCount());
                    existingItem->SetCount(existingItem->GetCount() + bAddCount);
                    item->SetCount(item->GetCount() - bAddCount);
                }
            }
        }
    }
}
#endif

ACMD(do_shutdown)
{
	sys_err("Accept shutdown command from %s.", (ch) ? ch->GetName() : "NONAME");

	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

	Shutdown(10);
}

#ifdef ENABLE_TITLE_SYSTEM	
ACMD(do_prestige_title)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "your argument is invalid");
		return;
	}
	
	if (!strcmp(arg1, "prestige_0")){			
		TitleManager::instance().SetTitle(ch, "disable");}
	
	if (!strcmp(arg1, "prestige_1")){		
		TitleManager::instance().SetTitle(ch, "title1");}
	
	if (!strcmp(arg1, "prestige_2")){		
		TitleManager::instance().SetTitle(ch, "title2");}	
	
	if (!strcmp(arg1, "prestige_3")){		
		TitleManager::instance().SetTitle(ch, "title3");}
	
	if (!strcmp(arg1, "prestige_4")){		
		TitleManager::instance().SetTitle(ch, "title4");}	
	
	if (!strcmp(arg1, "prestige_5")){		
		TitleManager::instance().SetTitle(ch, "title5");}	
	
	if (!strcmp(arg1, "prestige_6")){		
		TitleManager::instance().SetTitle(ch, "title6");}		
	
	if (!strcmp(arg1, "prestige_7")){		
		TitleManager::instance().SetTitle(ch, "title7");}	
	
	if (!strcmp(arg1, "prestige_8")){		
		TitleManager::instance().SetTitle(ch, "title8");}		
	
	if (!strcmp(arg1, "prestige_9")){		
		TitleManager::instance().SetTitle(ch, "title9");}	
	
	if (!strcmp(arg1, "prestige_10")){		
		TitleManager::instance().SetTitle(ch, "title10");}	
	
	if (!strcmp(arg1, "prestige_11")){		
		TitleManager::instance().SetTitle(ch, "title11");}	
	
	if (!strcmp(arg1, "prestige_12")){		
		TitleManager::instance().SetTitle(ch, "title12");}	
	
	if (!strcmp(arg1, "prestige_13")){		
		TitleManager::instance().SetTitle(ch, "title13");}	
	
	if (!strcmp(arg1, "prestige_14")){		
		TitleManager::instance().SetTitle(ch, "title14");}	
	
	if (!strcmp(arg1, "prestige_15")){		
		TitleManager::instance().SetTitle(ch, "title15");}
	
	if (!strcmp(arg1, "prestige_16")){		
		TitleManager::instance().SetTitle(ch, "title16");}	
	
	if (!strcmp(arg1, "prestige_17")){		
		TitleManager::instance().SetTitle(ch, "title17");}	
	
	if (!strcmp(arg1, "prestige_18")){		
		TitleManager::instance().SetTitle(ch, "title18");}	
	
	if (!strcmp(arg1, "prestige_19")){		
		TitleManager::instance().SetTitle(ch, "title19");}	

	if (!strcmp(arg1, "prestige_20")){		
		TitleManager::instance().SetTitle(ch, "title20");}	

	if (!strcmp(arg1, "prestige_21")){		
		TitleManager::instance().SetTitle(ch, "title21");}	

	if (!strcmp(arg1, "prestige_22")){		
		TitleManager::instance().SetTitle(ch, "title22");}	

	if (!strcmp(arg1, "buy_prestige_1")){		
		TitleManager::instance().BuyPotion(ch, "buy_potion_1");}	

	if (!strcmp(arg1, "buy_prestige_2")){		
		TitleManager::instance().BuyPotion(ch, "buy_potion_2");}		

	if (!strcmp(arg1, "buy_prestige_3")){		
		TitleManager::instance().BuyPotion(ch, "buy_potion_3");}	

	if (!strcmp(arg1, "vegas_transform_title")){		
		TitleManager::instance().TransformTitle(ch);}			
}
#endif

#ifdef ENABLE_CHANGE_CHANNEL
EVENTFUNC(change_channel_event)
{
	ChangeChannelEventInfo * info = dynamic_cast<ChangeChannelEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "change_channel_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (info->left_second <= 0)
	{
		ch->m_pkChangeChannelEvent = NULL;

		ch->ChangeChannel(info->channel_number);
	
		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Change channel in %d seconds."), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}
#endif


EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}
	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
			case SCMD_QUIT:
			case SCMD_PHASE_SELECT:
#ifdef ENABLE_CHANNEL_SWITCHER
			case SCMD_SWITCH_CHANNEL:
#endif
				{
					TPacketNeedLoginLogInfo acc_info;
					acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;
#ifdef __MULTI_LANGUAGE_SYSTEM__
					acc_info.bLanguage = ch->GetDesc()->GetAccountTable().bLanguage;
#endif
					db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );

					LogManager::instance().DetailLoginLog( false, ch );
				}
				break;
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

#ifdef ENABLE_CHANNEL_SWITCHER
			case SCMD_SWITCH_CHANNEL:
				ch->SwitchChannel(channel_index);
				break;
#endif

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				if (d) // @fixme197
					d->DelayedDisconnect(1);
				break;

			case SCMD_PHASE_SELECT:
				ch->Disconnect("timed_event - SCMD_PHASE_SELECT");
				if (d)
					d->SetPhase(PHASE_SELECT);
				break;
		}

		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d초 남았습니다."), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_cmd)
{
	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("취소 되었습니다."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

#ifdef __MAINTENANCE__
	if (CMaintenanceManager::Instance().GetGameMode()== GAME_MODE_LOBBY && !ch->IsGM())
		return;
#endif

	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("로그인 화면으로 돌아 갑니다. 잠시만 기다리세요."));
			break;

		case SCMD_QUIT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("게임을 종료 합니다. 잠시만 기다리세요."));
			break;

		case SCMD_PHASE_SELECT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("캐릭터를 전환 합니다. 잠시만 기다리세요."));
			break;

#ifdef ENABLE_CHANNEL_SWITCHER
		case SCMD_SWITCH_CHANNEL:
			char arg1[256];
			one_argument(argument, arg1, sizeof(arg1));
			if (!*arg1)
				return;

			int new_ch;
			str_to_number(new_ch, arg1);
			if (new_ch < 1 || new_ch > SWITCHER_CHANNELS)
				return;
			if (!ch->IsPC())
				return;
			channel_index = new_ch;
			break;
#endif

	}

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) &&
		false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()) &&
	   	(!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
			{
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

				{
					if (ch->IsPosition(POS_FIGHTING))
						info->left_second = 10;
					else
						info->left_second = 3;
				}

				info->ch		= ch;
				info->subcmd		= subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
			}
			break;
#ifdef ENABLE_CHANNEL_SWITCHER
			case SCMD_SWITCH_CHANNEL:
			{
				int lMapArray[] = { 66, 113, 207 };

				long lAddr, lMapIndex;
				WORD wPort;

				long x = ch->GetX();
				long y = ch->GetY();

				if (!CMapLocation::Instance().Get(x, y, lMapIndex, lAddr, wPort))
				{
					sys_err("Error with map location index[%ld] x[%ld] y[%ld] name[%s]", lMapIndex, x, y, ch->GetName());
					return;
				}

				if (lMapIndex >= 10000)
				{
					sys_err("Map index higher or equal to 10000");
					return;
				}

				for (unsigned int i = 0; i < _countof(lMapArray); i++)
				{
					if (lMapIndex == lMapArray[i])
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("channelswitch_banned_location"));
						return;
					}
				}

				std::map<WORD, BYTE> ChannelsPorts;
				for(int i = 1; i <= SWITCHER_CHANNELS; i++)
				{
					for(int i2 = 1; i2 <= SWITCHER_CORES; i2++)
					{ 
						ChannelsPorts[26*1000 + i*100 + i2] = i;
					}		
				}

				int iChannel = ChannelsPorts.find(wPort) != ChannelsPorts.end() ? ChannelsPorts[wPort] : 0;

				if (iChannel == 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dchannelswitch_port_not_avaiable"), wPort);
					return;
				}
				
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

				{
					if (ch->IsPosition(POS_FIGHTING))
						info->left_second = 10;
					else
						info->left_second = 3;
				}

				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("channelswitch_switching"));

				info->ch = ch;
				info->subcmd = subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent = event_create(timed_event, info, 1);
			}
			break;
#endif			
	}
}

ACMD(do_mount)
{
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

#ifdef __WORLD_BOSS_YUMA__
ACMD(do_boss_debug)
{
	CWorldBossManager::instance().GetWorldbossInfo(ch);
}
#endif

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

	if (NULL == ch->m_pkDeadEvent)
		return;

	int iTimeToDead = (event_time(ch->m_pkDeadEvent) / passes_per_sec);

#ifdef ENABLE_DEATHMATCH_SYSTEM
	if (quest::CQuestManager::instance().GetEventFlag("deathmatch_event") > DEATHMATCH_NOT_STARTED && ch->GetMapIndex() == DEATHMATCH_MAP_INDEX && subcmd == SCMD_RESTART_HERE)
	{
		if (iTimeToDead > 170)
			return;
		
		ch->GetDesc()->SetPhase(PHASE_GAME);
		ch->SetPosition(POS_STANDING);
		ch->RestartAtSamePos();
		ch->PointChange(POINT_HP, ch->GetMaxHP());
		ch->PointChange(POINT_SP, ch->GetMaxSP());
		return;
	}
#endif

	if (subcmd != SCMD_RESTART_TOWN)// && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (ch->IsHack())
			{
				if (false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 재시작 할 수 없습니다. (%d초 남음)"), iTimeToDead - (180 - g_nPortalLimitTime));
					return;
				}
			}
#define eFRS_HERESEC	174
			if (iTimeToDead > eFRS_HERESEC)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 재시작 할 수 없습니다. (%d초 남음)"), iTimeToDead - eFRS_HERESEC);
				return;
			}
		}
	}

	//PREVENT_HACK

	if (subcmd == SCMD_RESTART_TOWN)
	{
		if (ch->IsHack())
		{
			if ((!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) ||
			   	false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 재시작 할 수 없습니다. (%d초 남음)"), iTimeToDead - (180 - g_nPortalLimitTime));
				return;
			}
		}

#define eFRS_TOWNSEC	173
		if (iTimeToDead > eFRS_TOWNSEC)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 마을에서 재시작 할 수 없습니다. (%d 초 남음)"), iTimeToDead - eFRS_TOWNSEC);
			return;
		}
	}
	//END_PREVENT_HACK

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);
	ch->StartRecoveryEvent();

	//FORKED_LOAD

	if (1 == quest::CQuestManager::instance().GetEventFlag("threeway_war"))
	{
		if (subcmd == SCMD_RESTART_TOWN || subcmd == SCMD_RESTART_HERE)
		{
			if (true == CThreeWayWar::instance().IsThreeWayWarMapIndex(ch->GetMapIndex()) &&
					false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

				ch->ReviveInvisible(5);
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

				return;
			}

			if (true == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				if (CThreeWayWar::instance().GetReviveTokenForPlayer(ch->GetPlayerID()) <= 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성지에서 부활 기회를 모두 잃었습니다! 마을로 이동합니다!"));
					ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
				}
				else
				{
					ch->Show(ch->GetMapIndex(), GetSungziStartX(ch->GetEmpire()), GetSungziStartY(ch->GetEmpire()));
				}

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(5);

				return;
			}
		}
	}
	//END_FORKED_LOAD

	if (ch->GetDungeon())
		ch->GetDungeon()->UseRevive(ch);

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;

		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
				case SCMD_RESTART_TOWN:
					sys_log(0, "do_restart: restart town");
					PIXEL_POSITION pos;

					if (CWarMapManager::instance().GetStartPosition(ch->GetMapIndex(), ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						ch->Show(ch->GetMapIndex(), pos.x, pos.y);
					else
						ch->ExitToSavedLocation();

					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
					break;

				case SCMD_RESTART_HERE:
					sys_log(0, "do_restart: restart here");
					ch->RestartAtSamePos();
					//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
					break;
			}

			return;
		}
	}
	switch (subcmd)
	{
		case SCMD_RESTART_TOWN:
			sys_log(0, "do_restart: restart town");
			PIXEL_POSITION pos;

			if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
				ch->WarpSet(pos.x, pos.y);
			else
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
			ch->PointChange(POINT_HP, ch->GetMaxHP()/5 - ch->GetHP());
			ch->DeathPenalty(1);
			break;

		case SCMD_RESTART_HERE:
			sys_log(0, "do_restart: restart here");
			ch->RestartAtSamePos();
			//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
			ch->PointChange(POINT_HP, ch->GetMaxHP()/5 - ch->GetHP());
			ch->PointChange(POINT_SP, ch->GetMaxSP()/5 - ch->GetSP());
			ch->DeathPenalty(0);
			ch->ReviveInvisible(5);
			break;
	}
}

#define MAX_STAT g_iStatusPointSetMaxValue

ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 능력을 올릴 수 없습니다."));
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
			return;

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
			return;

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
			return;

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
			return;

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
		return;

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 능력을 올릴 수 없습니다."));
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	// ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT "%s GRP(%d) idx(%u), MAX_STAT(%d), expr(%d)", __FUNCTION__, ch->GetRealPoint(idx), idx, MAX_STAT, ch->GetRealPoint(idx) >= MAX_STAT);
	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -1);
	ch->ComputePoints();
}

#if defined(__CONQUEROR_LEVEL__)
ACMD(do_conqueror_point)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 능력을 올릴 수 없습니다."));
		return;
	}

	if (ch->GetPoint(POINT_CONQUEROR_POINT) <= 0)
		return;

	BYTE idx = 0;
	if (!strcmp(arg1, "sstr"))
		idx = POINT_SUNGMA_STR;
	else if (!strcmp(arg1, "shp"))
		idx = POINT_SUNGMA_HP;
	else if (!strcmp(arg1, "smove"))
		idx = POINT_SUNGMA_MOVE;
	else if (!strcmp(arg1, "simmune"))
		idx = POINT_SUNGMA_IMMUNE;
	else
		return;

	if (ch->GetRealPoint(idx) >= gPlayerMaxLevelStats)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	ch->PointChange(POINT_CONQUEROR_POINT, -1);
	ch->ComputePoints();

	ch->PointsPacket(); // Refresh points.
}
#endif

ACMD(do_pvp)
{
	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

#ifdef ENABLE_DEATHMATCH_SYSTEM
	if (ch->GetMapIndex() == DEATHMATCH_MAP_INDEX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't duel with %s because you are in deathmatch map."), pkVictim->GetName());
		return;
	}
#endif

	if (pkVictim->GetArena() != NULL)
	{
		pkVictim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 대련중입니다."));
		return;
	}

	CPVPManager::instance().Insert(ch, pkVictim);
}

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드에 속해있지 않습니다."));
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드 스킬 레벨을 변경할 권한이 없습니다."));
	}
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch(vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:

			case SKILL_7_A_ANTI_TANHWAN:
			case SKILL_7_B_ANTI_AMSEOP:
			case SKILL_7_C_ANTI_SWAERYUNG:
			case SKILL_7_D_ANTI_YONGBI:

			case SKILL_8_A_ANTI_GIGONGCHAM:
			case SKILL_8_B_ANTI_YEONSA:
			case SKILL_8_C_ANTI_MAHWAN:
			case SKILL_8_D_ANTI_BYEURAK:

#if defined(__NEW_PASSIVE_SKILLS__)
			case SKILL_PASSIVE_MONSTERS:
			case SKILL_PASSIVE_HP:
			case SKILL_PASSIVE_RHH:
			case SKILL_PASSIVE_HH:
			case SKILL_PASSIVE_RESIS_CRIT:
			case SKILL_PASSIVE_RESIS_PERF:
			case SKILL_PASSIVE_DROP:
#endif

			case SKILL_ADD_HP:
			case SKILL_RESIST_PENETRATE:
				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

//
//
ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

//
//
ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	if (!*arg2 || strlen(arg2)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 창고가 이미 열려있습니다."));
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 창고를 닫은지 10초 안에는 열 수 없습니다."));
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 던전 안에서는 파티에서 나갈 수 없습니다."));
		return;
	}

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티에서 나가셨습니다."));
		//pParty->SendPartyRemoveOneToAll(ch);
		pParty->Quit(ch->GetPlayerID());
		//pParty->SendPartyRemoveAllToOne(ch);
	}
}

ACMD(do_close_shop)
{
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

ACMD(do_war)
{
	CGuild * g = ch->GetGuild();

	if (!g)
		return;

	if (g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 이미 다른 전쟁에 참전 중 입니다."));
		return;
	}

	char arg1[256], arg2[256];
#if defined(ENABLE_MAP_PVP_TACT) && defined(ENABLE_WAR_MAP_PVP_TACT)
	char arg3[256];
#endif
	DWORD type = GUILD_WAR_TYPE_FIELD; //fixme102 base int modded uint
#if defined(ENABLE_MAP_PVP_TACT) && defined(ENABLE_WAR_MAP_PVP_TACT)
	const char *line;
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
#else
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
#endif
#if defined(ENABLE_MAP_PVP_TACT) && defined(ENABLE_WAR_MAP_PVP_TACT)
	bool isTactic = false;
	one_argument(line, arg3, sizeof(arg3));
#endif
	if (!*arg1)
		return;

	if (*arg2)
	{
		str_to_number(type, arg2);

		if (type >= GUILD_WAR_TYPE_MAX_NUM)
			type = GUILD_WAR_TYPE_FIELD;
	}

#if defined(ENABLE_MAP_PVP_TACT) && defined(ENABLE_WAR_MAP_PVP_TACT)
	if (*arg3)
		str_to_number(isTactic, arg3);
	
	if (type == GUILD_WAR_TYPE_FIELD && isTactic)
		return;
#endif



#ifdef ENABLE_COLEADER_WAR_PRIVILEGES

	DWORD leader_pid = g->GetMasterPID();
	DWORD cl_grades = g->GetMember(ch->GetPlayerID())->grade;
	bool bIsLeaderOnline = g->IsOnlineLeader();

	if (bIsLeaderOnline) {
		if ((leader_pid != ch->GetPlayerID()) && !(cl_grades <= 2)) {
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드전에 대한 권한이 없습니다."));
			return;
		}
		else {
			if (cl_grades == 2) {
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Can't Do, because Leader is Online!"));
				return;
			}
		}
	}
	else {
		if ((leader_pid != ch->GetPlayerID()) && !(cl_grades <= 2)) {
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드전에 대한 권한이 없습니다."));
			return;
		}
		else {
			if (cl_grades == 2)
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You grant rights of War, because Leader offline!"));
		}
	}
#else
	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드전에 대한 권한이 없습니다."));
		return;
	}
#endif
	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 그런 길드가 없습니다."));
		return;
	}

	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드가 이미 전쟁 중 입니다."));
					return;
				}

				int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

				if (g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 전비가 부족하여 길드전을 할 수 없습니다."));
					return;
				}

				if (opp_g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 전비가 부족하여 길드전을 할 수 없습니다."));
					return;
				}
			}
			break;

		case GUILD_WAR_SEND_DECLARE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 선전포고 중인 길드입니다."));
				return;
			}
			break;

		case GUILD_WAR_RECV_DECLARE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드가 이미 전쟁 중 입니다."));
					g->RequestRefuseWar(opp_g->GetID());
					return;
				}
			}
			break;

		case GUILD_WAR_RESERVE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 이미 전쟁이 예약된 길드 입니다."));
				return;
			}
			break;

		case GUILD_WAR_END:
			return;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 이미 전쟁 중인 길드입니다."));
			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		if (g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 레더 점수가 모자라서 길드전을 할 수 없습니다."));
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드전을 하기 위해선 최소한 %d명이 있어야 합니다."), GUILD_WAR_MIN_MEMBER_COUNT);
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
		if (opp_g->GetLadderPoint() == 0)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 레더 점수가 모자라서 길드전을 할 수 없습니다."));
		else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 길드원 수가 부족하여 길드전을 할 수 없습니다."));
		return;
	}

	do
	{

#ifdef ENABLE_COLEADER_WAR_PRIVILEGES
		if (bIsLeaderOnline) {
			if (g->GetMasterCharacter() != nullptr)
				break;
		}
		else {
			if (g->GetMember(ch->GetPlayerID())->grade == 2)
				break;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 길드장이 접속중이 아닙니다."));
#else
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 길드장이 접속중이 아닙니다."));
#endif

		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{


#ifdef ENABLE_COLEADER_WAR_PRIVILEGES
		bool boppIsLeaderOnline = opp_g->IsOnlineLeader();

		if (boppIsLeaderOnline) {
			if (opp_g->GetMasterCharacter() != nullptr)
				break;
		}
		else {
			if (opp_g->GetMember(ch->GetPlayerID())->grade == 2)
				break;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 길드장이 접속중이 아닙니다."));
#else
		if (opp_g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 길드장이 접속중이 아닙니다."));
#endif


		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type
#if defined(ENABLE_MAP_PVP_TACT) && defined(ENABLE_WAR_MAP_PVP_TACT)
	, isTactic
#endif
	);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드전에 대한 권한이 없습니다."));
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 그런 길드가 없습니다."));
		return;
	}

	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
		return;

#ifdef ENABLE_DEATHMATCH_SYSTEM
	if (ch->GetMapIndex() == DEATHMATCH_MAP_INDEX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't change pk mode in deathmatch map"));
		return;
	}
#endif

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	char answer = LOWER(*arg1);
	// @fixme130 AuthToAdd void -> bool
	bool bIsDenied = answer != 'y';
	bool bIsAdded = MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, bIsDenied); // DENY
	if (bIsAdded && bIsDenied)
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);

		if (tch)
			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 님으로 부터 친구 등록을 거부 당했습니다."), ch->GetName());
	}
}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{
	if (true == ch->UnEquipSpecialRideUniqueItem())
	{
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		if (ch->IsHorseRiding())
		{
			ch->StopRiding();
		}
	}
	else
	{
		ch->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("인벤토리가 꽉 차서 내릴 수 없습니다."));
	}
}

ACMD(do_observer_exit)
{
	if (ch->IsObserverMode())
	{
		if (ch->GetWarMap())
			ch->SetWarMap(NULL);

#ifdef WARP_VILLAGE_TO_END_OBSERVER
		if (quest::CQuestManager::instance().GetEventFlag("return_vill_observer_"+std::to_string(ch->GetMapIndex())))
		{
			BYTE bEmpire = ch->GetEmpire();
			ch->WarpSet( g_start_position[bEmpire][0], g_start_position[bEmpire][1] );
			ch->SetObserverMode(false);
			return;
		}
#endif

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else
		{
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
}

ACMD(do_view_equip)
{

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vid = 0;
		str_to_number(vid, arg1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

		if (!tch)
			return;

		if (!tch->IsPC())
			return;

		tch->SendEquipment(ch);
	}
}

ACMD(do_party_request)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

#ifdef ENABLE_DEATHMATCH_SYSTEM
	if (ch->GetMapIndex() == DEATHMATCH_MAP_INDEX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't invite in deathmatch map"));
		return;
	}
#endif

	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 파티에 속해 있으므로 가입신청을 할 수 없습니다."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

ACMD(do_monarch_warpto)
{
	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주만이 사용 가능한 기능입니다"));
		return;
	}

	if (!ch->IsMCOK(CHARACTER::MI_WARP))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 초간 쿨타임이 적용중입니다."), ch->GetMCLTime(CHARACTER::MI_WARP));
		return;
	}

	const int WarpPrice = 10000;

	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("국고에 돈이 부족합니다. 현재 : %u 필요금액 : %u"), NationMoney, WarpPrice);
		return;
	}

	int x = 0, y = 0;
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용법: warpto <character name>"));
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("타제국 유저에게는 이동할수 없습니다"));
				return;
			}

			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("해당 유저는 %d 채널에 있습니다. (현재 채널 %d)"), pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 이동할 수 없습니다."));
				return;
			}

			PIXEL_POSITION pos;

			if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(pkCCI->lMapIndex, pos))
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map (index %d)", pkCCI->lMapIndex);
			else
			{
				//ch->ChatPacket(CHAT_TYPE_INFO, "You warp to (%d, %d)", pos.x, pos.y);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 에게로 이동합니다"), arg1);
				ch->WarpSet(pos.x, pos.y);

				CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

				ch->SetMC(CHARACTER::MI_WARP);
			}
		}
		else if (NULL == CHARACTER_MANAGER::instance().FindPC(arg1))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "There is no one by that name");
		}

		return;
	}
	else
	{
		if (tch->GetEmpire() != ch->GetEmpire())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("타제국 유저에게는 이동할수 없습니다"));
			return;
		}
		if (!IsMonarchWarpZone(tch->GetMapIndex()))
		{
			ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 이동할 수 없습니다."));
			return;
		}
		x = tch->GetX();
		y = tch->GetY();
	}

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 에게로 이동합니다"), arg1);
	ch->WarpSet(x, y);
	ch->Stop();

	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	ch->SetMC(CHARACTER::MI_WARP);
}

ACMD(do_monarch_transfer)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용법: transfer <name>"));
		return;
	}

	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주만이 사용 가능한 기능입니다"));
		return;
	}

	if (!ch->IsMCOK(CHARACTER::MI_TRANSFER))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 초간 쿨타임이 적용중입니다."), ch->GetMCLTime(CHARACTER::MI_TRANSFER));
		return;
	}

	const int WarpPrice = 10000;

	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("국고에 돈이 부족합니다. 현재 : %u 필요금액 : %u"), NationMoney, WarpPrice);
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 제국 유저는 소환할 수 없습니다."));
				return;
			}
			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 님은 %d 채널에 접속 중 입니다. (현재 채널: %d)"), arg1, pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 이동할 수 없습니다."));
				return;
			}
			if (!IsMonarchWarpZone(ch->GetMapIndex()))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 소환할 수 없습니다."));
				return;
			}

			TPacketGGTransfer pgg;

			pgg.bHeader = HEADER_GG_TRANSFER;
			strlcpy(pgg.szName, arg1, sizeof(pgg.szName));
			pgg.lX = ch->GetX();
			pgg.lY = ch->GetY();

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGTransfer));
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 님을 소환하였습니다."), arg1);

			CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

			ch->SetMC(CHARACTER::MI_TRANSFER);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("입력하신 이름을 가진 사용자가 없습니다."));
		}

		return;
	}

	if (ch == tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("자신을 소환할 수 없습니다."));
		return;
	}

	if (tch->GetEmpire() != ch->GetEmpire())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 제국 유저는 소환할 수 없습니다."));
		return;
	}
	if (!IsMonarchWarpZone(tch->GetMapIndex()))
	{
		ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 이동할 수 없습니다."));
		return;
	}
	if (!IsMonarchWarpZone(ch->GetMapIndex()))
	{
		ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 소환할 수 없습니다."));
		return;
	}

	//tch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
	tch->WarpSet(ch->GetX(), ch->GetY(), ch->GetMapIndex());

	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	ch->SetMC(CHARACTER::MI_TRANSFER);
}

ACMD(do_monarch_info)
{
	if (CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("나의 군주 정보"));
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			if (n == ch->GetEmpire())
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s군주] : %s  보유금액 %lld "), EMPIRE_NAME(n), p->name[n], p->money[n]);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s군주] : %s  "), EMPIRE_NAME(n), p->name[n]);

		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주 정보"));
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s군주] : %s  "), EMPIRE_NAME(n), p->name[n]);

		}
	}
}

ACMD(do_elect)
{
	db_clientdesc->DBPacketHeader(HEADER_GD_COME_TO_VOTE, ch->GetDesc()->GetHandle(), 0);
}

// LUA_ADD_GOTO_INFO
struct GotoInfo
{
	std::string 	st_name;

	BYTE 	empire;
	int 	mapIndex;
	DWORD 	x, y;

	GotoInfo()
	{
		st_name 	= "";
		empire 		= 0;
		mapIndex 	= 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name 	= c_src.st_name;
		empire 		= c_src.empire;
		mapIndex 	= c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

ACMD(do_monarch_tax)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: monarch_tax <1-50>");
		return;
	}

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주만이 사용할수 있는 기능입니다"));
		return;
	}

	int tax = 0;
	str_to_number(tax,  arg1);

	if (tax < 1 || tax > 50)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1-50 사이의 수치를 선택해주세요"));

	quest::CQuestManager::instance().SetEventFlag("trade_tax", tax);

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("세금이 %d %로 설정되었습니다"));

	char szMsg[1024];

	snprintf(szMsg, sizeof(szMsg), "군주의 명으로 세금이 %d %% 로 변경되었습니다", tax);
	BroadcastNotice(szMsg);

	snprintf(szMsg, sizeof(szMsg), "앞으로는 거래 금액의 %d %% 가 국고로 들어가게됩니다.", tax);
	BroadcastNotice(szMsg);

	ch->SetMC(CHARACTER::MI_TAX);
}

static const DWORD cs_dwMonarchMobVnums[] =
{
	191,
	192,
	193,
	194,
	391,
	392,
	393,
	394,
	491,
	492,
	493,
	494,
	591,
	691,
	791,
	1304,
	1901,
	2091,
	2191,
	2206,
	0,
};

ACMD(do_monarch_mob)
{
	char arg1[256];
	LPCHARACTER	tch;

	one_argument(argument, arg1, sizeof(arg1));

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주만이 사용할수 있는 기능입니다"));
		return;
	}

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mmob <mob name>");
		return;
	}

#ifdef ENABLE_MONARCH_MOB_CMD_MAP_CHECK // @warme006
	BYTE pcEmpire = ch->GetEmpire();
	BYTE mapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(ch->GetMapIndex());
	if (mapEmpire != pcEmpire && mapEmpire != 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("자국 영토에서만 사용할 수 있는 기능입니다"));
		return;
	}
#endif

	const int SummonPrice = 5000000;

	if (!ch->IsMCOK(CHARACTER::MI_SUMMON))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 초간 쿨타임이 적용중입니다."), ch->GetMCLTime(CHARACTER::MI_SUMMON));
		return;
	}

	if (!CMonarch::instance().IsMoneyOk(SummonPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("국고에 돈이 부족합니다. 현재 : %u 필요금액 : %u"), NationMoney, SummonPrice);
		return;
	}

	const CMob * pkMob;
	DWORD vnum = 0;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	DWORD count;

	for (count = 0; cs_dwMonarchMobVnums[count] != 0; ++count)
		if (cs_dwMonarchMobVnums[count] == vnum)
			break;

	if (0 == cs_dwMonarchMobVnums[count])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소환할수 없는 몬스터 입니다. 소환가능한 몬스터는 홈페이지를 참조하세요"));
		return;
	}

	tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
			ch->GetMapIndex(),
			ch->GetX() - number(200, 750),
			ch->GetY() - number(200, 750),
			ch->GetX() + number(200, 750),
			ch->GetY() + number(200, 750),
			true,
			pkMob->m_table.bType == CHAR_TYPE_STONE,
			true);

	if (tch)
	{
		CMonarch::instance().SendtoDBDecMoney(SummonPrice, ch->GetEmpire(), ch);

		ch->SetMC(CHARACTER::MI_SUMMON);
	}
}

static const char* FN_point_string(int apply_number)
{
	switch (apply_number)
	{
		case POINT_MAX_HP:	return LC_TEXT("최대 생명력 +%d");
		case POINT_MAX_SP:	return LC_TEXT("최대 정신력 +%d");
		case POINT_HT:		return LC_TEXT("체력 +%d");
		case POINT_IQ:		return LC_TEXT("지능 +%d");
		case POINT_ST:		return LC_TEXT("근력 +%d");
		case POINT_DX:		return LC_TEXT("민첩 +%d");
		case POINT_ATT_SPEED:	return LC_TEXT("공격속도 +%d");
		case POINT_MOV_SPEED:	return LC_TEXT("이동속도 %d");
		case POINT_CASTING_SPEED:	return LC_TEXT("쿨타임 -%d");
		case POINT_HP_REGEN:	return LC_TEXT("생명력 회복 +%d");
		case POINT_SP_REGEN:	return LC_TEXT("정신력 회복 +%d");
		case POINT_POISON_PCT:	return LC_TEXT("독공격 %d");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_PCT:	return LC_TEXT("독공격 %d");
#endif
		case POINT_STUN_PCT:	return LC_TEXT("스턴 +%d");
		case POINT_SLOW_PCT:	return LC_TEXT("슬로우 +%d");
		case POINT_CRITICAL_PCT:	return LC_TEXT("%d%% 확률로 치명타 공격");
		case POINT_RESIST_CRITICAL:	return LC_TEXT("상대의 치명타 확률 %d%% 감소");
		case POINT_PENETRATE_PCT:	return LC_TEXT("%d%% 확률로 관통 공격");
		case POINT_RESIST_PENETRATE: return LC_TEXT("상대의 관통 공격 확률 %d%% 감소");
		case POINT_ATTBONUS_HUMAN:	return LC_TEXT("인간류 몬스터 타격치 +%d%%");
		case POINT_ATTBONUS_ANIMAL:	return LC_TEXT("동물류 몬스터 타격치 +%d%%");
		case POINT_ATTBONUS_ORC:	return LC_TEXT("웅귀족 타격치 +%d%%");
		case POINT_ATTBONUS_MILGYO:	return LC_TEXT("밀교류 타격치 +%d%%");
		case POINT_ATTBONUS_UNDEAD:	return LC_TEXT("시체류 타격치 +%d%%");
		case POINT_ATTBONUS_DEVIL:	return LC_TEXT("악마류 타격치 +%d%%");
		case POINT_STEAL_HP:		return LC_TEXT("타격치 %d%% 를 생명력으로 흡수");
		case POINT_STEAL_SP:		return LC_TEXT("타력치 %d%% 를 정신력으로 흡수");
		case POINT_MANA_BURN_PCT:	return LC_TEXT("%d%% 확률로 타격시 상대 전신력 소모");
		case POINT_DAMAGE_SP_RECOVER:	return LC_TEXT("%d%% 확률로 피해시 정신력 회복");
		case POINT_BLOCK:			return LC_TEXT("물리타격시 블럭 확률 %d%%");
		case POINT_DODGE:			return LC_TEXT("활 공격 회피 확률 %d%%");
		case POINT_RESIST_SWORD:	return LC_TEXT("한손검 방어 %d%%");
		case POINT_RESIST_TWOHAND:	return LC_TEXT("양손검 방어 %d%%");
		case POINT_RESIST_DAGGER:	return LC_TEXT("두손검 방어 %d%%");
		case POINT_RESIST_BELL:		return LC_TEXT("방울 방어 %d%%");
		case POINT_RESIST_FAN:		return LC_TEXT("부채 방어 %d%%");
		case POINT_RESIST_BOW:		return LC_TEXT("활공격 저항 %d%%");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_CLAW:		return LC_TEXT("두손검 방어 %d%%");
#endif
		case POINT_RESIST_FIRE:		return LC_TEXT("화염 저항 %d%%");
		case POINT_RESIST_ELEC:		return LC_TEXT("전기 저항 %d%%");
		case POINT_RESIST_MAGIC:	return LC_TEXT("마법 저항 %d%%");
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		case POINT_RESIST_MAGIC_REDUCTION:	return LC_TEXT("마법 저항 %d%%");
#endif
		case POINT_RESIST_WIND:		return LC_TEXT("바람 저항 %d%%");
		case POINT_RESIST_ICE:		return LC_TEXT("냉기 저항 %d%%");
		case POINT_RESIST_EARTH:	return LC_TEXT("대지 저항 %d%%");
		case POINT_RESIST_DARK:		return LC_TEXT("어둠 저항 %d%%");
		case POINT_REFLECT_MELEE:	return LC_TEXT("직접 타격치 반사 확률 : %d%%");
		case POINT_REFLECT_CURSE:	return LC_TEXT("저주 되돌리기 확률 %d%%");
		case POINT_POISON_REDUCE:	return LC_TEXT("독 저항 %d%%");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_REDUCE:	return LC_TEXT("독 저항 %d%%");
#endif
		case POINT_KILL_SP_RECOVER:	return LC_TEXT("%d%% 확률로 적퇴치시 정신력 회복");
		case POINT_EXP_DOUBLE_BONUS:	return LC_TEXT("%d%% 확률로 적퇴치시 경험치 추가 상승");
		case POINT_GOLD_DOUBLE_BONUS:	return LC_TEXT("%d%% 확률로 적퇴치시 돈 2배 드롭");
		case POINT_ITEM_DROP_BONUS:	return LC_TEXT("%d%% 확률로 적퇴치시 아이템 2배 드롭");
		case POINT_POTION_BONUS:	return LC_TEXT("물약 사용시 %d%% 성능 증가");
		case POINT_KILL_HP_RECOVERY:	return LC_TEXT("%d%% 확률로 적퇴치시 생명력 회복");
		case POINT_ATT_GRADE_BONUS:	return LC_TEXT("공격력 +%d");
		case POINT_DEF_GRADE_BONUS:	return LC_TEXT("방어력 +%d");
		case POINT_MAGIC_ATT_GRADE:	return LC_TEXT("마법 공격력 +%d");
		case POINT_MAGIC_DEF_GRADE:	return LC_TEXT("마법 방어력 +%d");
		case POINT_MAX_STAMINA:	return LC_TEXT("최대 지구력 +%d");
		case POINT_ATTBONUS_WARRIOR:	return LC_TEXT("무사에게 강함 +%d%%");
		case POINT_ATTBONUS_ASSASSIN:	return LC_TEXT("자객에게 강함 +%d%%");
		case POINT_ATTBONUS_SURA:		return LC_TEXT("수라에게 강함 +%d%%");
		case POINT_ATTBONUS_SHAMAN:		return LC_TEXT("무당에게 강함 +%d%%");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_ATTBONUS_WOLFMAN:	return LC_TEXT("무당에게 강함 +%d%%");
#endif
		case POINT_ATTBONUS_MONSTER:	return LC_TEXT("몬스터에게 강함 +%d%%");
		case POINT_MALL_ATTBONUS:		return LC_TEXT("공격력 +%d%%");
		case POINT_MALL_DEFBONUS:		return LC_TEXT("방어력 +%d%%");
		case POINT_MALL_EXPBONUS:		return LC_TEXT("경험치 %d%%");
		case POINT_MALL_ITEMBONUS:		return LC_TEXT("아이템 드롭율 %d배"); // @fixme180 float to int
		case POINT_MALL_GOLDBONUS:		return LC_TEXT("돈 드롭율 %d배"); // @fixme180 float to int
		case POINT_MAX_HP_PCT:			return LC_TEXT("최대 생명력 +%d%%");
		case POINT_MAX_SP_PCT:			return LC_TEXT("최대 정신력 +%d%%");
		case POINT_SKILL_DAMAGE_BONUS:	return LC_TEXT("스킬 데미지 %d%%");
		case POINT_NORMAL_HIT_DAMAGE_BONUS:	return LC_TEXT("평타 데미지 %d%%");
		case POINT_SKILL_DEFEND_BONUS:		return LC_TEXT("스킬 데미지 저항 %d%%");
		case POINT_NORMAL_HIT_DEFEND_BONUS:	return LC_TEXT("평타 데미지 저항 %d%%");
		case POINT_RESIST_WARRIOR:	return LC_TEXT("무사공격에 %d%% 저항");
		case POINT_RESIST_ASSASSIN:	return LC_TEXT("자객공격에 %d%% 저항");
		case POINT_RESIST_SURA:		return LC_TEXT("수라공격에 %d%% 저항");
		case POINT_RESIST_SHAMAN:	return LC_TEXT("무당공격에 %d%% 저항");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_WOLFMAN:	return LC_TEXT("무당공격에 %d%% 저항");
#endif
		default:					return "UNK_ID %d%%"; // @fixme180
	}
}

static bool FN_hair_affect_string(LPCHARACTER ch, char *buf, size_t bufsiz)
{
	if (NULL == ch || NULL == buf)
		return false;

	CAffect* aff = NULL;
	time_t expire = 0;
	struct tm ltm;
	int	year, mon, day;
	int	offset = 0;

	aff = ch->FindAffect(AFFECT_HAIR);

	if (NULL == aff)
		return false;

	expire = ch->GetQuestFlag("hair.limit_time");

	if (expire < get_global_time())
		return false;

	// set apply string
	offset = snprintf(buf, bufsiz, FN_point_string(aff->bApplyOn), aff->lApplyValue);

	if (offset < 0 || offset >= (int) bufsiz)
		offset = bufsiz - 1;

	localtime_r(&expire, &ltm);

	year	= ltm.tm_year + 1900;
	mon		= ltm.tm_mon + 1;
	day		= ltm.tm_mday;

	snprintf(buf + offset, bufsiz - offset, LC_TEXT(" (만료일 : %d년 %d월 %d일)"), year, mon, day);

	return true;
}

ACMD(do_costume)
{
	char buf[1024]; // @warme015
	const size_t bufferSize = sizeof(buf);

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	CItem* pBody = ch->GetWear(WEAR_COSTUME_BODY);
	CItem* pHair = ch->GetWear(WEAR_COSTUME_HAIR);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	CItem* pMount = ch->GetWear(WEAR_COSTUME_MOUNT);
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CItem* pAcce = ch->GetWear(WEAR_COSTUME_ACCE);
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	CItem* pWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
#endif

#ifdef __AURA_SYSTEM__
	CItem* pAura = ch->GetWear(WEAR_COSTUME_AURA);
	if (pAura)
	{
		const char* itemName = pAura->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  AURA : %s", itemName);
		if (pAura->IsEquipped() && arg1[0] == 'a')
			ch->UnequipItem(pAura);
	}
#endif

	ch->ChatPacket(CHAT_TYPE_INFO, "COSTUME status:");

	if (pHair)
	{
		const char* itemName = pHair->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  HAIR : %s", itemName);

		for (int i = 0; i < pHair->GetAttributeCount(); ++i)
		{
			const TPlayerItemAttribute& attr = pHair->GetAttribute(i);
			if (0 < attr.bType)
			{
				snprintf(buf, bufferSize, FN_point_string(attr.bType), attr.sValue);
				ch->ChatPacket(CHAT_TYPE_INFO, "     %s", buf);
			}
		}

		if (pHair->IsEquipped() && arg1[0] == 'h')
			ch->UnequipItem(pHair);
	}

	if (pBody)
	{
		const char* itemName = pBody->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  BODY : %s", itemName);

		if (pBody->IsEquipped() && arg1[0] == 'b')
			ch->UnequipItem(pBody);
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (pMount)
	{
		const char* itemName = pMount->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  MOUNT : %s", itemName);

		if (pMount->IsEquipped() && arg1[0] == 'm')
			ch->UnequipItem(pMount);
	}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (pAcce)
	{
		const char* itemName = pAcce->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  ACCE : %s", itemName);

		if (pAcce->IsEquipped() && arg1[0] == 'a')
			ch->UnequipItem(pAcce);
	}
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (pWeapon)
	{
		const char* itemName = pWeapon->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  WEAPON : %s", itemName);

		if (pWeapon->IsEquipped() && arg1[0] == 'w')
			ch->UnequipItem(pWeapon);
	}
#endif
}

ACMD(do_hair)
{
	char buf[256];

	if (false == FN_hair_affect_string(ch, buf, sizeof(buf)))
		return;

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

ACMD(do_inventory)
{
	int	index = 0;
	int	count		= 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: inventory <start_index> <count>");
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
		if (index >= INVENTORY_MAX_NUM)
			break;

		item = ch->GetInventoryItem(index);

		ch->ChatPacket(CHAT_TYPE_INFO, "inventory [%d] = %s",
						index, item ? item->GetName() : "<NONE>");
		++index;
	}
}

//gift notify quest command
ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}


#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
ACMD(do_cube)
{

	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		default:
			return;
	}
}
#else
ACMD(do_cube)
{
	if (!ch->CanDoCube())
		return;

	dev_log(LOG_DEB0, "CUBE COMMAND <%s>: %s", ch->GetName(), argument);
	int cube_index = 0, inven_index = 0;
#ifdef ENABLE_EXTRA_INVENTORY
	BYTE window_type = 0;
	char arg1[256], arg2[256], arg3[256], arg4[256];

	two_arguments(two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3), arg4, sizeof(arg4));
#else
	const char *line;
	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
#endif


	if (0 == arg1[0]) {
		return;
	}

	const std::string& strArg1 = std::string(arg1);

	// r_info (request information)
	// /cube r_info     ==> (Client -> Server) 현재 NPC가 만들 수 있는 레시피 요청
	//					    (Server -> Client) /cube r_list npcVNUM resultCOUNT 123,1/125,1/128,1/130,5
	//
	// /cube r_info 3   ==> (Client -> Server) 현재 NPC가 만들수 있는 레시피 중 3번째 아이템을 만드는 데 필요한 정보를 요청
	// /cube r_info 3 5 ==> (Client -> Server) 현재 NPC가 만들수 있는 레시피 중 3번째 아이템부터 이후 5개의 아이템을 만드는 데 필요한 재료 정보를 요청
	//					   (Server -> Client) /cube m_info startIndex count 125,1|126,2|127,2|123,5&555,5&555,4/120000@125,1|126,2|127,2|123,5&555,5&555,4/120000
	//
	if (strArg1 == "r_info")
	{
		if (0 == arg2[0])
			Cube_request_result_list(ch);
		else
		{
			if (isdigit(*arg2))
			{
				int listIndex = 0, requestCount = 1;
				str_to_number(listIndex, arg2);

				if (0 != arg3[0] && isdigit(*arg3))
					str_to_number(requestCount, arg3);

				Cube_request_material_info(ch, listIndex, requestCount);
			}
		}

		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		case 'c':	// close
			Cube_close(ch);
			break;

		case 'l':	// list
			Cube_show_list(ch);
			break;

		case 'a':	// add cue_index inven_index
			{
#ifdef ENABLE_EXTRA_INVENTORY
				if (arg2[0] == 0 || !isdigit(*arg2) || arg3[0] == 0 || !isdigit(*arg3) || arg4[0] == 0 || !isdigit(*arg4))
#else
				if (0 == arg2[0] || !isdigit(*arg2) ||
					0 == arg3[0] || !isdigit(*arg3))
#endif
					return;

				str_to_number(cube_index, arg2);
				str_to_number(inven_index, arg3);
#ifdef ENABLE_EXTRA_INVENTORY
				str_to_number(window_type, arg4);
				Cube_add_item(ch, cube_index, inven_index, window_type);
#else
				Cube_add_item (ch, cube_index, inven_index);
#endif
			}
			break;

		case 'd':	// delete
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;

				str_to_number(cube_index, arg2);
				Cube_delete_item (ch, cube_index);
			}
			break;

		case 'm':	// make
			if (0 != arg2[0])
			{
				while (true == Cube_make(ch))
					dev_log (LOG_DEB0, "cube make success");
			}
			else
				Cube_make(ch);
			break;

		default:
			return;
	}
}
#endif

ACMD(do_in_game_mall)
{
	if (LC_IsEurope() == true)
	{
		char country_code[3];

		switch (LC_GetLocalType())
		{
			case LC_GERMANY:	country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_FRANCE:		country_code[0] = 'f'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_ITALY:		country_code[0] = 'i'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_SPAIN:		country_code[0] = 'e'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_UK:			country_code[0] = 'e'; country_code[1] = 'n'; country_code[2] = '\0'; break;
			case LC_TURKEY:		country_code[0] = 't'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_POLAND:		country_code[0] = 'p'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_PORTUGAL:	country_code[0] = 'p'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_GREEK:		country_code[0] = 'g'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_RUSSIA:		country_code[0] = 'r'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_DENMARK:	country_code[0] = 'd'; country_code[1] = 'k'; country_code[2] = '\0'; break;
			case LC_BULGARIA:	country_code[0] = 'b'; country_code[1] = 'g'; country_code[2] = '\0'; break;
			case LC_CROATIA:	country_code[0] = 'h'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_MEXICO:		country_code[0] = 'm'; country_code[1] = 'x'; country_code[2] = '\0'; break;
			case LC_ARABIA:		country_code[0] = 'a'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_CZECH:		country_code[0] = 'c'; country_code[1] = 'z'; country_code[2] = '\0'; break;
			case LC_ROMANIA:	country_code[0] = 'r'; country_code[1] = 'o'; country_code[2] = '\0'; break;
			case LC_HUNGARY:	country_code[0] = 'h'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_NETHERLANDS: country_code[0] = 'n'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_USA:		country_code[0] = 'u'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_CANADA:	country_code[0] = 'c'; country_code[1] = 'a'; country_code[2] = '\0'; break;
#ifdef __MULTI_LANGUAGE_SYSTEM__
			case LC_EUROPE: country_code[0] = 'e'; country_code[1] = 'u'; country_code[2] = '\0'; break;
#endif
			default:
				if (test_server == true)
				{
#ifdef __MULTI_LANGUAGE_SYSTEM__
					country_code[0] = 'p'; country_code[1] = 't'; country_code[2] = '\0';
#else
					country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0';
#endif
				}
				break;
		}

		char buf[512+1];
		char sas[33];
		MD5_CTX ctx;
		const char sas_key[] = "sasiskeyptweb";//GF9001

		snprintf(buf, sizeof(buf), "%u%u%s", ch->GetPlayerID(), ch->GetAID(), sas_key);

		MD5Init(&ctx);
		MD5Update(&ctx, (const unsigned char *) buf, strlen(buf));
#ifdef __FreeBSD__
		MD5End(&ctx, sas);
#else
		static const char hex[] = "0123456789abcdef";
		unsigned char digest[16];
		MD5Final(digest, &ctx);
		int i;
		for (i = 0; i < 16; ++i) {
			sas[i+i] = hex[digest[i] >> 4];
			sas[i+i+1] = hex[digest[i] & 0x0f];
		}
		sas[i+i] = '\0';
#endif

		snprintf(buf, sizeof(buf), "mall http://%s/ishop?pid=%u&c=%s&sid=%d&sas=%s",
				g_strWebMallURL.c_str(), ch->GetPlayerID(), country_code, g_server_id, sas);

		ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
	}
}

ACMD(do_dice)
{
	char arg1[256], arg2[256];
	int start = 1, end = 100;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		start = atoi(arg1);
		end = atoi(arg2);
	}
	else if (*arg1 && !*arg2)
	{
		start = 1;
		end = atoi(arg1);
	}

	end = MAX(start, end);
	start = MIN(start, end);

	int n = number(start, end);

#ifdef ENABLE_DICE_SYSTEM
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_DICE_INFO, LC_TEXT("%s님이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), ch->GetName(), n, start, end);
	else
		ch->ChatPacket(CHAT_TYPE_DICE_INFO, LC_TEXT("당신이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), n, start, end);
#else
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_INFO, LC_TEXT("%s님이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), ch->GetName(), n, start, end);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("당신이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), n, start, end);
#endif
}

#ifdef ENABLE_NEWSTUFF
ACMD(do_click_safebox)
{
	if ((ch->GetGMLevel() <= GM_PLAYER) && (ch->GetDungeon() || ch->GetWarMap()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open the safebox in dungeon or at war."));
		return;
	}

	ch->SetSafeboxOpenPosition();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
}
ACMD(do_force_logout)
{
	LPDESC pDesc=DESC_MANAGER::instance().FindByCharacterName(ch->GetName());
	if (!pDesc)
		return;
	pDesc->DelayedDisconnect(0);
}
#endif

ACMD(do_click_mall)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

ACMD(do_ride)
{
    //sys_log(1, "[DO_RIDE] start");
    if (ch->IsDead() || ch->IsStun())
	return;

#ifdef __RENEWAL_MOUNT__
	if (ch->MountBlockMap())
		return;
	time_t now = get_global_time();
	if (ch->GetProtectTime("mount.ride") > now)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You must wait %d seconds."), ch->GetProtectTime("mount.ride") - now);
		return;
	}
	ch->SetProtectTime("mount.ride", now + 2);
#endif

    {
	if (ch->IsHorseRiding())
	{
	    sys_log(1, "[DO_RIDE] stop riding");
	    ch->StopRiding();
	    return;
	}

	if (ch->GetMountVnum())
	{
	    sys_log(1, "[DO_RIDE] unmount");
	    do_unmount(ch, NULL, 0, 0);
	    return;
	}
    }

#ifdef __RENEWAL_MOUNT__
	if (ch->GetWear(WEAR_COSTUME_MOUNT))
	{
		ch->StartRiding();
		return;
	}
	else
#endif
    {
	if (ch->GetHorse() != NULL)
	{
	    sys_log(1, "[DO_RIDE] start riding");
	    ch->StartRiding();
	    return;
	}

	for (BYTE i=0; i<INVENTORY_MAX_NUM; ++i)
	{
	    LPITEM item = ch->GetInventoryItem(i);
	    if (NULL == item)
			continue;
#ifdef __RENEWAL_MOUNT__
			else if (item->IsCostumeMountItem())
			{
				ch->EquipItem(item);
				return;
			}
#endif
		if (item->IsRideItem())
		{
			if (
				NULL==ch->GetWear(WEAR_UNIQUE1)
				|| NULL==ch->GetWear(WEAR_UNIQUE2)
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
				|| NULL==ch->GetWear(WEAR_COSTUME_MOUNT)
#endif
			)
			{
				sys_log(1, "[DO_RIDE] USE UNIQUE ITEM");
				//ch->EquipItem(item);
				ch->UseItem(TItemPos (INVENTORY, i));
				return;
			}
		}

	    switch (item->GetVnum())
	    {
		case 71114:
		case 71116:
		case 71118:
		case 71120:
		    sys_log(1, "[DO_RIDE] USE QUEST ITEM");
		    ch->UseItem(TItemPos (INVENTORY, i));
		    return;
	    }

		if( (item->GetVnum() > 52000) && (item->GetVnum() < 52091) )	{
			sys_log(1, "[DO_RIDE] USE QUEST ITEM");
			ch->UseItem(TItemPos (INVENTORY, i));
		    return;
		}
	}
    }

    ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 먼저 소환해주세요."));
}

#ifdef ENABLE_MOVE_CHANNEL
ACMD(DoChangeChannel)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	WORD channel = 0;
	str_to_number(channel, arg1);
	if (!channel)
		return;

	ch->ChangeChannel(channel);
}
#endif

#ifdef ENABLE_BIYOLOG
ACMD(do_bio)
{
	if (ch->GetLevel() < 30)
		return;

	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }

	int level = ch->GetQuestFlag("bio.level");

	if (level >= bio_max)
		return;
	else if (level < 1)
		return;
	else if (ch->GetLevel() < bio_data[level][0])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You must be level %d to be assigned a mission.", bio_data[level][0]);
		return;
	}

	int count = ch->GetQuestFlag("bio.count");
	int time = ch->GetQuestFlag("bio.time");

	if (vecArgs[1] == "mission")
	{
		if (vecArgs.size() < 4) { return; }
		BYTE isOzut = 0;
		BYTE isUnutkanlik = 0;

		str_to_number(isOzut, vecArgs[2].c_str());
		str_to_number(isUnutkanlik, vecArgs[3].c_str());

		if (count < bio_data[level][2])
		{
			if (ch->CountSpecifyItem(bio_data[level][1]) < 1)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "There are not enough %d to finish the mission.", bio_data[level][1]);
				return;
			}

			//bool isUnutkanlikDrink = false;

			if (isUnutkanlik)
			{
				if (ch->CountSpecifyItem(bio_data[level][16]) > 0 && time > get_global_time())//fix
				//if (ch->CountSpecifyItem(bio_data[level][16]) > 0)
				{
					ch->RemoveSpecifyItem(bio_data[level][16], 1);
					//isUnutkanlikDrink = true;
				}
				else
				{
					if (time > get_global_time())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "You cannot deliver an item if the waiting time has not passed. You can reduce the time by using %d.", bio_data[level][16]);
						return;
					}
				}
			}
			else
			{
				if (time > get_global_time())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You cannot deliver an item if the waiting time has not passed. You can reduce the time by using %d.",bio_data[level][16]);
					return;
				}
			}


			bool isOzutDrink = false;
			if (isOzut)
			{
				if (ch->CountSpecifyItem(bio_data[level][15]) > 0)
				{
					ch->RemoveSpecifyItem(bio_data[level][15], 1);
					isOzutDrink = true;
				}
			}
			
			int prob = isOzutDrink ? bio_data[level][4] + 50 : bio_data[level][4];
			ch->RemoveSpecifyItem(bio_data[level][1], 1);
			if (prob >= number(1, 100))
			{
				count += 1;
				time = get_global_time() + bio_data[level][3];
				ch->SetQuestFlag("bio.count", count);
				ch->SetQuestFlag("bio.time", time);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", level, count, time);

				ch->ChatPacket(CHAT_TYPE_INFO, "The object %d has good quality and has given a positive result.", bio_data[level][1]);
			}
			else
			{
				time = get_global_time() + bio_data[level][3];
				ch->SetQuestFlag("bio.time", time);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", level, count, time);

				ch->ChatPacket(CHAT_TYPE_INFO, "The object %d was in bad condition, bring me more.", bio_data[level][1]);
			}

			if (bio_data[level][5] != 0)
			{
				if (count == bio_data[level][2])
					ch->ChatPacket(CHAT_TYPE_COMMAND, "biostone %d", level);
				return;
			}
			
			
		}

		if (bio_data[level][5] != 0)
		{
			if (count == bio_data[level][2])
			{
				if (ch->CountSpecifyItem(bio_data[level][5]) < 1)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You can't finish the mission without bringing me the sacred item: %d.", bio_data[level][5]);
					return;
				}
				else
				{
					ch->RemoveSpecifyItem(bio_data[level][5], 1);
					ch->SetQuestFlag("bio.count", count+1);

					if (bio_data[level][14] == 0)
					{
						ch->SetQuestFlag("bio.count", 0);
						ch->SetQuestFlag("bio.level", level + 1);
						ch->SetQuestFlag("bio.time", 0);

						if (bio_data[level][6] != 0)
						{
							long value = bio_data[level][7];
							CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][6]);
							if (affect) {
								value += affect->lApplyValue;
								ch->RemoveAffect(affect);
							}
							ch->AddAffect(AFFECT_COLLECT, bio_data[level][6], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

							ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character:%d", pointToApply(bio_data[level][6]), bio_data[level][7]);
							//ch->ChatPacket(CHAT_TYPE_INFO, "569");
						}

						if (bio_data[level][8] != 0)
						{
							long value = bio_data[level][9];
							CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][8]);
							if (affect) {
								value += affect->lApplyValue;
								ch->RemoveAffect(affect);
							}
							ch->AddAffect(AFFECT_COLLECT, bio_data[level][8], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

							ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character:%d", pointToApply(bio_data[level][8]), bio_data[level][9]);
							//ch->ChatPacket(CHAT_TYPE_INFO, "569");
						}

						if (bio_data[level][10] != 0)
						{
							long value = bio_data[level][11];
							CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][10]);
							if (affect) {
								value += affect->lApplyValue;
								ch->RemoveAffect(affect);
							}
							ch->AddAffect(AFFECT_COLLECT, bio_data[level][10], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

							ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character:%d", pointToApply(bio_data[level][10]), bio_data[level][11]);
							//ch->ChatPacket(CHAT_TYPE_INFO, "569");
						}

						if (bio_data[level][12] != 0)
						{
							long value = bio_data[level][13];
							CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][12]);
							if (affect) {
								value += affect->lApplyValue;
								ch->RemoveAffect(affect);
							}
								
							ch->AddAffect(AFFECT_COLLECT, bio_data[level][12], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

							ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character:%d", pointToApply(bio_data[level][12]), bio_data[level][13]);
							//ch->ChatPacket(CHAT_TYPE_INFO, "569");
						}

						int newLevel = level + 1;
						if (newLevel >= bio_max)
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "You have finished all your missions. I hope to see you very soon with new missions.");
							ch->ChatPacket(CHAT_TYPE_COMMAND, "bioempty");
							return;
						}
						ch->ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", newLevel,0,0);
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_COMMAND, "bioodul %d", level);
						return;
					}
				}
			}
			else if (count == bio_data[level][2]+1)
				return;
		}
		else
		{
			if (count == bio_data[level][2])
			{
				if (bio_data[level][14] == 0)
				{
					ch->SetQuestFlag("bio.count", 0);
					ch->SetQuestFlag("bio.level", level + 1);
					ch->SetQuestFlag("bio.time", 0);

					if (bio_data[level][6] != 0)
					{
						long value = bio_data[level][7];
						CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][6]);
						if (affect) {
							value += affect->lApplyValue;
							ch->RemoveAffect(affect);
						}
						ch->AddAffect(AFFECT_COLLECT, bio_data[level][6], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

						ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character.:%d", pointToApply(bio_data[level][6]), bio_data[level][7]);
						//ch->ChatPacket(CHAT_TYPE_INFO, "569");
					}

					if (bio_data[level][8] != 0)
					{
						long value = bio_data[level][9];
						CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][8]);
						if (affect) {
							value += affect->lApplyValue;
							ch->RemoveAffect(affect);
						}
						ch->AddAffect(AFFECT_COLLECT, bio_data[level][8], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

						ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character.:%d", pointToApply(bio_data[level][8]), bio_data[level][9]);
						//ch->ChatPacket(CHAT_TYPE_INFO, "569");
					}

					if (bio_data[level][10] != 0)
					{
						long value = bio_data[level][11];
						CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][10]);
						if (affect) {
							value += affect->lApplyValue;
							ch->RemoveAffect(affect);
						}
						ch->AddAffect(AFFECT_COLLECT, bio_data[level][10], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

						ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character.:%d", pointToApply(bio_data[level][10]), bio_data[level][11]);
						//ch->ChatPacket(CHAT_TYPE_INFO, "569");
					}

					if (bio_data[level][12] != 0)
					{
						long value = bio_data[level][13];
						CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][12]);
						if (affect) {
							value += affect->lApplyValue;
							ch->RemoveAffect(affect);
						}
						ch->AddAffect(AFFECT_COLLECT, bio_data[level][12], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

						ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character.:%d", pointToApply(bio_data[level][12]), bio_data[level][13]);
						//ch->ChatPacket(CHAT_TYPE_INFO, "569");
					}

					int newLevel = level + 1;
					if (newLevel >= bio_max)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "You have finished all your missions. I hope to see you very soon with new missions.");
						ch->ChatPacket(CHAT_TYPE_COMMAND, "bioempty");
						return;
					}
					ch->ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", newLevel, 0, 0);
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_COMMAND, "bioodul %d", level);
					return;
				}
			}
		}
	}
	else if (vecArgs[1] == "gift")
	{
		if (vecArgs.size() < 3) { return; }
		BYTE index = 0;
		str_to_number(index, vecArgs[2].c_str());

		if (index > 3)
			return;

		if (bio_data[level][5] != 0)
		{
			if (count != bio_data[level][2] + 1)
				return;
		}
		else
		{
			if (count != bio_data[level][2])
				return;
		}

		ch->SetQuestFlag("bio.count", 0);
		ch->SetQuestFlag("bio.level", level + 1);
		ch->SetQuestFlag("bio.time", 0);

		if (bio_data[level][6] != 0 && index == 0)
		{
			long value = bio_data[level][7];
			CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][6]);
			if (affect) {
				value += affect->lApplyValue;
				ch->RemoveAffect(affect);
			}
			ch->AddAffect(AFFECT_COLLECT, bio_data[level][6], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

			ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character.:%d", pointToApply(bio_data[level][6]), bio_data[level][7]);
			//ch->ChatPacket(CHAT_TYPE_INFO, "569");
		}

		if (bio_data[level][8] != 0 && index == 1)
		{
			long value = bio_data[level][9];
			CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][8]);
			if (affect) {
				value += affect->lApplyValue;
				ch->RemoveAffect(affect);
			}
			ch->AddAffect(AFFECT_COLLECT, bio_data[level][8], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

			ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character.:%d", pointToApply(bio_data[level][8]), bio_data[level][9]);
			//ch->ChatPacket(CHAT_TYPE_INFO, "569");
		}

		if (bio_data[level][10] != 0 && index == 2)
		{
			long value = bio_data[level][11];
			CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][10]);
			if (affect) {
				value += affect->lApplyValue;
				ch->RemoveAffect(affect);
			}
			ch->AddAffect(AFFECT_COLLECT, bio_data[level][10], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

			ch->ChatPacket(CHAT_TYPE_INFO, "The effect %d has been successfully processed to character.:%d", pointToApply(bio_data[level][10]), bio_data[level][11]);
			//ch->ChatPacket(CHAT_TYPE_INFO, "569");
		}

		char flag[100];
		sprintf(flag, "bio.bonus%d", level);
		ch->SetQuestFlag(flag, index + 1);

		int newLevel = level + 1;
		if (newLevel >= bio_max)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You have finished all your missions. I hope to see you very soon with new missions.");
			ch->ChatPacket(CHAT_TYPE_COMMAND, "bioempty");
			return;
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", newLevel, 0, 0);
	}
}
#endif

ACMD(do_open_shop)
{
	
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	str_to_number(shopvnum, arg1);

	if (shopvnum == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("uzaknpchata"));
		return;
	}



	if (!ch->IsPC())
		return;

	int UzakNPCFlood = ch->GetQuestFlag("uzak_npc.son_giris");
	if (UzakNPCFlood){
		if (get_global_time() < UzakNPCFlood + 1 /* limit */) {
			ch->ChatPacket(CHAT_TYPE_INFO, "You must wait 10 seconds to perform this action again.");
			return;
		}
	}
	
	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("acik_pencereleri_kapat_uzak_hata"));
		return;
	}

	if (ch->GetMapIndex() >= 10000 || ch->GetMapIndex() == 201 || ch->GetMapIndex() == 113 || ch->GetMapIndex() == 207 || ch->GetMapIndex() == 216 || ch->GetMapIndex() == 217 || ch->GetMapIndex() == 235 || ch->GetMapIndex() == 240 || ch->GetMapIndex() == 223 || ch->GetMapIndex() == 353 || ch->GetMapIndex() == 354 || ch->GetMapIndex() == 355 || ch->GetMapIndex() == 357 || ch->GetMapIndex() == 212 || ch->GetMapIndex() == 213 || ch->GetMapIndex() == 211 || ch->GetMapIndex() == 66)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bu_bolgede_uzak_market_acilmaz"));
		return;
	}

	LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());

	if (NULL != pSecMap)
	{
		NPCBul f;
		pSecMap->for_each(f);

		LPCHARACTER yeninpc = CHARACTER_MANAGER::instance().Find(shopvid);

		if (yeninpc)
		{

			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("uzaknpc"));
			ch->SetQuestFlag("uzak_npc.son_giris", get_global_time() + 10);
			CShopManager::instance().NPCAC(ch, yeninpc, shopvnum);
		}

	}
	return;
}

#ifdef RENEWAL_MISSION_BOOKS
ACMD(do_missionbooks)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "load")
	{
		ch->SendMissionData();
	}
	else if (vecArgs[1] == "delete")
	{
		if (vecArgs.size() < 3) { return; }
		WORD missionID = 0;
		str_to_number(missionID, vecArgs[2].c_str());
		ch->DeleteBookMission(missionID);
	}
	else if (vecArgs[1] == "reward")
	{
		if (vecArgs.size() < 3) { return; }
		WORD missionID = 0;
		str_to_number(missionID, vecArgs[2].c_str());
		ch->RewardMissionBook(missionID);
	}
}
#endif

#ifdef ENABLE_ANTI_EXP
ACMD(do_anti_exp)
{
	time_t real_time = time(0);
	if (ch->GetProtectTime("anti.exp") > real_time)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You must wait %d seconds."), ch->GetProtectTime("anti.exp") - real_time);
		return;
	}
	ch->SetProtectTime("anti.exp", real_time + 3);
	ch->SetAntiExp(!ch->GetAntiExp());
	ch->ChatPacket(CHAT_TYPE_COMMAND, "SetAntiExp %d", ch->GetAntiExp()?1:0);
}
#endif

#ifdef ENABLE_ITEMSHOP
ACMD(do_ishop)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "data")
	{
		if (ch->GetProtectTime("itemshop.load") == 1)
			return;
		ch->SetProtectTime("itemshop.load", 1);
		if (vecArgs.size() < 3) { return; }
		int updateTime = 0;
		str_to_number(updateTime, vecArgs[2].c_str());
		CHARACTER_MANAGER::Instance().LoadItemShopData(ch, CHARACTER_MANAGER::Instance().GetItemShopUpdateTime() != updateTime);
	}
	else if (vecArgs[1] == "log")
	{
		if (ch->GetProtectTime("itemshop.log") == 1)
			return;
		ch->SetProtectTime("itemshop.log", 1);

		CHARACTER_MANAGER::Instance().LoadItemShopLog(ch);
	}
	else if (vecArgs[1] == "buy")
	{
		if (vecArgs.size() < 4) { return; }
		int itemID = 0;
		str_to_number(itemID, vecArgs[2].c_str());
		DWORD itemCount = 0;
		str_to_number(itemCount, vecArgs[3].c_str());
		if(itemCount < 1 || itemCount > 20)
			return;
		CHARACTER_MANAGER::Instance().LoadItemShopBuy(ch, itemID, itemCount);
	}
	else if (vecArgs[1] == "wheel")
	{
		if (vecArgs.size() < 3) { return; }
		else if (vecArgs[2] == "start")
		{
			if (vecArgs.size() < 4) { return; }
			BYTE ticketType;
			if (!str_to_number(ticketType, vecArgs[3].c_str()))
				return;
			if (ticketType > 1)
				return;
			else if (ch->GetProtectTime("WheelWorking") != 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "The roulete spinning...");
				return;
			}
			if (ticketType == 0)
			{
				if (ch->CountSpecifyItem(80013) <= 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You don't have enough tickets to spin the wheel of fortune.");
					return;
				}
				ch->RemoveSpecifyItem(80013, 1);
			}
			else if (ticketType == 1)
			{
				long long dragonCoin = ch->GetDragonCoin();
				if(dragonCoin-5 < 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You don't have enough coins to spin the wheel of fortune.");
					return;
				}
				ch->SetDragonCoin(dragonCoin-5);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "SetDragonCoin %lld", dragonCoin - 5);
				BYTE subIndex = ITEMSHOP_LOG_ADD;
				DWORD accountID = ch->GetDesc()->GetAccountTable().id;
				char playerName[CHARACTER_NAME_MAX_LEN+1];
				char ipAdress[16];
				strlcpy(playerName,ch->GetName(),sizeof(playerName));
				strlcpy(ipAdress,ch->GetDesc()->GetHostName(),sizeof(ipAdress));
				db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE)+sizeof(DWORD)+sizeof(playerName)+sizeof(ipAdress));
				db_clientdesc->Packet(&subIndex, sizeof(BYTE));
				db_clientdesc->Packet(&accountID, sizeof(DWORD));
				db_clientdesc->Packet(&playerName, sizeof(playerName));
				db_clientdesc->Packet(&ipAdress, sizeof(ipAdress));

				if (ch->GetProtectTime("itemshop.log") == 1)
				{
					char       timeText[21];
					time_t     now = time(0);
					struct tm  tstruct = *localtime(&now);
					strftime(timeText, sizeof(timeText), "%Y-%m-%d %X", &tstruct);
					ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopAppendLog %s %d %s %s 1 1 5", timeText, time(0), playerName, ipAdress);
				}
			}
			// Important items
			std::vector<std::pair<long, long>> m_important_item = {
				{71126,1}, //Paquete dopador(M)
				{53010,1}, //Sello Porkie (30d)
				{71085,10}, //Limpiador avanzado
				{71085,20}, //Paquete dopador(S)
				{71143,1}, //Sello Khan (7d)
			};

			// normal items
			std::map<std::pair<long, long>, int> m_normal_item = {
				{{71085,1},30},
				{{71084,1},30},
				{{25040,1},30},
				{{71152,1},30},
				{{71151,1},30},
				{{71107,1},30},
				{{70005,1},20},
				{{70043,1},20},
				{{72320,1},30}, 
				{{70038,200},30},
				{{39004,1},30},
				{{22010,10},30},
				{{71027,1},30},
				{{71028,1},30},
				{{71029,1},30},
				{{71030,1},30},
				{{71044,1},30},
				{{71045,1},30},
				{{30183,5},30},
				{{71001,1},20},
				{{71094,1},20},
				{{72725,1},30},
				{{72729,1},30},
				{{30190,1},30},
				{{71130,1},30},
				{{71035,1},20},
				{{30250,1},10},
				{{76012,3},30},
				{{76018,3},30},
				{{76009,1},20},
				{{70102,5},30},
			};

			std::vector<std::pair<long, long>> m_send_items;
			if (m_important_item.size())
			{
				int random = number(0,m_important_item.size()-1);
				m_send_items.emplace_back(m_important_item[random].first, m_important_item[random].second);
			}

			while (true)
			{
				for (auto it = m_normal_item.begin(); it != m_normal_item.end(); ++it)
				{
					int randomEx = number(0,4);
					if (randomEx == 4)
					{
						int random = number(0,100);
						if (it->second >= random)
						{
							auto itFind = std::find(m_send_items.begin(), m_send_items.end(), it->first);
							if (itFind == m_send_items.end())
							{
								m_send_items.emplace_back(it->first.first, it->first.second);
								if (m_send_items.size() >= 10)
									break;
							}
						}
					}
				}
				if (m_send_items.size() >= 10)
					break;
			}

			std::string cmd_wheel = "";
			if (m_send_items.size())
			{
				for (auto it = m_send_items.begin(); it != m_send_items.end(); ++it)
				{
					cmd_wheel += std::to_string(it->first);
					cmd_wheel += "|";
					cmd_wheel += std::to_string(it->second);
					cmd_wheel += "#";
				}
			}

			int luckyWheel = number(0, 9);
			if (luckyWheel == 0)
				if (number(0, 1) == 0)
					luckyWheel = number(0, 9);

			ch->SetProtectTime("WheelLuckyIndex", luckyWheel);
			ch->SetProtectTime("WheelLuckyItemVnum", m_send_items[luckyWheel].first);
			ch->SetProtectTime("WheelLuckyItemCount", m_send_items[luckyWheel].second);

			ch->SetProtectTime("WheelWorking", 1);

			ch->ChatPacket(CHAT_TYPE_COMMAND, "SetWheelItemData %s", cmd_wheel.c_str());
			ch->ChatPacket(CHAT_TYPE_COMMAND, "OnSetWhell %d", luckyWheel);
		}
		else if (vecArgs[2] == "done")
		{
			if (ch->GetProtectTime("WheelWorking") == 0)
				return;

			ch->AutoGiveItem(ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GetWheelGiftData %d %d", ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));
			ch->SetProtectTime("WheelLuckyIndex", 0);
			ch->SetProtectTime("WheelLuckyItemVnum", 0);
			ch->SetProtectTime("WheelLuckyItemCount", 0);
			ch->SetProtectTime("WheelWorking", 0);
		}

	}
}
#endif


#ifdef ENABLE_AFFECT_BUFF_REMOVE
ACMD(do_remove_buff)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch)
		return;

	int affect = 0;
	str_to_number(affect, arg1);
	CAffect* pAffect = ch->FindAffect(affect);

	if (pAffect)
		ch->RemoveAffect(affect);
}
#endif

#ifdef ENABLE_AFFECT_POLYMORPH_REMOVE
ACMD(do_remove_polymorph)
{
	if (!ch)
		return;

	if (!ch->IsPolymorphed())
		return;

	ch->SetPolymorph(0);
	ch->RemoveAffect(AFFECT_POLYMORPH);
}
#endif

#ifdef ENABLE_TRACK_WINDOW
#include "new_mob_timer.h"
ACMD(do_track_window)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "load")
	{
		if (ch->GetProtectTime("track_dungeon")==1)
			return;
		ch->GetDungeonCooldown(WORD_MAX);
		for (BYTE i = 2; i < vecArgs.size(); ++i)
		{
			WORD globalBossID;
			if (!str_to_number(globalBossID, vecArgs[i].c_str()))
				continue;
			CNewMobTimer::Instance().GetTrackData(ch, globalBossID);
		}
		ch->SetProtectTime("track_dungeon", 1);
	}
	else if (vecArgs[1] == "reenter")
	{
		if (!ch->IsGM())
			return;
		if (vecArgs.size() < 4) { return; }
		WORD testVnum;
		if (!str_to_number(testVnum, vecArgs[2].c_str()))
			return;
		int testTime;
		if (!str_to_number(testTime, vecArgs[3].c_str()))
			return;
		ch->GetDungeonCooldownTest(testVnum, testTime, false);
	}
	else if (vecArgs[1] == "cooldown")
	{
		if (!ch->IsGM())
			return;
		if (vecArgs.size() < 4) { return; }
		WORD testVnum;
		if (!str_to_number(testVnum, vecArgs[2].c_str()))
			return;
		int testTime;
		if (!str_to_number(testTime, vecArgs[3].c_str()))
			return;
		ch->GetDungeonCooldownTest(testVnum, testTime, true);
	}
	else if (vecArgs[1] == "teleport")
	{
		if (vecArgs.size() < 3) { return; }
		WORD mobIndex;
		if (!str_to_number(mobIndex, vecArgs[2].c_str()))
			return;

		//PORTAL WARP I PUT ONLY FOR FLAME B
		const std::map<WORD, std::pair<std::pair<long, long>,std::pair<WORD, std::pair<BYTE,BYTE>>>> m_TeleportData = {
			//{mobindex - {{X, Y}, {PORT, {MINLVL,MAXLVL},}}},
			{5115, { {8727, 2106}, {0, {1, 40}} }},
			{5163, { {2880, 5406}, {0, {1, 70}} }},
			{2812, { {3840, 14323}, {0, {40, 60}} }},
			{2842, { {3327, 14848}, {0, {65, 85}} }},
			{2762, { {3840, 14861}, {0, {80, 100}} }},
			{2852, { {4350, 14850}, {0, {95, 125}} }},
			{2792, { {3327, 15384}, {0, {100, 150}} }},
			// {4140, { {9341, 4134}, {0, {105, 120}} }},
			{1093, { {5899, 1116}, {0, {40, 120}} }},
			{2092, { {915, 5255}, {0, {60, 90}} }},
			{2493, { {1799, 12204}, {0, {80, 120}} }},
			{2598, { {5918, 993}, {0, {75, 120}} }},
			{6091, { {5984, 7073}, {0, {95, 125}} }},
			{6191, { {4319, 1647}, {0, {95, 125}} }},
			{9118, { {11082, 17824}, {0, {105, 125}} }},
			{20442, { {7358, 6237}, {0, {110, 125}} }},
			{4388, { {19020, 22690}, {0, {105, 150}} }},
			{4549, { {34209, 24441}, {0, {125, 150}} }},
		};
		const auto it = m_TeleportData.find(mobIndex);
		if (it != m_TeleportData.end())
		{
			if (ch->GetLevel() < it->second.second.second.first || ch->GetLevel() > it->second.second.second.second)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You don't has enought level for teleport!");
				return;
			}
			ch->WarpSet(it->second.first.first * 100, it->second.first.second * 100, it->second.second.first);
		}
	}
}
#endif

#ifdef ENABLE_EVENT_MANAGER
ACMD(do_event_manager)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "info")
	{
		CHARACTER_MANAGER::Instance().SendDataPlayer(ch);
	}
	else if (vecArgs[1] == "remove")
	{
		if (!ch->IsGM())
			return;

		if (vecArgs.size() < 3) { 
			
			ch->ChatPacket(CHAT_TYPE_INFO, "put the event index!!");
			return; 
		}

		BYTE removeIndex = 0;  // Inicializar removeIndex con un valor predeterminado
		str_to_number(removeIndex, vecArgs[2].c_str());

		if (CHARACTER_MANAGER::Instance().CloseEventManuel(removeIndex))
			ch->ChatPacket(CHAT_TYPE_INFO, "successfully removed!");
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "don't have any event!");
	}
	else if (vecArgs[1] == "update")
	{
		if (!ch->IsGM())
			return;
		const BYTE subHeader = EVENT_MANAGER_UPDATE;
		//db_clientdesc->DBPacketHeader(HEADER_GD_EVENT_MANAGER, 0, sizeof(BYTE));
		//db_clientdesc->Packet(&subHeader, sizeof(BYTE));
		db_clientdesc->DBPacket(HEADER_GD_EVENT_MANAGER, 0, &subHeader, sizeof(BYTE));

		ch->ChatPacket(CHAT_TYPE_INFO, "successfully update!");
	}
}
#endif

#ifdef ENABLE_PREMIUM_SYSTEM
ACMD(do_premium)
{
	//BASIC
	char arg1[256], arg2[256], arg3[256];
	int arg3_int = 0;
	argument = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	const char* arg3_2 = one_argument(argument, arg3, sizeof(arg3));
	strcat(strcat(arg3, " "),arg3_2);
	
	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<premium_command_syntax>"));
		return;
	}
	
	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);
	
	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s<premium_no_target>"), arg1);
		return;
	}
	
	if(!strcmp(arg1, "set")) {
		str_to_number(arg3_int, arg3);
		if(isdigit(*arg3) || arg3_int >= 0)
		{
			if(CPremiumSystem::instance().IsPremium(tch))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s<premium_is_already_active>"), tch->GetName());
				return;
			}
			
			CPremiumSystem::instance().SetPremium(tch, arg3_int);
			CPremiumSystem::instance().RefreshPremium(tch);
#ifdef ENABLE_UPDATE_PACKET_IN_PREMIUM_SYSTEM
			tch->UpdatePacket();
#endif

			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s%i<premium_set_admin_message>"), tch->GetName(), arg3_int);
			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s%i<premium_set_target_message>"), ch->GetName(), arg3_int);
			return;
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<premium_set_command_syntax>"));
			return;
		}
	}
	else if(!strcmp(arg1, "remove")) 
	{
		if(!CPremiumSystem::instance().IsPremium(tch))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s<premium_is_not_active>"), tch->GetName());
			return;			
		}
		
		CPremiumSystem::instance().RemovePremium(tch);
		CPremiumSystem::instance().RefreshPremium(tch);
#ifdef ENABLE_UPDATE_PACKET_IN_PREMIUM_SYSTEM
		tch->UpdatePacket();
#endif
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s<premium_remove_admin_message>"), tch->GetName());
		tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s<premium_remove_target_message>"), ch->GetName());		
		return;
	}
	else if(!strcmp(arg1, "refresh"))
	{
		CPremiumSystem::instance().RefreshPremium(tch);
#ifdef ENABLE_UPDATE_PACKET_IN_PREMIUM_SYSTEM
		tch->UpdatePacket();
#endif
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s<premium_refresh_admin_message>"), tch->GetName());
		tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s<premium_refresh_target_message>"), ch->GetName());		
		return;
	}
	else if(!strcmp(arg1, "check"))
	{
		const char* status = "0";
		if(CPremiumSystem::instance().IsPremium(tch))
			status = "active";
		else
			status = "noactive";
		
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s%s<premium_status_message>"), tch->GetName(), status);
		return;
	}
}

ACMD(do_refresh_premium)
{
	CPremiumSystem::instance().RefreshPremium(ch);
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<premium_refreshed>"));
}
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
ACMD(do_hide_costume)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	bool hidden = true;
	uint8_t bPartPos = 0;
	uint8_t bHidden = 0;

	str_to_number(bPartPos, arg1);

	if (*arg2)
	{
		str_to_number(bHidden, arg2);

		if (bHidden == 0)
			hidden = false;
	}

	if (ch->IsDead())
		return;

	bool bAttacking = (get_dword_time() - ch->GetLastAttackTime()) < 1500;
	bool bMoving = (get_dword_time() - ch->GetLastMoveTime()) < 1500;
	bool bDelayedCMD = false;

	if (ch->IsStateMove() || bAttacking || bMoving)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to stand still to hide your costume."));
		return;
	}

	if (bDelayedCMD)
	{
		int iPulse = thecore_pulse();
		if (iPulse - ch->GetHideCostumePulse() < passes_per_sec * 3)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait 3 seconds before you can hide your costume again."));
			return;
		}
		ch->SetHideCostumePulse(iPulse);
	}

	if (bPartPos == 1)
		ch->SetBodyCostumeHidden(hidden);
	else if (bPartPos == 2)
		ch->SetHairCostumeHidden(hidden);
# ifdef ENABLE_ACCE_COSTUME_SYSTEM
	else if (bPartPos == 3)
		ch->SetAcceCostumeHidden(hidden);
# endif
# ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	else if (bPartPos == 4)
		ch->SetWeaponCostumeHidden(hidden);
# endif
#ifdef __AURA_SYSTEM__
	else if (bPartPos == 5)
		ch->SetAuraCostumeHidden(hidden);
# endif
	else
		return;

	ch->UpdatePacket();
}
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
ACMD(do_multi_farm)
{
	if (!ch->GetDesc())
		return;
	if (ch->GetProtectTime("multi-farm") > get_global_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need be slow! You can try after %d second."), ch->GetProtectTime("multi-farm") - get_global_time());
		return;
	}
	ch->SetProtectTime("multi-farm", get_global_time() + 10);
	CHARACTER_MANAGER::Instance().CheckMultiFarmAccount(ch->GetDesc()->GetHostName(), ch->GetPlayerID(),ch->GetName(), !ch->GetRewardStatus());
}
#endif


#ifdef __GEM_SYSTEM__
ACMD(do_gem)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "load")
	{
		if (time(0) >= ch->GetQuestFlag("gem.left_time"))
		{
			ch->RefreshGemALL(true);
			ch->ChatPacket(CHAT_TYPE_INFO, "Tienda actualizada.");
		}
		else
			ch->RefreshGemPlayer();
	}
	else if (vecArgs[1] == "time")
	{
		if (time(0) >= ch->GetQuestFlag("gem.left_time"))
		{
			ch->RefreshGemALL(true);
			ch->ChatPacket(CHAT_TYPE_INFO, "Tienda Gaya actualizada.");
		}
	}
	else if (vecArgs[1] == "slot")
	{
		ch->OpenGemSlot();
	}
	else if (vecArgs[1] == "refresh")
	{
		if (ch->CountSpecifyItem(OPEN_GEM_REFRESH_ITEM) < 1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "No tienes el objeto para actualizar la tienda de gaya");
			return;
		}
		ch->RemoveSpecifyItem(OPEN_GEM_REFRESH_ITEM, 1);
		ch->RefreshGemALL();
	}
	else if (vecArgs[1] == "buy")
	{
		if (vecArgs.size() < 3) { return; }
		BYTE slotIndex;
		if (!str_to_number(slotIndex, vecArgs[2].c_str()))
			return;
		ch->BuyGemItem(slotIndex);
	}
	else if (vecArgs[1] == "gm")
	{
		if (!ch->IsGM())
			return;
		if (vecArgs.size() < 3) { return; }
		if (vecArgs[2] == "slotcount")
		{
			if (vecArgs.size() < 4) { return; }
			BYTE slotCount;
			if (!str_to_number(slotCount, vecArgs[3].c_str()))
				return;
			if (slotCount > 27 - 9)
				return;
			ch->SetQuestFlag("gem.open_slot", slotCount);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GemUpdateSlotCount %d", slotCount);
		}
		else if (vecArgs[2] == "time")
		{
			if (vecArgs.size() < 4) { return; }
			int newTime;
			if (!str_to_number(newTime, vecArgs[3].c_str()))
				return;
			ch->SetQuestFlag("gem.left_time", time(0)+newTime);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GemSetRefreshLeftTime %d", newTime);
		}
	}
}
#endif




#include "fishing.h"
ACMD(do_fishgame)
{
	LPITEM rod = ch->GetWear(WEAR_WEAPON);
	if (!rod)
		return;
	if (ch->m_pkFishingEvent)
	{
		char arg1[256];
		one_argument(argument, arg1, sizeof(arg1));
		int arg1_1 = atoi(arg1);
		if (arg1_1 == 58236)
		{
			++ch->fishGame;
		}
		else if (arg1_1 == 78531)
		{
			if (rod->GetType() == ITEM_ROD)
			{
				ch->fishing_take();
			}
		}
		else if (arg1_1 == 90295)
		{
			ch->m_pkFishingEvent = NULL;
			rod->SetSocket(2, 0);
			//fishing::FishingFail(ch); // :)
		}
	}

}


#ifdef ENABLE_EXCHANGE_LOG
ACMD(do_ex_log)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "load")
		ch->SendExchangeLogPacket(SUB_EXCHANGELOG_LOAD);
	else if (vecArgs[1] == "load_item")
	{
		if (vecArgs.size() < 3) { return; }
		DWORD logID;
		if(!str_to_number(logID, vecArgs[2].c_str()))
			return;
		ch->SendExchangeLogPacket(SUB_EXCHANGELOG_LOAD_ITEM, logID);
	}
	else if (vecArgs[1] == "delete")
	{
		if (vecArgs.size() < 4) { return; }
		const std::string playerCode(ch->GetDesc()->GetAccountTable().social_id);
		if (playerCode != vecArgs[2])
			return;
		if (vecArgs[3] == "all")
			ch->DeleteExchangeLog(0);
		else
		{
			for (DWORD j = 3; j < vecArgs.size(); ++j)
			{
				DWORD logID;
				if(!str_to_number(logID, vecArgs[j].c_str()))
					return;
				ch->DeleteExchangeLog(logID);
			}
		}
	}
}
#endif


#ifdef __AUTO_HUNT__
ACMD(do_auto_hunt)
{
	ch->GetAutoHuntCommand(argument);
}
#endif


//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe

#ifdef ENABLE_CHANGE_CHANNEL
ACMD(do_change_channel)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD channel_number = 0;
	str_to_number(channel_number, arg1);
	
	//if (ch->m_pkChangeChannelEvent)
	//{
	//	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Change channel canceled."));
	//	event_cancel(&ch->m_pkChangeChannelEvent);
	//	return;
	//}
	
	if(!ch)
	{
		return;	
	}
	
	if(channel_number == 99 || g_bChannel == 99){
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't change channel in this map."));
		return;		
	}
	
	if(channel_number == g_bChannel)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are already in this channel."));
		return;		
	}
	
    if (ch->IsDead() || !ch->CanWarp())
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't do that now. Wait 10 seconds and try again."));
        return;
	}
	
	if(channel_number <= 0 || channel_number > 6)
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This channel is not valid."));
        return;
	}
	
	if (channel_number != 0)
	{
		if (ch->m_pkChangeChannelEvent)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Change channel canceled."));
			event_cancel(&ch->m_pkChangeChannelEvent);
			return;
		}
	
		ChangeChannelEventInfo* info = AllocEventInfo<ChangeChannelEventInfo>();
	
		{
			if (ch->IsPosition(POS_FIGHTING))
				info->left_second = 10;
			else
				info->left_second = 3;
		}
	
		info->ch					= ch;
		info->channel_number		= channel_number;
	
		ch->m_pkChangeChannelEvent	= event_create(change_channel_event, info, 1);
	}
}
#endif

#ifdef __RANKING_SYSTEM__

ACMD(do_request_rank_info)
{
    int iPulse = thecore_pulse();

    if (iPulse - ch->GetRequestRankTimer() < PASSES_PER_SEC(1))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait %d seconds and try again."), 1);
        return;
    }
    
    char arg1[256];
    char arg2[256]; // Empire filter
    char arg3[256]; // Race filter
    const char *line = three_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3)); // Using three_arguments here

    if (!*arg1)
        return;

    BYTE rankID = 0;
    str_to_number(rankID, arg1);
    
    int empireFilter = 0; // To hold the empire filter value
    if (*arg2) {
        str_to_number(empireFilter, arg2); // Convert the second argument to an integer
    }

    int raceFilter = 0; // To hold the race filter value
    if (*arg3) {
        str_to_number(raceFilter, arg3); // Convert the third argument to an integer
    }

    if (rankID >= 0 && rankID < RANK_MAX)
    {
        if (empireFilter > 0 && empireFilter <= 3 && raceFilter > 0 && raceFilter <= 4) {
            // Filter by both empire and race
            RankPlayer::instance().RequestInfoRank(ch, rankID, empireFilter, raceFilter);
        } else if (empireFilter > 0 && empireFilter <= 3 && raceFilter==0) {
            // Filter by empire only
            RankPlayer::instance().RequestInfoRank(ch, rankID, empireFilter, 0); // Here, 0 is used for invalid race
        } else if (raceFilter > 0 && raceFilter <= 4 && empireFilter ==0) {
            // Filter by race only
            RankPlayer::instance().RequestInfoRank(ch, rankID, 0, raceFilter); // Here, 0 is used for invalid empire
        } else {
            // No valid filters provided, show all ranks with default values
            RankPlayer::instance().RequestInfoRank(ch, rankID, 0, 0); // Both empire and race filters as 0
        }
    }
    
    ch->SetRequestRankTimer();
}
#endif
//new




#if defined(ENABLE_RENEWAL_AFFECT_SHOWER)
ACMD(do_remove_affect)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	DWORD affect = 0;
	str_to_number(affect, vecArgs[1].c_str());
	if (AFFECT_POLYMORPH == affect)
	{
		if (!ch->IsPolymorphed())
			return;
		ch->SetPolymorph(0);
	}
if (affect == 66)
    {
        return;
    }//risipa
	if(ch->FindAffect(affect))
		ch->RemoveAffect(affect);
}
#endif



ACMD(do_split_items) //SPLIT ITEMS
{
//i'm lazy..same as pet
	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}

	if (!ch)
		return;
	
	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
	
	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Wrong command use.");
		return;
	}
	
	if (!ch->CanWarp())
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
		return;
	}
	
	WORD count = 0;
	WORD cell = 0;
	WORD destCell = 0;
	
	str_to_number(cell, arg1);
	str_to_number(count, arg2);
	str_to_number(destCell, arg3);

	if (count <= 0) return;
	
	LPITEM item = ch->GetInventoryItem(cell);
	if (item != NULL)
	{
		WORD itemCount = item->GetCount();
		while (itemCount > 0)
		{
			if (count > itemCount)
				count = itemCount;
			
			int iEmptyPosition = ch->GetEmptyInventoryFromIndex(destCell, item->GetSize());
			if (iEmptyPosition == -1)
				break;
			
			itemCount -= count;
			ch->MoveItem(TItemPos(INVENTORY, cell), TItemPos(INVENTORY, iEmptyPosition), count);
		}
	}



	ch->SetRequestPetTimer(get_global_time() + 3);//
}

ACMD(do_split_items_upp) //SPLIT ITEMS
{
//i'm lazy..same as pet
	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}



	if (!ch)
		return;
	
	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
	
	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Wrong command use.");
		return;
	}
	
	if (!ch->CanWarp())
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
		return;
	}
	
	WORD count = 0;
	WORD cell = 0;
	WORD destCell = 0;
	
	str_to_number(cell, arg1);
	str_to_number(count, arg2);
	str_to_number(destCell, arg3);
	
	if (count <= 0) return;

	LPITEM item = ch->GetUpgradeInventoryItem(cell);
	if (item != NULL)
	{
		WORD itemCount = item->GetCount();
		while (itemCount > 0)
		{
			if (count > itemCount)
				count = itemCount;
			
			int iEmptyPosition = ch->GetEmptyUppInventoryFromIndex(destCell, item->GetSize());
			if (iEmptyPosition == -1)
				break;
			
			itemCount -= count;
			ch->MoveItem(TItemPos(UPGRADE_INVENTORY, cell), TItemPos(UPGRADE_INVENTORY, iEmptyPosition), count);
		}
	}


	ch->SetRequestPetTimer(get_global_time() + 3);//
}


ACMD(do_split_items_book) //SPLIT ITEMS
{
//i'm lazy..same as pet
	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}


	if (!ch)
		return;
	
	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
	
	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Wrong command use.");
		return;
	}
	
	if (!ch->CanWarp())
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
		return;
	}
	
	WORD count = 0;
	WORD cell = 0;
	WORD destCell = 0;
	
	str_to_number(cell, arg1);
	str_to_number(count, arg2);
	str_to_number(destCell, arg3);
	
	if (count <= 0) return;

	LPITEM item = ch->GetBookInventoryItem(cell);
	if (item != NULL)
	{
		WORD itemCount = item->GetCount();
		while (itemCount > 0)
		{
			if (count > itemCount)
				count = itemCount;
			
			int iEmptyPosition = ch->GetEmptyBookInventoryFromIndex(destCell, item->GetSize());
			if (iEmptyPosition == -1)
				break;
			
			itemCount -= count;
			ch->MoveItem(TItemPos(BOOK_INVENTORY, cell), TItemPos(BOOK_INVENTORY, iEmptyPosition), count);
		}
	}


	ch->SetRequestPetTimer(get_global_time() + 3);//
}


ACMD(do_split_items_stone) //SPLIT ITEMS
{
//i'm lazy..same as pet
	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}


	if (!ch)
		return;
	
	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
	
	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Wrong command use.");
		return;
	}
	
	if (!ch->CanWarp())
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
		return;
	}
	
	WORD count = 0;
	WORD cell = 0;
	WORD destCell = 0;
	
	str_to_number(cell, arg1);
	str_to_number(count, arg2);
	str_to_number(destCell, arg3);
	
	if (count <= 0) return;

	LPITEM item = ch->GetStoneInventoryItem(cell);
	if (item != NULL)
	{
		WORD itemCount = item->GetCount();
		while (itemCount > 0)
		{
			if (count > itemCount)
				count = itemCount;
			
			int iEmptyPosition = ch->GetEmptyStoneInventoryFromIndex(destCell, item->GetSize());
			if (iEmptyPosition == -1)
				break;
			
			itemCount -= count;
			ch->MoveItem(TItemPos(STONE_INVENTORY, cell), TItemPos(STONE_INVENTORY, iEmptyPosition), count);
		}
	}


	ch->SetRequestPetTimer(get_global_time() + 3);//
}



ACMD(do_split_items_chest) //SPLIT ITEMS
{
//i'm lazy..same as pet
	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}


	if (!ch)
		return;
	
	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
	
	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Wrong command use.");
		return;
	}
	
	if (!ch->CanWarp())
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
		return;
	}
	
	WORD count = 0;
	WORD cell = 0;
	WORD destCell = 0;
	
	str_to_number(cell, arg1);
	str_to_number(count, arg2);
	str_to_number(destCell, arg3);
	
	if (count <= 0) return;

	LPITEM item = ch->GetChestInventoryItem(cell);
	if (item != NULL)
	{
		WORD itemCount = item->GetCount();
		while (itemCount > 0)
		{
			if (count > itemCount)
				count = itemCount;
			
			int iEmptyPosition = ch->GetEmptyChestInventoryFromIndex(destCell, item->GetSize());
			if (iEmptyPosition == -1)
				break;
			
			itemCount -= count;
			ch->MoveItem(TItemPos(CHEST_INVENTORY, cell), TItemPos(CHEST_INVENTORY, iEmptyPosition), count);
		}
	}

	ch->SetRequestPetTimer(get_global_time() + 3);//
}



ACMD(do_combo_up)
{
//i'm lazy..same as pet
	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}


    if (ch->IsObserverMode() || ch->IsDead())
        return;
	int a = ch->GetSkillLevel(122);
	if (a >= 2)
		return;
	ch->SetSkillLevel(122, a+1);
	ch->ComputePoints();
	ch->SkillLevelPacket();

	ch->SetRequestPetTimer(get_global_time() + 3);//
}
ACMD(do_combo_down)
{

//i'm lazy..same as pet
	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}


    if (ch->IsObserverMode() || ch->IsDead())
        return;
	int a = ch->GetSkillLevel(122);
	if (a <= 0)
		return;
	ch->SetSkillLevel(122, a-1);
	ch->ComputePoints();
	ch->SkillLevelPacket();

	ch->SetRequestPetTimer(get_global_time() + 3);//
}
ACMD(do_horse_upgrade)
{
//i'm lazy..same as pet
	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}


	if (ch->IsObserverMode() || ch->IsDead())
		return;
	int a = ch->GetSkillLevel(130);
	if (a >= 30)
		return;

	int y;
	if (a >= 0 && a <= 10) y = 1;
	else if (a >= 11 && a <= 20) y = 2;
	else if (a >= 21 && a <= 30) y = 3;
	else return;

    	int itemVnum = 50050;

    	LPITEM item = ch->FindSpecifyItem(itemVnum);
    	int itemCount = (item) ? item->GetCount() : 0;

    if (itemCount >= y) {
        ch->RemoveSpecifyItem(itemVnum, y);
        ch->SetHorseLevel(a + 1);
        ch->ComputePoints();
        ch->SkillLevelPacket();


        ch->ChatPacket(CHAT_TYPE_INFO, "You used %d x %s to upgrade horse level", y, item->GetName());
    } else {
        ch->ChatPacket(CHAT_TYPE_INFO, "You need %d x %s", y, item->GetName());
    }

	ch->SetRequestPetTimer(get_global_time() + 3);//
}


ACMD(do_polymorph_skill)
{
//i'm lazy..same as pet
	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}


    char arg1[256], arg2[256];

    two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
    if (*arg1)
    {
        if (strcmp(arg1, "691") != 0 && strcmp(arg1, "1091") != 0 &&
            strcmp(arg1, "1092") != 0 && strcmp(arg1, "1093") != 0) {
            ch->ChatPacket(CHAT_TYPE_INFO, "Invalid transformation");
            return;
        }


        DWORD dwVnum = 0;
        str_to_number(dwVnum, arg1);
        bool bMaintainStat = false;
        if (*arg2)
        {
            int value = 0;
            str_to_number(value, arg2);
            bMaintainStat = (value > 0);
        }

        //129-skill id
        int skillLevel = ch->GetSkillLevel(129);
	if (skillLevel ==0)
		return;
        int iDuration = skillLevel * 10;

        int yangCost = (skillLevel >= 1) ? 100000 : (100000 * (1 + skillLevel));

        if (ch->GetGold() < yangCost) {
            ch->ChatPacket(CHAT_TYPE_INFO, "You need %d yang to use the polymorph skill.", yangCost);
            return;
        }

        ch->SetGold(ch->GetGold()-yangCost);

 //       ch->SetPolymorph(dwVnum, bMaintainStat);

        ch->AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
        ch->ChatPacket(CHAT_TYPE_INFO, "You transformed into a %d for %d minutes at the cost of %d yang.", dwVnum, iDuration / 60, yangCost);
    }
	ch->SetRequestPetTimer(get_global_time() + 3);//
}


ACMD(do_pet_move)
{


	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bad usage");
		return;
	}

	long int x = 0, y = 0;
	str_to_number(x, arg1);
	str_to_number(y, arg2);

	CPetSystem* pPetSystem = ch->GetPetSystem();
	if (NULL != pPetSystem)
	{
			CPetActor* petActor = pPetSystem->GetByVnum(pPetSystem->GetPetVnum());

			if (NULL == petActor)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "No pet!");
				return;
			}

			if (!petActor->IsSummoned())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Dead/unsummoned");
				return;
			}


			pPetSystem->PetMouseFollow(x, y);
	}

	ch->SetRequestPetTimer(get_global_time() + 3);//
}


ACMD(do_pet_attack)
{

	if (ch->GetRequestPetTimer() > get_global_time()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
		return;
	}



	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || 0 == arg1[0])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bad usage!");
		return;
	}


//	if (arg1 == "stop")
	if (strcmp(arg1, "stop") == 0)
	{
		CPetSystem* pPetSystem = ch->GetPetSystem();
		if (NULL != pPetSystem)
		{
			CPetActor* petActor = pPetSystem->GetByVnum(pPetSystem->GetPetVnum());

			if (NULL == petActor)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "No pet!");
				return;
			}

			if (!petActor->IsSummoned())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Dead/unsummoned");
				return;
			}
			pPetSystem->StopAttack();
//			pPetSystem->SetPetAtak(false);
//			pPetSystem->SetVictim(NULL);
			pPetSystem->PetUpdateFollow();
		}
	}

//	else if (arg1 == "target")
	    else if (strcmp(arg1, "target") == 0)
	{
		if (!*arg2)
		{
			return;
		}

		DWORD vid = 0;
		str_to_number(vid, arg2);
		LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

    if (pkVictim == nullptr)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Target not found.");
        return;
    }

    if (pkVictim == ch)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "You cannot target yourself.");
        return;
    }

		if (pkVictim && pkVictim != ch && !ch->IsDead() && !pkVictim->IsDead() && !pkVictim->IsPC() && !pkVictim->IsPet() && !pkVictim->IsHorse())
		{
			CPetSystem* pPetSystem = ch->GetPetSystem();
			if (NULL != pPetSystem)
			{
				CPetActor* petActor = pPetSystem->GetByVnum(pPetSystem->GetPetVnum());

				if (NULL == petActor)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "No pet!");
					return;
				}
/*
				if (!petActor->IsSummoned())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "dead/usummoned");
					return;
				}
*/
				if ( pPetSystem->CountSummoned() > 0)
				{
					pPetSystem->LaunchAttack(pkVictim);
//					ch->ChatPacket(CHAT_TYPE_INFO, "start");
				}
			}
		}
	}
	else
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, "woops!");
		return;
	}
	ch->SetRequestPetTimer(get_global_time() + 3);//
}


ACMD(do_guild_skill_decrease)
{
    if (ch->GetRequestPetTimer() > get_global_time()) {
        ch->ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", ch->GetRequestPetTimer() - get_global_time());
        return;
    }

    char arg1[128];

    if (!one_argument(argument, arg1, sizeof(arg1))) {
//        ch->ChatPacket(CHAT_TYPE_INFO, "Invalid usage. Please provide a skill ID.");
        return;
    }


    DWORD skill_id = atoi(arg1);

    CGuild *guild = ch->GetGuild();
    if (!guild) {
        ch->ChatPacket(CHAT_TYPE_INFO, "You are not in a guild.");
        return;
    }

    if (ch->GetPlayerID() != guild->GetMasterPID()) {
        ch->ChatPacket(CHAT_TYPE_INFO, "You are not the leader of the guild.");
        return;
    }

    int skillLevel = guild->GetSkillLevel(skill_id);

    if (skillLevel <= 0) 
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "You do not have this skill in your guild.");
        return;
    }

    long cost = 10000000; // 10kk

    if (ch->GetGold() < cost) {
        ch->ChatPacket(CHAT_TYPE_INFO, "You do not have enough gold to decrease this skill. Needed: 10.000.000 Yang");
        return;
    }

    guild->SkillLevelDown(skill_id);
    ch->PointChange(POINT_GOLD, -cost);

    ch->ChatPacket(CHAT_TYPE_INFO, "Successfully decreased skill. You have paid %ld gold and received 1 Skill Point.", skill_id, cost);
    ch->SetRequestPetTimer(get_global_time() + 3);
}





ACMD(do_guild_deposit)
{

	if (!ch->CanDeposit()) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 잠시후에 이용해주십시오"));
		return;
	}
    int amount = atoi(argument);
    CGuild *guild = ch->GetGuild();

    if (amount <= 0)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Please enter a valid amount to deposit.");
        return;
    }

    if (guild->GetGuildMoney() + amount > 1999999999)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Guild storage cannot exceed 1,999,999,999.");
        return;
    }

    if (ch->GetGold() < amount)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "You do not have enough gold to deposit that amount.");
        return;
    }

    guild->RequestDepositMoney(ch, amount); //add to guild
    ch->ChatPacket(CHAT_TYPE_INFO, "Successfully deposited %d gold into the guild.", amount);
	ch->UpdateDepositPulse();
}

ACMD(do_guild_withdraw)
{
	if (!ch->CanDeposit()) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 잠시후에 이용해주십시오"));
		return;
	}


    int amount = atoi(argument);
    CGuild *guild = ch->GetGuild();

/////
    if (ch->GetPlayerID() != guild->GetMasterPID()) {
        ch->ChatPacket(CHAT_TYPE_INFO, "You are not the leader of the guild.");
        return;
    }
//only leader should be able to withdraw profits

    if (amount <= 0)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Please enter a valid amount to withdraw.");
        return;
    }

    if (amount > guild->GetGuildMoney())
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "The guild does not have enough money for that withdrawal.");
        return;
    }

    guild->RequestWithdrawMoney(ch, amount);
    ch->ChatPacket(CHAT_TYPE_INFO, "Successfully withdrew %d gold from the guild.", amount);
	ch->UpdateDepositPulse();
}


ACMD(do_change_leader)
{
    char newLeaderName[CHARACTER_NAME_MAX_LEN + 1];

    if (!one_argument(argument, newLeaderName, sizeof(newLeaderName))) {
//        ch->ChatPacket(CHAT_TYPE_INFO, "Usage: change_leader [newleadername]");
        return;
    }

    CGuild *guild = ch->GetGuild();
    if (!guild) {
        ch->ChatPacket(CHAT_TYPE_INFO, "You are not in a guild.");
        return;
    }

    if (ch->GetPlayerID() != guild->GetMasterPID()) {
        ch->ChatPacket(CHAT_TYPE_INFO, "You are not the leader of the guild.");
        return;
    }

    DWORD newLeaderPID = guild->GetMemberPID(newLeaderName);
    if (newLeaderPID == 0) {
        ch->ChatPacket(CHAT_TYPE_INFO, "No member with the name '%s' exists in the guild.", newLeaderName);
        return;
    }

/////////////////////////////
    if (guild->ChangeMasterTo(newLeaderPID)) {
        ch->ChatPacket(CHAT_TYPE_INFO, "You have successfully changed the guild leader to %s.", newLeaderName);
    } else {
        ch->ChatPacket(CHAT_TYPE_INFO, "Failed to change the guild leader. Please try again.");
    }
}




#ifdef ENABLE_NEW_STONE_DETACH
ACMD(do_detach_stone)
{
	if (!ch)
	{
		return;
	}

	if (ch->IsObserverMode() || ch->GetExchange() || ch->IsCubeOpen() || ch->IsOpenSafebox() || ch->GetMyShop() || ch->GetShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_WINDOWS_FIRST"));
		return;
	}

	const char* line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		return;
	}

	BYTE socketIdx;
	WORD scrollSlotPos, targetSlotPos;

	str_to_number(socketIdx, arg1);
	str_to_number(scrollSlotPos, arg2);
	str_to_number(targetSlotPos, arg3);

	const DWORD SCROLL_VNUM = 25100;
	const DWORD ITEM_BROKEN_METIN_VNUM = 28960;

	LPITEM scroll = ch->GetItem(TItemPos(INVENTORY, scrollSlotPos));

	if (!scroll || scroll->GetVnum() != SCROLL_VNUM)
	{
		return;
	}

	LPITEM item = ch->GetItem(TItemPos(INVENTORY, targetSlotPos));

	if (!item || (item->GetType() != ITEM_WEAPON && (item->GetType() != ITEM_ARMOR && item->GetSubType() != ARMOR_BODY)) || item->IsEquipped())
	{
		return;
	}

	long socket = item->GetSocket(socketIdx);

	if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
	{
		char hint[64];
		snprintf(hint, sizeof(hint), "USE_DETACHMENT, STONE: %ld", socket);
		LogManager::instance().ItemLog(ch, item, hint, item->GetName());

		item->SetSocket(socketIdx, 1);
		ch->AutoGiveItem(socket);
		scroll->SetCount(scroll->GetCount() - 1);
	}
}
#endif


#ifdef __TELEPORT_GUI__
ACMD(do_tp_new)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	DWORD dwTpIdx = 0;
	if (!str_to_number(dwTpIdx, arg1))
		return;
	const auto it = mapTeleportData.find(dwTpIdx);
	if (it == mapTeleportData.end())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "This tp request not has any data in game side. Please contact with game master!");
		return;
	}
	const DWORD NEED_TP_ITEM = 70007;
	if (ch->CountSpecifyItem(NEED_TP_ITEM) == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You don't have warp item!");
		return;
	}
	const WORD wLevel = ch->GetLevel();
	if ((it->second.wMinLevel != 0 && wLevel < it->second.wMinLevel) || (wLevel > it->second.wMaxLevel && it->second.wMaxLevel != 0) || ch->GetGold() < it->second.dwYang)
		return;
	if (it->second.dwYang)
		ch->PointChange(POINT_GOLD, -it->second.dwYang);
	ch->RemoveSpecifyItem(NEED_TP_ITEM, 1);
	ch->WarpSet(it->second.lX, it->second.lY);

}
#endif





