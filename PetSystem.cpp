#include "stdafx.h"
#include "config.h"
#include "utils.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "PetSystem.h"
#include "../../common/VnumHelper.h"
#include "packet.h"
#include "item_manager.h"
#include "item.h"

EVENTINFO(petsystem_event_info)
{
	CPetSystem* pPetSystem;
};

EVENTFUNC(petsystem_update_event)
{
	petsystem_event_info* info = dynamic_cast<petsystem_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	CPetSystem*	pPetSystem = info->pPetSystem;

	if (NULL == pPetSystem)
		return 0;

	pPetSystem->Update(0);

	return PASSES_PER_SEC(1) / 4;
}

const float PET_COUNT_LIMIT = 3;

///////////////////////////////////////////////////////////////////////////////////////
//  CPetActor
///////////////////////////////////////////////////////////////////////////////////////

CPetActor::CPetActor(LPCHARACTER owner, DWORD vnum, DWORD options)
{
	m_dwVnum = vnum;
	m_dwVID = 0;
	m_dwOptions = options;
	m_dwLastActionTime = 0;

	m_pkChar = 0;
	m_pkOwner = owner;

	m_originalMoveSpeed = 0;

	m_dwSummonItemVID = 0;
	m_dwSummonItemVnum = 0;

#ifdef PET_AUTO_PICKUP
	m_pickup_item = nullptr;
	m_is_pickup = false;
	pickTime = 0;
#endif

}

CPetActor::~CPetActor()
{
	this->Unsummon();

	m_pkOwner = 0;

#ifdef PET_AUTO_PICKUP
	m_pickup_item = nullptr;
	m_is_pickup = false;
	pickTime = 0;
#endif
}

bool CPetActor::IsAttackingPet(DWORD vnum)
{
    switch (vnum)
    {
        case 34200:
        case 34201:
        case 34202:
        case 34203:
        case 34204:
        case 34205:
        case 34206:
        case 34207:
            return true;
        default:
            return false;
    }
}


void CPetActor::SetName(const char* name)
{


	std::string petName = m_pkOwner->GetName();


#ifndef __MULTI_LANGUAGE_SYSTEM__//n
	if (0 != m_pkOwner &&
		0 == name &&
		0 != m_pkOwner->GetName())
	{
		petName += "'s Pet";
	}
	else
		petName += name;
#endif




	if (true == IsSummoned())
		m_pkChar->SetName(petName);

	m_name = petName;
}

bool CPetActor::Mount()
{
	if (0 == m_pkOwner)
		return false;

	if (true == HasOption(EPetOption_Mountable))
		m_pkOwner->MountVnum(m_dwVnum);

	return m_pkOwner->GetMountVnum() == m_dwVnum;;
}

void CPetActor::Unmount()
{
	if (0 == m_pkOwner)
		return;

	if (m_pkOwner->IsHorseRiding())
		m_pkOwner->StopRiding();
}

void CPetActor::Unsummon()
{
	if (true == this->IsSummoned())
	{
		this->ClearBuff();
		this->SetSummonItem(NULL);

#ifdef PET_AUTO_PICKUP
		this->SetPickupItem(nullptr);
		this->SetPickup(false);
		this->SetPickupTime(0);
#endif

		if (NULL != m_pkOwner)
			m_pkOwner->ComputePoints();

		if (NULL != m_pkChar)
			M2_DESTROY_CHARACTER(m_pkChar);

		m_pkChar = 0;
		m_dwVID = 0;
	}
}

