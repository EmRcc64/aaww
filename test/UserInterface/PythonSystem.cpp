#include "StdAfx.h"
#include "PythonSystem.h"
#include "PythonApplication.h"

#define DEFAULT_VALUE_ALWAYS_SHOW_NAME		true

void CPythonSystem::SetInterfaceHandler(PyObject * poHandler)
{
//	if (m_poInterfaceHandler)
//		Py_DECREF(m_poInterfaceHandler);

	m_poInterfaceHandler = poHandler;

//	if (m_poInterfaceHandler)
//		Py_INCREF(m_poInterfaceHandler);
}

void CPythonSystem::DestroyInterfaceHandler()
{
	m_poInterfaceHandler = NULL;
}

void CPythonSystem::SaveWindowStatus(int iIndex, int iVisible, int iMinimized, int ix, int iy, int iHeight)
{
	m_WindowStatus[iIndex].isVisible	= iVisible;
	m_WindowStatus[iIndex].isMinimized	= iMinimized;
	m_WindowStatus[iIndex].ixPosition	= ix;
	m_WindowStatus[iIndex].iyPosition	= iy;
	m_WindowStatus[iIndex].iHeight		= iHeight;
}

void CPythonSystem::GetDisplaySettings()
{
	memset(m_ResolutionList, 0, sizeof(TResolution) * RESOLUTION_MAX_NUM);
	m_ResolutionCount = 0;

	LPDIRECT3D8 lpD3D = CPythonGraphic::Instance().GetD3D();

	D3DADAPTER_IDENTIFIER8 d3dAdapterIdentifier;
	D3DDISPLAYMODE d3ddmDesktop;

#ifdef DIRECTX9
	lpD3D->GetAdapterIdentifier(0, D3DENUM_WHQL_LEVEL, &d3dAdapterIdentifier);
#else
	lpD3D->GetAdapterIdentifier(0, D3DENUM_NO_WHQL_LEVEL, &d3dAdapterIdentifier);
#endif
	lpD3D->GetAdapterDisplayMode(0, &d3ddmDesktop);

#ifdef DIRECTX9
	DWORD dwNumAdapterModes = lpD3D->GetAdapterModeCount(0, d3ddmDesktop.Format);
#else
	DWORD dwNumAdapterModes = lpD3D->GetAdapterModeCount(0);
#endif

	for (UINT iMode = 0; iMode < dwNumAdapterModes; iMode++)
	{
		D3DDISPLAYMODE DisplayMode;
#ifdef DIRECTX9
		lpD3D->EnumAdapterModes(0, d3ddmDesktop.Format, iMode, &DisplayMode);
#else
		lpD3D->EnumAdapterModes(0, iMode, &DisplayMode);
#endif
		DWORD bpp = 0;

		if (DisplayMode.Width < 800 || DisplayMode.Height < 600)
			continue;

		if (DisplayMode.Format == D3DFMT_R5G6B5)
			bpp = 16;
		else if (DisplayMode.Format == D3DFMT_X8R8G8B8)
			bpp = 32;
		else
			continue;

		int check_res = false;

		for (int i = 0; !check_res && i < m_ResolutionCount; ++i)
		{
			if (m_ResolutionList[i].bpp != bpp ||
				m_ResolutionList[i].width != DisplayMode.Width ||
				m_ResolutionList[i].height != DisplayMode.Height)
				continue;

			int check_fre = false;

			for (int j = 0; j < m_ResolutionList[i].frequency_count; ++j)
			{
				if (m_ResolutionList[i].frequency[j] == DisplayMode.RefreshRate)
				{
					check_fre = true;
					break;
				}
			}

			if (!check_fre)
				if (m_ResolutionList[i].frequency_count < FREQUENCY_MAX_NUM)
					m_ResolutionList[i].frequency[m_ResolutionList[i].frequency_count++] = DisplayMode.RefreshRate;

			check_res = true;
		}

		if (!check_res)
		{
			if (m_ResolutionCount < RESOLUTION_MAX_NUM)
			{
				m_ResolutionList[m_ResolutionCount].width			= DisplayMode.Width;
				m_ResolutionList[m_ResolutionCount].height			= DisplayMode.Height;
				m_ResolutionList[m_ResolutionCount].bpp				= bpp;
				m_ResolutionList[m_ResolutionCount].frequency[0]	= DisplayMode.RefreshRate;
				m_ResolutionList[m_ResolutionCount].frequency_count	= 1;

				++m_ResolutionCount;
			}
		}
	}
}

int	CPythonSystem::GetResolutionCount()
{
	return m_ResolutionCount;
}

int CPythonSystem::GetFrequencyCount(int index)
{
	if (index >= m_ResolutionCount)
		return 0;

    return m_ResolutionList[index].frequency_count;
}

bool CPythonSystem::GetResolution(int index, OUT DWORD *width, OUT DWORD *height, OUT DWORD *bpp)
{
	if (index >= m_ResolutionCount)
		return false;

	*width = m_ResolutionList[index].width;
	*height = m_ResolutionList[index].height;
	*bpp = m_ResolutionList[index].bpp;
	return true;
}

bool CPythonSystem::GetFrequency(int index, int freq_index, OUT DWORD *frequncy)
{
	if (index >= m_ResolutionCount)
		return false;

	if (freq_index >= m_ResolutionList[index].frequency_count)
		return false;

	*frequncy = m_ResolutionList[index].frequency[freq_index];
	return true;
}

int	CPythonSystem::GetResolutionIndex(DWORD width, DWORD height, DWORD bit)
{
	DWORD re_width, re_height, re_bit;
	int i = 0;

	while (GetResolution(i, &re_width, &re_height, &re_bit))
	{
		if (re_width == width)
			if (re_height == height)
				if (re_bit == bit)
					return i;
		i++;
	}

	return 0;
}

int	CPythonSystem::GetFrequencyIndex(int res_index, DWORD frequency)
{
	DWORD re_frequency;
	int i = 0;

	while (GetFrequency(res_index, i, &re_frequency))
	{
		if (re_frequency == frequency)
			return i;

		i++;
	}

	return 0;
}

DWORD CPythonSystem::GetWidth()
{
	return m_Config.width;
}

