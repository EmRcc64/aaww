#include "stdafx.h"
#ifdef ENABLE_OFFLINESHOP_SYSTEM
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "offline_shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "db.h"
#include "questmanager.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "desc_client.h"
#include "group_text_parse_tree.h"
//#include <boost/algorithm/string/predicate.hpp>
#include <cctype>
#include "offlineshop_manager.h"
#include "p2p.h"
#include "entity.h"
#include "sectree_manager.h"
#include "target.h"
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
#include "offline_shop_search.h"
#endif

COfflineShopManager::COfflineShopManager(){Initialize();}
COfflineShopManager::~COfflineShopManager(){Initialize();}

void COfflineShopManager::Initialize()
{
	for (auto it = m_Map_pkOfflineShopByNPC.begin(); it != m_Map_pkOfflineShopByNPC.end(); it++)
		if (it->second)
			M2_DELETE(it->second);
	m_Map_pkOfflineShopByNPC.clear();
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
	if (m_pShopTimeEvent)
		event_cancel(&m_pShopTimeEvent);
	m_pShopTimeEvent = NULL;
	
	//m_itemMap  //all
    m_itemMapWeapon.clear();             // Clear all weapons
    m_itemMapArmor.clear();              // Clear all armors
    m_itemMapCostume.clear();            // Clear all costumes

    m_itemMapCosSash.clear();            // Clear costumes for sash (accessory)
    m_itemMapCosWeapon.clear();          // Clear costumes for weapon
    m_itemMapCosArmor.clear();           // Clear costumes for armor
    m_itemMapCosHair.clear();            // Clear costumes for hairstyle
    m_itemMapCosMount.clear();           // Clear costumes for mounts

    m_itemMapPet.clear();                // Clear costumes for pets

    m_itemMapWeaponSword.clear();        // Clear sword type weapons
    m_itemMapWeaponTwoHanded.clear();    // Clear two-handed weapons
    m_itemMapWeaponDagger.clear();       // Clear daggers
    m_itemMapWeaponBow.clear();          // Clear bows
    m_itemMapWeaponFan.clear();          // Clear fans
    m_itemMapWeaponBell.clear();         // Clear bells

    m_itemMapArmorBody.clear();          // Clear body armor
    m_itemMapArmorShield.clear();        // Clear shields
    m_itemMapArmorEarring.clear();       // Clear earrings
    m_itemMapArmorNecklace.clear();      // Clear necklaces
    m_itemMapArmorWrist.clear();         // Clear wrist items
    m_itemMapArmorFeet.clear();          // Clear feet armor
    m_itemMapArmorBelt.clear();          // Clear belts
    m_itemMapArmorHelmet.clear();        // Clear helmets
m_itemMapArmorPendant.clear();
m_itemMapArmorGlove.clear();


    m_itemMapSkillBook.clear();          // Clear skill books
    m_itemMapMaterial.clear();           // Clear materials
    m_itemMapSpecial.clear();             // Clear special items
m_itemMapConsumable.clear(); // potion
m_itemMapPotion.clear(); // potion
m_itemMapStone.clear(); // stone
m_itemMapChest.clear(); // chests
m_itemMapOthers.clear(); // others

	for (auto it = m_itemMap.begin(); it != m_itemMap.end(); it++)
		if (it->second)
			M2_DELETE(it->second);
	m_itemMap.clear();
#endif
}

EVENTINFO(TShopTimeEventInfo)
{
	DWORD	data;
	TShopTimeEventInfo(): data(0){}
};

EVENTFUNC(shop_time_event)
{
	if (!event)
		return 0;
	COfflineShopManager& shopMngr = COfflineShopManager::Instance();
	for (DWORD j = 0; j < shopMngr.m_Map_pkShopTimes.size(); ++j)
	{
		LPOFFLINESHOP pkOfflineShop = shopMngr.FindOfflineShopPID(shopMngr.m_Map_pkShopTimes[j]);
		if (!pkOfflineShop)
			continue;
		if (time(0) >= pkOfflineShop->m_data.time)
		{
			shopMngr.CloseOfflineShopForTime(pkOfflineShop);
			continue;
		}
	}
	return PASSES_PER_SEC(5);
}

void COfflineShopManager::CompareOffShopEventTime()
{
	if (!m_pShopTimeEvent)
		return;
	const int eventNextRun = event_time(m_pShopTimeEvent);
	long lowerTime = eventNextRun;
	for (DWORD j = 0; j < m_Map_pkShopTimes.size(); ++j)
	{
		LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(m_Map_pkShopTimes[j]);
		if (!pkOfflineShop)
			continue;
		const int shopTime = (pkOfflineShop->m_data.time - time(0));
		if (shopTime < lowerTime)
			lowerTime = shopTime;
	}
	if (lowerTime < 0)
		lowerTime = 0;
	event_reset_time(m_pShopTimeEvent, PASSES_PER_SEC(lowerTime));
}

