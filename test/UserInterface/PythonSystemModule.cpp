#include "StdAfx.h"
#include "PythonSystem.h"

#ifdef ENABLE_DOG_MODE
#include "PythonCharacterManager.h"
#include "../GameLib/RaceManager.h"
#endif

PyObject * systemGetWidth(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetWidth());
}

PyObject * systemGetHeight(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetHeight());
}

PyObject * systemSetInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonSystem::Instance().SetInterfaceHandler(poHandler);
	return Py_BuildNone();
}

PyObject * systemDestroyInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	CPythonSystem::Instance().DestroyInterfaceHandler();
	return Py_BuildNone();
}

PyObject * systemReserveResource(PyObject* poSelf, PyObject* poArgs)
{
	std::set<std::string> ResourceSet;
	CResourceManager::Instance().PushBackgroundLoadingSet(ResourceSet);
	return Py_BuildNone();
}

PyObject * systemisInterfaceConfig(PyObject* poSelf, PyObject* poArgs)
{
	int isInterfaceConfig = CPythonSystem::Instance().isInterfaceConfig();
	return Py_BuildValue("i", isInterfaceConfig);
}

PyObject * systemSaveWindowStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iVisible;
	if (!PyTuple_GetInteger(poArgs, 1, &iVisible))
		return Py_BuildException();

	int iMinimized;
	if (!PyTuple_GetInteger(poArgs, 2, &iMinimized))
		return Py_BuildException();

	int ix;
	if (!PyTuple_GetInteger(poArgs, 3, &ix))
		return Py_BuildException();

	int iy;
	if (!PyTuple_GetInteger(poArgs, 4, &iy))
		return Py_BuildException();

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 5, &iHeight))
		return Py_BuildException();

	CPythonSystem::Instance().SaveWindowStatus(iIndex, iVisible, iMinimized, ix, iy, iHeight);
	return Py_BuildNone();
}

PyObject * systemGetWindowStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const CPythonSystem::TWindowStatus & c_rWindowStatus = CPythonSystem::Instance().GetWindowStatusReference(iIndex);
	return Py_BuildValue("iiiii", c_rWindowStatus.isVisible,
								  c_rWindowStatus.isMinimized,
								  c_rWindowStatus.ixPosition,
								  c_rWindowStatus.iyPosition,
								  c_rWindowStatus.iHeight);
}

PyObject * systemGetConfig(PyObject * poSelf, PyObject * poArgs)
{
	CPythonSystem::TConfig *tmp = CPythonSystem::Instance().GetConfig();

	int iRes = CPythonSystem::Instance().GetResolutionIndex(tmp->width, tmp->height, tmp->bpp);
	int iFrequency = CPythonSystem::Instance().GetFrequencyIndex(iRes, tmp->frequency);

	return Py_BuildValue("iiiiiiii",  iRes,
									  iFrequency,
									  tmp->is_software_cursor,
									  tmp->is_object_culling,
									  tmp->music_volume,
									  tmp->voice_volume,
									  tmp->gamma,
									  tmp->iDistance);
}

PyObject * systemSetSaveID(PyObject * poSelf, PyObject * poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();

	char * szSaveID;
	if (!PyTuple_GetString(poArgs, 1, &szSaveID))
		return Py_BuildException();

	CPythonSystem::Instance().SetSaveID(iValue, szSaveID);
	return Py_BuildNone();
}

PyObject * systemisSaveID(PyObject * poSelf, PyObject * poArgs)
{
	int value = CPythonSystem::Instance().IsSaveID();
	return Py_BuildValue("i", value);
}

PyObject * systemGetSaveID(PyObject * poSelf, PyObject * poArgs)
{
	const char * c_szSaveID = CPythonSystem::Instance().GetSaveID();
	return Py_BuildValue("s", c_szSaveID);
}

PyObject * systemGetMusicVolume(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetMusicVolume());
}

PyObject * systemGetSoundVolume(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSoundVolume());
}

PyObject * systemSetMusicVolume(PyObject * poSelf, PyObject * poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetMusicVolume(fVolume);
	return Py_BuildNone();
}