DWORD CPetActor::Summon(const char* petName, LPITEM pSummonItem, bool bSpawnFar)
{
	long x = m_pkOwner->GetX();
	long y = m_pkOwner->GetY();
	long z = m_pkOwner->GetZ();

	if (true == bSpawnFar)
	{
		x += (number(0, 1) * 2 - 1) * number(2000, 2500);
		y += (number(0, 1) * 2 - 1) * number(2000, 2500);
	}
	else
	{
		x += number(-100, 100);
		y += number(-100, 100);
	}

	if (0 != m_pkChar)
	{
#ifdef PET_ATTACK
		m_pkChar->SetOwner(m_pkOwner);
		m_pkChar->SetPet();
#endif

		m_pkChar->Show (m_pkOwner->GetMapIndex(), x, y);
		m_dwVID = m_pkChar->GetVID();

		return m_dwVID;
	}

	m_pkChar = CHARACTER_MANAGER::instance().SpawnMob(
				m_dwVnum,
				m_pkOwner->GetMapIndex(),
				x, y, z,
				false, (int)(m_pkOwner->GetRotation()+180), false);

	if (0 == m_pkChar)
	{
		sys_err("[CPetSystem::Summon] Failed to summon the pet. (vnum: %d)", m_dwVnum);
		return 0;
	}

	m_pkChar->SetPet();
#ifdef PET_LV
	m_pkChar->SetLevel(m_pkOwner->GetLevel());
#endif
//	m_pkOwner->DetailLog();
//	m_pkChar->DetailLog();

	m_pkChar->SetEmpire(m_pkOwner->GetEmpire());

#ifdef PET_ATTACK
	m_pkChar->SetOwner(m_pkOwner);
#endif

	m_dwVID = m_pkChar->GetVID();

	this->SetName(petName);

	this->SetSummonItem(pSummonItem);
	m_pkOwner->ComputePoints();
	m_pkChar->Show(m_pkOwner->GetMapIndex(), x, y, z);

	return m_dwVID;
}

bool CPetActor::_UpdatAloneActionAI(float fMinDist, float fMaxDist)
{
	float fDist = number(fMinDist, fMaxDist);
	float r = (float)number (0, 359);
	float dest_x = GetOwner()->GetX() + fDist * cos(r);
	float dest_y = GetOwner()->GetY() + fDist * sin(r);

	//GetDeltaByDegree(m_pkChar->GetRotation(), fDist, &fx, &fy);

	//if (!(SECTREE_MANAGER::instance().IsMovablePosition(m_pkChar->GetMapIndex(), m_pkChar->GetX() + (int) fx, m_pkChar->GetY() + (int) fy)
	//			&& SECTREE_MANAGER::instance().IsMovablePosition(m_pkChar->GetMapIndex(), m_pkChar->GetX() + (int) fx/2, m_pkChar->GetY() + (int) fy/2)))
	//	return true;

	m_pkChar->SetNowWalking(true);

	//if (m_pkChar->Goto(m_pkChar->GetX() + (int) fx, m_pkChar->GetY() + (int) fy))
	//	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	if (!m_pkChar->IsStateMove() && m_pkChar->Goto(dest_x, dest_y))
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	m_dwLastActionTime = get_dword_time();

	return true;
}