void COfflineShopManager::CreateOfflineShop(TOfflineShop* offlineshop)
{
	if (!offlineshop)
		return;

	if (FindOfflineShopPID(offlineshop->owner_id) != NULL) {
		sys_err("COfflineShopManager::CreateOfflineShop: duplicate create offlineshop! owner_id=%d", offlineshop->owner_id);
		return;
	}

	if (time(0) > offlineshop->time)
	{
		bool status = false;
		for (DWORD i = 0; i < OFFLINE_SHOP_HOST_ITEM_MAX_NUM; ++i)
		{
			if (offlineshop->items[i].vnum != 0 && offlineshop->items[i].status == 0)
			{
				status = true;
				break;
			}
		}

		if(status)
			m_Map_pkOfflineShopCache.push_back(offlineshop->owner_id);
		return;
	}

	LPOFFLINESHOP pkOfflineShop = M2_NEW COfflineShop;
	//if (pkOfflineShop)
	{
		thecore_memcpy(&pkOfflineShop->m_data, offlineshop, sizeof(TOfflineShop));

		LPCHARACTER npc = NULL;
		if (g_bChannel == offlineshop->channel)
			npc = CHARACTER_MANAGER::instance().SpawnMob(offlineshop->type, offlineshop->mapindex, offlineshop->x, offlineshop->y, offlineshop->z, false, -1, false);
		pkOfflineShop->SetOfflineShopNPC(npc);
		m_Map_pkOfflineShopByNPC.insert(std::map<DWORD, COfflineShop*>::value_type(offlineshop->owner_id, pkOfflineShop));
		if (npc)
		{
			m_Map_pkShopTimes.push_back(offlineshop->owner_id);

			if (!m_pShopTimeEvent) {
				TShopTimeEventInfo* info = AllocEventInfo<TShopTimeEventInfo>();
				m_pShopTimeEvent = event_create(shop_time_event, info, PASSES_PER_SEC(offlineshop->time-time(0)));
			}

			npc->SetOfflineShop(pkOfflineShop);
			npc->SetName(offlineshop->owner_name);
			npc->Show(offlineshop->mapindex, offlineshop->x, offlineshop->y, offlineshop->z, true);
			npc->ViewReencode();
		}
		HasOfflineShop(CHARACTER_MANAGER::Instance().FindByPID(offlineshop->owner_id));
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
		for (DWORD i = 0; i < OFFLINE_SHOP_HOST_ITEM_MAX_NUM; ++i)
		{
			if (pkOfflineShop->m_data.items[i].vnum == 0 || pkOfflineShop->m_data.items[i].status != 0)
				continue;
			InsertItem(&pkOfflineShop->m_data.items[i]);
		}
#endif
		sys_log(0, "shop insert: owner_id %d owner_name %s sign %s x %ld y %ld mapIndex %d channel %d item_count %d", offlineshop->owner_id, offlineshop->owner_name, offlineshop->sign, offlineshop->x, offlineshop->y, offlineshop->mapindex, offlineshop->channel, pkOfflineShop->GetItemCount());
	}
}

LPOFFLINESHOP COfflineShopManager::FindOfflineShopPID(DWORD pid)
{
	auto it = m_Map_pkOfflineShopByNPC.find(pid);
	if (it == m_Map_pkOfflineShopByNPC.end())
		return NULL;
	return it->second;
}

void COfflineShopManager::OpenOfflineShop(LPCHARACTER ch)
{
	if (!ch)
		return;
	if (ch->CanOpenShopPanel())
		return;

	auto it = std::find(m_Map_pkOfflineShopCache.begin(), m_Map_pkOfflineShopCache.end(), ch->GetPlayerID());
	if (it != m_Map_pkOfflineShopCache.end())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenBackAllItem");
		return;
	}

	if (!HasOfflineShop(ch))
	{
		ch->SetOfflineShopPanel(true);
		char cmd[256];
		snprintf(cmd, sizeof(cmd), "OfflineShopSetFlag %llu", ch->GetOfflineShopFlag());
		ch->ChatPacket(CHAT_TYPE_COMMAND,cmd);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOfflineShop");
		return;
	}

	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (!pkOfflineShop)
		return;
	pkOfflineShop->AddGuest(ch, pkOfflineShop->GetOfflineShopNPC());
	sys_log(0, "COfflineShopManager::OpenOfflineShop owner open offlineshop panel: %s:%d", ch->GetName(), ch->GetPlayerID());
}

void COfflineShopManager::DestroyOfflineShopReal(DWORD ch)
{
	auto it = m_Map_pkOfflineShopByNPC.find(ch);
	if (it == m_Map_pkOfflineShopByNPC.end())
		return;
	LPOFFLINESHOP pkOfflineShop = it->second;
	if (!pkOfflineShop)
		return;

	auto it2 = std::find(m_Map_pkOfflineShopCache.begin(), m_Map_pkOfflineShopCache.end(), ch);
	if (it2 != m_Map_pkOfflineShopCache.end())
	{
		m_Map_pkOfflineShopCache.erase(it2);
	}

	pkOfflineShop->Destroy();
	m_Map_pkOfflineShopByNPC.erase(ch);
	M2_DELETE(pkOfflineShop);
	pkOfflineShop = NULL;
	HasOfflineShop(CHARACTER_MANAGER::Instance().FindByPID(ch));

}

void COfflineShopManager::DestroyOfflineShop(LPCHARACTER ch)
{
	if (!ch)
		return;
	if (ch->CanDestroyShop())
		return;

	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (!pkOfflineShop)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_FIND"));
		return;
	}
	else if (pkOfflineShop->GetItemCount() > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_CLOSE_SHOP"));
		return;
	}

	shop_owner n;
	n.subheader = DESTROY_OFFLINESHOP;
	n.owner_id = ch->GetPlayerID();
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_owner));
}

void COfflineShopManager::AddItemReal(OFFLINE_SHOP_ITEM* item)
{
	if (!item) {
		sys_err("COfflineShopManager::AddItemReal item data is null!!!!!!!!!");
		return;
	}
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(item->owner_id);
	if (!pkOfflineShop)
		return;
	thecore_memcpy(&pkOfflineShop->m_data.items[item->pos], item, sizeof(OFFLINE_SHOP_ITEM));
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
	InsertItem(&pkOfflineShop->m_data.items[item->pos]);
#endif
	pkOfflineShop->BroadcastUpdateItem(item->pos);
}

