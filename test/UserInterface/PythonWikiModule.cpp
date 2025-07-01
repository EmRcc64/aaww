#include "StdAfx.h"

#ifdef INGAME_WIKI

#include "../gamelib/ItemManager.h"
#include "../gamelib/in_game_wiki.h"
#include "../EterBase/Stl.h"
#include "PythonApplication.h"
#include "PythonNetworkStream.h"
#include "PythonNonPlayer.h"
#include "PythonWikiRenderTarget.h"

PyObject* wikiModule = NULL;

PyObject* wikiGetBaseClass(PyObject* poSelf, PyObject* poArgs)
{
	if (!wikiModule)
		return Py_BuildNone();
	
	return Py_BuildValue("O", wikiModule);
}

PyObject* wikiGetChestInfo(PyObject* poSelf, PyObject* poArgs)
{
	int itemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &itemVnum))
		return Py_BadArgument();
	
	CItemData* table;
	if (CItemManager::instance().GetItemDataPointer(itemVnum, &table) && !table->GetWikiTable()->pChestInfo.empty())
	{
		const size_t dataCount = table->GetWikiTable()->pChestInfo.size();
		PyObject* ret = PyTuple_New(dataCount);
		for (int i = 0; i < dataCount; ++i)
			PyTuple_SetItem(ret, i, Py_BuildValue("ii", table->GetWikiTable()->pChestInfo[i].vnum, table->GetWikiTable()->pChestInfo[i].count));
		return Py_BuildValue("iiO", table->GetWikiTable()->dwOrigin, table->GetWikiTable()->bIsCommon ? 1 : 0, ret);
	}
	
	return Py_BuildValue("ii", table->GetWikiTable()->dwOrigin, table->GetWikiTable()->bIsCommon ? 1 : 0);;
}

PyObject* wikiGetMobInfo(PyObject* poSelf, PyObject* poArgs)
{
	int itemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &itemVnum))
		return Py_BadArgument();
	
	const CPythonNonPlayer::TWikiInfoTable* mobData = CPythonNonPlayer::instance().GetWikiTable(itemVnum);
	if (!mobData->dropList.empty())
	{
		const size_t dataCount = mobData->dropList.size();
		PyObject* ret = PyTuple_New(dataCount);
		
		int idx = 0;
		for (const auto& it: mobData->dropList) {
			PyTuple_SetItem(ret, idx, Py_BuildValue("ii", it.vnum, it.count));
			++idx;
		}
		return Py_BuildValue("O", ret);
	}
	
	return Py_BuildNone();
}

PyObject* wikiLoadClassItems(PyObject* poSelf, PyObject* poArgs)
{
	BYTE classType;
	int raceFilter;
	
	if (!PyTuple_GetByte(poArgs, 0, &classType))
		return Py_BadArgument();
	
	if (!PyTuple_GetInteger(poArgs, 1, &raceFilter))
		return Py_BadArgument();
	
	return Py_BuildValue("i", CItemManager::Instance().WikiLoadClassItems(classType, raceFilter));
}

PyObject* wikiLoadClassMobs(PyObject* poSelf, PyObject* poArgs)
{
	BYTE classType;
	if (!PyTuple_GetByte(poArgs, 0, &classType))
		return Py_BadArgument();
	
	unsigned short fromLvl, toLvl;
	if (!PyTuple_GetInteger(poArgs, 1, &fromLvl))
		return Py_BadArgument();
	
	if (!PyTuple_GetInteger(poArgs, 2, &toLvl))
		return Py_BadArgument();
	
	return Py_BuildValue("i", CPythonNonPlayer::instance().WikiLoadClassMobs(classType, fromLvl, toLvl));
}

PyObject* GetWikiModule()
{
	return wikiModule;
}