bool CPetActor::_UpdateFollowAI()
{
	if (0 == m_pkChar->m_pkMobData)
	{
		//sys_err("[CPetActor::_UpdateFollowAI] m_pkChar->m_pkMobData is NULL");
		return false;
	}

	if (0 == m_originalMoveSpeed)
	{
		const CMob* mobData = CMobManager::Instance().Get(m_dwVnum);

		if (0 != mobData)
			m_originalMoveSpeed = mobData->m_table.sMovingSpeed;
	}
	float	START_FOLLOW_DISTANCE = 1500.0f;
	float	START_RUN_DISTANCE = 3000.0f;

	float	RESPAWN_DISTANCE = 4500.0f;
	int		APPROACH = 750.0f;

	bool bRun = true;

	DWORD currentTime = get_dword_time();

	long ownerX = m_pkOwner->GetX();		long ownerY = m_pkOwner->GetY();
	long charX = m_pkChar->GetX();			long charY = m_pkChar->GetY();

	float fDist = DISTANCE_APPROX(charX - ownerX, charY - ownerY);

	if (fDist >= RESPAWN_DISTANCE)
	{
		float fOwnerRot = m_pkOwner->GetRotation() * 3.141592f / 180.f;
		float fx = -APPROACH * cos(fOwnerRot);
		float fy = -APPROACH * sin(fOwnerRot);
		if (m_pkChar->Show(m_pkOwner->GetMapIndex(), ownerX + fx, ownerY + fy))
		{
			return true;
		}
	}

	if (fDist >= START_FOLLOW_DISTANCE)
	{
		if( fDist >= START_RUN_DISTANCE)
		{
			bRun = true;
		}

		m_pkChar->SetNowWalking(!bRun);

		Follow(APPROACH);

		m_pkChar->SetLastAttacked(currentTime);
		m_dwLastActionTime = currentTime;
	}
	//else
	//{
	//	if (fabs(m_pkChar->GetRotation() - GetDegreeFromPositionXY(charX, charY, ownerX, ownerX)) > 10.f || fabs(m_pkChar->GetRotation() - GetDegreeFromPositionXY(charX, charY, ownerX, ownerX)) < 350.f)
	//	{
	//		m_pkChar->Follow(m_pkOwner, APPROACH);
	//		m_pkChar->SetLastAttacked(currentTime);
	//		m_dwLastActionTime = currentTime;
	//	}
	//}

	else
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	//else if (currentTime - m_dwLastActionTime > number(5000, 12000))
	//{
	//	this->_UpdatAloneActionAI(START_FOLLOW_DISTANCE / 2, START_FOLLOW_DISTANCE);
	//}

	return true;
}
#ifdef PET_ATTACK
bool CPetActor::Update(DWORD deltaTime)
{
    bool bResult = false;

    if (this->GetCharacter()->GetVictim() == m_pkOwner)
    {
        this->GetCharacter()->SetVictim(nullptr);
        return false;
    }

    if (m_pkOwner->IsDead() || (IsSummoned() && m_pkChar->IsDead())
            || NULL == ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())
            || ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetOwner() != this->GetOwner()
            )
    {
        this->Unsummon();
        return true;
    }


    if (this->GetCharacter()->GetVictim())
    {
        bResult = this->Attack();
    }
 
    if (!bResult)
    {
    if (this->IsSummoned() && HasOption(EPetOption_Followable))
        bResult = this->_UpdateFollowAI();
    }

#ifdef PET_AUTO_PICKUP
	if (this->IsSummoned() && IsAttackingPet(this->m_pkChar->GetRaceNum()))
	{
		this->CheckPetPickup(get_global_time());
	}
#endif

    return bResult;
}
#else
bool CPetActor::Update(DWORD deltaTime)
{
	bool bResult = true;


	if (m_pkOwner->IsDead() || (IsSummoned() && m_pkChar->IsDead())
		|| NULL == ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())
		|| ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetOwner() != this->GetOwner()
		)

	{
		this->Unsummon();
		return true;
	}

	if (this->IsSummoned() && HasOption(EPetOption_Followable))
		bResult = bResult && this->_UpdateFollowAI();

#ifdef PET_AUTO_PICKUP
	if (this->IsSummoned() && this->m_pkChar->GetRaceNum() == 34200)	//Bruce
	{
		this->CheckPetPickup(get_global_time());
	}
#endif


	return bResult;
}
#endif