void COfflineShopManager::AddItem(LPCHARACTER ch, BYTE bDisplayPos, TItemPos bPos, long long iPrice)
{
	
	if (!ch || bDisplayPos >= OFFLINE_SHOP_HOST_ITEM_MAX_NUM)
		return;

	if (bDisplayPos >= 40)
	{
		BYTE cell = bDisplayPos - 40;
		if (!IS_SET(ch->GetOfflineShopFlag(), 1ULL<<cell))
			return;
	}
	
	if (!ch->CanAddItemShop())
		return;
	

	LPITEM pkItem = ch->GetItem(bPos);
	if (!pkItem)
		return;
	const TItemTable* item_table = pkItem->GetProto();
	if (!item_table)
		return;
	else if (item_table && (IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_ADD_ITEM"),pkItem->GetName());
		return;
	}
	else if (pkItem->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_ADD_ITEM"),pkItem->GetName());
		return;
	}
	else if (pkItem->IsEquipped())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_ADD_ITEM"),pkItem->GetName());
		return;
	}
	else if (pkItem->IsExchanging())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_ADD_ITEM"),pkItem->GetName());
		return;
	}

	const TItemTable * itemTable = pkItem->GetProto();
	if (itemTable && IS_SET(itemTable->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP))
		return;
	
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (!pkOfflineShop)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_FIND"));
		return;
	}

	OFFLINE_SHOP_ITEM item;
	item.vnum = pkItem->GetVnum();
	item.id = pkItem->GetID();
	item.owner_id = ch->GetPlayerID();
	item.pos = bDisplayPos;
	item.count = pkItem->GetCount();
	item.price = iPrice;
	item.status = 0;
	thecore_memcpy(item.alSockets, pkItem->GetSockets(), sizeof(item.alSockets));
	thecore_memcpy(item.aAttr, pkItem->GetAttributes(), sizeof(item.aAttr));
#ifdef __CHANGELOOK_SYSTEM__
	item.transmutation = pkItem->GetTransmutation();
#endif

	strlcpy(item.szOwnerName, ch->GetName(), sizeof(item.szOwnerName));
	pkItem->RemoveFromCharacter();
	ITEM_MANAGER::instance().SaveSingleItem(pkItem);

	shop_item n;
	n.subheader = ADD_ITEM;
	thecore_memcpy(&n.item, &item, sizeof(n.item));
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_item));
}

void COfflineShopManager::RemoveItemReal(OFFLINE_SHOP_ITEM* item)
{
	if (!item)
		return;
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(item->owner_id);
	if (!pkOfflineShop)
		return;
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
	ClearItem(pkOfflineShop->m_data.items[item->pos].id);
#endif
	memset(&pkOfflineShop->m_data.items[item->pos], 0, sizeof(OFFLINE_SHOP_ITEM));
	pkOfflineShop->BroadcastUpdateItem(item->pos, true);
	if (item->status == 0)
	{
		LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindByPID(item->owner_id);
		if (ch != NULL)
		{
			LPITEM pItem = ch->AutoGiveItem(item->vnum, item->count,0,true);
			if (pItem)
			{
				pItem->SetAttributes(item->aAttr);
				pItem->SetSockets(item->alSockets);
#ifdef __CHANGELOOK_SYSTEM__
				pItem->SetTransmutation(item->transmutation);
#endif
				pItem->UpdatePacket();
				//int iEmptyPos = -1;
				//iEmptyPos = ch->CalculateItemPos(pItem);
				//if (iEmptyPos < 0)
				//{
				//	pItem->AddToGround(ch->GetMapIndex(), ch->GetXYZ());
				//	pItem->StartDestroyEvent();
				//	pItem->SetOwnership(ch, 500);
				//}
				//else
				//{
				//	pItem->AddToCharacter(ch, TItemPos(pItem->IsDragonSoul() ? DRAGON_SOUL_INVENTORY : INVENTORY, iEmptyPos));
				//	pItem->Save();
				//}
			}
		}
	}
}

void COfflineShopManager::RemoveItem(LPCHARACTER ch, BYTE bPos)
{
	if (!ch || bPos >= OFFLINE_SHOP_HOST_ITEM_MAX_NUM)
		return;
	if (ch->CanRemoveItemShop())
		return;
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (!pkOfflineShop)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_FIND"));
		return;
	}
	TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(pkOfflineShop->m_data.items[bPos].vnum);
	if (!itemTable)
		return;

//	LPITEM checkItem = ITEM_MANAGER::Instance().CreateItem(pkOfflineShop->m_data.items[bPos].vnum);
//	int emptyCell = -1;
//	if (checkItem->IsDragonSoul())
//		emptyCell = ch->GetEmptyInventory(checkItem);
//	else
//		emptyCell = ch->GetEmptyInventory(checkItem);
//	M2_DESTROY_ITEM(checkItem);
//	if(emptyCell == -1)
//	{
//		ch->ChatPacket(CHAT_TYPE_INFO, "You don't has enought space for remove!");
//		return;
//	}

	shop_item n;
	n.subheader = REMOVE_ITEM;
	thecore_memcpy(&n.item, &pkOfflineShop->m_data.items[bPos], sizeof(n.item));
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_item));
}

void COfflineShopManager::StopShopping(LPCHARACTER ch)
{
	if(!ch)
		return;

	if (ch->GetOfflineShopPanel())
	{
		ch->SetOfflineShopPanel(false);

		return;
	}

	LPOFFLINESHOP pkOfflineShop = ch->GetOfflineShop();
	if (!pkOfflineShop)
		return;

	pkOfflineShop->RemoveGuest(ch);
}