PyObject* wikiChangePage(PyObject* poSelf, PyObject* poArgs)
{
	unsigned short from;
	unsigned short to;
	int isMob = 0;
	
	if (!PyTuple_GetInteger(poArgs, 0, &from))
		return Py_BadArgument();
	
	if (!PyTuple_GetInteger(poArgs, 1, &to))
		return Py_BadArgument();
	
	if (PyTuple_Size(poArgs) > 2 && !PyTuple_GetInteger(poArgs, 2, &isMob))
		return Py_BadArgument();
	
	if (from > to)
		return Py_BuildException("to must be bigger than from");
	
	std::vector<DWORD>* loadVec;
	
	if (!isMob)
		loadVec = CItemManager::Instance().WikiGetLastItems();
	else
		loadVec = CPythonNonPlayer::instance().WikiGetLastMobs();
	
	PyObject* tuple;
	if (loadVec->size() > 0)
	{
		if (loadVec->size() < to)
			to = (unsigned short)loadVec->size();
		
		if (from > to)
			return Py_BuildException("to must be bigger than from");
		
		tuple = PyTuple_New(to - from);
		for (unsigned short i = from; i < to; ++i)
			PyTuple_SetItem(tuple, i - from, Py_BuildValue("i", (*loadVec)[i]));
		
		return tuple;
	}
	
	return Py_BuildNone();
}

PyObject* wikiLoadInfo(PyObject* poSelf, PyObject* poArgs)
{
	int iretID;
	if (!PyTuple_GetInteger(poArgs, 0, &iretID))
		return Py_BadArgument();
	
	unsigned long long retID = static_cast<unsigned long long>(iretID);
	
	int itemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &itemVnum))
		return Py_BadArgument();
	
	int isMob = 0;
	if (PyTuple_Size(poArgs) > 2 && !PyTuple_GetInteger(poArgs, 2, &isMob))
		return Py_BadArgument();
	
	if (!isMob)
	{
		CItemData* itemData;
		if (CItemManager::Instance().GetItemDataPointer(itemVnum, &itemData))
		{
			if (!itemData->GetWikiTable()->isSet)
				CPythonNetworkStream::instance().SendWikiRequestInfo(retID, itemVnum, false);
			else
			{
				if (wikiModule)
					PyCallClassMemberFunc(wikiModule, "BINARY_LoadInfo", Py_BuildValue("(Ki)", retID, itemVnum));
			}
		}
	}
	else
	{
		const CPythonNonPlayer::TWikiInfoTable* mobData = CPythonNonPlayer::instance().GetWikiTable(itemVnum);
		if (mobData)
		{
			if (!mobData->isSet)
				CPythonNetworkStream::instance().SendWikiRequestInfo(retID, itemVnum, true);
			else
			{
				if (wikiModule)
					PyCallClassMemberFunc(wikiModule, "BINARY_LoadInfo", Py_BuildValue("(Ki)", retID, itemVnum));
			}
		}
	}
	
	return Py_BuildNone();
}

PyObject* wikiGetRefineInfo(PyObject* poSelf, PyObject* poArgs)
{
	int itemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &itemVnum))
		return Py_BadArgument();
	
	CItemData* table;
	if (CItemManager::instance().GetItemDataPointer(itemVnum, &table))
	{
		const int refineMaxLevel = table->GetWikiTable()->maxRefineLevel;
		PyObject* ret = PyTuple_New(refineMaxLevel);
		
		for (int i = 0; i < refineMaxLevel; ++i)
		{
			PyObject* ret2 = PyTuple_New(CommonWikiData::REFINE_MATERIAL_MAX_NUM);
			
			for (int j = 0; j < CommonWikiData::REFINE_MATERIAL_MAX_NUM; ++j)
				PyTuple_SetItem(ret2, j, Py_BuildValue("ii", table->GetWikiTable()->pRefineData[i].materials[j].vnum, table->GetWikiTable()->pRefineData[i].materials[j].count));
			
			PyTuple_SetItem(ret, i, Py_BuildValue("iO", table->GetWikiTable()->pRefineData[i].price, ret2));
		}
		return Py_BuildValue("iO", refineMaxLevel, ret);
	}
	
	return Py_BuildNone();
}