#ifdef PET_ATTACK
bool CPetActor::MouseFollow(long int x, long int y)
{

	if( !m_pkOwner || !m_pkChar) 
		return false;

	long AmkPetX = x;
	long AmkPetY = y;
	AmkPetX /= 100;
	AmkPetY /= 100;

	PIXEL_POSITION p;
	if (SECTREE_MANAGER::instance().GetMapBasePosition(m_pkChar->GetX(), m_pkChar->GetY(), p))
	{
		AmkPetX += p.x / 100;
		AmkPetY += p.y / 100;
	}

	float fOwnerX = AmkPetX*100;
	float fOwnerY = AmkPetY*100;

	float fPetX = m_pkChar->GetX();
	float fPetY = m_pkChar->GetY();

	float fDist = DISTANCE_SQRT(fOwnerX - fPetX, fOwnerY - fPetY);

	m_pkChar->SetRotationToXY(fOwnerX, fOwnerY);

	float fx, fy;

	float fDistToGo = fDist;
	GetDeltaByDegree(m_pkChar->GetRotation(), fDistToGo, &fx, &fy);

	if (!m_pkChar->Goto((int)(fPetX+fx+0.5f), (int)(fPetY+fy+0.5f)) )
		return false;

	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);

	return true;
}
#endif
bool CPetActor::Follow(float fMinDistance)
{
	if( !m_pkOwner || !m_pkChar)
		return false;

	float fOwnerX = m_pkOwner->GetX();
	float fOwnerY = m_pkOwner->GetY();

	float fPetX = m_pkChar->GetX();
	float fPetY = m_pkChar->GetY();

	float fDist = DISTANCE_SQRT(fOwnerX - fPetX, fOwnerY - fPetY);
	if (fDist <= fMinDistance)
		return false;

	m_pkChar->SetRotationToXY(fOwnerX, fOwnerY);

	float fx, fy;

	float fDistToGo = fDist - fMinDistance;
	GetDeltaByDegree(m_pkChar->GetRotation(), fDistToGo, &fx, &fy);

	if (!m_pkChar->Goto((int)(fPetX+fx+0.5f), (int)(fPetY+fy+0.5f)) )
		return false;

	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);

	return true;
}

#define ENABLE_ACTIVE_PET_SEAL_EFFECT
void CPetActor::SetSummonItem(LPITEM pItem)
{
	if (!pItem)
	{
		#ifdef ENABLE_ACTIVE_PET_SEAL_EFFECT
		LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(m_dwSummonItemVID);
		if (pSummonItem)
			pSummonItem->SetSocket(2, false);
		#endif
		m_dwSummonItemVID = 0;
		m_dwSummonItemVnum = 0;
#ifdef PET_AUTO_PICKUP
		this->SetPickupItem(nullptr);
		this->SetPickup(false);
		this->SetPickupTime(0);
#endif

		return;
	}
	#ifdef ENABLE_ACTIVE_PET_SEAL_EFFECT
	pItem->SetSocket(2, true);
	#endif
	m_dwSummonItemVID = pItem->GetVID();
	m_dwSummonItemVnum = pItem->GetVnum();
}

bool __PetCheckBuff(const CPetActor* pPetActor)
{
	bool bMustHaveBuff = true;
	switch (pPetActor->GetVnum())
	{
		case 34004:
		case 34009:
			if (!pPetActor->GetOwner()->GetDungeon())
				bMustHaveBuff = false;
		default:
			break;
	}
	return bMustHaveBuff;
}

#ifdef PET_ATTACK
bool CPetActor::Attack(LPCHARACTER pkVictim)
{
    // Check if attacking pets are allowed
    if (!IsAttackingPet(this->GetVnum()))
        return false;

    // Validate m_pkChar
    if (!m_pkChar)
        return false;

    // If a victim is provided
    if (pkVictim)
    {
        // Validate pkVictim
        if (!pkVictim)
            return false;

        if (!(pkVictim->IsMonster() || pkVictim->IsStone()) || pkVictim->IsDead())
            return false;

        // Check if pet already has a victim
        if (m_pkChar->GetVictim())
            return false;

        // Set the victim's owner as the pet's victim
        pkVictim->SetVictim(m_pkOwner);
    }
    else
    {
        // No victim provided, get current victim
        pkVictim = m_pkChar->GetVictim();

        // Validate retrieved victim
        if (!pkVictim)
            return false;
    }

    // Set victim
    m_pkChar->SetVictim(pkVictim);

    // Validate GetXYZ() calls
    const PIXEL_POSITION& rkPetPos = m_pkChar->GetXYZ();
    const PIXEL_POSITION& rkVictimPos = pkVictim->GetXYZ();

    int iDistance = DISTANCE_APPROX(rkPetPos.x - rkVictimPos.x, rkPetPos.y - rkVictimPos.y);

    if (iDistance >= m_pkChar->GetMobAttackRange())
    {
        m_pkChar->Follow(pkVictim, m_pkChar->GetMobAttackRange());
    }
    else
    {
        if (get_dword_time() - m_pkChar->GetLastAttackTime() >= 3000)
        {
            if (m_pkChar->Attack(pkVictim))
            {
                m_pkChar->SendMovePacket(FUNC_ATTACK, 0, rkPetPos.x, rkPetPos.y, 0, get_dword_time());
                m_pkChar->SetLastAttacked(get_dword_time());
                m_pkChar->EffectPacket(SE_CRITICAL);
            }
        }
    }

    return true;
}
#endif