PyObject * systemSetSoundVolumef(PyObject * poSelf, PyObject * poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetSoundVolumef(fVolume);
	return Py_BuildNone();
}

PyObject * systemIsSoftwareCursor(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsSoftwareCursor());
}

PyObject * systemSetViewChatFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetViewChatFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemIsViewChat(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsViewChat());
}

PyObject * systemSetAlwaysShowNameFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetAlwaysShowNameFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemSetShowDamageFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowDamageFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemSetShowSalesTextFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowSalesTextFlag(iFlag);

	return Py_BuildNone();
}

#ifdef WJ_SHOW_MOB_INFO
#include "PythonCharacterManager.h"
#include "PythonNonPlayer.h"
#include "PythonSystem.h"
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
PyObject * systemIsShowMobAIFlag(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowMobAIFlag());
}
PyObject * systemSetShowMobAIFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMobAIFlagFlag(iFlag);

	for (CPythonCharacterManager::CharacterIterator it=CPythonCharacterManager::Instance().CharacterInstanceBegin(), ti=CPythonCharacterManager::Instance().CharacterInstanceEnd();
			it!=ti;
			++it)
	{
		CInstanceBase * pkInst = *it;
		if (pkInst && pkInst->IsEnemy())
			pkInst->MobInfoAiFlagRefresh();
	}
	return Py_BuildNone();
}
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
PyObject * systemIsShowMobLevel(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowMobLevel());
}
PyObject * systemSetShowMobLevel(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMobLevelFlag(iFlag);

	for (CPythonCharacterManager::CharacterIterator it=CPythonCharacterManager::Instance().CharacterInstanceBegin(), ti=CPythonCharacterManager::Instance().CharacterInstanceEnd();
			it!=ti;
			++it)
	{
		CInstanceBase * pkInst = *it;
		if (pkInst && pkInst->IsEnemy())
			pkInst->MobInfoLevelRefresh();
	}
	return Py_BuildNone();
}
#endif
#endif

PyObject * systemIsAlwaysShowName(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsAlwaysShowName());
}

PyObject * systemIsShowDamage(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowDamage());
}

PyObject * systemIsShowSalesText(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowSalesText());
}

PyObject * systemSetConfig(PyObject * poSelf, PyObject * poArgs)
{
	int res_index;
	int width;
	int height;
	int bpp;
	int frequency_index;
	int frequency;
	int software_cursor;
	int shadow;
	int object_culling;
	int music_volume;
	int voice_volume;
	int gamma;
	int distance;

	if (!PyTuple_GetInteger(poArgs, 0, &res_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &software_cursor))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 3, &shadow))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 4, &object_culling))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 5, &music_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 6, &voice_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 7, &gamma))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 8, &distance))
		return Py_BuildException();

	if (!CPythonSystem::Instance().GetResolution(res_index, (DWORD *) &width, (DWORD *) &height, (DWORD *) &bpp))
		return Py_BuildNone();

	if (!CPythonSystem::Instance().GetFrequency(res_index,frequency_index, (DWORD *) &frequency))
		return Py_BuildNone();

	CPythonSystem::TConfig tmp;

	memcpy(&tmp, CPythonSystem::Instance().GetConfig(), sizeof(tmp));

	tmp.width				= width;
	tmp.height				= height;
	tmp.bpp					= bpp;
	tmp.frequency			= frequency;
	tmp.is_software_cursor	= software_cursor ? true : false;
	tmp.is_object_culling	= object_culling ? true : false;
	tmp.music_volume		= (char) music_volume;
	tmp.voice_volume		= (char) voice_volume;
	tmp.gamma				= gamma;
	tmp.iDistance			= distance;

	CPythonSystem::Instance().SetConfig(&tmp);
	return Py_BuildNone();
}

PyObject * systemApplyConfig(PyObject * poSelf, PyObject * poArgs)
{
	CPythonSystem::Instance().ApplyConfig();
	return Py_BuildNone();
}

PyObject * systemSaveConfig(PyObject * poSelf, PyObject * poArgs)
{
	int ret = CPythonSystem::Instance().SaveConfig();
	return Py_BuildValue("i", ret);
}

PyObject * systemGetResolutionCount(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetResolutionCount());
}