void SendReturnPacket(LPCHARACTER ch, BYTE ret)
{
	if (ch)
	{
		TPacketGCShop pack;
		pack.header = HEADER_GC_OFFLINE_SHOP;
		pack.subheader = ret;
		pack.size = sizeof(TPacketGCShop);
		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}

void COfflineShopManager::BuyItemReal(TOfflineShopBuy* item)
{
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(item->item.owner_id);
	if (!pkOfflineShop)
		return;
	thecore_memcpy(&pkOfflineShop->m_data.items[item->item.pos] ,&item->item, sizeof(pkOfflineShop->m_data.items[item->item.pos]));

//
    long long tax = (item->item.price * 5) / 100;
    if (tax < 1)
    {
        tax = 1;
    };
    long long sellerEarnings = item->item.price - tax;
	pkOfflineShop->m_data.price += sellerEarnings;
//
//	pkOfflineShop->m_data.price += item->item.price;

	if (item->log_index >= 0 && item->log_index < OFFLINE_SHOP_HOST_ITEM_MAX_NUM)
		thecore_memcpy(&pkOfflineShop->m_data.log[item->log_index], &item->log, sizeof(pkOfflineShop->m_data.log[item->log_index]));
	pkOfflineShop->BroadcastUpdateItem(item->item.pos,false, item->log_index);
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
	ClearItem(item->item.id);
#endif
	LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindByPID(item->customer_id);
	if (ch != NULL)
	{
		LPITEM pItem = ch->AutoGiveItem(item->item.vnum, item->item.count,0,true);
		if (pItem != NULL)
		{
			pItem->SetAttributes(item->item.aAttr);
			pItem->SetSockets(item->item.alSockets);
#ifdef __CHANGELOOK_SYSTEM__
			pItem->SetTransmutation(item->item.transmutation);
#endif
			pItem->UpdatePacket();
			//int iEmptyPos = -1;
			//iEmptyPos = ch->CalculateItemPos(pItem);
			//if (iEmptyPos < 0)
			//{
			//	pItem->AddToGround(ch->GetMapIndex(), ch->GetXYZ());
			//	pItem->StartDestroyEvent();
			//	pItem->SetOwnership(ch, 500);
			//	
			//}
			//else
			//{
			//	pItem->AddToCharacter(ch, TItemPos(pItem->IsDragonSoul() ? DRAGON_SOUL_INVENTORY : INVENTORY, iEmptyPos));
			//	pItem->Save();
			//}
			SendReturnPacket(ch, SHOP_SUBHEADER_GC_OK);
		}
	}
}

void COfflineShopManager::Buy(LPCHARACTER ch, DWORD vid, BYTE pos)
{
	if(!ch)
		return;
	if (ch->CanBuyItemOfflineShop())
		return;

	//LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(vid);
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(GetOwnerPID(vid));
	if (!pkOfflineShop)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_FIND"));
		return;
	}

	OFFLINE_SHOP_ITEM& item = pkOfflineShop->m_data.items[pos];
	
	if (item.id != vid)
		return;
	
//	LPITEM checkItem = ITEM_MANAGER::Instance().CreateItem(item.vnum);
//	if(!checkItem)
//		return;
//	int emptyCell = -1;
//	if (checkItem->IsDragonSoul())
//		emptyCell = ch->GetEmptyInventory(checkItem);
//	else
//		emptyCell = ch->GetEmptyInventory(checkItem);
//	M2_DESTROY_ITEM(checkItem);
//	if(emptyCell == -1)
//	{
//		ch->ChatPacket(CHAT_TYPE_INFO, "You don't has enought space for buy!");
//		return;
//	}
	
	if (item.owner_id == ch->GetPlayerID()){
		//ch->ChatPacket(CHAT_TYPE_INFO, "You can't buy your own shop!");
		return;
	}
	else if (item.status != 0){
		SendReturnPacket(ch, SHOP_SUBHEADER_GC_SOLD_OUT);
		return;
	}
	if (ch->GetGold() < item.price) {
		SendReturnPacket(ch, SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY);
		return;
	}
	
	ch->PointChange(POINT_GOLD, -item.price);

	shop_buy n;
	n.subheader = BUY_ITEM;
	memset(&n.buyItem, 0, sizeof(TOfflineShopBuy));
	thecore_memcpy(&n.buyItem.item, &item, sizeof(OFFLINE_SHOP_ITEM));
	n.buyItem.customer_id = ch->GetPlayerID();
	strlcpy(n.buyItem.customer_name, ch->GetName(), sizeof(n.buyItem.customer_name));
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_buy));
}

bool COfflineShopManager::HasOfflineShop(LPCHARACTER ch)
{
	if (!ch)
		return false;
	BYTE ptr = 1;
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (!pkOfflineShop)
		ptr = 0;
	TPacketGCShop p;
	p.header = HEADER_GC_OFFLINE_SHOP;
	p.subheader = SHOP_SUBHEADER_GC_CHECK_RESULT;
	p.size = sizeof(p)+sizeof(BYTE);
	ch->GetDesc()->BufferedPacket(&p, sizeof(p));
	ch->GetDesc()->Packet(&ptr, sizeof(BYTE));
	return ptr;
}