void CPetActor::GiveBuff()
{
	if (!__PetCheckBuff(this))
		return;
	LPITEM item = ITEM_MANAGER::instance().FindByVID(m_dwSummonItemVID);
	if (item)
		item->ModifyPoints(true);
	return ;
}

void CPetActor::ClearBuff()
{
	if (NULL == m_pkOwner)
		return ;
	TItemTable* item_proto = ITEM_MANAGER::instance().GetTable(m_dwSummonItemVnum);
	if (NULL == item_proto)
		return;
	if (!__PetCheckBuff(this)) // @fixme129
		return;
	for (int i = 0; i < ITEM_APPLY_MAX_NUM; i++)
	{
		if (item_proto->aApplies[i].bType == APPLY_NONE)
			continue;
		m_pkOwner->ApplyPoint(item_proto->aApplies[i].bType, -item_proto->aApplies[i].lValue);
	}

	return ;
}


#ifdef PET_AUTO_PICKUP
bool CPetActor::CheckPetPickup(time_t time)
{
	if (!GetPickupTime())
	{
		SetPickupTime(time);
		//m_pkOwner->ChatPacket(7, "gobale time set %d", pickTime);
	}
	else
	{
		if (GetPickupTime() + 10 < time)
		{
			//m_pkOwner->ChatPacket(7, "Start Pickup Timer %d", time);
			PickUpItems(900); // 900 = RANGE
			BringItem();
			return true;
		}
	}

	return true;
}

struct PetPickUpItemStruct
{
	CPetActor* pet;
	int range;
	PetPickUpItemStruct(CPetActor* p, int r)
	{
		pet = p;
		range = r;
	}

	void operator()(LPENTITY pEnt)
	{
		if (!pet->GetOwner() || !pet->GetCharacter())
			return;

		if (pet->IsPickup())
			return;

		if (pEnt->IsType(ENTITY_ITEM) == true)
		{
			LPITEM item = (LPITEM)pEnt;
			LPCHARACTER player = pet->GetOwner();

			if (!item || !player)
				return;

			if (!item->GetSectree() || !item->IsOwnership(player))
				return;

			const int iDist = DISTANCE_APPROX(item->GetX() - player->GetX(), player->GetY() - player->GetY());

			if (iDist > range)
				return;

			pet->SetPickup(true);
			pet->SetPickupItem(item);
		}

	}
};

void CPetActor::PickUpItems(int range)
{
	if (IsPickup())
		return;

	const long map = m_pkChar->GetMapIndex();
	const PIXEL_POSITION m = m_pkChar->GetXYZ();
	LPSECTREE tree = SECTREE_MANAGER::Instance().Get(map, m.x, m.y);
	if (!tree)
	{
		sys_err("cannot find sectree by %dx%d", m.x, m.y);
		return;
	}

	PetPickUpItemStruct f(this, range);
	tree->ForEachAround(f);
}

void CPetActor::BringItem()
{
	if (!IsPickup())
		return;

	if (GetPickupItem() == nullptr)
		return;

	if (!m_pkOwner || !m_pkChar)
		return;

	LPITEM item = GetPickupItem();
	if (item == nullptr)
		return;

	if (item)
	{
		constexpr float fMinDistance = 20.0f;
		const float fDist = DISTANCE_SQRT(item->GetX() - m_pkChar->GetX(), item->GetY() - m_pkChar->GetY());
		if (fDist <= 250.0)
		{
			SetPickup(false);
			m_pkOwner->PickupItemByPet(item->GetVID());
			SetPickupItem(nullptr);
			m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);
			return;
		}
		m_pkChar->SetRotationToXY(item->GetX(), item->GetY());
		float fx, fy;

		const float fDistToGo = fDist - fMinDistance;
		GetDeltaByDegree(m_pkChar->GetRotation(), fDistToGo, &fx, &fy);

		if (!m_pkChar->Goto(static_cast<long>(m_pkChar->GetX() + fx + 0.5f), static_cast<long>(m_pkChar->GetY() + fy + 0.5f)))
			return;

		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);
	}
}
#endif