PyObject* wikiGetOriginInfo(PyObject* poSelf, PyObject* poArgs)
{
	int itemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &itemVnum))
		return Py_BadArgument();
	
	CItemData* table;
	if (CItemManager::instance().GetItemDataPointer(itemVnum, &table) && !table->GetWikiTable()->pOriginInfo.empty())
	{
		CItemData::TWikiItemInfo* wikiTbl = table->GetWikiTable();
		PyObject* ret = PyTuple_New(wikiTbl->pOriginInfo.size());
		for (int i = 0; i < wikiTbl->pOriginInfo.size(); ++i)
			PyTuple_SetItem(ret, i, Py_BuildValue("ii", wikiTbl->pOriginInfo[i].vnum, wikiTbl->pOriginInfo[i].is_mob ? 1 : 0));

		return Py_BuildValue("O", ret);
	}
	
	return Py_BuildNone();
}

PyObject* wikiGetWikiItemStartRefineVnum(PyObject* poSelf, PyObject* poArgs)
{
	int itemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &itemVnum))
		return Py_BadArgument();
	
	return Py_BuildValue("i", CItemManager::instance().GetWikiItemStartRefineVnum(itemVnum));
}

PyObject* wikiIsSet(PyObject* poSelf, PyObject* poArgs)
{
	int itemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &itemVnum))
		return Py_BadArgument();
	
	bool isMob = false;
	if (PyTuple_Size(poArgs) > 1)
		if (!PyTuple_GetBoolean(poArgs, 1, &isMob))
			return Py_BadArgument();
	
	if (!isMob)
	{
		CItemData* table;
		if (CItemManager::instance().GetItemDataPointer(itemVnum, &table))
			return Py_BuildValue("i", table->GetWikiTable()->isSet ? 1 : 0);
	}
	else
	{
		CPythonNonPlayer::TWikiInfoTable* ptr = CPythonNonPlayer::instance().GetWikiTable(itemVnum);
		if (ptr)
			return Py_BuildValue("i", ptr->isSet ? 1 : 0);
	}
	
	return Py_BuildValue("i", 0);
}

PyObject* wikiHasData(PyObject* poSelf, PyObject* poArgs)
{
	int itemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &itemVnum))
		return Py_BadArgument();
	
	CItemData* table;
	if (CItemManager::instance().GetItemDataPointer(itemVnum, &table))
	{
		return Py_BuildValue("i", table->GetWikiTable()->hasData ? 1 : 0);
	}
	
	return Py_BuildValue("i", 0);
}

PyObject* wikiRegisterClass(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* mClass;
	if (!PyTuple_GetObject(poArgs, 0, &mClass))
		return Py_BadArgument();

	wikiModule = mClass;
	return Py_BuildNone();
}

PyObject* wikiUnregisterClass(PyObject* poSelf, PyObject* poArgs)
{
	wikiModule = NULL;
	return Py_BuildNone();
}

PyObject* wikiRegisterItemBlacklist(PyObject* poSelf, PyObject* poArgs)
{
	int vnum;
	if (!PyTuple_GetInteger(poArgs, 0, &vnum))
		return Py_BadArgument();
	
	CItemManager::instance().WikiAddVnumToBlacklist(vnum);
	return Py_BuildNone();
}

PyObject* wikiRegisterMonsterBlacklist(PyObject* poSelf, PyObject* poArgs)
{
	int vnum;
	if (!PyTuple_GetInteger(poArgs, 0, &vnum))
		return Py_BadArgument();
	
	CPythonNonPlayer::instance().WikiSetBlacklisted(vnum);
	return Py_BuildNone();
}

PyObject* wikiCanIncrRefineLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CItemManager::instance().CanIncrSelectedItemRefineLevel());
}

PyObject* wikiRegisterModelViewWindow(PyObject* poSelf, PyObject* poArgs)
{
	int moduleID;
	if (!PyTuple_GetInteger(poArgs, 0, &moduleID))
		return Py_BadArgument();
	
	int hwnd;
	if (!PyTuple_GetInteger(poArgs, 1, &hwnd))
		return Py_BadArgument();
	
	CPythonWikiRenderTarget::instance().RegisterRenderModule(moduleID, hwnd);
	return Py_BuildNone();
}

