#ifndef __INC_METIN_II_GAME_CHARACTER_MANAGER_H__
#define __INC_METIN_II_GAME_CHARACTER_MANAGER_H__

#ifdef M2_USE_POOL
#include "pool.h"
#endif

#include "../../common/stl.h"
#include "../../common/length.h"

#include "vid.h"

#ifdef ENABLE_EVENT_MANAGER
#include "buffer_manager.h"
#endif

class CDungeon;
class CHARACTER;
class CharacterVectorInteractor;

#if defined(__EVENT_BANNER_FLAG__)
typedef std::map<DWORD, std::string> BannerMapType;
#endif

class CHARACTER_MANAGER : public singleton<CHARACTER_MANAGER>
{
	public:
		typedef TR1_NS::unordered_map<std::string, LPCHARACTER> NAME_MAP;

		CHARACTER_MANAGER();
		virtual ~CHARACTER_MANAGER();

		void                    Destroy();

		void			GracefulShutdown();

		DWORD			AllocVID();

		LPCHARACTER             CreateCharacter(const char * name, DWORD dwPID = 0);
#ifndef DEBUG_ALLOC
		void DestroyCharacter(LPCHARACTER ch);
#else
		void DestroyCharacter(LPCHARACTER ch, const char* file, size_t line);
#endif

		void			Update(int iPulse);

		LPCHARACTER		SpawnMob(DWORD dwVnum, long lMapIndex, long x, long y, long z, bool bSpawnMotion = false, int iRot = -1, bool bShow = true);
		LPCHARACTER		SpawnMobRange(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, bool bIsException=false, bool bSpawnMotion = false , bool bAggressive = false);
		LPCHARACTER		SpawnGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen = NULL, bool bAggressive_ = false, LPDUNGEON pDungeon = NULL);
		bool			SpawnGroupGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen = NULL, bool bAggressive_ = false, LPDUNGEON pDungeon = NULL);
		bool			SpawnMoveGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, int tx, int ty, LPREGEN pkRegen = NULL, bool bAggressive_ = false);
		LPCHARACTER		SpawnMobRandomPosition(DWORD dwVnum, long lMapIndex, bool is_aggressive = false, BYTE dir = 0);

		void			SelectStone(LPCHARACTER pkChrStone);

		NAME_MAP &		GetPCMap() { return m_map_pkPCChr; }

		LPCHARACTER		Find(DWORD dwVID);
		LPCHARACTER		Find(const VID & vid);
		LPCHARACTER		FindPC(const char * name);
		LPCHARACTER		FindByPID(DWORD dwPID);

		bool			AddToStateList(LPCHARACTER ch);
		void			RemoveFromStateList(LPCHARACTER ch);

		void                    DelayedSave(LPCHARACTER ch);
		bool                    FlushDelayedSave(LPCHARACTER ch);
		void			ProcessDelayedSave();

		template<class Func>	Func for_each_pc(Func f);

		void			RegisterForMonsterLog(LPCHARACTER ch);
		void			UnregisterForMonsterLog(LPCHARACTER ch);
		void			PacketMonsterLog(LPCHARACTER ch, const void* buf, int size);

		void			KillLog(DWORD dwVnum);

		void			RegisterRaceNum(DWORD dwVnum);
		void			RegisterRaceNumMap(LPCHARACTER ch);
		void			UnregisterRaceNumMap(LPCHARACTER ch);
		bool			GetCharactersByRaceNum(DWORD dwRaceNum, CharacterVectorInteractor & i);

		LPCHARACTER		FindSpecifyPC(unsigned int uiJobFlag, long lMapIndex, LPCHARACTER except=NULL, int iMinLevel = 1, int iMaxLevel = PLAYER_MAX_LEVEL_CONST);

		void			SetMobItemRate(int value)	{ m_iMobItemRate = value; }
		void			SetMobDamageRate(int value)	{ m_iMobDamageRate = value; }
		void			SetMobGoldAmountRate(int value)	{ m_iMobGoldAmountRate = value; }
		void			SetMobGoldDropRate(int value)	{ m_iMobGoldDropRate = value; }
		void			SetMobExpRate(int value)	{ m_iMobExpRate = value; }

		void			SetMobItemRatePremium(int value)	{ m_iMobItemRatePremium = value; }
		void			SetMobGoldAmountRatePremium(int value)	{ m_iMobGoldAmountRatePremium = value; }
		void			SetMobGoldDropRatePremium(int value)	{ m_iMobGoldDropRatePremium = value; }
		void			SetMobExpRatePremium(int value)		{ m_iMobExpRatePremium = value; }

		void			SetUserDamageRatePremium(int value)	{ m_iUserDamageRatePremium = value; }
		void			SetUserDamageRate(int value ) { m_iUserDamageRate = value; }
		int			GetMobItemRate(LPCHARACTER ch);
		int			GetMobDamageRate(LPCHARACTER ch);
		int			GetMobGoldAmountRate(LPCHARACTER ch);
		int			GetMobGoldDropRate(LPCHARACTER ch);
		int			GetMobExpRate(LPCHARACTER ch);

		int			GetUserDamageRate(LPCHARACTER ch);
		void		SendScriptToMap(long lMapIndex, const std::string & s);

		bool			BeginPendingDestroy();
		void			FlushPendingDestroy();