DWORD CPythonSystem::GetHeight()
{
	return m_Config.height;
}
DWORD CPythonSystem::GetBPP()
{
	return m_Config.bpp;
}
DWORD CPythonSystem::GetFrequency()
{
	return m_Config.frequency;
}

bool CPythonSystem::IsNoSoundCard()
{
	return m_Config.bNoSoundCard;
}

bool CPythonSystem::IsSoftwareCursor()
{
	return m_Config.is_software_cursor;
}

float CPythonSystem::GetMusicVolume()
{
	return m_Config.music_volume;
}

int CPythonSystem::GetSoundVolume()
{
	return m_Config.voice_volume;
}

void CPythonSystem::SetMusicVolume(float fVolume)
{
	m_Config.music_volume = fVolume;
}

void CPythonSystem::SetSoundVolumef(float fVolume)
{
	m_Config.voice_volume = int(5 * fVolume);
}

int CPythonSystem::GetDistance()
{
	return m_Config.iDistance;
}

#ifdef ENABLE_SHADOW_RENDER_QUALITY_OPTION
int CPythonSystem::GetShadowTargetLevel()
{
	return m_Config.iShadowTargetLevel;
}

int CPythonSystem::GetRainLevel()
{
	return MIN(MAX(m_Config.iRainLevel,
		CPythonBackground::RAIN_0), /* min */
		CPythonBackground::RAIN_3 /* max */
	);
}

void CPythonSystem::SetRainLevel(unsigned int level)
{
	m_Config.iRainLevel = MIN(MAX(level,
		CPythonBackground::RAIN_0), /* min */
		CPythonBackground::RAIN_3 /* max */
	);
}

void CPythonSystem::SetShadowTargetLevel(unsigned int level)
{
	m_Config.iShadowTargetLevel = MINMAX(CPythonBackground::SHADOW_NONE, level, CPythonBackground::GRAPHICS_NONE);
	CPythonBackground::Instance().RefreshShadowTargetLevel();
}

int CPythonSystem::GetShadowQualityLevel()
{
	return m_Config.iShadowQualityLevel;
}

void CPythonSystem::SetShadowQualityLevel(unsigned int level)
{
	m_Config.iShadowQualityLevel = MINMAX(CPythonBackground::SHADOW_BAD, level, CPythonBackground::SHADOW_GOOD);
	CPythonBackground::Instance().RefreshShadowQualityLevel();
}
#else
int CPythonSystem::GetShadowLevel() const
{
	return m_Config.iShadowLevel;
}

void CPythonSystem::SetShadowLevel(unsigned int level)
{
	m_Config.iShadowLevel = MIN(level, 5);
	CPythonBackground::Instance().RefreshShadowLevel();
}
#endif
#ifdef STONE_SCALE_SYSTEM
void CPythonSystem::SetStoneScale(float fScale)
{
	m_Config.m_fStoneScale = fScale;
}

float CPythonSystem::GetStoneScale()
{
	return m_Config.m_fStoneScale;
}
#endif
int CPythonSystem::IsSaveID()
{
	return m_Config.isSaveID;
}

const char * CPythonSystem::GetSaveID()
{
	return m_Config.SaveID;
}

bool CPythonSystem::isViewCulling()
{
	return m_Config.is_object_culling;
}

void CPythonSystem::SetSaveID(int iValue, const char * c_szSaveID)
{
	if (iValue != 1)
		return;

	m_Config.isSaveID = iValue;
	strncpy(m_Config.SaveID, c_szSaveID, sizeof(m_Config.SaveID) - 1);
}

CPythonSystem::TConfig * CPythonSystem::GetConfig()
{
	return &m_Config;
}

void CPythonSystem::SetConfig(TConfig * pNewConfig)
{
	m_Config = *pNewConfig;
}

void CPythonSystem::SetDefaultConfig()
{
	memset(&m_Config, 0, sizeof(m_Config));

	m_Config.width				= 1024;
	m_Config.height				= 768;
	m_Config.bpp				= 32;

#if defined( LOCALE_SERVICE_WE_JAPAN )
	m_Config.bWindowed			= true;
#else
	m_Config.bWindowed			= false;
#endif

	m_Config.is_software_cursor	= false;
	m_Config.is_object_culling	= true;
	m_Config.iDistance			= 3;

#ifdef ENABLE_OFFLINESHOP_SYSTEM
	m_Config.shopnames_range = 0.5f;
#endif

	m_Config.gamma				= 3;
	m_Config.music_volume		= 1.0f;
	m_Config.voice_volume		= 5;

#ifdef ENABLE_TRACK_WINDOW
	m_Config.bDungeonTrack = false;
	m_Config.bBossTrack = false;
#endif

	m_Config.bDecompressDDS		= 0;
	m_Config.bSoftwareTiling	= 0;
#ifdef ENABLE_SHADOW_RENDER_QUALITY_OPTION
	m_Config.iShadowTargetLevel = CPythonBackground::SHADOW_NONE;
	m_Config.iShadowQualityLevel = CPythonBackground::SHADOW_BAD;
#else
	m_Config.iShadowLevel = 3;
#endif
#if defined(ENABLE_FOV_OPTION)
	m_Config.fFOV = c_fDefaultCameraPerspective;
#endif
	m_Config.bViewChat			= true;
	m_Config.bAlwaysShowName	= DEFAULT_VALUE_ALWAYS_SHOW_NAME;
	m_Config.bShowDamage		= true;
	m_Config.bShowSalesText		= true;
#ifdef STONE_SCALE_SYSTEM
	m_Config.m_fStoneScale = 1.0f;
#endif
#ifdef ENABLE_FOG_FIX
	m_Config.bFogMode			= false;
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
	m_Config.bShowMobAIFlag		= true;
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
	m_Config.bShowMobLevel		= true;
#endif
#if defined(__BL_SAVE_CAMERA_MODE__)
	m_Config.bCameraMode = 0;
#endif

#ifdef ENABLE_DOG_MODE
	m_Config.bDogModeStatus = false;
#endif

#ifdef ENABLE_GRAPHIC_ON_OFF
	m_Config.iEffectLevel		= 0;
	m_Config.iPrivateShopLevel	= 0;
	m_Config.iDropItemLevel		= 0;

	m_Config.bPetStatus			= 0; //true en lugar del (0)normalmente
	m_Config.bNpcNameStatus		= 0; //true en lugar del (0)normalmente
#endif

}

