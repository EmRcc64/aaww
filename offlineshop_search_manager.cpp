#include "stdafx.h"

#ifdef ENABLE_OFFLINESHOP_SYSTEM
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "char.h"
#include "item_manager.h"
#include "packet.h"
#include "offline_shop_search.h"
#include "offlineshop_manager.h"

DWORD COfflineShopManager::GetOwnerPID(DWORD itemID)
{
	const auto it = m_itemMap.find(itemID);
	return it != m_itemMap.end() ? it->second->owner_id : 0;
}

void COfflineShopManager::SaveInMemory(OFFLINE_SHOP_ITEM* item)
{
    auto pItemTable = ITEM_MANAGER::instance().GetTable(item->vnum);
    if (nullptr == pItemTable)
        return;

    // Insert item into appropriate maps based on type and subtype
    switch (pItemTable->bType)
    {
        case ITEM_WEAPON:
            m_itemMapWeapon.insert(std::make_pair(item->id, item));
            switch (pItemTable->bSubType)
            {
                case WEAPON_SWORD:
                    m_itemMapWeaponSword.insert(std::make_pair(item->id, item));
                    break;
                case WEAPON_TWO_HANDED:
                    m_itemMapWeaponTwoHanded.insert(std::make_pair(item->id, item));
                    break;
                case WEAPON_DAGGER:
                    m_itemMapWeaponDagger.insert(std::make_pair(item->id, item));
                    break;
                case WEAPON_BOW:
                    m_itemMapWeaponBow.insert(std::make_pair(item->id, item));
                    break;
                case WEAPON_FAN:
                    m_itemMapWeaponFan.insert(std::make_pair(item->id, item));
                    break;
                case WEAPON_BELL:
                    m_itemMapWeaponBell.insert(std::make_pair(item->id, item));
                    break;
                default:
                    break;
            }
            break;

        case ITEM_ARMOR:
            m_itemMapArmor.insert(std::make_pair(item->id, item));
            switch (pItemTable->bSubType)
            {
                case ARMOR_BODY:
                    m_itemMapArmorBody.insert(std::make_pair(item->id, item));
                    break;
                case ARMOR_SHIELD:
                    m_itemMapArmorShield.insert(std::make_pair(item->id, item));
                    break;
                case ARMOR_EAR:
                    m_itemMapArmorEarring.insert(std::make_pair(item->id, item));
                    break;
                case ARMOR_NECK:
                    m_itemMapArmorNecklace.insert(std::make_pair(item->id, item));
                    break;
                case ARMOR_WRIST:
                    m_itemMapArmorWrist.insert(std::make_pair(item->id, item));
                    break;
                case ARMOR_FOOTS:
                    m_itemMapArmorFeet.insert(std::make_pair(item->id, item));
                    break;
                case ARMOR_HEAD:
                    m_itemMapArmorHelmet.insert(std::make_pair(item->id, item));
                    break;
                case ARMOR_PENDANT:
                    m_itemMapArmorPendant.insert(std::make_pair(item->id, item));
                    break;
                case ARMOR_GLOVE:
                    m_itemMapArmorGlove.insert(std::make_pair(item->id, item));
                    break;

                default:
                    break;
            }
            break;

        case ITEM_COSTUME:
            //m_itemMapCostume.insert(std::make_pair(item->id, item));
            switch (pItemTable->bSubType)
            {
                case COSTUME_BODY:
                    m_itemMapCostume.insert(std::make_pair(item->id, item));//
                    m_itemMapCosArmor.insert(std::make_pair(item->id, item));
                    break;
                case COSTUME_HAIR:
                    m_itemMapCostume.insert(std::make_pair(item->id, item));//
                    m_itemMapCosHair.insert(std::make_pair(item->id, item));
                    break;
                case COSTUME_MOUNT:
                    m_itemMapCosMount.insert(std::make_pair(item->id, item));//only as mount
                    break;
                case COSTUME_WEAPON:
                    m_itemMapCostume.insert(std::make_pair(item->id, item));//
                    m_itemMapCosWeapon.insert(std::make_pair(item->id, item));
                    break;
                case COSTUME_ACCE: // If this represents a sash
                    m_itemMapCostume.insert(std::make_pair(item->id, item));//
                    m_itemMapCosSash.insert(std::make_pair(item->id, item));
                    break;
                default:
                    break;
            }
            break;

        case ITEM_BELT:
            m_itemMapArmorBelt.insert(std::make_pair(item->id, item));
            break;

        case ITEM_SKILLBOOK:
            m_itemMapSkillBook.insert(std::make_pair(item->id, item));
            break;

        case ITEM_MATERIAL:
            m_itemMapMaterial.insert(std::make_pair(item->id, item));
            break;

        case ITEM_SPECIAL:
            if (SPECIAL_MAP == pItemTable->bSubType)
                m_itemMapSpecial.insert(std::make_pair(item->id, item));
            break;

        case ITEM_USE:
            m_itemMapConsumable.insert(std::make_pair(item->id, item));
            switch (pItemTable->bSubType)
            {
                case USE_ADD_ATTRIBUTE: 
                case USE_CHANGE_ATTRIBUTE:
                case USE_AFFECT: 
                case USE_TUNING: 
                case USE_DETACHMENT:
                    m_itemMapSpecial.insert(std::make_pair(item->id, item));
                    break;
                case USE_PUT_INTO_ACCESSORY_SOCKET:
                case USE_SPECIAL:
                    m_itemMapSkillBook.insert(std::make_pair(item->id, item));
                    break;
                default:
                    break;
            }
            break;
        case ITEM_BLEND:
            m_itemMapPotion.insert(std::make_pair(item->id, item)); // For potions
            break;
        case ITEM_GACHA:
        case ITEM_GIFTBOX:
        case ITEM_TREASURE_BOX:
        case ITEM_TREASURE_KEY:
            m_itemMapChest.insert(std::make_pair(item->id, item)); // For potions
            break;

        case ITEM_METIN:
            m_itemMapStone.insert(std::make_pair(item->id, item)); // For potions
            break;
//m_itemMapOthers  unused yet
        case ITEM_FISH:
        case ITEM_ROD:
        case ITEM_UNIQUE:
        case ITEM_RESOURCE:
        case ITEM_PICK:
        case ITEM_CAMPFIRE:
            m_itemMapOthers.insert(std::make_pair(item->id, item)); // ETC
            break;

        case ITEM_PET:
            m_itemMapPet.insert(std::make_pair(item->id, item)); // For pet costumes
            break;

        case ITEM_RING:
            if (UNIQUE_NONE == pItemTable->bSubType)
                m_itemMapSpecial.insert(std::make_pair(item->id, item));
            break;

        // Add other item types as needed...
        case ITEM_QUEST:
            if (0 == pItemTable->bSubType)
                m_itemMapSkillBook.insert(std::make_pair(item->id, item));
            else
                m_itemMapSpecial.insert(std::make_pair(item->id, item));
            break;

        default:
            break;
    }
}

