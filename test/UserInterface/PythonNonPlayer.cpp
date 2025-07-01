#include "stdafx.h"
#include "../eterPack/EterPackManager.h"
#include "pythonnonplayer.h"
#include "InstanceBase.h"
#include "PythonCharacterManager.h"

bool CPythonNonPlayer::LoadNonPlayerData(const char * c_szFileName)
{
	static DWORD s_adwMobProtoKey[4] =
	{
		4813894,
		18955,
		552631,
		6822045
	};

	CMappedFile file;
	LPCVOID pvData;

	Tracef("CPythonNonPlayer::LoadNonPlayerData: %s, sizeof(TMobTable)=%u\n", c_szFileName, sizeof(TMobTable));

	if (!CEterPackManager::Instance().Get(file, c_szFileName, &pvData))
		return false;

	DWORD dwFourCC, dwElements, dwDataSize;

	file.Read(&dwFourCC, sizeof(DWORD));

	if (dwFourCC != MAKEFOURCC('M', 'M', 'P', 'T'))
	{
		TraceError("CPythonNonPlayer::LoadNonPlayerData: invalid Mob proto type %s", c_szFileName);
		return false;
	}

	file.Read(&dwElements, sizeof(DWORD));
	file.Read(&dwDataSize, sizeof(DWORD));

	BYTE * pbData = new BYTE[dwDataSize];
	file.Read(pbData, dwDataSize);
	/////

	CLZObject zObj;

	if (!CLZO::Instance().Decompress(zObj, pbData, s_adwMobProtoKey))
	{
		delete [] pbData;
		return false;
	}

	DWORD structSize = zObj.GetSize() / dwElements;
	DWORD structDiff = zObj.GetSize() % dwElements;
#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
	if (structDiff!=0 && !CPythonNonPlayer::TMobTableAll::IsValidStruct(structSize))
#else
	if ((zObj.GetSize() % sizeof(TMobTable)) != 0)
#endif
	{
		TraceError("CPythonNonPlayer::LoadNonPlayerData: invalid size %u check data format. structSize %u, structDiff %u", zObj.GetSize(), structSize, structDiff);
		return false;
	}

    for (DWORD i = 0; i < dwElements; ++i)
	{
#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
		CPythonNonPlayer::TMobTable t = {0};
		CPythonNonPlayer::TMobTableAll::Process(zObj.GetBuffer(), structSize, i, t);
#else
		CPythonNonPlayer::TMobTable & t = *((CPythonNonPlayer::TMobTable *) zObj.GetBuffer() + i);
#endif
		TMobTable * pTable = &t;


#ifdef INGAME_WIKI
		auto ptr = std::make_unique <TMobTable>();
		*ptr = t;
		m_NonPlayerDataMap[t.dwVnum].mobTable = std::move(ptr);
		m_NonPlayerDataMap[t.dwVnum].isSet = false;
		m_NonPlayerDataMap[t.dwVnum].isFiltered = false;
		m_NonPlayerDataMap[t.dwVnum].dropList.clear();
#else
		TMobTable * pNonPlayerData = new TMobTable;
		memcpy(pNonPlayerData, pTable, sizeof(TMobTable));

		//TraceError("%d : %s type[%d] color[%d]", pNonPlayerData->dwVnum, pNonPlayerData->szLocaleName, pNonPlayerData->bType, pNonPlayerData->dwMonsterColor);
		m_NonPlayerDataMap.insert(TNonPlayerDataMap::value_type(pNonPlayerData->dwVnum, pNonPlayerData));
#endif

	}

	delete [] pbData;
	return true;
}

bool CPythonNonPlayer::GetName(DWORD dwVnum, const char ** c_pszName)
{
	const TMobTable * p = GetTable(dwVnum);

	if (!p)
		return false;

	*c_pszName = p->szLocaleName;

	return true;
}

bool CPythonNonPlayer::GetInstanceType(DWORD dwVnum, BYTE* pbType)
{
	const TMobTable * p = GetTable(dwVnum);

	if (!p)
		return false;

	*pbType=p->bType;

	return true;
}