PyObject* wikiGetFreeModelViewID(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonWikiRenderTarget::instance().GetFreeID());
}

PyObject* wikiManageModelViewVisibility(PyObject* poSelf, PyObject* poArgs)
{
	int moduleID;
	if (!PyTuple_GetInteger(poArgs, 0, &moduleID))
		return Py_BadArgument();
	
	bool flag = false;
	if (!PyTuple_GetBoolean(poArgs, 1, &flag))
		return Py_BadArgument();
	
	CPythonWikiRenderTarget::instance().ManageModelViewVisibility(moduleID, flag);
	return Py_BuildNone();
}

PyObject* wikiShowModelViewManager(PyObject* poSelf, PyObject* poArgs)
{
	bool flag = false;
	if (!PyTuple_GetBoolean(poArgs, 0, &flag))
		return Py_BadArgument();
	
	CPythonWikiRenderTarget::instance().ShowModelViewManager(flag);
	return Py_BuildNone();
}

PyObject* wikiSetModelViewModel(PyObject* poSelf, PyObject* poArgs)
{
	int moduleID;
	if (!PyTuple_GetInteger(poArgs, 0, &moduleID))
		return Py_BadArgument();
	
	int moduleVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &moduleVnum))
		return Py_BadArgument();
	
	CPythonWikiRenderTarget::instance().SetModelViewModel(moduleID, moduleVnum);
	return Py_BuildNone();
}

PyObject* wikiSetWeaponModel(PyObject* poSelf, PyObject* poArgs)
{
	int moduleID;
	if (!PyTuple_GetInteger(poArgs, 0, &moduleID))
		return Py_BadArgument();
	
	int weaponVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &weaponVnum))
		return Py_BadArgument();
	
	CPythonWikiRenderTarget::instance().SetWeaponModel(moduleID, weaponVnum);
	return Py_BuildNone();
}

PyObject* wikiSetModelForm(PyObject* poSelf, PyObject* poArgs)
{
	int moduleID;
	if (!PyTuple_GetInteger(poArgs, 0, &moduleID))
		return Py_BadArgument();
	
	int mainVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &mainVnum))
		return Py_BadArgument();
	
	CPythonWikiRenderTarget::instance().SetModelForm(moduleID, mainVnum);
	return Py_BuildNone();
}

PyObject* wikiSetModelHair(PyObject* poSelf, PyObject* poArgs)
{
	int moduleID;
	if (!PyTuple_GetInteger(poArgs, 0, &moduleID))
		return Py_BadArgument();
	
	int hairVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &hairVnum))
		return Py_BadArgument();
	
	CPythonWikiRenderTarget::instance().SetModelHair(moduleID, hairVnum);
	return Py_BuildNone();
}

PyObject* wikiSetModelV3Eye(PyObject* poSelf, PyObject* poArgs)
{
	int moduleID;
	if (!PyTuple_GetInteger(poArgs, 0, &moduleID))
		return Py_BadArgument();
	
	float x, y, z;
	if (!PyTuple_GetFloat(poArgs, 1, &x))
		return Py_BadArgument();

	if (!PyTuple_GetFloat(poArgs, 2, &y))
		return Py_BadArgument();

	if (!PyTuple_GetFloat(poArgs, 3, &z))
		return Py_BadArgument();
	
	CPythonWikiRenderTarget::instance().SetModelV3Eye(moduleID, x, y, z);
	return Py_BuildNone();
}

PyObject* wikiSetModelV3Target(PyObject* poSelf, PyObject* poArgs)
{
	int moduleID;
	if (!PyTuple_GetInteger(poArgs, 0, &moduleID))
		return Py_BadArgument();
	
	float x, y, z;
	if (!PyTuple_GetFloat(poArgs, 1, &x))
		return Py_BadArgument();

	if (!PyTuple_GetFloat(poArgs, 2, &y))
		return Py_BadArgument();

	if (!PyTuple_GetFloat(poArgs, 3, &z))
		return Py_BadArgument();
	
	CPythonWikiRenderTarget::instance().SetModelV3Target(moduleID, x, y, z);
	return Py_BuildNone();
}