void COfflineShopManager::RemoveInMemory(OFFLINE_SHOP_ITEM* item)
{
    auto pItemTable = ITEM_MANAGER::instance().GetTable(item->vnum);
    if (nullptr == pItemTable)
        return;

    switch (pItemTable->bType)
    {
        case ITEM_WEAPON:
            m_itemMapWeapon.erase(item->id);
            switch (pItemTable->bSubType)
            {
                case WEAPON_SWORD:
                    m_itemMapWeaponSword.erase(item->id);
                    break;
                case WEAPON_TWO_HANDED:
                    m_itemMapWeaponTwoHanded.erase(item->id);
                    break;
                case WEAPON_DAGGER:
                    m_itemMapWeaponDagger.erase(item->id);
                    break;
                case WEAPON_BOW:
                    m_itemMapWeaponBow.erase(item->id);
                    break;
                case WEAPON_FAN:
                    m_itemMapWeaponFan.erase(item->id);
                    break;
                case WEAPON_BELL:
                    m_itemMapWeaponBell.erase(item->id);
                    break;
                default:
                    break;
            }
            break;

        case ITEM_ARMOR:
            m_itemMapArmor.erase(item->id);
            switch (pItemTable->bSubType)
            {
                case ARMOR_BODY:
                    m_itemMapArmorBody.erase(item->id);
                    break;
                case ARMOR_SHIELD:
                    m_itemMapArmorShield.erase(item->id);
                    break;
                case ARMOR_EAR:
                    m_itemMapArmorEarring.erase(item->id);
                    break;
                case ARMOR_NECK:
                    m_itemMapArmorNecklace.erase(item->id);
                    break;
                case ARMOR_WRIST:
                    m_itemMapArmorWrist.erase(item->id);
                    break;
                case ARMOR_FOOTS:
                    m_itemMapArmorFeet.erase(item->id);
                    break;
                case ARMOR_HEAD:
                    m_itemMapArmorHelmet.erase(item->id);
                    break;
		case ARMOR_PENDANT:
			m_itemMapArmorPendant.erase(item->id);
			break;
		case ARMOR_GLOVE:
			m_itemMapArmorGlove.erase(item->id);
			break;

                default:
                    break;
            }
            break;

        case ITEM_COSTUME: 
            //m_itemMapCostume.erase(item->id);
            switch (pItemTable->bSubType)
            {
                case COSTUME_BODY:
            	    m_itemMapCostume.erase(item->id);//
                    m_itemMapCosArmor.erase(item->id);
                    break;
                case COSTUME_HAIR:
            	    m_itemMapCostume.erase(item->id);//
                    m_itemMapCosHair.erase(item->id);
                    break;
                case COSTUME_MOUNT:
                    m_itemMapCosMount.erase(item->id);
                    break;
                case COSTUME_WEAPON:
            	    m_itemMapCostume.erase(item->id);//
                    m_itemMapCosWeapon.erase(item->id);
                    break;
                case COSTUME_ACCE: // If this represents a sash
            	    m_itemMapCostume.erase(item->id);//
                    m_itemMapCosSash.erase(item->id);
                    break;
                default:
                    break;
            }
            break;

        case ITEM_BELT:
            m_itemMapArmorBelt.erase(item->id);
            break;

        case ITEM_SKILLBOOK:
            m_itemMapSkillBook.erase(item->id);
            break;

        case ITEM_MATERIAL:
            m_itemMapMaterial.erase(item->id);
            break;

        case ITEM_SPECIAL:
            if (SPECIAL_MAP == pItemTable->bSubType)
                m_itemMapSpecial.erase(item->id);
            break;

        case ITEM_USE:
            m_itemMapConsumable.erase(item->id);
            switch (pItemTable->bSubType)
            {
                case USE_ADD_ATTRIBUTE: 
                case USE_CHANGE_ATTRIBUTE:
                case USE_AFFECT: 
                case USE_TUNING: 
                case USE_DETACHMENT:
                    m_itemMapSpecial.erase(item->id);
                    break;
                case USE_PUT_INTO_ACCESSORY_SOCKET:
                case USE_SPECIAL:
                    m_itemMapSkillBook.erase(item->id);
                    break;
                default:
                    break;
            }
            break;


        case ITEM_BLEND:
            m_itemMapPotion.erase(item->id);
            break;
        case ITEM_GACHA:
        case ITEM_GIFTBOX:
        case ITEM_TREASURE_BOX:
        case ITEM_TREASURE_KEY:
            m_itemMapChest.erase(item->id);
            break;

        case ITEM_METIN:
            m_itemMapStone.erase(item->id);
            break;
//m_itemMapOthers  unused yet
        case ITEM_FISH:
        case ITEM_ROD:
        case ITEM_UNIQUE:
        case ITEM_RESOURCE:
        case ITEM_PICK:
        case ITEM_CAMPFIRE:
            m_itemMapOthers.erase(item->id);
            break;

        case ITEM_PET:
            m_itemMapPet.erase(item->id); // For removing pet costumes
            break;

        case ITEM_RING:
            if (UNIQUE_NONE == pItemTable->bSubType)
                m_itemMapSpecial.erase(item->id);
            break;

        case ITEM_QUEST:
            if (0 == pItemTable->bSubType)
                m_itemMapSkillBook.erase(item->id);
            else
                m_itemMapSpecial.erase(item->id);
            break;

        default:
            break;       
    }
}