void CPetActor::SetVictim(LPCHARACTER victim)
{
    m_pkChar->SetVictim(victim); // Assuming m_pkChar has a method for this
}
///////////////////////////////////////////////////////////////////////////////////////
//  CPetSystem
///////////////////////////////////////////////////////////////////////////////////////

CPetSystem::CPetSystem(LPCHARACTER owner)
{
//	assert(0 != owner && "[CPetSystem::CPetSystem] Invalid owner");

	m_pkOwner = owner;
	m_dwUpdatePeriod = 400;

	m_dwLastUpdateTime = 0;
#ifdef PET_ATTACK
	m_GetPetVnum = 0;
#endif
}

CPetSystem::~CPetSystem()
{
	Destroy();
}

void CPetSystem::Destroy()
{
	for (TPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CPetActor* petActor = iter->second;

		if (0 != petActor)
		{
			delete petActor;
		}
	}
	event_cancel(&m_pkPetSystemUpdateEvent);
	m_petActorMap.clear();
}

#ifdef PET_ATTACK
void CPetSystem::LaunchAttack(LPCHARACTER pkVictim)
{
    if (!pkVictim)
        return;

    for (itertype(m_petActorMap) it = m_petActorMap.begin(); it != m_petActorMap.end(); ++it)
    {
        CPetActor* pkPetActor = it->second;

        pkPetActor->Attack(pkVictim);


    }
}


///

void CPetSystem::PetMouseFollow(long int x, long int y)
{
	for (itertype(m_petActorMap) it = m_petActorMap.begin(); it != m_petActorMap.end(); ++it)
	{
		CPetActor* pkPetActor = it->second;
		if (pkPetActor->IsSummoned())
			pkPetActor->MouseFollow(x, y);
	}
}

void CPetSystem::PetUpdateFollow()
{
	for (itertype(m_petActorMap) it = m_petActorMap.begin(); it != m_petActorMap.end(); ++it)
	{
		CPetActor* pkPetActor = it->second;
		if (pkPetActor->IsSummoned())
			pkPetActor->Follow(80);
	}
}

#endif

bool CPetSystem::Update(DWORD deltaTime)
{
	bool bResult = true;

	DWORD currentTime = get_dword_time();

	if (m_dwUpdatePeriod > currentTime - m_dwLastUpdateTime)
		return true;

	std::vector <CPetActor*> v_garbageActor;

	for (TPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CPetActor* petActor = iter->second;

		if (0 != petActor && petActor->IsSummoned())
		{
			LPCHARACTER pPet = petActor->GetCharacter();

			if (NULL == CHARACTER_MANAGER::instance().Find(pPet->GetVID()))
			{
				v_garbageActor.emplace_back(petActor);
			}
			else
			{
				bResult = bResult && petActor->Update(deltaTime);
			}
		}
			SetPetVnum(petActor->GetVnum());
	}
	for (std::vector<CPetActor*>::iterator it = v_garbageActor.begin(); it != v_garbageActor.end(); it++)
		DeletePet(*it);

	m_dwLastUpdateTime = currentTime;

	return bResult;
}

void CPetSystem::DeletePet(DWORD mobVnum)
{
	TPetActorMap::iterator iter = m_petActorMap.find(mobVnum);

	if (m_petActorMap.end() == iter)
	{
		sys_err("[CPetSystem::DeletePet] Can't find pet on my list (VNUM: %d)", mobVnum);
		return;
	}

	CPetActor* petActor = iter->second;

	if (0 == petActor)
		sys_err("[CPetSystem::DeletePet] Null Pointer (petActor)");
	else
		delete petActor;

	m_petActorMap.erase(iter);
}