PyObject * systemGetFrequencyCount(PyObject * poSelf, PyObject * poArgs)
{
	int	index;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonSystem::Instance().GetFrequencyCount(index));
}

PyObject * systemGetResolution(PyObject * poSelf, PyObject * poArgs)
{
	int	index;
	DWORD width = 0, height = 0, bpp = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	CPythonSystem::Instance().GetResolution(index, &width, &height, &bpp);
	return Py_BuildValue("iii", width, height, bpp);
}

PyObject * systemGetCurrentResolution(PyObject * poSelf, PyObject *poArgs)
{
	CPythonSystem::TConfig *tmp = CPythonSystem::Instance().GetConfig();
	return Py_BuildValue("iii", tmp->width, tmp->height, tmp->bpp);
}

PyObject * systemGetFrequency(PyObject * poSelf, PyObject * poArgs)
{
	int	index, frequency_index;
	DWORD frequency = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	CPythonSystem::Instance().GetFrequency(index, frequency_index, &frequency);
	return Py_BuildValue("i", frequency);
}

PyObject* systemGetRainLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetRainLevel());
}

PyObject* systemSetRainLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetRainLevel(iLevel);
	return Py_BuildNone();
}

#ifdef ENABLE_OFFLINESHOP_SYSTEM
PyObject* systemSetShopNamesRange(PyObject* poSelf, PyObject* poArgs)
{
	float fRange;
	if (!PyTuple_GetFloat(poArgs, 0, &fRange))
		return Py_BuildException();

	CPythonSystem::Instance().SetShopNamesRange(fRange);
	return Py_BuildNone();
}

PyObject* systemGetShopNamesRange(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetShopNamesRange());
}
#endif

PyObject* systemGetPlayerInfo(PyObject* poSelf, PyObject* poArgs)
{
	int type;
	if (!PyTuple_GetInteger(poArgs, 0, &type))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonSystem::Instance().GetPlayerInfo(type));
}

#ifdef ENABLE_TRACK_WINDOW
PyObject* systemSetDungeonTrack(PyObject* poSelf, PyObject* poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();
	CPythonSystem::Instance().SetDungeonTrack(iValue);
	return Py_BuildNone();
}
PyObject* systemGetDungeonTrack(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetDungeonTrack());
}
PyObject* systemSetBossTrack(PyObject* poSelf, PyObject* poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();
	CPythonSystem::Instance().SetBossTrack(iValue);
	return Py_BuildNone();
}
PyObject* systemGetBossTrack(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetBossTrack());
}
#endif

#if defined(__BL_PICK_FILTER__)
PyObject* systemSetPickUpFilter(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const bool bFilter = CPythonSystem::Instance().TPickUpFilter.GetFilter(iIndex);
	CPythonSystem::Instance().TPickUpFilter.SetFilter(iIndex, !bFilter);

	return Py_BuildNone();
}

PyObject* systemGetPickUpFilter(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const bool bFilter = CPythonSystem::Instance().TPickUpFilter.GetFilter(iIndex);

	return Py_BuildValue("i", bFilter);
}

PyObject* systemSetPickUpFilterSize(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const bool bSize = CPythonSystem::Instance().TPickUpFilter.GetSize(iIndex);
	CPythonSystem::Instance().TPickUpFilter.SetSize(iIndex, !bSize);

	return Py_BuildNone();
}

PyObject* systemGetPickUpFilterSize(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const bool bSize = CPythonSystem::Instance().TPickUpFilter.GetSize(iIndex);

	return Py_BuildValue("i", bSize);
}

PyObject* systemSetPickUpFilterRefine(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bMin;
	if (!PyTuple_GetByte(poArgs, 0, &bMin))
		return Py_BuildException();

	BYTE bMax;
	if (!PyTuple_GetByte(poArgs, 1, &bMax))
		return Py_BuildException();

	CPythonSystem::Instance().TPickUpFilter.SetRefine(bMin, bMax);

	return Py_BuildNone();
}