void COfflineShopManager::ChangeTitleReal(TOfflineShopChangeTitle* p)
{
	if (!p)
		return;
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(p->owner_id);
	if (!pkOfflineShop)
		return;
	strlcpy(pkOfflineShop->m_data.sign, p->sign, sizeof(pkOfflineShop->m_data.sign));

	LPCHARACTER shop = pkOfflineShop->GetOfflineShopNPC();
	if (shop)
	{
		TPacketGCShopSign p;
		p.bHeader = HEADER_GC_OFFLINE_SHOP_SIGN;
		p.dwVID = shop->GetVID();
		strlcpy(p.szSign, pkOfflineShop->m_data.sign, sizeof(p.szSign));
		shop->PacketAround(&p, sizeof(TPacketGCShopSign));
	}

	TPacketGCShop pack;
	TEMP_BUFFER buf;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_CHANGE_TITLE;
	pack.size = sizeof(pack) + sizeof(pkOfflineShop->m_data.sign);
	buf.write(&pack, sizeof(pack));
	buf.write(&pkOfflineShop->m_data.sign, sizeof(pkOfflineShop->m_data.sign));
	pkOfflineShop->Broadcast(buf.read_peek(), buf.size());
}

void COfflineShopManager::ChangeTitle(LPCHARACTER ch, const char* title)
{
	if (!ch || getInjectText(title))
		return;	
	if (ch->CanChangeTitle())
		return;

	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (!pkOfflineShop)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_FIND"));
		return;
	}

	char sign[SHOP_SIGN_MAX_LEN + 1];
	snprintf(sign, sizeof(sign), "%c%s", pkOfflineShop->m_data.sign[0], title);


	shop_title n;
	n.subheader = CHANGE_TITLE;
	n.title.owner_id = ch->GetPlayerID();
	strlcpy(n.title.sign, sign, sizeof(n.title.sign));
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_title));
}

void COfflineShopManager::WithdrawMoneyReal(DWORD ch)
{
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch);
	if (!pkOfflineShop)
		return;
	
	LPCHARACTER rch = CHARACTER_MANAGER::Instance().FindByPID(ch);
	if (rch != NULL && rch->GetOfflineShop())
	{
		rch->PointChange(POINT_GOLD, pkOfflineShop->m_data.price);
		long long price = 0;
		TPacketGCShop pack;
		TEMP_BUFFER buf;
		pack.header = HEADER_GC_OFFLINE_SHOP;
		pack.subheader = SHOP_SUBHEADER_GC_REFRESH_MONEY;
		pack.size = sizeof(pack) + sizeof(long long);
		buf.write(&pack, sizeof(pack));
		buf.write(&price, sizeof(long long));
		rch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
	pkOfflineShop->m_data.price = 0;
}

void COfflineShopManager::WithdrawMoney(LPCHARACTER ch)
{
	if (!ch)
		return;
	if (ch->CanWithdrawMoney())
		return;
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (!pkOfflineShop)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_FIND"));
		return;
	}
	if (pkOfflineShop->m_data.price == 0)
		return;
	const long long nTotalMoney = static_cast<long long>(ch->GetGold()) + static_cast<long long>(pkOfflineShop->m_data.price);
	if (GOLD_MAX <= nTotalMoney)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_GET_MONEY"));
		return;
	}

	shop_owner n;
	n.subheader = WITHDRAW_MONEY;
	n.owner_id = ch->GetPlayerID();
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_owner));
}

void COfflineShopManager::ShopLogRemove(LPCHARACTER ch)
{
	if (!ch)
		return;
	if (ch->CanRemoveLogShop())
		return;

	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (!pkOfflineShop)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_FIND"));
		return;
	}
	if (pkOfflineShop->m_data.log[0].itemVnum == 0)
		return;
	shop_owner n;
	n.subheader = CLEAR_LOG;
	n.owner_id = ch->GetPlayerID();
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_owner));
}

void COfflineShopManager::ShopLogRemoveReal(DWORD ch)
{
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch);
	if (!pkOfflineShop)
		return;
	memset(&pkOfflineShop->m_data.log, 0, sizeof(pkOfflineShop->m_data.log));
	LPCHARACTER rch = CHARACTER_MANAGER::Instance().FindByPID(ch);
	if (rch != NULL && rch->GetOfflineShop())
		rch->ChatPacket(CHAT_TYPE_COMMAND, "ClearOfflineShopLog");
}

void COfflineShopManager::ChangeDecoration(LPCHARACTER ch, TShopDecoration* data)
{
	if (!ch || !data)
		return;
	if (ch->CanChangeDecoration())
		return;
	if (getInjectText(data->sign))
		return;

	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (!pkOfflineShop)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_FIND"));
		return;
	}

	if (data->vnum > 15 || data->type > 6)
		return;
	else if (data->vnum+30000 == pkOfflineShop->m_data.type && strstr(pkOfflineShop->m_data.sign, data->sign))
		return;
	data->owner_id = ch->GetPlayerID();
	shop_decoration n;
	n.subheader = CHANGE_DECORATION;
	thecore_memcpy(&n.decoration, data, sizeof(n.decoration));
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_decoration));

}