bool ComparePrice(OFFLINE_SHOP_ITEM* i, OFFLINE_SHOP_ITEM* j)
{
	return (i->price < j->price);
}

bool CompareCount(OFFLINE_SHOP_ITEM* i, OFFLINE_SHOP_ITEM* j)
{
	return (i->count < j->count);
}

bool getFilter(TPacketCGShopSearch* pinfo, TItemTable* table, OFFLINE_SHOP_ITEM* item)
{
	if (pinfo->level[0] != 0) // min level
	{ 
		for (int x = 0; x < ITEM_LIMIT_MAX_NUM; ++x)
		{
			if (table->aLimits[x].bType == LIMIT_LEVEL)
			{
				if (table->aLimits[x].lValue < pinfo->level[0])
				{ 
					//sys_err("step 1");
					return false;
					
				} 
			}
		}
	}
	if (pinfo->level[1] != 120) // max level
	{
		for (int x = 0; x < ITEM_LIMIT_MAX_NUM; ++x)
		{
			if (table->aLimits[x].bType == LIMIT_LEVEL)
			{
				if (table->aLimits[x].lValue > pinfo->level[1])
				{
					//sys_err("step 2 %d", pinfo->level[1]);
					return false;
					
				}
			}
		}
	}

	if (table->sAddonType == -1)
	{
		if (pinfo->avg[0] != 0)
		{
			if (item->aAttr[0].bType == APPLY_NORMAL_HIT_DAMAGE_BONUS)
			{
				if (item->aAttr[0].sValue < pinfo->avg[0]) {
					//sys_err("step 3");
					return false;
				}
					
			}
		}

		if (pinfo->avg[1] != 80)
		{
			if (item->aAttr[0].bType == APPLY_NORMAL_HIT_DAMAGE_BONUS)
			{
				if (item->aAttr[0].sValue > pinfo->avg[1]) {
					//sys_err("step 4");
					return false;
				}
			}
		}

		if (pinfo->skill[0] != 0)
		{
			if (item->aAttr[1].bType == APPLY_SKILL_DAMAGE_BONUS)
			{
				if (item->aAttr[1].sValue < pinfo->skill[0]) {
					//sys_err("step 5");
					return false;
				}
					
			}
		}

		if (pinfo->skill[1] != 40)
		{
			if (item->aAttr[1].bType == APPLY_SKILL_DAMAGE_BONUS)
			{
				if (item->aAttr[1].sValue > pinfo->skill[1]) {
					//sys_err("step 6");
					return false;
				}
					
			}
		}
	}

	if (table->bType == ITEM_COSTUME && table->bSubType==COSTUME_ACCE)
	{
		if (pinfo->abs[0] != 0)
		{
			if (item->alSockets[0] < pinfo->abs[0]) {
				//sys_err("step x6x");
				return false;
			}
		}
		if (pinfo->abs[1] != 40)
		{
			if (item->alSockets[0] > pinfo->abs[1]) {
				//sys_err("step 6x");
				return false;
			}
		}
	}


	if (pinfo->sex != 0)
	{
		if (pinfo->sex == 1) { if (IS_SET(table->dwAntiFlags, ITEM_ANTIFLAG_FEMALE)) { 
			//sys_err("step 7"); 
			return false; } }
		if (pinfo->sex == 2) { if (IS_SET(table->dwAntiFlags, ITEM_ANTIFLAG_MALE)) { 
			//sys_err("step 8"); 
			return false; } }
	}

	if (pinfo->enchant != 0)
	{
		if (pinfo->enchant == 1) { if (item->aAttr[0].bType >= 1) { 
			//sys_err("step 9"); 
			return false; } }
		if (pinfo->enchant == 2) { if (item->aAttr[0].bType < 1) { 
			//sys_err("step 10"); 
			return false; } }
	}

	if (table->bType == ITEM_DS)
	{
		if (pinfo->alchemy != 0 && table->bSubType != pinfo->alchemy) {
			//sys_err("step 11"); 
			return false; }
	}

	if (pinfo->character != 0)
	{
		if (pinfo->character == 1)//warrior
		{
			if (table->dwAntiFlags & ITEM_ANTIFLAG_WARRIOR) {
				//sys_err("step 12");
				return false;
			}
				
		}
		else if (pinfo->character == 2)//assassin
		{
			if (table->dwAntiFlags & ITEM_ANTIFLAG_ASSASSIN) {
				//sys_err("step 13");
				return false;
			}
		}
		else if (pinfo->character == 3)//shaman
		{
			if (table->dwAntiFlags & ITEM_ANTIFLAG_SHAMAN) {
				//sys_err("step 14");
				return false;
			}
				
		}
		else if (pinfo->character == 4)//sura
		{
			if (table->dwAntiFlags & ITEM_ANTIFLAG_SURA) {
				//sys_err("step 15");
				return false;
			}
		}
	}
	return true;
}