#ifdef u1x
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
#endif
#ifdef ENABLE_ITEMSHOP
public:
	void	LoadItemShopData(const char* c_pData);
	void	LoadItemShopData(LPCHARACTER ch, bool isAll = true);
	void	LoadItemShopLog(LPCHARACTER ch);
	void	LoadItemShopLogReal(LPCHARACTER ch, const char* c_pData);
	void	LoadItemShopBuy(LPCHARACTER ch, DWORD itemID, int itemCount);
	void	LoadItemShopBuyReal(LPCHARACTER ch, const char* c_pData);
	int		GetItemShopUpdateTime() { return itemshopUpdateTime; }
	void	UpdateItemShopItem(const char* c_pData);

protected:
	int		itemshopUpdateTime;
	std::map<BYTE, std::map<BYTE, std::vector<TIShopData>>> m_IShopManager;
#endif

#ifdef ENABLE_EVENT_MANAGER
public:
	void			ClearEventData();
	bool			CloseEventManuel(BYTE eventIndex);
	void			SetEventData(BYTE dayIndex, const std::vector<TEventManagerData>& m_data);
	void			SetEventStatus(const WORD eventID, const bool eventStatus, const int endTime);
	void			SendDataPlayer(LPCHARACTER ch);
	void			CheckBonusEvent(LPCHARACTER ch);
	void			UpdateAllPlayerEventData();
	void			CompareEventSendData(TEMP_BUFFER* buf);
	const TEventManagerData* CheckEventIsActive(BYTE eventIndex, BYTE empireIndex = 0);
	void			CheckEventForDrop(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item);
protected:
	std::map<BYTE, std::vector<TEventManagerData>>	m_eventData;
#endif

#if defined(__EVENT_BANNER_FLAG__)
public:
	enum EBannerMapIndex
	{
		EMPIRE_A = 1,
		EMPIRE_B = 21,
		EMPIRE_C = 41,
	};
	bool InitializeBanners();
	bool SpawnBanners(int iEnable, const char* c_szBannerName = NULL);

private:
	BannerMapType BannerMap;
	bool m_bIsLoadedBanners;
#endif


	private:
		int					m_iMobItemRate;
		int					m_iMobDamageRate;
		int					m_iMobGoldAmountRate;
		int					m_iMobGoldDropRate;
		int					m_iMobExpRate;

		int					m_iMobItemRatePremium;
		int					m_iMobGoldAmountRatePremium;
		int					m_iMobGoldDropRatePremium;
		int					m_iMobExpRatePremium;

		int					m_iUserDamageRate;
		int					m_iUserDamageRatePremium;
		int					m_iVIDCount;

		TR1_NS::unordered_map<DWORD, LPCHARACTER> m_map_pkChrByVID;
		TR1_NS::unordered_map<DWORD, LPCHARACTER> m_map_pkChrByPID;
		NAME_MAP			m_map_pkPCChr;

		char				dummy1[1024];	// memory barrier
		CHARACTER_SET		m_set_pkChrState;
		CHARACTER_SET		m_set_pkChrForDelayedSave;
		CHARACTER_SET		m_set_pkChrMonsterLog;

		LPCHARACTER			m_pkChrSelectedStone;

		std::map<DWORD, DWORD> m_map_dwMobKillCount;

		std::set<DWORD>		m_set_dwRegisteredRaceNum;
		std::map<DWORD, CHARACTER_SET> m_map_pkChrByRaceNum;

		bool				m_bUsePendingDestroy;
		CHARACTER_SET		m_set_pkChrPendingDestroy;

#ifdef M2_USE_POOL
		ObjectPool<CHARACTER> pool_;
#endif
#ifdef RENEWAL_MISSION_BOOKS
	public:
		void	LoadMissionBook();
		void	GiveNewMission(LPITEM missionBook, LPCHARACTER ch);
		const TMissionBookData* GetMissionData(WORD id);
	protected:
		std::map<WORD, TMissionBookData> m_mapMissionData;
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
	public:
		int		GetMultiFarmCount(const char* playerIP, std::map<DWORD, std::pair<std::string, bool>>& m_mapNames);
		void	CheckMultiFarmAccount(const char* szIP, const DWORD playerID, const char* playerName, const bool bStatus, BYTE affectType = 0, int affectDuration = 0, bool isP2P = false);
		void	SetMultiFarm(const char* szIP, const DWORD playerID, const char* playerName, const bool bStatus, const BYTE affectType, const int affectTime);
		void	RemoveMultiFarm(const char* szIP, const DWORD playerID, const bool isP2P);
		void	CheckMultiFarmAccounts(const char* szIP);
		void	CheckEnergyCrystal(LPCHARACTER ch);
	protected:
		std::map<std::string, std::vector<TMultiFarm>> m_mapmultiFarm;
#endif

};

	template<class Func>
Func CHARACTER_MANAGER::for_each_pc(Func f)
{
	TR1_NS::unordered_map<DWORD, LPCHARACTER>::iterator it;

	for (it = m_map_pkChrByPID.begin(); it != m_map_pkChrByPID.end(); ++it)
		f(it->second);

	return f;
}

class CharacterVectorInteractor : public CHARACTER_VECTOR
{
	public:
		CharacterVectorInteractor() : m_bMyBegin(false) { }

		CharacterVectorInteractor(const CHARACTER_SET & r);
		virtual ~CharacterVectorInteractor();

	private:
		bool m_bMyBegin;
};

#ifndef DEBUG_ALLOC
#define M2_DESTROY_CHARACTER(ptr) CHARACTER_MANAGER::instance().DestroyCharacter(ptr)
#else
#define M2_DESTROY_CHARACTER(ptr) CHARACTER_MANAGER::instance().DestroyCharacter(ptr, __FILE__, __LINE__)
#endif

#define M2_DESTROY_CHARACTER_EX(ptr) if (ptr) { M2_DESTROY_CHARACTER(ptr); ptr = nullptr; }
#endif
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