void COfflineShopManager::ChangeDecorationReal(TShopDecoration* ch)
{
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->owner_id);
	if (!pkOfflineShop)
		return;

	if (pkOfflineShop->m_data.type == 30000 + ch->vnum && strstr(pkOfflineShop->m_data.sign, ch->sign))
		return;

	pkOfflineShop->m_data.type = 30000 + ch->vnum;
	strlcpy(pkOfflineShop->m_data.sign, ch->sign, sizeof(pkOfflineShop->m_data.sign));
	LPCHARACTER shop = pkOfflineShop->GetOfflineShopNPC();
	if (shop)
	{
		if (shop->GetRaceNum() == pkOfflineShop->m_data.type)
		{
			shop->ViewReencode();
		}
		else
		{
			M2_DESTROY_CHARACTER(shop);
			pkOfflineShop->SetOfflineShopNPC(NULL);
			shop = CHARACTER_MANAGER::instance().SpawnMob(pkOfflineShop->m_data.type, pkOfflineShop->m_data.mapindex, pkOfflineShop->m_data.x, pkOfflineShop->m_data.y, pkOfflineShop->m_data.z, false, -1, false);
			if (shop)
			{
				pkOfflineShop->SetOfflineShopNPC(shop);
				shop->SetOfflineShop(pkOfflineShop);
				shop->SetName(pkOfflineShop->m_data.owner_name);
				shop->Show(pkOfflineShop->m_data.mapindex, pkOfflineShop->m_data.x, pkOfflineShop->m_data.y, pkOfflineShop->m_data.z, true);
				shop->ViewReencode();
			}
		}
	}
	pkOfflineShop->BroadcastUpdateItem(0); 
}


struct FuncSearchNearShops
{
	long m_lNewX;
	long m_lNewY;
	bool m_bResult;
	LPCHARACTER m_ch;
	FuncSearchNearShops(LPCHARACTER ch, long lNewX, long lNewY) { m_ch = ch; m_bResult = false; m_lNewX = lNewX; m_lNewY = lNewY; }
	void operator() (LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER)ent;
			if (ch && ch->IsOfflineShopNPC())
			{
				if (DISTANCE_APPROX(ch->GetX() - m_lNewX, ch->GetY() - m_lNewY) < 300)
					m_bResult = true;
			}
		}
	}
};


bool HasNearOfflineShop(LPCHARACTER ch, long newX, long newY)
{
	LPSECTREE pSec = ch->GetSectree();
	if (!pSec)
		return true;
	FuncSearchNearShops f(ch, newX, newY);
	pSec->ForEachAround(f);
	if (f.m_bResult == true)
		return true;
	return false;
}

void COfflineShopManager::OpenMyOfflineShop(LPCHARACTER ch, const char* c_pszSign, TOfflineShopItemTable* pTable, BYTE bItemCount, DWORD shopVnum, BYTE titleType)
{
	
	if (bItemCount == 0)
		return;
	if (ch->CanCreateShop())
		return;

	if (ch->GetMapIndex() != 1 && ch->GetMapIndex() != 41)//woops, shop doar map1
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't open a shop in this map"));
		return;
	}


	

	if (shopVnum >= 30001 || titleType >= 1)
	{
		if (!ch->FindAffect(AFFECT_DECORATION))
		{
			shopVnum = 30000;
			titleType = 0;
		}
	}


	char szSign[SHOP_SIGN_MAX_LEN + 1];
	snprintf(szSign, sizeof(szSign), "%d%s", titleType, c_pszSign);
	if (strlen(c_pszSign) == 0 || strstr(szSign, "%") || strstr(szSign, "'") || getInjectText(szSign))
		return;
	else if (HasNearOfflineShop(ch, ch->GetX(), ch->GetY()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_CREATE_PLACE"));
		return;
	}
	
	std::set<TItemPos> cont;
	for (BYTE i = 0; i < bItemCount; ++i)
	{
		if (cont.find((pTable+i)->pos) != cont.end())
		{
			sys_err("MY_OFFLINE_SHOP: duplicate shop item detected! (name: %s)", ch->GetName());
			return;
		}
		LPITEM pkItem = ch->GetItem((pTable + i)->pos);

		if ((pTable + i)->display_pos >= 40)
		{
			BYTE cell = (pTable + i)->display_pos - 40;
			if (!IS_SET(ch->GetOfflineShopFlag(), 1ULL<<cell))
				return;
		}

		if (pkItem != NULL)
		{
			const TItemTable* item_table = pkItem->GetProto();
			if (!item_table)
				return;
			else if (item_table && (IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_ADD_ITEM"),pkItem->GetName());
				return;
			}
			else if (pkItem->IsEquipped() == true)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_ADD_ITEM"),pkItem->GetName());
				return;
			}
			else if (true == pkItem->isLocked())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_ADD_ITEM"),pkItem->GetName());
				return;
			}
			else if (true == pkItem->IsExchanging())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_ADD_ITEM"),pkItem->GetName());
				return;
			}
		}
	}

	//ch->PointChange(POINT_GOLD, -100000);//woops no price
	TOfflineShop m_data;
	m_data.owner_id = ch->GetPlayerID();
	strlcpy(m_data.owner_name, ch->GetName(), sizeof(m_data.owner_name));
	strlcpy(m_data.sign, szSign, sizeof(m_data.sign));
	m_data.x = ch->GetX();
	m_data.y = ch->GetY();
	m_data.z = ch->GetZ();
	m_data.mapindex = ch->GetMapIndex();
	m_data.type = shopVnum;
	m_data.channel = g_bChannel;
	m_data.slotflag = ch->GetOfflineShopFlag();
	//m_data.time = time(0) + (60*60*12);
	m_data.time = time(0) + (60 * 60 * 24 * 3); // 7 días//
	memset(&m_data.items, 0, sizeof(OFFLINE_SHOP_ITEM) * OFFLINE_SHOP_HOST_ITEM_MAX_NUM);
	for (int i = 0; i < bItemCount; ++i)
	{
		LPITEM pkItem = ch->GetItem(pTable->pos);
		if (!pkItem)
			continue;
		if (!pkItem->GetVnum())
			continue;

		TItemTable* item_table = ITEM_MANAGER::instance().GetTable(pkItem->GetVnum());
		if (!item_table)
		{
			sys_err("OfflineShop: no item table by item vnum #%d", pkItem->GetVnum());
			continue;
		}

		OFFLINE_SHOP_ITEM& item = m_data.items[pTable->display_pos];
		item.vnum = pkItem->GetVnum();
		item.id = pkItem->GetID();
		item.owner_id = ch->GetPlayerID();
		item.pos = pTable->display_pos;
		item.count = pkItem->GetCount();
		item.price = pTable->price;
		item.status = 0;
		thecore_memcpy(item.alSockets, pkItem->GetSockets(), sizeof(item.alSockets));
		thecore_memcpy(item.aAttr, pkItem->GetAttributes(), sizeof(item.aAttr));
#ifdef __CHANGELOOK_SYSTEM__
		item.transmutation = pkItem->GetTransmutation();
#endif
		strlcpy(item.szOwnerName, ch->GetName(), sizeof(item.szOwnerName));
		strlcpy(item.szBuyerName, "NONAME", sizeof(item.szBuyerName));
		pkItem->RemoveFromCharacter();
		ITEM_MANAGER::instance().SaveSingleItem(pkItem);
		++pTable;
	}
	

	shop_create n;
	n.subheader = CREATE_OFFLINESHOP;
	thecore_memcpy(&n.offlineshop, &m_data, sizeof(n.offlineshop));
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_create));
}

