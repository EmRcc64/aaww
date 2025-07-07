#ifndef	__HEADER_PET_SYSTEM__
#define	__HEADER_PET_SYSTEM__

class CHARACTER;

struct SPetAbility
{
};

/**
*/
class CPetActor //: public CHARACTER
{
public:
	enum EPetOptions
	{
		EPetOption_Followable		= 1 << 0,
		EPetOption_Mountable		= 1 << 1,
		EPetOption_Summonable		= 1 << 2,
		EPetOption_Combatable		= 1 << 3,
	};

protected:
	friend class CPetSystem;

	CPetActor(LPCHARACTER owner, DWORD vnum, DWORD options = EPetOption_Followable | EPetOption_Summonable);
//	CPetActor(LPCHARACTER owner, DWORD vnum, const SPetAbility& petAbility, DWORD options = EPetOption_Followable | EPetOption_Summonable);

	virtual ~CPetActor();

	virtual bool	Update(DWORD deltaTime);

protected:
	virtual bool	_UpdateFollowAI();
	virtual bool	_UpdatAloneActionAI(float fMinDist, float fMaxDist);

	/// @TODO
	//virtual bool	_UpdateCombatAI();

private:
	bool Follow(float fMinDistance = 50.f);

public:
	LPCHARACTER		GetCharacter()	const					{ return m_pkChar; }
	LPCHARACTER		GetOwner()	const						{ return m_pkOwner; }
	DWORD			GetVID() const							{ return m_dwVID; }
	DWORD			GetVnum() const							{ return m_dwVnum; }

	bool			HasOption(EPetOptions option) const		{ return m_dwOptions & option; }
	bool			IsAttackingPet(DWORD vnum);
	void			SetName(const char* petName);

	bool			Mount();
	void			Unmount();
#ifdef PET_ATTACK
	bool            Attack(LPCHARACTER pkVictim = NULL);

    void            SetVictim(LPCHARACTER victim);
#endif
	DWORD			Summon(const char* petName, LPITEM pSummonItem, bool bSpawnFar = false);
	void			Unsummon();

	bool			IsSummoned() const			{ return 0 != m_pkChar; }
	void			SetSummonItem (LPITEM pItem);
	DWORD			GetSummonItemVID () { return m_dwSummonItemVID; }

	void			GiveBuff();
	void			ClearBuff();

#ifdef PET_ATTACK
	bool			MouseFollow(long int x, long int y);
#endif
#ifdef PET_AUTO_PICKUP
	bool CheckPetPickup(time_t time);
	void PickUpItems(int range);
	void BringItem();
	void SetPickup(bool is_pickup) { m_is_pickup = is_pickup; }
	bool IsPickup() { return m_is_pickup; }
	void SetPickupItem(LPITEM item) { m_pickup_item = item; }
	LPITEM GetPickupItem() { return m_pickup_item; }
	void SetPickupTime(time_t zTime) { pickTime = zTime; }
	time_t GetPickupTime() { return pickTime; }
#endif

private:
	DWORD			m_dwVnum;
	DWORD			m_dwVID;
	DWORD			m_dwOptions;
	DWORD			m_dwLastActionTime;
	DWORD			m_dwSummonItemVID;
	DWORD			m_dwSummonItemVnum;

	short			m_originalMoveSpeed;

	std::string		m_name;

	LPCHARACTER		m_pkChar;					// Instance of pet(CHARACTER)
	LPCHARACTER		m_pkOwner;
#ifdef PET_AUTO_PICKUP
	bool m_is_pickup;
	LPITEM m_pickup_item;
	time_t pickTime;
#endif
};

/**
*/
class CPetSystem
{
public:
	typedef	boost::unordered_map<DWORD,	CPetActor*>		TPetActorMap;

public:
	CPetSystem(LPCHARACTER owner);
	virtual ~CPetSystem();

	CPetActor*	GetByVID(DWORD vid) const;
	CPetActor*	GetByVnum(DWORD vnum) const;
#ifdef PET_ATTACK
	void        LaunchAttack(LPCHARACTER pkVictim = NULL);
    	void        StopAttack();
#endif
	bool		Update(DWORD deltaTime);
	void		Destroy();

	size_t		CountSummoned() const;
#ifdef PET_ATTACK
	void		PetMouseFollow(long int x, long int y);
	void		PetUpdateFollow();

	void		SetPetVnum(DWORD vnum) { m_GetPetVnum = vnum; }
	DWORD		GetPetVnum() const	{ return m_GetPetVnum; }
#endif
public:
	void		SetUpdatePeriod(DWORD ms);

	CPetActor*	Summon(DWORD mobVnum, LPITEM pSummonItem, const char* petName, bool bSpawnFar, DWORD options = CPetActor::EPetOption_Followable | CPetActor::EPetOption_Summonable);

	void		Unsummon(DWORD mobVnum, bool bDeleteFromList = false);
	void		Unsummon(CPetActor* petActor, bool bDeleteFromList = false);
	void		UnsummonAll();

	CPetActor*	AddPet(DWORD mobVnum, const char* petName, const SPetAbility& ability, DWORD options = CPetActor::EPetOption_Followable | CPetActor::EPetOption_Summonable | CPetActor::EPetOption_Combatable);

	void		DeletePet(DWORD mobVnum);
	void		DeletePet(CPetActor* petActor);
	void		RefreshBuff();

private:
	TPetActorMap	m_petActorMap;
	LPCHARACTER		m_pkOwner;
	DWORD			m_dwUpdatePeriod;
	DWORD			m_dwLastUpdateTime;
	LPEVENT			m_pkPetSystemUpdateEvent;
#ifdef PET_ATTACK
	DWORD			m_GetPetVnum;
	bool			m_attackPs;
#endif
};

/**
// Summon Pet
CPetSystem* petSystem = mainChar->GetPetSystem();
CPetActor* petActor = petSystem->Summon(~~~);

DWORD petVID = petActor->GetVID();
if (0 == petActor)
{
	ERROR_LOG(...)
};

// Unsummon Pet
petSystem->Unsummon(petVID);

// Mount Pet
petActor->Mount()..

CPetActor::Update(...)
{
	// AI : Follow, actions, etc...
}

*/

#endif	//__HEADER_PET_SYSTEM__
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