PyObject* systemGetPickUpFilterRefine(PyObject* poSelf, PyObject* poArgs)
{
	const std::pair<BYTE, BYTE> pRefine = CPythonSystem::Instance().TPickUpFilter.GetRefine();

	return Py_BuildValue("ii", pRefine.first, pRefine.second);
}

PyObject* systemSetPickUpFilterLevel(PyObject* poSelf, PyObject* poArgs)
{
	long lMin;
	if (!PyTuple_GetLong(poArgs, 0, &lMin))
		return Py_BuildException();

	long lMax;
	if (!PyTuple_GetLong(poArgs, 1, &lMax))
		return Py_BuildException();

	CPythonSystem::Instance().TPickUpFilter.SetLevel(lMin, lMax);

	return Py_BuildNone();
}

PyObject* systemGetPickUpFilterLevel(PyObject* poSelf, PyObject* poArgs)
{
	const std::pair<long, long> pLevel = CPythonSystem::Instance().TPickUpFilter.GetLevel();

	return Py_BuildValue("ii", pLevel.first, pLevel.second);
}

PyObject* systemSetPickUpFilterModeAll(PyObject* poSelf, PyObject* poArgs)
{
	bool bAll;
	if (!PyTuple_GetBoolean(poArgs, 0, &bAll))
		return Py_BuildException();

	CPythonSystem::Instance().TPickUpFilter.SetModeAll(bAll);

	return Py_BuildNone();
}

PyObject* systemGetPickUpFilterModeAll(PyObject* poSelf, PyObject* poArgs)
{
	const bool bAll = CPythonSystem::Instance().TPickUpFilter.IsModeAll();

	return Py_BuildValue("i", bAll);
}
#endif


#ifdef ENABLE_SHADOW_RENDER_QUALITY_OPTION
PyObject* systemGetShadowTargetLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShadowTargetLevel());
}

PyObject* systemSetShadowTargetLevel(PyObject* poSelf, PyObject* poArgs)
{
	int level;
	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	CPythonSystem::Instance().SetShadowTargetLevel(level);
	return Py_BuildNone();
}

PyObject* systemGetShadowQualityLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShadowQualityLevel());
}

PyObject* systemSetShadowQualityLevel(PyObject* poSelf, PyObject* poArgs)
{
	int level;
	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	CPythonSystem::Instance().SetShadowQualityLevel(level);
	return Py_BuildNone();
}
#else
PyObject * systemGetShadowLevel(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShadowLevel());
}

PyObject * systemSetShadowLevel(PyObject * poSelf, PyObject * poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level > 0)
		CPythonSystem::Instance().SetShadowLevel(level);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_FOG_FIX
PyObject * systemSetFogMode(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetFogMode(iFlag);

	return Py_BuildNone();
}

PyObject * systemIsFogMode(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsFogMode());
}
#endif

#if defined(ENABLE_FOV_OPTION)
PyObject* systemSetFOV(PyObject* poSelf, PyObject* poArgs)
{
	float fValue;
	if (!PyTuple_GetFloat(poArgs, 0, &fValue))
		return Py_BuildException();

	CPythonSystem::Instance().SetFOV(fValue);
	return Py_BuildNone();
}

PyObject* systemGetFOV(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetFOV());
}
#endif

#if defined(__BL_SAVE_CAMERA_MODE__)
PyObject* systemSetCameraMode(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bMode;
	if (!PyTuple_GetByte(poArgs, 0, &bMode))
		return Py_BadArgument();

	CPythonSystem::Instance().SetCameraMode(bMode);
	return Py_BuildNone();
}

PyObject* systemGetCameraMode(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetCameraMode());
}
#endif

#ifdef ENABLE_DOG_MODE
PyObject* systemSetDogModeStatusFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();
	CPythonSystem::Instance().SetDogModeStatusFlag(iFlag);
	return Py_BuildNone();
}

PyObject* systemIsDogModeStatus(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsDogModeStatus());
}

PyObject* systemReloadInstance(PyObject* poSelf, PyObject* poArgs)
{
	CPythonCharacterManager::CharacterIterator it;

	for (it = CPythonCharacterManager::Instance().CharacterInstanceBegin(); it != CPythonCharacterManager::Instance().CharacterInstanceEnd(); ++it)
	{
		CInstanceBase* pkInst = *it;
		if (pkInst && pkInst->IsEnemy() || pkInst->IsStone())
			pkInst->Destroy();
	}
	return Py_BuildNone();
}
#endif