bool CPythonSystem::IsWindowed()
{
	return m_Config.bWindowed;
}

bool CPythonSystem::IsViewChat()
{
	return m_Config.bViewChat;
}

void CPythonSystem::SetViewChatFlag(int iFlag)
{
	m_Config.bViewChat = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsAlwaysShowName()
{
	return m_Config.bAlwaysShowName;
}

void CPythonSystem::SetAlwaysShowNameFlag(int iFlag)
{
	m_Config.bAlwaysShowName = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsShowDamage()
{
	return m_Config.bShowDamage;
}

void CPythonSystem::SetShowDamageFlag(int iFlag)
{
	m_Config.bShowDamage = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsShowSalesText()
{
	return m_Config.bShowSalesText;
}

void CPythonSystem::SetShowSalesTextFlag(int iFlag)
{
	m_Config.bShowSalesText = iFlag == 1 ? true : false;
}

#ifdef ENABLE_FOG_FIX
void CPythonSystem::SetFogMode(int iFlag)
{
	m_Config.bFogMode = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsFogMode()
{
	return m_Config.bFogMode;
}
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
bool CPythonSystem::IsShowMobAIFlag()
{
	return m_Config.bShowMobAIFlag;
}

void CPythonSystem::SetShowMobAIFlagFlag(int iFlag)
{
	m_Config.bShowMobAIFlag = iFlag == 1 ? true : false;
}
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
bool CPythonSystem::IsShowMobLevel()
{
	return m_Config.bShowMobLevel;
}

void CPythonSystem::SetShowMobLevelFlag(int iFlag)
{
	m_Config.bShowMobLevel = iFlag == 1 ? true : false;
}
#endif

int CPythonSystem::GetPlayerInfo(int type)
{
	return m_Config.pInfo[type];
}

bool CPythonSystem::IsAutoTiling()
{
	if (m_Config.bSoftwareTiling == 0)
		return true;

	return false;
}

#ifdef ENABLE_DOG_MODE
bool CPythonSystem::IsDogModeStatus()
{
	return m_Config.bDogModeStatus;
}
void CPythonSystem::SetDogModeStatusFlag(int iFlag)
{
	m_Config.bDogModeStatus = iFlag == 1 ? true : false;
}
#endif

#ifdef ENABLE_GRAPHIC_ON_OFF
int CPythonSystem::GetEffectLevel()
{
	return m_Config.iEffectLevel;
}
void CPythonSystem::SetEffectLevel(unsigned int level)
{
	m_Config.iEffectLevel = MIN(level, 5);
}

int CPythonSystem::GetPrivateShopLevel()
{
	return m_Config.iPrivateShopLevel;
}
void CPythonSystem::SetPrivateShopLevel(unsigned int level)
{
	m_Config.iPrivateShopLevel = MIN(level, 5);
}

int CPythonSystem::GetDropItemLevel()
{
	return m_Config.iDropItemLevel;
}
void CPythonSystem::SetDropItemLevel(unsigned int level)
{
	m_Config.iDropItemLevel = MIN(level, 5);
}


bool CPythonSystem::IsPetStatus()
{
	return m_Config.bPetStatus;
}
void CPythonSystem::SetPetStatusFlag(int iFlag)
{
	m_Config.bPetStatus = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsNpcNameStatus()
{
	return m_Config.bNpcNameStatus;
}
void CPythonSystem::SetNpcNameStatusFlag(int iFlag)
{
	m_Config.bNpcNameStatus = iFlag == 1 ? true : false;
}
#endif

void CPythonSystem::SetSoftwareTiling(bool isEnable)
{
	if (isEnable)
		m_Config.bSoftwareTiling=1;
	else
		m_Config.bSoftwareTiling=2;
}

bool CPythonSystem::IsSoftwareTiling()
{
	if (m_Config.bSoftwareTiling==1)
		return true;

	return false;
}

#if defined(ENABLE_FOV_OPTION)
float CPythonSystem::GetFOV()
{
	return m_Config.fFOV;
}

void CPythonSystem::SetFOV(float fFlag)
{
	m_Config.fFOV = fMINMAX(c_fMinCameraPerspective, fFlag, c_fMaxCameraPerspective);
}
#endif

bool CPythonSystem::IsUseDefaultIME()
{
	return m_Config.bUseDefaultIME;
}

bool CPythonSystem::LoadConfig()
{
	FILE * fp = NULL;

	if (NULL == (fp = fopen("metin2.cfg", "rt")))
		return false;

	char buf[256];
	char command[256];
	char value[256];

	while (fgets(buf, 256, fp))
	{
		if (sscanf(buf, " %s %s\n", command, value) == EOF)
			break;

		if (!stricmp(command, "WIDTH"))
			m_Config.width = atoi(value);
		else if (!stricmp(command, "HEIGHT"))
			m_Config.height = atoi(value);
		else if (!stricmp(command, "BPP"))
			m_Config.bpp = atoi(value);
		else if (!stricmp(command, "FREQUENCY"))
			m_Config.frequency = atoi(value);
		else if (!stricmp(command, "SOFTWARE_CURSOR"))
			m_Config.is_software_cursor = atoi(value) ? true : false;
		else if (!stricmp(command, "OBJECT_CULLING"))
			m_Config.is_object_culling = atoi(value) ? true : false;
		else if (!stricmp(command, "VISIBILITY"))
			m_Config.iDistance = atoi(value);
		else if (!stricmp(command, "MUSIC_VOLUME")) {
			if(strchr(value, '.') == 0) { // Old compatiability
				m_Config.music_volume = pow(10.0f, (-1.0f + (((float) atoi(value)) / 5.0f)));
				if(atoi(value) == 0)
					m_Config.music_volume = 0.0f;
			} else
				m_Config.music_volume = atof(value);
		} else if (!stricmp(command, "VOICE_VOLUME"))
			m_Config.voice_volume = (char) atoi(value);
		else if (!stricmp(command, "GAMMA"))
			m_Config.gamma = atoi(value);
		else if (!stricmp(command, "IS_SAVE_ID"))
			m_Config.isSaveID = atoi(value);
		else if (!stricmp(command, "SAVE_ID"))
			strncpy(m_Config.SaveID, value, 20);

#ifdef ENABLE_OFFLINESHOP_SYSTEM
		else if (!stricmp(command, "shop_range"))
		{
			if (strchr(value, '.') == 0)
			{
				m_Config.shopnames_range = pow(10.0f, (-1.0f + (((float)atoi(value)) / 5.0f)));
				if (atoi(value) == 0)
					m_Config.shopnames_range = 0.5f;
			}
			else
				m_Config.shopnames_range = atof(value);
		}
#endif

#ifdef ENABLE_TRACK_WINDOW
		else if (!stricmp(command, "DUNGEON_TRACK"))
			m_Config.bDungeonTrack = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "BOSS_TRACK"))
			m_Config.bBossTrack = atoi(value) == 1 ? true : false;
#endif

		else if (!stricmp(command, "WINDOWED"))
		{
			m_Config.bWindowed = atoi(value) == 1 ? true : false;
		}
		else if (!stricmp(command, "USE_DEFAULT_IME"))
			m_Config.bUseDefaultIME = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SOFTWARE_TILING"))
			m_Config.bSoftwareTiling = atoi(value);
#if defined(ENABLE_FOV_OPTION)
		else if (!stricmp(command, "FOV"))
			m_Config.fFOV = atof(value);
#endif
#ifdef ENABLE_SHADOW_RENDER_QUALITY_OPTION
		else if (!stricmp(command, "SHADOW_TARGET_LEVEL"))
			m_Config.iShadowTargetLevel = atoi(value);
		else if (!stricmp(command, "SHADOW_QUALITY_LEVEL"))
			m_Config.iShadowQualityLevel = atoi(value);
#else
		else if (!stricmp(command, "SHADOW_LEVEL"))
			m_Config.iShadowLevel = atoi(value);
#endif
		else if (!stricmp(command, "DECOMPRESSED_TEXTURE"))
			m_Config.bDecompressDDS = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "NO_SOUND_CARD"))
			m_Config.bNoSoundCard = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "VIEW_CHAT"))
			m_Config.bViewChat = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "ALWAYS_VIEW_NAME"))
			m_Config.bAlwaysShowName = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SHOW_DAMAGE"))
			m_Config.bShowDamage = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SHOW_SALESTEXT"))
			m_Config.bShowSalesText = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "RAIN_LEVEL"))
			m_Config.iRainLevel = atoi(value);