PyObject* renderTargetSetWikiBackground(PyObject* poSelf, PyObject* poArgs)
{
	int moduleID;
	if (!PyTuple_GetInteger(poArgs, 0, &moduleID))
		return Py_BadArgument();

	CPythonWikiRenderTarget::Instance().CreateWikiBackground(
		moduleID, CPythonApplication::Instance().GetWidth(),
		CPythonApplication::Instance().GetHeight());

	return Py_BuildNone();
}

void initWiki()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetBaseClass",						wikiGetBaseClass,					METH_VARARGS },
		{ "GetChestInfo",						wikiGetChestInfo,					METH_VARARGS },
		{ "LoadClassItems",						wikiLoadClassItems,					METH_VARARGS },
		{ "ChangePage",							wikiChangePage,						METH_VARARGS },
		{ "LoadInfo",							wikiLoadInfo,						METH_VARARGS },
		{ "RegisterClass",						wikiRegisterClass,					METH_VARARGS },
		{ "UnregisterClass",					wikiUnregisterClass,				METH_VARARGS },
		{ "HasData",							wikiHasData,						METH_VARARGS },
		{ "IsSet",								wikiIsSet,							METH_VARARGS },
		{ "GetRefineInfo",						wikiGetRefineInfo,					METH_VARARGS },
		{ "GetOriginInfo",						wikiGetOriginInfo,					METH_VARARGS },
		{ "GetWikiItemStartRefineVnum",			wikiGetWikiItemStartRefineVnum,		METH_VARARGS },
		
		{ "RegisterItemBlacklist",				wikiRegisterItemBlacklist,			METH_VARARGS },
		{ "RegisterMonsterBlacklist",			wikiRegisterMonsterBlacklist,		METH_VARARGS },
		
		{ "LoadClassMobs",						wikiLoadClassMobs,					METH_VARARGS },
		{ "GetMobInfo",							wikiGetMobInfo,						METH_VARARGS },
		
		{ "CanIncrRefineLevel",					wikiCanIncrRefineLevel,				METH_VARARGS },
		
		{ "GetFreeModelViewID",					wikiGetFreeModelViewID,				METH_VARARGS },
		{ "RegisterModelViewWindow",			wikiRegisterModelViewWindow,		METH_VARARGS },
		{ "ManageModelViewVisibility",			wikiManageModelViewVisibility,		METH_VARARGS },
		{ "ShowModelViewManager",				wikiShowModelViewManager,			METH_VARARGS },
		
		{ "SetModelViewModel",					wikiSetModelViewModel,				METH_VARARGS },
		{ "SetWeaponModel",						wikiSetWeaponModel,					METH_VARARGS },
		{ "SetModelForm",						wikiSetModelForm,					METH_VARARGS },
		{ "SetModelHair",						wikiSetModelHair,					METH_VARARGS },
		{ "SetModelV3Eye",						wikiSetModelV3Eye,					METH_VARARGS },
		{ "SetModelV3Target",					wikiSetModelV3Target,				METH_VARARGS },
		{ "SetBackground",				renderTargetSetWikiBackground,	METH_VARARGS },
		{ NULL,									NULL,								NULL		 }
	};
	
	PyObject* module = Py_InitModule("wiki", s_methods);
	
	PyModule_AddIntConstant(module, "MAX_REFINE_COUNT", CommonWikiData::MAX_REFINE_COUNT);
	PyModule_AddIntConstant(module, "REFINE_MATERIAL_MAX_NUM", CommonWikiData::REFINE_MATERIAL_MAX_NUM);
	PyModule_AddIntConstant(module, "CONTROL_ITEM_VNUM", 10); //This is static value! Please dont touch in him.
	
	PyModule_AddIntConstant(module, "WIKI_RENDER_MODULE_DELETE", CPythonWikiRenderTarget::DELETE_PARM);
	PyModule_AddIntConstant(module, "WIKI_RENDER_MODULE_START", CPythonWikiRenderTarget::START_MODULE);
}
#endif