#ifdef ENABLE_GRAPHIC_ON_OFF
PyObject* systemGetEffectLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetEffectLevel());
}

PyObject* systemSetEffectLevel(PyObject* poSelf, PyObject* poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level >= 0)
		CPythonSystem::Instance().SetEffectLevel(level);

	return Py_BuildNone();
}

PyObject* systemGetPrivateShopLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetPrivateShopLevel());
}

PyObject* systemSetPrivateShopLevel(PyObject* poSelf, PyObject* poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level >= 0)
		CPythonSystem::Instance().SetPrivateShopLevel(level);

	return Py_BuildNone();
}

PyObject* systemGetDropItemLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetDropItemLevel());
}

PyObject* systemSetDropItemLevel(PyObject* poSelf, PyObject* poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level >= 0)
		CPythonSystem::Instance().SetDropItemLevel(level);

	return Py_BuildNone();
}

PyObject* systemSetPetStatusFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();
	CPythonSystem::Instance().SetPetStatusFlag(iFlag);
	return Py_BuildNone();
}

PyObject* systemIsPetStatus(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsPetStatus());
}

PyObject* systemSetNpcNameStatusFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();
	CPythonSystem::Instance().SetNpcNameStatusFlag(iFlag);
	return Py_BuildNone();
}

PyObject* systemIsNpcNameStatus(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsNpcNameStatus());
}
#endif

#ifdef STONE_SCALE_SYSTEM
PyObject* systemGetStoneScale(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetStoneScale());
}

PyObject* systemSetStoneScale(PyObject* poSelf, PyObject* poArgs)
{
	float fScale;
	if (!PyTuple_GetFloat(poArgs, 0, &fScale))
		return Py_BuildException();

	CPythonSystem::Instance().SetStoneScale(fScale);
	return Py_BuildNone();
}
#endif

void initsystemSetting()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetWidth",					systemGetWidth,					METH_VARARGS },
		{ "GetHeight",					systemGetHeight,				METH_VARARGS },

		{ "SetInterfaceHandler",		systemSetInterfaceHandler,		METH_VARARGS },
		{ "DestroyInterfaceHandler",	systemDestroyInterfaceHandler,	METH_VARARGS },
		{ "ReserveResource",			systemReserveResource,			METH_VARARGS },

		{ "isInterfaceConfig",			systemisInterfaceConfig,		METH_VARARGS },
		{ "SaveWindowStatus",			systemSaveWindowStatus,			METH_VARARGS },
		{ "GetWindowStatus",			systemGetWindowStatus,			METH_VARARGS },

		{ "GetResolutionCount",			systemGetResolutionCount,		METH_VARARGS },
		{ "GetFrequencyCount",			systemGetFrequencyCount,		METH_VARARGS },

		{ "GetCurrentResolution",		systemGetCurrentResolution,		METH_VARARGS },

		{ "GetResolution",				systemGetResolution,			METH_VARARGS },
		{ "GetFrequency",				systemGetFrequency,				METH_VARARGS },

		{ "ApplyConfig",				systemApplyConfig,				METH_VARARGS },
		{ "SetConfig",					systemSetConfig,				METH_VARARGS },
		{ "SaveConfig",					systemSaveConfig,				METH_VARARGS },
		{ "GetConfig",					systemGetConfig,				METH_VARARGS },

		{ "GetRainLevel",				systemGetRainLevel,				METH_VARARGS },
		{ "SetRainLevel",				systemSetRainLevel,				METH_VARARGS },

		{ "SetSaveID",					systemSetSaveID,				METH_VARARGS },
		{ "isSaveID",					systemisSaveID,					METH_VARARGS },
		{ "GetSaveID",					systemGetSaveID,				METH_VARARGS },

		{ "GetMusicVolume",				systemGetMusicVolume,			METH_VARARGS },
		{ "GetSoundVolume",				systemGetSoundVolume,			METH_VARARGS },

		{ "SetMusicVolume",				systemSetMusicVolume,			METH_VARARGS },
		{ "SetSoundVolumef",			systemSetSoundVolumef,			METH_VARARGS },
		{ "IsSoftwareCursor",			systemIsSoftwareCursor,			METH_VARARGS },

		{ "SetViewChatFlag",			systemSetViewChatFlag,			METH_VARARGS },
		{ "IsViewChat",					systemIsViewChat,				METH_VARARGS },

		{ "SetAlwaysShowNameFlag",		systemSetAlwaysShowNameFlag,	METH_VARARGS },
		{ "IsAlwaysShowName",			systemIsAlwaysShowName,			METH_VARARGS },

		{ "SetShowDamageFlag",			systemSetShowDamageFlag,		METH_VARARGS },
		{ "IsShowDamage",				systemIsShowDamage,				METH_VARARGS },

		{ "SetShowSalesTextFlag",		systemSetShowSalesTextFlag,		METH_VARARGS },
		{ "IsShowSalesText",			systemIsShowSalesText,			METH_VARARGS },