#ifdef ENABLE_FOG_FIX
		else if (!stricmp(command, "FOG_MODE_ON"))
			m_Config.bFogMode = atoi(value) == 1 ? true : false;
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
		else if (!stricmp(command, "SHOW_MOBAIFLAG"))
			m_Config.bShowMobAIFlag = atoi(value) == 1 ? true : false;
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
		else if (!stricmp(command, "SHOW_MOBLEVEL"))
			m_Config.bShowMobLevel = atoi(value) == 1 ? true : false;
#endif
#if defined(__BL_SAVE_CAMERA_MODE__)
		else if (!stricmp(command, "CAMERA_MODE"))
			m_Config.bCameraMode = atoi(value);
#endif
#ifdef ENABLE_DOG_MODE
		else if (!stricmp(command, "DOG_MODE_STATUS"))
			m_Config.bDogModeStatus = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_GRAPHIC_ON_OFF
		else if (!stricmp(command, "EFFECT_LEVEL"))
			m_Config.iEffectLevel = atoi(value);
		else if (!stricmp(command, "PRIVATE_SHOP_LEVEL"))
			m_Config.iPrivateShopLevel = atoi(value);
		else if (!stricmp(command, "DROP_ITEM_LEVEL"))
			m_Config.iDropItemLevel = atoi(value);

		//else if (!stricmp(command, "PET_STATUS"))
		//	m_Config.bPetStatus = atoi(value) == 1 ? true : false;
		//else if (!stricmp(command, "NPC_NAME_STATUS"))
		//	m_Config.bNpcNameStatus = atoi(value) == 1 ? true : false;
#endif

#ifdef STONE_SCALE_SYSTEM
		else if (!stricmp(command, "STONE_SCALE"))
			m_Config.m_fStoneScale = atof(value);
#endif

	}

	if (m_Config.bWindowed)
	{
		unsigned screen_width = GetSystemMetrics(SM_CXFULLSCREEN);
		unsigned screen_height = GetSystemMetrics(SM_CYFULLSCREEN);

		if (m_Config.width >= screen_width)
		{
			m_Config.width = screen_width;
		}
		if (m_Config.height >= screen_height)
		{
			m_Config.height = screen_height;
		}
	}

	m_OldConfig = m_Config;

	fclose(fp);

//	Tracef("LoadConfig: Resolution: %dx%d %dBPP %dHZ Software Cursor: %d, Music/Voice Volume: %d/%d Gamma: %d\n",
//		m_Config.width,
//		m_Config.height,
//		m_Config.bpp,
//		m_Config.frequency,
//		m_Config.is_software_cursor,
//		m_Config.music_volume,
//		m_Config.voice_volume,
//		m_Config.gamma);

	return true;
}