const CPythonNonPlayer::TMobTable * CPythonNonPlayer::GetTable(DWORD dwVnum)
{
	TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.find(dwVnum);

	if (itor == m_NonPlayerDataMap.end())
		return NULL;


#ifdef INGAME_WIKI
	return itor->second.mobTable.get();
#else
	return itor->second;
#endif
}

BYTE CPythonNonPlayer::GetEventType(DWORD dwVnum)
{
	const TMobTable * p = GetTable(dwVnum);

	if (!p)
	{
		//Tracef("CPythonNonPlayer::GetEventType - Failed to find virtual number\n");
		return ON_CLICK_EVENT_NONE;
	}

	return p->bOnClickType;
}

#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
DWORD CPythonNonPlayer::GetMonsterLevel(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
		return 0;

	return c_pTable->bLevel;
}
#endif

#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
bool CPythonNonPlayer::IsAggressive(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
		return 0;

	return (IS_SET(c_pTable->dwAIFlag, AIFLAG_AGGRESSIVE));
}
#endif

BYTE CPythonNonPlayer::GetEventTypeByVID(DWORD dwVID)
{
	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwVID);

	if (NULL == pInstance)
	{
		//Tracef("CPythonNonPlayer::GetEventTypeByVID - There is no Virtual Number\n");
		return ON_CLICK_EVENT_NONE;
	}

	WORD dwVnum = pInstance->GetVirtualNumber();
	return GetEventType(dwVnum);
}

const char*	CPythonNonPlayer::GetMonsterName(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		static const char* sc_szEmpty="";
		return sc_szEmpty;
	}

	return c_pTable->szLocaleName;
}

DWORD CPythonNonPlayer::GetMonsterColor(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
		return 0;

	return c_pTable->dwMonsterColor;
}

DWORD CPythonNonPlayer::GetMonsterType(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
		return 0;

	return c_pTable->bType;
}

DWORD CPythonNonPlayer::GetMonsterRank(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
		return 0;

	return c_pTable->bRank;
}

#ifdef ENABLE_SEND_TARGET_INFO
DWORD CPythonNonPlayer::GetMonsterMaxHP(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		DWORD dwMaxHP = 0;
		return dwMaxHP;
	}

	return c_pTable->dwMaxHP;
}

DWORD CPythonNonPlayer::GetMonsterRaceFlag(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		DWORD dwRaceFlag = 0;
		return dwRaceFlag;
	}

	return c_pTable->dwRaceFlag;
}

//DWORD CPythonNonPlayer::GetMonsterLevel(DWORD dwVnum)
//{
//	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
//	if (!c_pTable)
//	{
//		DWORD level = 0;
//		return level;
//	}

//	return c_pTable->bLevel;
//}

DWORD CPythonNonPlayer::GetMonsterDamage1(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		DWORD range = 0;
		return range;
	}

	return c_pTable->dwDamageRange[0];
}

DWORD CPythonNonPlayer::GetMonsterDamage2(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		DWORD range = 0;
		return range;
	}

	return c_pTable->dwDamageRange[1];
}

DWORD CPythonNonPlayer::GetMonsterExp(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		DWORD dwExp = 0;
		return dwExp;
	}

	return c_pTable->dwExp;
}

float CPythonNonPlayer::GetMonsterDamageMultiply(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		DWORD fDamMultiply = 0;
		return fDamMultiply;
	}

	return c_pTable->fDamMultiply;
}

DWORD CPythonNonPlayer::GetMonsterST(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		DWORD bStr = 0;
		return bStr;
	}

	return c_pTable->bStr;
}

DWORD CPythonNonPlayer::GetMonsterDX(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		DWORD bDex = 0;
		return bDex;
	}

	return c_pTable->bDex;
}

bool CPythonNonPlayer::IsMonsterStone(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		DWORD bType = 0;
		return bType;
	}

	return c_pTable->bType == 2;
}
#endif