#ifdef ENABLE_SHADOW_RENDER_QUALITY_OPTION
		{ "GetShadowTargetLevel",		systemGetShadowTargetLevel,		METH_VARARGS },
		{ "SetShadowTargetLevel",		systemSetShadowTargetLevel,		METH_VARARGS },
		{ "GetShadowQualityLevel",		systemGetShadowQualityLevel,	METH_VARARGS },
		{ "SetShadowQualityLevel",		systemSetShadowQualityLevel,	METH_VARARGS },
#else
		{ "GetShadowLevel",				systemGetShadowLevel,			METH_VARARGS },
		{ "SetShadowLevel",				systemSetShadowLevel,			METH_VARARGS },
#endif
		{ "GetPlayerInfo", systemGetPlayerInfo, METH_VARARGS },


#ifdef WJ_SHOW_MOB_INFO
		{ "IsShowMobAIFlag",			systemIsShowMobAIFlag,			METH_VARARGS },
		{ "SetShowMobAIFlag",			systemSetShowMobAIFlag,			METH_VARARGS },

		{ "IsShowMobLevel",				systemIsShowMobLevel,			METH_VARARGS },
		{ "SetShowMobLevel",			systemSetShowMobLevel,			METH_VARARGS },
#endif

#ifdef ENABLE_OFFLINESHOP_SYSTEM
		{ "SetShopNamesRange",			systemSetShopNamesRange, METH_VARARGS },
		{ "GetShopNamesRange",			systemGetShopNamesRange, METH_VARARGS },
#endif

#ifdef ENABLE_TRACK_WINDOW
		{ "SetDungeonTrack",			systemSetDungeonTrack,                METH_VARARGS },
		{ "GetDungeonTrack",			systemGetDungeonTrack,                METH_VARARGS },
		{ "SetBossTrack",                systemSetBossTrack,                METH_VARARGS },
		{ "GetBossTrack",                systemGetBossTrack,                METH_VARARGS },
#endif

#if defined(__BL_PICK_FILTER__)
		{ "SetPickUpFilter",			systemSetPickUpFilter,			METH_VARARGS },
		{ "GetPickUpFilter",			systemGetPickUpFilter,			METH_VARARGS },

		{ "SetPickUpFilterSize",		systemSetPickUpFilterSize,		METH_VARARGS },
		{ "GetPickUpFilterSize",		systemGetPickUpFilterSize,		METH_VARARGS },

		{ "SetPickUpFilterRefine",		systemSetPickUpFilterRefine,	METH_VARARGS },
		{ "GetPickUpFilterRefine",		systemGetPickUpFilterRefine,	METH_VARARGS },

		{ "SetPickUpFilterLevel",		systemSetPickUpFilterLevel,		METH_VARARGS },
		{ "GetPickUpFilterLevel",		systemGetPickUpFilterLevel,		METH_VARARGS },

		{ "SetPickUpFilterModeAll",		systemSetPickUpFilterModeAll,	METH_VARARGS },
		{ "GetPickUpFilterModeAll",		systemGetPickUpFilterModeAll,	METH_VARARGS },
#endif