bool CPythonSystem::SaveConfig()
{
	FILE *fp;

	if (NULL == (fp = fopen("metin2.cfg", "wt")))
		return false;

	fprintf(fp, "WIDTH						%d\n"
				"HEIGHT						%d\n"
				"BPP						%d\n"
				"FREQUENCY					%d\n"
				"SOFTWARE_CURSOR			%d\n"
				"OBJECT_CULLING				%d\n"
				"VISIBILITY					%d\n"
				"MUSIC_VOLUME				%.3f\n"
				"VOICE_VOLUME				%d\n"
				"GAMMA						%d\n"
				"IS_SAVE_ID					%d\n"
				"SAVE_ID					%s\n"
				"DECOMPRESSED_TEXTURE		%d\n",
				m_Config.width,
				m_Config.height,
				m_Config.bpp,
				m_Config.frequency,
				m_Config.is_software_cursor,
				m_Config.is_object_culling,
				m_Config.iDistance,
				m_Config.music_volume,
				m_Config.voice_volume,
				m_Config.gamma,
				m_Config.isSaveID,
				m_Config.SaveID,
				m_Config.bDecompressDDS);

#ifdef ENABLE_OFFLINESHOP_SYSTEM
	fprintf(fp, "shop_range\t%.3f\n", m_Config.shopnames_range);
#endif

#ifdef ENABLE_TRACK_WINDOW
	fprintf(fp, "DUNGEON_TRACK\t%d\n", m_Config.bDungeonTrack);
	fprintf(fp, "BOSS_TRACK\t%d\n", m_Config.bBossTrack);
#endif

	if (m_Config.bWindowed == 1)
		fprintf(fp, "WINDOWED				%d\n", m_Config.bWindowed);
	if (m_Config.bViewChat == 0)
		fprintf(fp, "VIEW_CHAT				%d\n", m_Config.bViewChat);
	if (m_Config.bAlwaysShowName != DEFAULT_VALUE_ALWAYS_SHOW_NAME)
		fprintf(fp, "ALWAYS_VIEW_NAME		%d\n", m_Config.bAlwaysShowName);
	if (m_Config.bShowDamage == 0)
		fprintf(fp, "SHOW_DAMAGE		%d\n", m_Config.bShowDamage);
	if (m_Config.bShowSalesText == 0)
		fprintf(fp, "SHOW_SALESTEXT		%d\n", m_Config.bShowSalesText);
#ifdef ENABLE_FOG_FIX
	if (m_Config.bFogMode == 0)
		fprintf(fp, "FOG_MODE_ON		%d\n", m_Config.bFogMode);
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
	if (m_Config.bShowMobAIFlag == 0)
		fprintf(fp, "SHOW_MOBAIFLAG		%d\n", m_Config.bShowMobAIFlag);
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
	if (m_Config.bShowMobLevel == 0)
		fprintf(fp, "SHOW_MOBLEVEL		%d\n", m_Config.bShowMobLevel);
#endif
#if defined(__BL_SAVE_CAMERA_MODE__)
	fprintf(fp, "CAMERA_MODE\t\t\t%d\n", m_Config.bCameraMode);
#endif
	fprintf(fp, "USE_DEFAULT_IME		%d\n", m_Config.bUseDefaultIME);
	fprintf(fp, "SOFTWARE_TILING		%d\n", m_Config.bSoftwareTiling);
#ifdef ENABLE_SHADOW_RENDER_QUALITY_OPTION
	fprintf(fp, "SHADOW_TARGET_LEVEL		%d\n", m_Config.iShadowTargetLevel);
	fprintf(fp, "SHADOW_QUALITY_LEVEL		%d\n", m_Config.iShadowQualityLevel);
#else
	fprintf(fp, "SHADOW_LEVEL				%d\n", m_Config.iShadowLevel);
#endif
#if defined(ENABLE_FOV_OPTION)
	fprintf(fp, "FOV						%.2f\n", m_Config.fFOV);
#endif

#ifdef ENABLE_DOG_MODE
	fprintf(fp, "DOG_MODE_STATUS			%d\n", m_Config.bDogModeStatus);
#endif

#ifdef ENABLE_GRAPHIC_ON_OFF
	fprintf(fp, "EFFECT_LEVEL			%d\n", m_Config.iEffectLevel);
	fprintf(fp, "PRIVATE_SHOP_LEVEL		%d\n", m_Config.iPrivateShopLevel);
	fprintf(fp, "DROP_ITEM_LEVEL		%d\n", m_Config.iDropItemLevel);

	if (m_Config.bPetStatus	== 0)
		fprintf(fp, "PET_STATUS			%d\n", m_Config.bPetStatus);

	if (m_Config.bNpcNameStatus	== 0)
		fprintf(fp, "NPC_NAME_STATUS	%d\n", m_Config.bNpcNameStatus);
#endif
	fprintf(fp, "RAIN_LEVEL					%d\n", m_Config.iRainLevel);
#ifdef STONE_SCALE_SYSTEM
	fprintf(fp, "STONE_SCALE			%f\n", m_Config.m_fStoneScale);
#endif

	fprintf(fp, "\n");

	fclose(fp);
	return true;
}

bool CPythonSystem::LoadInterfaceStatus()
{
	FILE * File;
	File = fopen("interface.cfg", "rb");

	if (!File)
		return false;

	fread(m_WindowStatus, 1, sizeof(TWindowStatus) * WINDOW_MAX_NUM, File);
	fclose(File);
	return true;
}

void CPythonSystem::SaveInterfaceStatus()
{
	if (!m_poInterfaceHandler)
		return;

	PyCallClassMemberFunc(m_poInterfaceHandler, "OnSaveInterfaceStatus", Py_BuildValue("()"));

	FILE * File;

	File = fopen("interface.cfg", "wb");

	if (!File)
	{
		TraceError("Cannot open interface.cfg");
		return;
	}

	fwrite(m_WindowStatus, 1, sizeof(TWindowStatus) * WINDOW_MAX_NUM, File);
	fclose(File);
}

bool CPythonSystem::isInterfaceConfig()
{
	return m_isInterfaceConfig;
}

const CPythonSystem::TWindowStatus & CPythonSystem::GetWindowStatusReference(int iIndex)
{
	return m_WindowStatus[iIndex];
}

void CPythonSystem::ApplyConfig()
{
	if (m_OldConfig.gamma != m_Config.gamma)
	{
		float val = 1.0f;

		switch (m_Config.gamma)
		{
			case 0: val = 0.4f;	break;
			case 1: val = 0.7f; break;
			case 2: val = 1.0f; break;
			case 3: val = 1.2f; break;
			case 4: val = 1.4f; break;
		}

		CPythonGraphic::Instance().SetGamma(val);
	}

	if (m_OldConfig.is_software_cursor != m_Config.is_software_cursor)
	{
		if (m_Config.is_software_cursor)
			CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_SOFTWARE);
		else
			CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_HARDWARE);
	}

	m_OldConfig = m_Config;

	ChangeSystem();
}