void CPetSystem::DeletePet(CPetActor* petActor)
{
	for (TPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		if (iter->second == petActor)
		{
			delete petActor;
			m_petActorMap.erase(iter);

			return;
		}
	}

	sys_err("[CPetSystem::DeletePet] Can't find petActor(0x%x) on my list(size: %d) ", petActor, m_petActorMap.size());
}

void CPetSystem::Unsummon(DWORD vnum, bool bDeleteFromList)
{
	CPetActor* actor = this->GetByVnum(vnum);

	if (0 == actor)
	{
		sys_err("[CPetSystem::GetByVnum(%d)] Null Pointer (petActor)", vnum);
		return;
	}
	actor->Unsummon();

	if (true == bDeleteFromList)
		this->DeletePet(actor);

	bool bActive = false;
	for (TPetActorMap::iterator it = m_petActorMap.begin(); it != m_petActorMap.end(); it++)
	{
		bActive |= it->second->IsSummoned();
	}
	if (false == bActive)
	{
		event_cancel(&m_pkPetSystemUpdateEvent);
		m_pkPetSystemUpdateEvent = NULL;
	}
}

void CPetSystem::UnsummonAll()
{
	for (auto & iter : m_petActorMap)
	{
		auto * actor = iter.second;
		if (actor)
			actor->Unsummon();
	}

	bool bActive = false;
	for (auto & it : m_petActorMap)
		bActive |= it.second->IsSummoned();
	if (!bActive)
	{
		event_cancel(&m_pkPetSystemUpdateEvent);
		m_pkPetSystemUpdateEvent = nullptr;
	}
}

CPetActor* CPetSystem::Summon(DWORD mobVnum, LPITEM pSummonItem, const char* petName, bool bSpawnFar, DWORD options)
{
	CPetActor* petActor = this->GetByVnum(mobVnum);

	if (0 == petActor)
	{
		petActor = M2_NEW CPetActor(m_pkOwner, mobVnum, options);
		m_petActorMap.emplace(mobVnum, petActor);
	}

#ifdef ENABLE_NEWSTUFF
	DWORD petVID = petActor->Summon(petName, pSummonItem, bSpawnFar);
	if (!petVID)
		sys_err("[CPetSystem::Summon(%d)] Null Pointer (petVID)", pSummonItem);
#endif

	if (NULL == m_pkPetSystemUpdateEvent)
	{
		petsystem_event_info* info = AllocEventInfo<petsystem_event_info>();

		info->pPetSystem = this;

		m_pkPetSystemUpdateEvent = event_create(petsystem_update_event, info, PASSES_PER_SEC(1) / 4);
	}

	return petActor;
}

CPetActor* CPetSystem::GetByVID(DWORD vid) const
{
	CPetActor* petActor = 0;

	bool bFound = false;

	for (TPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		petActor = iter->second;

		if (0 == petActor)
		{
			sys_err("[CPetSystem::GetByVID(%d)] Null Pointer (petActor)", vid);
			continue;
		}

		bFound = petActor->GetVID() == vid;

		if (true == bFound)
			break;
	}

	return bFound ? petActor : 0;
}

CPetActor* CPetSystem::GetByVnum(DWORD vnum) const
{
	CPetActor* petActor = 0;

	TPetActorMap::const_iterator iter = m_petActorMap.find(vnum);

	if (m_petActorMap.end() != iter)
		petActor = iter->second;

	return petActor;
}

size_t CPetSystem::CountSummoned() const
{
	size_t count = 0;

	for (TPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CPetActor* petActor = iter->second;

		if (0 != petActor)
		{
			if (petActor->IsSummoned())
				++count;
		}
	}

	return count;
}

void CPetSystem::RefreshBuff()
{
	for (TPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CPetActor* petActor = iter->second;
		if (petActor && petActor->IsSummoned())
			petActor->GiveBuff();
	}
}



void CPetSystem::StopAttack()
{
    for (auto& iter : m_petActorMap)
    {
        CPetActor* petActor = iter.second;
        if (petActor->IsSummoned())
        {
            petActor->SetVictim(NULL);  // Assuming this method exists
        }
    }
}
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