#ifdef ENABLE_FOG_FIX
		{ "SetFogMode",				systemSetFogMode,				METH_VARARGS },
		{ "IsFogMode",				systemIsFogMode,				METH_VARARGS },
#endif

#if defined(ENABLE_FOV_OPTION)
		{ "SetFOV",						systemSetFOV,					METH_VARARGS },
		{ "GetFOV",						systemGetFOV,					METH_VARARGS },
#endif

#if defined(__BL_SAVE_CAMERA_MODE__)
		{ "SetCameraMode",				systemSetCameraMode,			METH_VARARGS },
		{ "GetCameraMode",				systemGetCameraMode,			METH_VARARGS },
#endif

#ifdef ENABLE_DOG_MODE
		{ "SetDogModeStatusFlag", systemSetDogModeStatusFlag, METH_VARARGS },
		{ "IsDogModeStatus", systemIsDogModeStatus, METH_VARARGS },
		{ "ReloadInstance", systemReloadInstance, METH_VARARGS},
#endif

#ifdef ENABLE_GRAPHIC_ON_OFF
		{ "GetEffectLevel", systemGetEffectLevel, METH_VARARGS },
		{ "SetEffectLevel", systemSetEffectLevel, METH_VARARGS },

		{ "GetPrivateShopLevel", systemGetPrivateShopLevel, METH_VARARGS },
		{ "SetPrivateShopLevel", systemSetPrivateShopLevel, METH_VARARGS },

		{ "GetDropItemLevel", systemGetDropItemLevel, METH_VARARGS },
		{ "SetDropItemLevel", systemSetDropItemLevel, METH_VARARGS },

		{ "SetPetStatusFlag", systemSetPetStatusFlag, METH_VARARGS },
		{ "IsPetStatus", systemIsPetStatus, METH_VARARGS },

		{ "SetNpcNameStatusFlag", systemSetNpcNameStatusFlag, METH_VARARGS },
		{ "IsNpcNameStatus", systemIsNpcNameStatus, METH_VARARGS },
#endif

#ifdef STONE_SCALE_SYSTEM
		{ "GetStoneScale",				systemGetStoneScale,				METH_VARARGS },
		{ "SetStoneScale",				systemSetStoneScale,				METH_VARARGS },
#endif

		{ NULL,							NULL,							NULL }
	};

	PyObject * poModule = Py_InitModule("systemSetting", s_methods);

	PyModule_AddIntConstant(poModule, "WINDOW_STATUS",		CPythonSystem::WINDOW_STATUS);
	PyModule_AddIntConstant(poModule, "WINDOW_INVENTORY",	CPythonSystem::WINDOW_INVENTORY);
	PyModule_AddIntConstant(poModule, "WINDOW_ABILITY",		CPythonSystem::WINDOW_ABILITY);
	PyModule_AddIntConstant(poModule, "WINDOW_SOCIETY",		CPythonSystem::WINDOW_SOCIETY);
	PyModule_AddIntConstant(poModule, "WINDOW_JOURNAL",		CPythonSystem::WINDOW_JOURNAL);
	PyModule_AddIntConstant(poModule, "WINDOW_COMMAND",		CPythonSystem::WINDOW_COMMAND);

	PyModule_AddIntConstant(poModule, "WINDOW_QUICK",		CPythonSystem::WINDOW_QUICK);
	PyModule_AddIntConstant(poModule, "WINDOW_GAUGE",		CPythonSystem::WINDOW_GAUGE);
	PyModule_AddIntConstant(poModule, "WINDOW_MINIMAP",		CPythonSystem::WINDOW_MINIMAP);
	PyModule_AddIntConstant(poModule, "WINDOW_CHAT",		CPythonSystem::WINDOW_CHAT);

#if defined(__BL_PICK_FILTER__)
	PyModule_AddIntConstant(poModule, "PICKUP_FILTER_COUNT", CPythonSystem::CPickUpFilter::EPICKFILTER::EPICKFILTER_MAX);
	PyModule_AddIntConstant(poModule, "PICKUP_FILTER_SIZE_COUNT", CPythonSystem::CPickUpFilter::ESIZE::ESIZE_MAX);
#endif

}
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