void COfflineShopManager::OpenSlotReal(TOfflineShopOpenSlot* ch)
{
	if (!ch)
		return;
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->owner_id);
	if (!pkOfflineShop)
		return;
	pkOfflineShop->m_data.slotflag = ch->flag;
	pkOfflineShop->BroadcastUpdateItem(0);
}


void COfflineShopManager::OpenSlot(LPCHARACTER ch, BYTE bPos)
{
	if (!ch)
		return;
	long long myFlag = ch->GetOfflineShopFlag();
	long long flag = 1ULL << bPos;
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (IS_SET(myFlag, flag))
		return;
	else if (ch->CountSpecifyItem(72320) <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_SLOT_OPEN"));
		return;
	}

	if (pkOfflineShop)
	{
		SET_BIT(myFlag, flag);
		ch->SetOfflineShopFlag(myFlag);
		ch->RemoveSpecifyItem(72320, 1);

		shop_slot n;
		n.subheader = CHANGE_OPEN_SLOT;
		n.ch.owner_id = ch->GetPlayerID();
		n.ch.flag = myFlag;
		db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_slot));
	}
	else if (ch->GetOfflineShopPanel())
	{
		ch->RemoveSpecifyItem(72320, 1);
		SET_BIT(myFlag, flag);
		ch->SetOfflineShopFlag(myFlag);

		char cmd[256];
		snprintf(cmd, sizeof(cmd), "OfflineShopSetFlag %llu", myFlag);
		ch->ChatPacket(CHAT_TYPE_COMMAND, cmd);
	}
	ch->Save();
	
}

void COfflineShopManager::CloseOfflineShopForTimeReal(DWORD offlineshop)
{
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(offlineshop);
	if (!pkOfflineShop)
		return;
	
	bool status = false;
	for (DWORD i = 0; i < OFFLINE_SHOP_HOST_ITEM_MAX_NUM; ++i)
		if (pkOfflineShop->m_data.items[i].vnum != 0 && pkOfflineShop->m_data.items[i].status == 0) {
			ClearItem(pkOfflineShop->m_data.items[i].id);
			if (!status)
				status = true;
		}
	if (status)
		m_Map_pkOfflineShopCache.push_back(offlineshop);

	pkOfflineShop->Destroy();
	//pkOfflineShop = NULL;
	m_Map_pkOfflineShopByNPC.erase(offlineshop);
	HasOfflineShop(CHARACTER_MANAGER::Instance().FindByPID(offlineshop));
	M2_DELETE(pkOfflineShop);
}

void COfflineShopManager::CloseOfflineShopForTime(LPOFFLINESHOP offlineshop)
{
	if (!offlineshop)
		return;

	shop_owner n;
	n.subheader = TIME_DONE;
	n.owner_id = offlineshop->m_data.owner_id;
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_owner));
}


void COfflineShopManager::GetBackItem(LPCHARACTER ch)
{
	if (!ch)
		return;
	if (ch->CanGetBackItems())
		return;

	auto it = std::find(m_Map_pkOfflineShopCache.begin(), m_Map_pkOfflineShopCache.end(), ch->GetPlayerID());
	if (it== m_Map_pkOfflineShopCache.end())
		return;

	shop_owner n;
	n.subheader = GET_BACK_ITEM;
	n.owner_id = ch->GetPlayerID();
	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_owner));
}