void CPythonNonPlayer::GetMatchableMobList(int iLevel, int iInterval, TMobTableList * pMobTableList)
{
/*
	pMobTableList->clear();

	TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.begin();
	for (; itor != m_NonPlayerDataMap.end(); ++itor)
	{
		TMobTable * pMobTable = itor->second;

		int iLowerLevelLimit = iLevel-iInterval;
		int iUpperLevelLimit = iLevel+iInterval;

		if ((pMobTable->abLevelRange[0] >= iLowerLevelLimit && pMobTable->abLevelRange[0] <= iUpperLevelLimit) ||
			(pMobTable->abLevelRange[1] >= iLowerLevelLimit && pMobTable->abLevelRange[1] <= iUpperLevelLimit))
		{
			pMobTableList->push_back(pMobTable);
		}
	}
*/
}

#ifdef INGAME_WIKI
#include "../GameLib/RaceManager.h"

bool CPythonNonPlayer::CanRenderMonsterModel(DWORD dwMonsterVnum)
{
	CRaceData * pRaceData;
	if (!CRaceManager::Instance().GetRaceDataPointer(dwMonsterVnum, &pRaceData, false))
		return false;
	
	return true;
}

size_t CPythonNonPlayer::WikiLoadClassMobs(BYTE bType, unsigned short fromLvl, unsigned short toLvl)
{
	m_vecTempMob.clear();
	for (auto it = m_NonPlayerDataMap.begin(); it != m_NonPlayerDataMap.end(); ++it)
	{
		if (!it->second.isFiltered && it->second.mobTable->bLevel >= fromLvl &&
			it->second.mobTable->bLevel < toLvl && CanRenderMonsterModel(it->second.mobTable->dwVnum))
		{
			if (bType == 0 && it->second.mobTable->bType == MONSTER && it->second.mobTable->bRank >= 4)
				m_vecTempMob.push_back(it->first);
			else if (bType == 1 && it->second.mobTable->bType == MONSTER && it->second.mobTable->bRank < 4)
				m_vecTempMob.push_back(it->first);
			else if (bType == 2 && it->second.mobTable->bType == STONE)
				m_vecTempMob.push_back(it->first);
		}
	}
	
	return m_vecTempMob.size();
}

void CPythonNonPlayer::WikiSetBlacklisted(DWORD vnum)
{
	auto it = m_NonPlayerDataMap.find(vnum);
		if (it != m_NonPlayerDataMap.end())
			it->second.isFiltered = true;
}

std::tuple<const char*, int> CPythonNonPlayer::GetMonsterDataByNamePart(const char* namePart)
{
	char searchName[CHARACTER_NAME_MAX_LEN + 1];
	memcpy(searchName, namePart, sizeof(searchName));
	for (size_t j = 0; j < sizeof(searchName); j++)
		searchName[j] = static_cast<char>(tolower(searchName[j]));
	std::string tempSearchName = searchName;

	TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.begin();
	for (; itor != m_NonPlayerDataMap.end(); ++itor)
	{
		TMobTable* pMobTable = itor->second.mobTable.get();
		
		if (itor->second.isFiltered)
			continue;
		
		const char * mobBaseName = CPythonNonPlayer::Instance().GetNameString(pMobTable->dwVnum).c_str();
		std::string tempName = mobBaseName;
		if (!tempName.size())
			continue;
		
		std::transform(tempName.begin(), tempName.end(), tempName.begin(), ::tolower);

		const size_t tempSearchNameLenght = tempSearchName.length();
		if (tempName.length() < tempSearchNameLenght)
			continue;

		if (!tempName.substr(0, tempSearchNameLenght).compare(tempSearchName))
			return std::make_tuple(mobBaseName, pMobTable->dwVnum);
	}

	return std::make_tuple("", -1);
}

void CPythonNonPlayer::BuildWikiSearchList()
{
	m_vecWikiNameSort.clear();
	for (auto it = m_NonPlayerDataMap.begin(); it != m_NonPlayerDataMap.end(); ++it)
		if (!it->second.isFiltered)
			m_vecWikiNameSort.push_back(it->second.mobTable.get());
	
	SortMobDataName();
}

