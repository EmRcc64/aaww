#pragma once

#pragma warning(disable:4702)
#pragma warning(disable:4100)
#pragma warning(disable:4201)
#pragma warning(disable:4511)
#pragma warning(disable:4663)
#pragma warning(disable:4018)
#pragma warning(disable:4245)

#if _MSC_VER >= 1400
//if don't use below, time_t is 64bit
#define _USE_32BIT_TIME_T
#endif
#include <iterator>
#include "../eterLib/StdAfx.h"
#include "../eterPythonLib/StdAfx.h"
#include "../gameLib/StdAfx.h"
#include "../scriptLib/StdAfx.h"
#include "../milesLib/StdAfx.h"
#include "../EffectLib/StdAfx.h"
#include "../PRTerrainLib/StdAfx.h"
#include "../SpeedTreeLib/StdAfx.h"

#ifndef __D3DRM_H__
#define __D3DRM_H__
#endif

#include <dshow.h>
#include <qedit.h>

#include "Locale.h"

#include "GameType.h"
extern DWORD __DEFAULT_CODE_PAGE__;

#define APP_NAME	"Metin 2"

enum
{
	POINT_MAX_NUM = 255,
	CHARACTER_NAME_MAX_LEN = 24,
#if defined(LOCALE_SERVICE_JAPAN)
	PLAYER_NAME_MAX_LEN = 16,
#else
	PLAYER_NAME_MAX_LEN = 12,
#endif
};

void initudp();
void initapp();
void initime();
void initsystemSetting();
void initchr();
void initchrmgr();
void initChat();
void initTextTail();
void initime();
void initItem();
void initNonPlayer();
void initnet();
void initPlayer();
void initSectionDisplayer();
void initServerStateChecker();
#ifdef ENABLE_SWITCHBOT
void initSwitchbot();
#endif
#ifdef ENABLE_RENDER_TARGET
void initRenderTarget();
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
void intcuberenewal();
#endif
#ifdef INGAME_WIKI
void initWiki();
#endif

void initTrade();
void initMiniMap();
void initProfiler();
void initEvent();
void initeffect();
void initsnd();
void initeventmgr();
void initBackground();
void initwndMgr();
void initshop();
void initpack();
void initskill();
void initfly();
void initquest();
void initsafebox();
#ifdef ENABLE_CONFIG_MODULE
void initcfg();
#endif
void initguild();
void initMessenger();
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void initAcce();
#endif

#ifdef ENABLE_SHOP_SEARCH_SYSTEM
void initprivateShopSearch();
#endif

#ifdef ENABLE_CPP_PSM
void initplayersettingsmodule();
#endif


#ifdef __AUTO_HUNT__
extern float GetDistanceNew(const TPixelPosition& PixelPosition, const TPixelPosition& c_rPixelPosition);
extern void split_argument(const char* argument, std::vector<std::string>& vecArgs, const char* arg = " ");
#endif


extern std::string decrypt(std::string& encrypted_msg, std::string key = "13");
extern std::string encrypt(std::string& msg, std::string key = "13");

extern const std::string& ApplicationStringTable_GetString(DWORD dwID);
extern const std::string& ApplicationStringTable_GetString(DWORD dwID, LPCSTR szKey);

extern const char* ApplicationStringTable_GetStringz(DWORD dwID);
extern const char* ApplicationStringTable_GetStringz(DWORD dwID, LPCSTR szKey);

extern void ApplicationSetErrorString(const char* szErrorString);
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