void CPythonSystem::ChangeSystem()
{
	// Shadow
	/*
	if (m_Config.is_shadow)
		CScreen::SetShadowFlag(true);
	else
		CScreen::SetShadowFlag(false);
	*/
	CSoundManager& rkSndMgr = CSoundManager::Instance();
	/*
	float fMusicVolume;
	if (0 == m_Config.music_volume)
		fMusicVolume = 0.0f;
	else
		fMusicVolume= (float)pow(10.0f, (-1.0f + (float)m_Config.music_volume / 5.0f));
		*/
	rkSndMgr.SetMusicVolume(m_Config.music_volume);

	/*
	float fVoiceVolume;
	if (0 == m_Config.voice_volume)
		fVoiceVolume = 0.0f;
	else
		fVoiceVolume = (float)pow(10.0f, (-1.0f + (float)m_Config.voice_volume / 5.0f));
	*/
	rkSndMgr.SetSoundVolumeGrade(m_Config.voice_volume);
}

void CPythonSystem::Clear()
{
	SetInterfaceHandler(NULL);
}

CPythonSystem::CPythonSystem()
{
	memset(&m_Config, 0, sizeof(TConfig));

	m_poInterfaceHandler = NULL;

	SetDefaultConfig();

	LoadConfig();

	ChangeSystem();

	if (LoadInterfaceStatus())
		m_isInterfaceConfig = true;
	else
		m_isInterfaceConfig = false;
}

CPythonSystem::~CPythonSystem()
{
	assert(m_poInterfaceHandler==NULL && "CPythonSystem MUST CLEAR!");
}

#if defined(__BL_SAVE_CAMERA_MODE__)
void CPythonSystem::SetCameraMode(BYTE bMode)
{
	m_Config.bCameraMode = bMode;
}

BYTE CPythonSystem::GetCameraMode() const
{
	return m_Config.bCameraMode;
}
#endif

#ifdef ENABLE_HWID_BAN
#include <iomanip>
#include "picosha2.h"
#include "smbios.cpp"
TCHAR* registry_read(LPCTSTR subkey, LPCTSTR name, unsigned long type)
{
	HKEY key;
	TCHAR value[255];
	unsigned long value_length = 255;
	RegOpenKey(HKEY_LOCAL_MACHINE, subkey, &key);
	RegQueryValueEx(key, name, nullptr, &type, (LPBYTE)&value, &value_length);
	RegCloseKey(key);
	return value;
}

DWORD CPythonSystem::getVolumeHash() {
	CHAR windowsDirectory[MAX_PATH];
	CHAR volumeName[8] = { 0 };
	unsigned long serialNum = 0;

	if (!GetWindowsDirectory(windowsDirectory, sizeof(windowsDirectory)))
		windowsDirectory[0] = L'C';

	volumeName[0] = windowsDirectory[0];
	volumeName[1] = ':';
	volumeName[2] = '\\';
	volumeName[3] = '\0';

	GetVolumeInformation(volumeName, nullptr, 0, &serialNum, 0, nullptr, nullptr, 0);

	return serialNum;
}

const char* CPythonSystem::getCpuInfos() {
	SYSTEM_INFO kSystemInfo;
	GetSystemInfo(&kSystemInfo);

	std::string stTemp;
	char szNum[15 + 1];
#define AddNumber(num) _itoa_s(num, szNum, 10), stTemp += szNum
	AddNumber(kSystemInfo.wProcessorArchitecture);
	AddNumber(kSystemInfo.dwNumberOfProcessors);
	AddNumber(kSystemInfo.dwProcessorType);
	AddNumber(kSystemInfo.wProcessorLevel);
	AddNumber(kSystemInfo.wProcessorRevision);
	AddNumber(IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE));
	AddNumber(IsProcessorFeaturePresent(PF_CHANNELS_ENABLED));
	AddNumber(IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE_DOUBLE));
	AddNumber(IsProcessorFeaturePresent(PF_FLOATING_POINT_EMULATED));
	AddNumber(IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE));
	AddNumber(IsProcessorFeaturePresent(PF_PAE_ENABLED));
	AddNumber(IsProcessorFeaturePresent(PF_RDTSC_INSTRUCTION_AVAILABLE));
	AddNumber(IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE));
	AddNumber(IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE));
#undef AddNumber

	return stTemp.c_str();
}

const char* CPythonSystem::getMachineName() {
	static char computerName[1024];
	unsigned long size = 1024;
	GetComputerName(computerName, &size);
	return &(computerName[0]);
}

const char* CPythonSystem::getBiosDate()
{
	static char buf[1024];
	strncpy(buf, registry_read("HARDWARE\\DESCRIPTION\\System\\BIOS", "BIOSReleaseDate", REG_SZ), sizeof(buf));
	return buf;
}

const char* CPythonSystem::getMainboardName()
{
	static char buf[1024];
	strncpy(buf, registry_read("HARDWARE\\DESCRIPTION\\System\\BIOS", "BaseBoardProduct", REG_SZ), sizeof(buf));
	return buf;
}

const char* CPythonSystem::getGPUName()
{
	static char buf[1024];
	IDirect3D8* d8object = Direct3DCreate8(D3D_SDK_VERSION);
	unsigned int adaptercount = d8object->GetAdapterCount();
	D3DADAPTER_IDENTIFIER8* adapters = new D3DADAPTER_IDENTIFIER8[sizeof(adaptercount)];

	for (unsigned int i = 0; i < adaptercount; i++) {
		d8object->GetAdapterIdentifier(i, 0, &(adapters[i]));
	}

	strncpy(buf, adapters->Description, sizeof(buf));
	return buf;
}

const char* CPythonSystem::GetHWID()
{
	static const unsigned long TargetLength = 64;

	std::stringstream stream;

#ifdef HDD_COMPONENT
	stream << getVolumeHash();
#endif
#ifdef CPU_COMPONENT
	stream << getCpuInfos();
#endif
#ifdef PC_NAME_COMPONENT
	stream << getMachineName();
#endif
#ifdef BIOS_DATE_COMPONENT
	stream << getBiosDate();
#endif
#ifdef MAINBOARD_NAME_COMPONENT
	stream << getMainboardName();
#endif
#ifdef GPU_NAME_COMPONENT
	stream << getGPUName();
#endif

	auto string = stream.str();

	while (string.size() < TargetLength) {
		string = string + string;
	}

	if (string.size() > TargetLength) {
		string = string.substr(0, TargetLength);
	}

	return generateHash(string);
}

