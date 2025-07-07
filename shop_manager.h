#ifndef __INC_METIN_II_GAME_SHOP_MANAGER_H__
#define __INC_METIN_II_GAME_SHOP_MANAGER_H__

class CShop;
typedef class CShop * LPSHOP;

class CShopManager : public singleton<CShopManager>
{
public:
	typedef std::map<DWORD, CShop *> TShopMap;

public:
	CShopManager();
	virtual ~CShopManager();

	bool	Initialize(TShopTable * table, int size);
	void	Destroy();

	LPSHOP	Get(DWORD dwVnum);
	LPSHOP	GetByNPCVnum(DWORD dwVnum);

	bool	StartShopping(LPCHARACTER pkChr, LPCHARACTER pkShopKeeper, int iShopVnum = 0);
	void	StopShopping(LPCHARACTER ch);

	bool	NPCAC(LPCHARACTER pkChr, LPCHARACTER pkShopKeeper, int iShopVnum = 0);

	void	Buy(LPCHARACTER ch, BYTE pos);

#ifdef ENABLE_SPECIAL_STORAGE
	void	Sell(LPCHARACTER ch, UINT bCell, uint16_t wCount = 0, BYTE bType = 0);
#else
	void	Sell(LPCHARACTER ch, BYTE bCell, uint16_t wCount = 0);
#endif

	LPSHOP	CreatePCShop(LPCHARACTER ch, TShopItemTable * pTable, uint16_t bItemCount);
	LPSHOP	FindPCShop(DWORD dwVID);
	void	DestroyPCShop(LPCHARACTER ch);

private:
	TShopMap	m_map_pkShop;
	TShopMap	m_map_pkShopByNPCVnum;
	TShopMap	m_map_pkShopByPC;

	bool	ReadShopTableEx(const char* stFileName);
};

#endif
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