void CPythonNonPlayer::SortMobDataName()
{
	std::qsort(&m_vecWikiNameSort[0], m_vecWikiNameSort.size(), sizeof(m_vecWikiNameSort[0]), [](const void* a, const void* b)
		{
			TMobTable* pItem1 = *(TMobTable**)(static_cast<const TMobTable*>(a));
			std::string stRealName1 = pItem1->szLocaleName;
			std::transform(stRealName1.begin(), stRealName1.end(), stRealName1.begin(), ::tolower);
			
			TMobTable* pItem2 = *(TMobTable**)(static_cast<const TMobTable*>(b));
			std::string stRealName2 = pItem2->szLocaleName;
			std::transform(stRealName2.begin(), stRealName2.end(), stRealName2.begin(), ::tolower);
			
			int iSmallLen = min(stRealName1.length(), stRealName2.length());
			int iRetCompare = stRealName1.compare(0, iSmallLen, stRealName2, 0, iSmallLen);
			
			if (iRetCompare != 0)
				return iRetCompare;
			
			if (stRealName1.length() < stRealName2.length())
				return -1;
			else if (stRealName2.length() < stRealName1.length())
				return 1;
			
			return 0;
		});
}

CPythonNonPlayer::TWikiInfoTable* CPythonNonPlayer::GetWikiTable(DWORD dwVnum)
{
	TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.find(dwVnum);

	if (itor == m_NonPlayerDataMap.end())
		return NULL;

	return &(itor->second);
}
std::string CPythonNonPlayer::GetNameString(DWORD dwVnum)
{
	const TMobTable * p = GetTable(dwVnum);

	if (!p)
	{
		return "";
	}

	// std::string translatedName = GetNameTranslation(dwVnum);
	// if (translatedName != "")
	// {
		// return translatedName;
	// }

	return std::string(p->szLocaleName);
}
#endif

void CPythonNonPlayer::Clear()
{
}

void CPythonNonPlayer::Destroy()
{
	for (TNonPlayerDataMap::iterator itor=m_NonPlayerDataMap.begin(); itor!=m_NonPlayerDataMap.end(); ++itor)
	{
#ifdef INGAME_WIKI
		m_NonPlayerDataMap.erase(itor);
#else
		delete itor->second;
#endif
	}
	m_NonPlayerDataMap.clear();
}

CPythonNonPlayer::CPythonNonPlayer()
{
	Clear();
}

CPythonNonPlayer::~CPythonNonPlayer(void)
{
	Destroy();
}
#ifdef ENABLE_NPC_WEAR_ITEM
#include "PythonBackground.h"
bool CPythonNonPlayer::LoadNPCWear(const char* szNPCWearFile)
{
	m_mapnpcWear.clear();
	CMappedFile File;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(File, szNPCWearFile, &pData))
		return false;
	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);
	CTokenVector TokenVector;
	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
			continue;
		else if (TokenVector.size() != NPC_WEAR_MAX+2)
			continue;
		const DWORD npcVnum = atoi(TokenVector[0].c_str());
		std::vector<int> m_vec;
		m_vec.resize(NPC_WEAR_MAX);
		for (BYTE i = 0; i < NPC_WEAR_MAX; ++i)
			m_vec[i] = atoi(TokenVector[i+2].c_str());
		if (m_vec[NPC_WEAR_RACE] > 7)
			continue;
		auto it = m_mapnpcWear.find(TokenVector[1].c_str());
		if (it != m_mapnpcWear.end())
			it->second.emplace(npcVnum, m_vec);
		else
		{
			std::map<DWORD, std::vector<int>> m_map;
			m_map.emplace(npcVnum, m_vec);
			m_mapnpcWear.emplace(TokenVector[1].c_str(), m_map);
		}
	}
	return true;
}
int CPythonNonPlayer::GetNPCWear(const int raceIndex, const BYTE wearIndex)
{
	if (wearIndex >= NPC_WEAR_MAX)
		return -1;
	CPythonBackground& rkBG = CPythonBackground::Instance();
	const auto itSpecial = m_mapnpcWear.find(rkBG.GetWarpMapName());
	if (itSpecial != m_mapnpcWear.end())
	{
		auto itEx = itSpecial->second.find(raceIndex);
		if (itEx != itSpecial->second.end())
			return itEx->second[wearIndex];
	}
	const auto itAll = m_mapnpcWear.find("all");
	if (itAll != m_mapnpcWear.end())
	{
		auto itEx = itAll->second.find(raceIndex);
		if (itEx != itAll->second.end())
			return itEx->second[wearIndex];
	}
	return -1;
}
#endif



//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