const char* CPythonSystem::generateHash(const std::string& bytes) {
	static char s_szHWIDBuffer[HWID_MAX_LEN];
	static char chars[] = "0123456789ABCDEF";
	std::stringstream stream;

	auto size = bytes.size();
	for (unsigned long i = 0; i < size; ++i) {
		unsigned char ch = ~((unsigned char)((WORD)bytes[i] +
			(WORD)bytes[(i + 1) % size] +
			(WORD)bytes[(i + 2) % size] +
			(WORD)bytes[(i + 3) % size])) * (i + 1);

		stream << chars[(ch >> 4) & 0x0F] << chars[ch & 0x0F];
	}

	strncpy(s_szHWIDBuffer, stream.str().c_str(), sizeof(s_szHWIDBuffer));

	return s_szHWIDBuffer;
}
#endif

#if defined(__BL_PICK_FILTER__)
#include <fstream>

CPythonSystem::CPickUpFilter::CPickUpFilter()
{
	///// READ ////
	std::ifstream inputFile;
	inputFile.open(cPickUpFilterFileName, std::ios::binary);
	if (inputFile.is_open())
	{
		inputFile.read(reinterpret_cast<char*>(bPickFilter),	sizeof(bool) * EPICKFILTER::EPICKFILTER_MAX);
		inputFile.read(reinterpret_cast<char*>(bPickSize),		sizeof(bool) * ESIZE::ESIZE_MAX);
		inputFile.read(reinterpret_cast<char*>(&bModeAll),		sizeof(bool));
		inputFile.read(reinterpret_cast<char*>(&m_bRefineMin),	sizeof(BYTE));
		inputFile.read(reinterpret_cast<char*>(&m_bRefineMax),	sizeof(BYTE));
		inputFile.read(reinterpret_cast<char*>(&m_lLevelMin),	sizeof(long));
		inputFile.read(reinterpret_cast<char*>(&m_lLevelMax),	sizeof(long));

		inputFile.close();
	}
	else
	{
		///// SET DEFAULT ////
		std::fill(std::begin(bPickFilter), std::end(bPickFilter), true);
		std::fill(std::begin(bPickSize), std::end(bPickSize), true);

		bModeAll = false;
		
		m_bRefineMin = 0;
		m_bRefineMax = 9;

		m_lLevelMin = 0;
		m_lLevelMax = 999;
	}
}

CPythonSystem::CPickUpFilter::~CPickUpFilter()
{
	///// SAVE ////
	std::ofstream outputFile;
	outputFile.open(cPickUpFilterFileName, std::ios::binary);
	if (outputFile.is_open())
	{
		outputFile.write(reinterpret_cast<char*>(bPickFilter),		sizeof(bool) * EPICKFILTER::EPICKFILTER_MAX);
		outputFile.write(reinterpret_cast<char*>(bPickSize),		sizeof(bool) * ESIZE::ESIZE_MAX);
		outputFile.write(reinterpret_cast<char*>(&bModeAll),		sizeof(bool));
		outputFile.write(reinterpret_cast<char*>(&m_bRefineMin),	sizeof(BYTE));
		outputFile.write(reinterpret_cast<char*>(&m_bRefineMax),	sizeof(BYTE));
		outputFile.write(reinterpret_cast<char*>(&m_lLevelMin),		sizeof(long));
		outputFile.write(reinterpret_cast<char*>(&m_lLevelMax),		sizeof(long));

		outputFile.close();
	}
	else
	{
		Tracenf("CPickUpFilter::~CPickUpFilter() Cannot create a file for save settings.");
	}
}

void CPythonSystem::CPickUpFilter::SetFilter(size_t sIndex, bool b)
{
	if (sIndex >= EPICKFILTER::EPICKFILTER_MAX)
	{
		Tracenf("CPickUpFilter::SetFilter(Index=%d) : Out of range", sIndex);
		return;
	}

	bPickFilter[sIndex] = b;
}

void CPythonSystem::CPickUpFilter::SetSize(size_t sIndex, bool b)
{
	if (sIndex >= ESIZE::ESIZE_MAX)
	{
		Tracenf("CPickUpFilter::SetSize(Index=%d) : Out of range", sIndex);
		return;
	}

	if (sIndex == ESIZE::BIG && b == false)
		SetFilter(EPICKFILTER::SUB_WEAPON_TWO_HANDED, false);

	bPickSize[sIndex] = b;
}

void CPythonSystem::CPickUpFilter::SetRefine(BYTE min, BYTE max)
{
	m_bRefineMin = static_cast<BYTE>(MINMAX(0, min, 9));
	m_bRefineMax = static_cast<BYTE>(MINMAX(0, max, 9));
}

void CPythonSystem::CPickUpFilter::SetLevel(long min, long max)
{
	m_lLevelMin = MINMAX(0, min, 999);
	m_lLevelMax = MINMAX(0, max, 999);
}

void CPythonSystem::CPickUpFilter::SetModeAll(bool b)
{
	bModeAll = b;
}

bool CPythonSystem::CPickUpFilter::CanPickItem(DWORD dwIID)
{
	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(CPythonItem::Instance().GetVirtualNumberOfGroundItem(dwIID), &pItemData))
	{
		Tracenf("CPickUpFilter::CanPickItem(dwIID=%d) : Non-exist item.", dwIID);
		return true;
	}

	if (CheckRefine(pItemData) == false)
		return false;

	if (CheckLevel(pItemData) == false)
		return false;

	if (CheckSize(pItemData) == false)
		return false;

	if (CheckType(pItemData) == false)
		return false;
	
	return true;
}

std::pair<BYTE, BYTE> CPythonSystem::CPickUpFilter::GetRefine()
{
	return std::make_pair(m_bRefineMin, m_bRefineMax);
}

std::pair<long, long> CPythonSystem::CPickUpFilter::GetLevel()
{
	return std::make_pair(m_lLevelMin, m_lLevelMax);
}