bool COfflineShopManager::getMap(TPacketCGShopSearch* pinfo, std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*>::iterator& it, std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*>::iterator& itend)
{
    if (pinfo->Value == 0) // Button for type & subtype search
    {
        switch (pinfo->ItemCat)
        {
            case 0: // All
                it = m_itemMap.begin();
                itend = m_itemMap.end();
                break;
//weapon
            case 1: // Weapons
                it = m_itemMapWeapon.begin();
                itend = m_itemMapWeapon.end();
                break;
            case 2: // Sword
                it = m_itemMapWeaponSword.begin();
                itend = m_itemMapWeaponSword.end();
                break;
            case 3: // Two-Handed
                it = m_itemMapWeaponTwoHanded.begin();
                itend = m_itemMapWeaponTwoHanded.end();
                break;
            case 4: // Dagger
                it = m_itemMapWeaponDagger.begin();
                itend = m_itemMapWeaponDagger.end();
                break;
            case 5: // Bow
                it = m_itemMapWeaponBow.begin();
                itend = m_itemMapWeaponBow.end();
                break;
            case 6: // Fan
                it = m_itemMapWeaponFan.begin();
                itend = m_itemMapWeaponFan.end();
                break;
            case 7: // Bell
                it = m_itemMapWeaponBell.begin();
                itend = m_itemMapWeaponBell.end();
                break;
//armor
            case 8: // all Armor
                it = m_itemMapArmor.begin();
                itend = m_itemMapArmor.end();
                break;



            case 9: // Armor Body
                it = m_itemMapArmorBody.begin();
                itend = m_itemMapArmorBody.end();
                break;
            case 10: // Helmet
                it = m_itemMapArmorHelmet.begin();
                itend = m_itemMapArmorHelmet.end();
                break;
            case 11: // Shield
                it = m_itemMapArmorShield.begin();
                itend = m_itemMapArmorShield.end();
                break;
            case 12: // Wrist
                it = m_itemMapArmorWrist.begin();
                itend = m_itemMapArmorWrist.end();
                break;
            case 13: // Feet
                it = m_itemMapArmorFeet.begin();
                itend = m_itemMapArmorFeet.end();
                break;
            case 14: // Necklace
                it = m_itemMapArmorNecklace.begin();
                itend = m_itemMapArmorNecklace.end();
                break;
            case 15: // Earrings
                it = m_itemMapArmorEarring.begin();
                itend = m_itemMapArmorEarring.end();
                break;
            case 16: // Belt
                it = m_itemMapArmorBelt.begin();
                itend = m_itemMapArmorBelt.end();
                break;
            case 17: // pendant
                it = m_itemMapArmorPendant.begin();
                itend = m_itemMapArmorPendant.end();
                break;
            case 18: // glove
                it = m_itemMapArmorGlove.begin();
                itend = m_itemMapArmorGlove.end();
                break;

//costumes
            case 19: // Costumes
                it = m_itemMapCostume.begin();
                itend = m_itemMapCostume.end();
                break;
            case 20: // Costume Armor
                it = m_itemMapCosArmor.begin();
                itend = m_itemMapCosArmor.end();
                break;
            case 21: // Hairstyle
                it = m_itemMapCosHair.begin();
                itend = m_itemMapCosHair.end();
                break;
            case 22: // Costume Weapon
                it = m_itemMapCosWeapon.begin();
                itend = m_itemMapCosWeapon.end();
                break;
            case 23: // Costume Sash
                it = m_itemMapCosSash.begin();
                itend = m_itemMapCosSash.end();
                break;
//others
            case 24: // materials Items
                it = m_itemMapMaterial.begin();
                itend = m_itemMapMaterial.end();
                break;
            case 25: // special
                it = m_itemMapSpecial.begin();
                itend = m_itemMapSpecial.end();
                break;
            case 26: // skillbook
                it = m_itemMapSkillBook.begin();
                itend = m_itemMapSkillBook.end();
                break;
//potions
//chests
//others
            case 27: // stones
                it = m_itemMapStone.begin();
                itend = m_itemMapStone.end();
                break;
            case 28: // potions
                it = m_itemMapPotion.begin();
                itend = m_itemMapPotion.end();
                break;
            case 29: // chests
                it = m_itemMapChest.begin();
                itend = m_itemMapChest.end();
                break;
            case 30: // consumabile
                it = m_itemMapConsumable.begin();
                itend = m_itemMapConsumable.end();
                break;
            case 31: // others
                it = m_itemMapOthers.begin();
                itend = m_itemMapOthers.end();
                break;
//companions
            case 32: // Pets
                it = m_itemMapPet.begin();
                itend = m_itemMapPet.end();
                break;
            case 33: // Mounts
                it = m_itemMapCosMount.begin();
                itend = m_itemMapCosMount.end();
                break;

            default:
                return false; // Invalid category
        }
    }
    else
    {
        return false; // Value is not zero
    }

    return true; // Successfully found items in the map
}
void COfflineShopManager::SearchItem(LPCHARACTER ch, const char * data, bool bVnum)
{
	if(!ch || !data)
		return;
	TPacketCGShopSearch* pinfo = (TPacketCGShopSearch *)data;
	bVnum = pinfo->itemVnum != 0 ? true : false;
	if ((pinfo->ItemCat < ITEM_NONE || pinfo->ItemCat > ITEM_PET)
		|| (pinfo->avg[0] < 0 || pinfo->avg[0] > 80)
		|| (pinfo->avg[1] < 0 || pinfo->avg[1] > 80)
		|| (pinfo->skill[0] < 0 || pinfo->skill[0] > 40)
		|| (pinfo->skill[1] < 0 || pinfo->skill[1] > 40)
		|| (pinfo->abs[0] < 0 || pinfo->abs[0] > 40)
		|| (pinfo->abs[1] < 0 || pinfo->abs[1] > 40)
		|| (pinfo->level[0] < 0 || pinfo->level[0] > PLAYER_MAX_LEVEL_CONST)
		|| (pinfo->level[1] < 0 || pinfo->level[1] > PLAYER_MAX_LEVEL_CONST)
		|| (pinfo->refine[0] < 0 || pinfo->refine[0] > 200)
		|| (pinfo->refine[1] < 0 || pinfo->refine[1] > 200)
		|| (pinfo->sex < 0 || pinfo->sex > 2)
		|| (pinfo->enchant < 0 || pinfo->enchant > 2)
		|| (pinfo->alchemy < 0 || pinfo->alchemy > 7)
		)
		return;

	std::vector<OFFLINE_SHOP_ITEM*> sendItems;
	std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*>::iterator it;
	std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*>::iterator itend;

	if (bVnum)
	{
		it = m_itemMap.begin();
		for (; it != m_itemMap.end(); ++it)
		{
			if (sendItems.size() >= 250)
			{
				break;
			}
			OFFLINE_SHOP_ITEM* item = it->second;
			if (!item)
				continue;
			if (pinfo->itemVnum == 50300)
			{
				if (!(item->vnum == pinfo->itemVnum && pinfo->Value == item->alSockets[0]))
					continue;
			}
			else if(pinfo->Value != 0)
			{
				if(!(pinfo->itemVnum-pinfo->Value <= item->vnum && pinfo->itemVnum >= item->vnum))
					continue;
			}
			else
			{
				if (item->vnum != pinfo->itemVnum)
					continue;
			}

			TItemTable* table = ITEM_MANAGER::instance().GetTable(item->vnum);
			if (!table)
				continue;
			if (getFilter(pinfo, table, item))
				sendItems.push_back(item);
		}
	}
	else
	{
		if (getMap(pinfo, it, itend))
		{
			for (; it != itend; ++it)
			{
				if (sendItems.size() >= 250)
				{
					break;
				}
				OFFLINE_SHOP_ITEM* item = it->second;
				if (!item)
					continue;
				TItemTable* table = ITEM_MANAGER::instance().GetTable(item->vnum);
				if (!table)
					continue;
				if (getFilter(pinfo, table, item))
					sendItems.push_back(item);
			}
		}
	}

	if (sendItems.size() > 0)
	{
		std::stable_sort(sendItems.begin(), sendItems.end(), ComparePrice);
		std::stable_sort(sendItems.begin(), sendItems.end(), CompareCount);
		TPacketGCShopSearchItemSet pack;
		pack.header = HEADER_GC_SHOPSEARCH_SET;
		pack.count = sendItems.size();
		for (BYTE j = 0; j < sendItems.size(); j++)
		{
			OFFLINE_SHOP_ITEM*& item = sendItems[j];
			if (item->vnum == 0)
				continue;
			pack.items[j].vnum = item->vnum;
			pack.items[j].price = item->price;
			pack.items[j].count = item->count;
			pack.items[j].display_pos = item->pos;
			//pack.items[j].owner_id = item->owner_id;
			pack.items[j].owner_id = item->id;
			pack.items[j].status = 0;
			thecore_memcpy(&pack.items[j].alSockets, &item->alSockets, sizeof(pack.items[j].alSockets));
			thecore_memcpy(&pack.items[j].aAttr, &item->aAttr, sizeof(pack.items[j].aAttr));
			strlcpy(pack.items[j].szBuyerName, item->szOwnerName, sizeof(pack.items[j].szBuyerName));
#ifdef __CHANGELOOK_SYSTEM__
			pack.items[j].transmutation = item->transmutation;
#endif
#ifdef ENABLE_NEW_NAME_ITEM
			strlcpy(pack.items[j].name, item->name, sizeof(pack.items[j].name));
#endif
#ifdef ENABLE_PERMA_ITEM
			pack.items[j].perma = item->perma;
#endif
		}
		ch->GetDesc()->Packet(&pack, sizeof(TPacketGCShopSearchItemSet));
	}

	/*
	if (bVnum)
		sys_log(0, "SHOP_SEARCH: CharName: %s Search: Cat: %d SubCat: %d MaxRefine: %d MinLevel: %d MaxLevel: %d Enchant : %d alchemySub: %d Sex: %d Value: %d itemVnum: %d ", ch->GetName(), pinfo->ItemCat, pinfo->SubCat, pinfo->MaxRefine,pinfo->MinLevel,pinfo->MaxLevel,pinfo->Enchant,pinfo->alchemySub,pinfo->Sex,pinfo->Value,pinfo->itemVnum);
	else
		sys_log(0, "SHOP_SEARCH: CharName: %s Search: Cat: %d SubCat: %d MaxRefine: %d MinLevel: %d MaxLevel: %d Enchant : %d alchemySub: %d Sex: %d Value: %d  ", ch->GetName(), pinfo->ItemCat, pinfo->SubCat, pinfo->MaxRefine,pinfo->MinLevel,pinfo->MaxLevel,pinfo->Enchant,pinfo->alchemySub,pinfo->Sex,pinfo->Value);
	*/
}

void COfflineShopManager::ClearItem(DWORD id)
{
	auto it = m_itemMap.find(id);
	if (it != m_itemMap.end()) {
		OFFLINE_SHOP_ITEM* item = it->second;
		RemoveInMemory(item);
		m_itemMap.erase(it);
		M2_DELETE(item);
	}
}

void COfflineShopManager::InsertItem(OFFLINE_SHOP_ITEM* p)
{
	auto it = m_itemMap.find(p->id);
	if (it != m_itemMap.end()){
		sys_err("wtf have 2 id in game? item id %u",p->id);
		return;
	}
	OFFLINE_SHOP_ITEM* item = new OFFLINE_SHOP_ITEM;
	thecore_memcpy(item, p, sizeof(OFFLINE_SHOP_ITEM));
	m_itemMap.insert(std::make_pair(p->id, item));
	SaveInMemory(item);
}

#endif
#endif

