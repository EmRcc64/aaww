#pragma once
#ifdef ENABLE_OFFLINESHOP_SYSTEM
class COfflineShopManager : public singleton<COfflineShopManager>
{
	public:
		COfflineShopManager();
		~COfflineShopManager();
		void			Initialize();

		LPOFFLINESHOP	FindOfflineShopPID(DWORD pid);
		void			CompareOffShopEventTime();

		void			OpenMyOfflineShop(LPCHARACTER ch, const char* c_pszSign, TOfflineShopItemTable* pTable, BYTE bItemCount, DWORD shopVnum, BYTE titleType);
		void			CreateOfflineShop(TOfflineShop* offlineshop);
		void			StopShopping(LPCHARACTER ch);
		void			OpenOfflineShop(LPCHARACTER ch);
		void			OpenOfflineShopWithVID(LPCHARACTER ch, DWORD vid);
		bool			HasOfflineShop(LPCHARACTER ch);

		void			Buy(LPCHARACTER ch, DWORD vid, BYTE bPos);
		void			BuyItemReal(TOfflineShopBuy* item);

		void			AddItem(LPCHARACTER ch, BYTE bDisplayPos, TItemPos bPos, long long iPrice);
		void			AddItemReal(OFFLINE_SHOP_ITEM* item);
		
		void			OpenSlot(LPCHARACTER ch, BYTE bPos);
		void			OpenSlotReal(TOfflineShopOpenSlot* ch);

		void			RemoveItem(LPCHARACTER ch, BYTE bPos);
		void			RemoveItemReal(OFFLINE_SHOP_ITEM* item);
		
		void			ShopLogRemove(LPCHARACTER ch);
		void			ShopLogRemoveReal(DWORD ch);

		void			ChangeDecoration(LPCHARACTER ch, TShopDecoration* data);
		void			ChangeDecorationReal(TShopDecoration* ch);

		void			WithdrawMoney(LPCHARACTER ch);
		void			WithdrawMoneyReal(DWORD ch);

		void			DestroyOfflineShop(LPCHARACTER ch);
		void			DestroyOfflineShopReal(DWORD ch);

		void			ChangeTitle(LPCHARACTER ch, const char* title);
		void			ChangeTitleReal(TOfflineShopChangeTitle* p);

		void			CloseOfflineShopForTime(LPOFFLINESHOP offlineshop);
		void			CloseOfflineShopForTimeReal(DWORD offlineshop);

		void			GetBackItem(LPCHARACTER ch);
		void			GetBackItemReal(TOfflineShopBackItem* ch);

		void			ShopAddTime(LPCHARACTER ch);
		void			ShopAddTimeReal(DWORD ch);

		void			RecvPackets(const char * data);
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
		void			ClearItem(DWORD id);
		DWORD			GetOwnerPID(DWORD id);
		void			InsertItem(OFFLINE_SHOP_ITEM* p);
		void			SearchItem(LPCHARACTER ch, const char * data, bool bVnum);
		void			RemoveInMemory(OFFLINE_SHOP_ITEM* item);
		void			SaveInMemory(OFFLINE_SHOP_ITEM* item);
		bool			getMap(TPacketCGShopSearch* pinfo, std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*>::iterator& it, std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*>::iterator& itend);
#endif
		std::vector<DWORD> 					m_Map_pkShopTimes;

	private:
		LPEVENT								m_pShopTimeEvent;
		
		std::vector<DWORD> 					m_Map_pkOfflineShopCache;
		std::map<DWORD, COfflineShop*> 		m_Map_pkOfflineShopByNPC;
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
// Declaration of maps for different categories of items
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMap; // All items
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapWeapon; // All weapons
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmor; // All armors
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapCostume; // Costumes all

std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapCosSash; // Costumes for sash (accessory)
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapCosWeapon; // Costumes for weapon
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapCosArmor; // Costumes for armor
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapCosHair; // Costumes for hairstyle
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapCosMount; // Costumes for mounts

std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapPet; // Costumes for pets (ITEM_PET type)

// Maps for weapon subtypes
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapWeaponSword;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapWeaponTwoHanded;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapWeaponDagger;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapWeaponBow;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapWeaponFan;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapWeaponBell;

// Maps for armor subtypes
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmorBody;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmorShield;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmorEarring;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmorNecklace;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmorWrist;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmorFeet;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmorBelt;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmorHelmet;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmorPendant;
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapArmorGlove;
// Other types of items
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapSkillBook; // Skill books
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapMaterial; // Materials
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapSpecial; // Special items

std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapConsumable; // potion
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapPotion; // potion
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapStone; // stone
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapChest; // chests
std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*> m_itemMapOthers; // others



#endif
};
#endif