bool CPythonSystem::CPickUpFilter::GetFilter(size_t sIndex) const
{
	if (sIndex >= EPICKFILTER::EPICKFILTER_MAX)
	{
		Tracenf("CPickUpFilter::GetFilter(Index=%d) : Out of range", sIndex);
		return false;
	}

	return bPickFilter[sIndex];
}

bool CPythonSystem::CPickUpFilter::GetSize(size_t sIndex) const
{
	if (sIndex >= ESIZE::ESIZE_MAX)
	{
		Tracenf("CPickUpFilter::GetSize(Index=%d) : Out of range", sIndex);
		return false;
	}

	return bPickSize[sIndex];
}

bool CPythonSystem::CPickUpFilter::IsModeAll() const
{
	return bModeAll;
}

bool CPythonSystem::CPickUpFilter::CheckRefine(const CItemData* pItem) const
{
	const BYTE bRefineLevel = static_cast<BYTE>(pItem->GetRefine());
	if (bRefineLevel >= m_bRefineMin && bRefineLevel <= m_bRefineMax)
		return true;

	return false;
}

bool CPythonSystem::CPickUpFilter::CheckLevel(const CItemData* pItem) const
{
	CItemData::TItemLimit ItemLimit;
	for (BYTE i = 0; i < CItemData::ITEM_LIMIT_MAX_NUM; i++)
	{
		if (!pItem->GetLimit(i, &ItemLimit))
			continue;

		if (ItemLimit.bType != CItemData::LIMIT_LEVEL)
			continue;

		const long lLimitLevel = ItemLimit.lValue;
		return (lLimitLevel >= m_lLevelMin && lLimitLevel <= m_lLevelMax);
	}

	return true;
}

bool CPythonSystem::CPickUpFilter::CheckSize(const CItemData* pItem) const
{
	return GetSize(pItem->GetSize() - 1);
}

bool CPythonSystem::CPickUpFilter::CheckType(const CItemData* pItem) const
{
	const BYTE bType = pItem->GetType();
	const BYTE bSubType = pItem->GetSubType();

	switch (bType)
	{
	case CItemData::EItemType::ITEM_TYPE_WEAPON:
		switch (bSubType)
		{
		case CItemData::EWeaponSubTypes::WEAPON_SWORD:
			return GetFilter(EPICKFILTER::SUB_WEAPON_SWORD);

		case CItemData::EWeaponSubTypes::WEAPON_DAGGER:
			return GetFilter(EPICKFILTER::SUB_WEAPON_DAGGER);

		case CItemData::EWeaponSubTypes::WEAPON_BOW:
			return GetFilter(EPICKFILTER::SUB_WEAPON_BOW);

		case CItemData::EWeaponSubTypes::WEAPON_TWO_HANDED:
			return GetFilter(EPICKFILTER::SUB_WEAPON_TWO_HANDED);

		case CItemData::EWeaponSubTypes::WEAPON_BELL:
			return GetFilter(EPICKFILTER::SUB_WEAPON_BELL);

		case CItemData::EWeaponSubTypes::WEAPON_FAN:
			return GetFilter(EPICKFILTER::SUB_WEAPON_FAN);

		case CItemData::EWeaponSubTypes::WEAPON_ARROW:
			return GetFilter(EPICKFILTER::SUB_WEAPON_ARROW);

		/*case CItemData::EWeaponSubTypes::WEAPON_MOUNT_SPEAR:
			return GetFilter(EPICKFILTER::SUB_WEAPON_MOUNT_SPEAR);*/
		}
		break;

	case CItemData::EItemType::ITEM_TYPE_ARMOR:
		switch (bSubType)
		{
		case CItemData::EArmorSubTypes::ARMOR_BODY:
			return GetFilter(EPICKFILTER::SUB_ARMOR_BODY);

		case CItemData::EArmorSubTypes::ARMOR_HEAD:
			return GetFilter(EPICKFILTER::SUB_ARMOR_HEAD);

		case CItemData::EArmorSubTypes::ARMOR_SHIELD:
			return GetFilter(EPICKFILTER::SUB_ARMOR_SHIELD);

		case CItemData::EArmorSubTypes::ARMOR_WRIST:
			return GetFilter(EPICKFILTER::SUB_ARMOR_WRIST);

		case CItemData::EArmorSubTypes::ARMOR_FOOTS:
			return GetFilter(EPICKFILTER::SUB_ARMOR_FOOTS);

		case CItemData::EArmorSubTypes::ARMOR_NECK:
			return GetFilter(EPICKFILTER::SUB_ARMOR_NECK);

		case CItemData::EArmorSubTypes::ARMOR_EAR:
			return GetFilter(EPICKFILTER::SUB_ARMOR_EAR);
		}
		break;

	case CItemData::EItemType::ITEM_TYPE_METIN:
		return GetFilter(EPICKFILTER::TYPE_METIN);

	case CItemData::EItemType::ITEM_TYPE_ELK:
		return GetFilter(EPICKFILTER::TYPE_YANG);

	case CItemData::EItemType::ITEM_TYPE_SKILLBOOK:
		return GetFilter(EPICKFILTER::TYPE_SKILLBOOK);

	case CItemData::EItemType::ITEM_TYPE_GIFTBOX:
		return GetFilter(EPICKFILTER::TYPE_GIFTBOX);

	case CItemData::EItemType::ITEM_TYPE_BELT:
		return GetFilter(EPICKFILTER::TYPE_BELT);

	case CItemData::EItemType::ITEM_TYPE_POLYMORPH:
		return GetFilter(EPICKFILTER::TYPE_POLY);

	case CItemData::EItemType::ITEM_TYPE_RING:
		return GetFilter(EPICKFILTER::TYPE_RING);
	
	case CItemData::EItemType::ITEM_TYPE_USE:
		switch (bSubType)
		{
		case CItemData::EUseSubTypes::USE_POTION:
		case CItemData::EUseSubTypes::USE_ABILITY_UP:
		case CItemData::EUseSubTypes::USE_POTION_NODELAY:
		case CItemData::EUseSubTypes::USE_POTION_CONTINUE:
			return GetFilter(EPICKFILTER::SUB_POTION);
		}
		break;

	case CItemData::EItemType::ITEM_TYPE_MATERIAL:
		return GetFilter(EPICKFILTER::TYPE_MATERIAL);
	}

	return true;
}
#endif

//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