void COfflineShopManager::GetBackItemReal(TOfflineShopBackItem* ch)
{
	if (!ch)
		return;

	LPCHARACTER t = CHARACTER_MANAGER::Instance().FindByPID(ch->owner_id);
	if (t)
	{
		
		for (DWORD i = 0; i < OFFLINE_SHOP_HOST_ITEM_MAX_NUM; ++i)
		{
			if (ch->items[i].vnum == 0 || ch->items[i].status == 1)
				continue;
			LPITEM pItem = t->AutoGiveItem(ch->items[i].vnum, ch->items[i].count,0,true);
			if (pItem != NULL)
			{
				pItem->SetAttributes(ch->items[i].aAttr);
				pItem->SetSockets(ch->items[i].alSockets);
#ifdef __CHANGELOOK_SYSTEM__
				pItem->SetTransmutation(ch->items[i].transmutation);
#endif
				pItem->UpdatePacket();
				//int iEmptyPos = -1;
				//iEmptyPos = t->CalculateItemPos(pItem);
				//if (iEmptyPos < 0)
				//{
				//	pItem->AddToGround(t->GetMapIndex(), t->GetXYZ());
				//	pItem->StartDestroyEvent();
				//	pItem->SetOwnership(t, 500);
				//}
				//else
				//{
				//	pItem->AddToCharacter(t, TItemPos(pItem->IsDragonSoul() ? DRAGON_SOUL_INVENTORY : INVENTORY, iEmptyPos));
				//	pItem->Save();
				//}

				sys_log(0, "GetBackItemReal: %s item vnum %d count %d price %llu", t->GetName(),pItem->GetVnum(), pItem->GetCount(), ch->items[i].price);
			}
		}
	}

	auto it = std::find(m_Map_pkOfflineShopCache.begin(), m_Map_pkOfflineShopCache.end(), ch->owner_id);
	if (it != m_Map_pkOfflineShopCache.end())
	{
		m_Map_pkOfflineShopCache.erase(it);
	}

	auto it2 = m_Map_pkOfflineShopByNPC.find(ch->owner_id);
	if (it2 != m_Map_pkOfflineShopByNPC.end())
	{
		LPOFFLINESHOP pkOfflineShop = it2->second;
		if (pkOfflineShop) {
			pkOfflineShop->Destroy();
			M2_DELETE(pkOfflineShop);
			pkOfflineShop = NULL;
			m_Map_pkOfflineShopByNPC.erase(ch->owner_id);
		}
	}

	HasOfflineShop(t);
}

void COfflineShopManager::ShopAddTime(LPCHARACTER ch)
{
	if (!ch)
		return;
	if (ch->CanAddTimeShop())
		return;
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch->GetPlayerID());
	if (!pkOfflineShop)
		return;
	int oldtime = pkOfflineShop->m_data.time - time(0);
	if (oldtime > (60 * 60 * 24 * 15))
		return;

	ch->PointChange(POINT_GOLD, -100000);//price only to extend
	shop_owner n;
	n.subheader = ADD_TIME;
	n.owner_id = ch->GetPlayerID();

	db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP,0, &n, sizeof(shop_owner));
}

void COfflineShopManager::ShopAddTimeReal(DWORD ch)
{
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(ch);
	if (!pkOfflineShop)
		return;
	int oldtime = pkOfflineShop->m_data.time - time(0);
	if(oldtime < 0)
		oldtime = 0;
	//const int addtime = (60 * 60 * 12);
	const int addtime = (60 * 60 * 24);
	//int newTime = MIN((60 * 60 * 24 * 3) - oldtime, addtime);
	pkOfflineShop->m_data.time = time(0) + (oldtime + addtime);
	pkOfflineShop->BroadcastUpdateItem(0);
}

void COfflineShopManager::RecvPackets(const char* data)
{
	BYTE sub_header = (BYTE)*data;
	switch (sub_header)
	{
		case CREATE_OFFLINESHOP:
		{
			shop_create* p = (shop_create*)data;
			if (p)
				CreateOfflineShop(&p->offlineshop);
		}
		break;

		case DESTROY_OFFLINESHOP:
		{
			shop_owner* p = (shop_owner*)data;
			if (p)
				DestroyOfflineShopReal(p->owner_id);
		}
		break;

		case REMOVE_ITEM:
		{
			shop_item* p = (shop_item*)data;
			if (p)
				RemoveItemReal(&p->item);
		}
		break;

		case ADD_ITEM:
		{
			shop_item* p = (shop_item*)data;
			if (p)
				AddItemReal(&p->item);
		}
		break;

		case BUY_ITEM:
		{
			shop_buy* p = (shop_buy*)data;
			if (p)
				BuyItemReal(&p->buyItem);
		}
		break;

		case WITHDRAW_MONEY:
		{
			shop_owner* p = (shop_owner*)data;
			if (p)
				WithdrawMoneyReal(p->owner_id);
		}
		break;

		case CHANGE_TITLE:
		{
			shop_title* p = (shop_title*)data;
			if (p)
				ChangeTitleReal(&p->title);
		}
		break;

		case CLEAR_LOG:
		{
			shop_owner* p = (shop_owner*)data;
			if (p)
				ShopLogRemoveReal(p->owner_id);
		}
		break;

		case CHANGE_DECORATION:
		{
			shop_decoration* p = (shop_decoration*)data;
			if (p)
				ChangeDecorationReal(&p->decoration);
		}
		break;

		case CHANGE_OPEN_SLOT:
		{
			shop_slot* p = (shop_slot*)data;
			if (p)
				OpenSlotReal(&p->ch);
		}
		break;

		case TIME_DONE:
		{
			shop_owner* p = (shop_owner*)data;
			if (p)
				CloseOfflineShopForTimeReal(p->owner_id);
		}
		break;

		case GET_BACK_ITEM:
		{
			shop_back* p = (shop_back*)data;
			if (p)
				GetBackItemReal(&p->back);
		}
		break;

		case ADD_TIME:
		{
			shop_owner* p = (shop_owner*)data;
			if(p)
				ShopAddTimeReal(p->owner_id);
		}
		break;

	}
}

void COfflineShopManager::OpenOfflineShopWithVID(LPCHARACTER ch, DWORD vid)
{
	if (!ch)
		return;
	if (ch->CanOpenOfflineShop())
		return;
	LPOFFLINESHOP pkOfflineShop = FindOfflineShopPID(vid);
	if (!pkOfflineShop)
		return;
	pkOfflineShop->AddGuest(ch, pkOfflineShop->GetOfflineShopNPC());
}
#endif


