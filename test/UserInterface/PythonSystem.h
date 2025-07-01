#pragma once
#include "../GameLib/ItemData.h"

class CPythonSystem : public CSingleton<CPythonSystem>
{
	public:
		enum EWindow
		{
			WINDOW_STATUS,
			WINDOW_INVENTORY,
			WINDOW_ABILITY,
			WINDOW_SOCIETY,
			WINDOW_JOURNAL,
			WINDOW_COMMAND,

			WINDOW_QUICK,
			WINDOW_GAUGE,
			WINDOW_MINIMAP,
			WINDOW_CHAT,

			WINDOW_MAX_NUM,
		};

		enum
		{
			FREQUENCY_MAX_NUM  = 30,
			RESOLUTION_MAX_NUM = 100
		};

		typedef struct SResolution
		{
			DWORD	width;
			DWORD	height;
			DWORD	bpp;		// bits per pixel (high-color = 16bpp, true-color = 32bpp)

			DWORD	frequency[20];
			BYTE	frequency_count;
		} TResolution;

		typedef struct SWindowStatus
		{
			int		isVisible;
			int		isMinimized;

			int		ixPosition;
			int		iyPosition;
			int		iHeight;
		} TWindowStatus;

		typedef struct SConfig
		{
			DWORD			width;
			DWORD			height;
			DWORD			bpp;
			DWORD			frequency;

			bool			is_software_cursor;
			bool			is_object_culling;
			int				iDistance;
#ifdef ENABLE_SHADOW_RENDER_QUALITY_OPTION
			int				iShadowTargetLevel;
			int				iShadowQualityLevel;
			int				iRainLevel;
#else
			int				iShadowLevel;
#endif


			FLOAT			music_volume;
			BYTE			voice_volume;

			int				gamma;

#ifdef ENABLE_TRACK_WINDOW
			bool			bDungeonTrack;
			bool			bBossTrack;
#endif


#ifdef ENABLE_OFFLINESHOP_SYSTEM
			float			shopnames_range;
#endif

			int				isSaveID;
			char			SaveID[20];

			bool			bWindowed;
			bool			bDecompressDDS;
			bool			bNoSoundCard;
			bool			bUseDefaultIME;
			BYTE			bSoftwareTiling;
			bool			bViewChat;
			bool			bAlwaysShowName;
			bool			bShowDamage;
			bool			bShowSalesText;
#ifdef ENABLE_FOG_FIX
			bool			bFogMode;
#endif
			int pInfo[5];
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
			bool			bShowMobAIFlag;
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
			bool			bShowMobLevel;
#endif
#if defined(__BL_SAVE_CAMERA_MODE__)
			BYTE			bCameraMode;
#endif
#if defined(ENABLE_FOV_OPTION)
			float			fFOV;
#endif
#ifdef ENABLE_DOG_MODE
		bool bDogModeStatus;
#endif
#ifdef ENABLE_GRAPHIC_ON_OFF
			int				iEffectLevel;
			int				iPrivateShopLevel;
			int				iDropItemLevel;

			bool			bPetStatus;
			bool			bNpcNameStatus;
#endif

#ifdef STONE_SCALE_SYSTEM
		float	m_fStoneScale;
#endif

		} TConfig;

#if defined(__BL_PICK_FILTER__)
		class CPickUpFilter final
		{
		public:
			CPickUpFilter();
			~CPickUpFilter();

			void	SetFilter(size_t sIndex, bool b);
			void	SetSize(size_t sIndex, bool b);
			void	SetRefine(BYTE min, BYTE max);
			void	SetLevel(long min, long max);
			void	SetModeAll(bool b);

			bool	CanPickItem(DWORD dwIID);

			bool	GetFilter(size_t sIndex) const;
			bool	GetSize(size_t sIndex) const;
			bool	IsModeAll() const;

			std::pair<BYTE, BYTE> GetRefine();
			std::pair<long, long> GetLevel();

		private:
			bool	CheckRefine(const CItemData* pItem) const;
			bool	CheckLevel(const CItemData* pItem) const;
			bool	CheckSize(const CItemData* pItem) const;
			bool	CheckType(const CItemData* pItem) const;

			static constexpr const char* cPickUpFilterFileName = "PickUpFilter.dat";

		public:
			enum EPICKFILTER
			{
				/*WEAPON-SUB*/
				SUB_WEAPON_SWORD,
				SUB_WEAPON_DAGGER,
				SUB_WEAPON_BOW,
				SUB_WEAPON_TWO_HANDED,
				SUB_WEAPON_BELL,
				SUB_WEAPON_FAN,
				SUB_WEAPON_ARROW,
				//SUB_WEAPON_MOUNT_SPEAR,
				/*WEAPON-SUB*/

				/*ARMOR-SUB*/
				SUB_ARMOR_BODY,
				SUB_ARMOR_HEAD,
				SUB_ARMOR_SHIELD,
				SUB_ARMOR_WRIST,
				SUB_ARMOR_FOOTS,
				SUB_ARMOR_NECK,
				SUB_ARMOR_EAR,
				/*ARMOR-SUB*/

				/*OTHER*/
				TYPE_METIN,
				TYPE_YANG,
				TYPE_SKILLBOOK,
				TYPE_GIFTBOX,
				TYPE_BELT,
				TYPE_POLY,
				TYPE_RING,
				SUB_POTION,
				TYPE_MATERIAL,
				/*OTHER*/

				EPICKFILTER_MAX
			};

			enum ESIZE
			{
				SMALL,
				MID,
				BIG,

				ESIZE_MAX
			};
			
		private:
			bool bPickFilter[EPICKFILTER::EPICKFILTER_MAX];
			bool bPickSize[ESIZE::ESIZE_MAX];

			bool bModeAll;
			
			BYTE m_bRefineMin;
			BYTE m_bRefineMax;
			
			long m_lLevelMin;
			long m_lLevelMax;
		} TPickUpFilter;
#endif

	public:
		CPythonSystem();
		virtual ~CPythonSystem();

		void Clear();
		void SetInterfaceHandler(PyObject * poHandler);
		void DestroyInterfaceHandler();

		// Config
		void							SetDefaultConfig();
		bool							LoadConfig();
		bool							SaveConfig();
		void							ApplyConfig();
		void							SetConfig(TConfig * set_config);
		TConfig *						GetConfig();
		void							ChangeSystem();

		// Interface
		bool							LoadInterfaceStatus();

#ifdef ENABLE_TRACK_WINDOW
		bool							GetDungeonTrack() { return m_Config.bDungeonTrack; }
		void							SetDungeonTrack(bool flag) { m_Config.bDungeonTrack = flag; }

		bool							GetBossTrack() { return m_Config.bBossTrack; }
		void							SetBossTrack(bool flag) { m_Config.bBossTrack = flag; }
#endif

		void							SaveInterfaceStatus();
		bool							isInterfaceConfig();
		const TWindowStatus &			GetWindowStatusReference(int iIndex);

		DWORD							GetWidth();
		DWORD							GetHeight();
		DWORD							GetBPP();
		DWORD							GetFrequency();
		bool							IsSoftwareCursor();
		bool							IsWindowed();
		bool							IsViewChat();
		bool							IsAlwaysShowName();
		bool							IsShowDamage();
		bool							IsShowSalesText();
		bool							IsUseDefaultIME();
		bool							IsNoSoundCard();
		bool							IsAutoTiling();

#ifdef ENABLE_OFFLINESHOP_SYSTEM
		void							SetShopNamesRange(float fRange) { m_Config.shopnames_range = fRange; }
		float							GetShopNamesRange() { return m_Config.shopnames_range; }
#endif

		bool							IsSoftwareTiling();
		void							SetSoftwareTiling(bool isEnable);
		void							SetViewChatFlag(int iFlag);
		void							SetAlwaysShowNameFlag(int iFlag);
		void							SetShowDamageFlag(int iFlag);
		void							SetShowSalesTextFlag(int iFlag);
#ifdef ENABLE_FOG_FIX
		void							SetFogMode(int iFlag);
		bool							IsFogMode();
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
		bool							IsShowMobAIFlag();
		void							SetShowMobAIFlagFlag(int iFlag);
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
		bool							IsShowMobLevel();
		void							SetShowMobLevelFlag(int iFlag);
#endif
#if defined(__BL_SAVE_CAMERA_MODE__)
		void							SetCameraMode(BYTE bMode);
		BYTE							GetCameraMode() const;
#endif


#ifdef ENABLE_DOG_MODE
	bool IsDogModeStatus();
	void SetDogModeStatusFlag(int iFlag);
#endif

#ifdef ENABLE_GRAPHIC_ON_OFF
		int								GetEffectLevel();
		void							SetEffectLevel(unsigned int level);

		int								GetPrivateShopLevel();
		void							SetPrivateShopLevel(unsigned int level);

		int								GetDropItemLevel();
		void							SetDropItemLevel(unsigned int level);


		bool							IsPetStatus();
		void							SetPetStatusFlag(int iFlag);

		bool							IsNpcNameStatus();
		void							SetNpcNameStatusFlag(int iFlag);
#endif

		// Window
		void							SaveWindowStatus(int iIndex, int iVisible, int iMinimized, int ix, int iy, int iHeight);

		// SaveID
		int								IsSaveID();
		const char *					GetSaveID();
		void							SetSaveID(int iValue, const char * c_szSaveID);

		/// Display
		void							GetDisplaySettings();

		int								GetResolutionCount();
		int								GetFrequencyCount(int index);
		bool							GetResolution(int index, OUT DWORD *width, OUT DWORD *height, OUT DWORD *bpp);
		bool							GetFrequency(int index, int freq_index, OUT DWORD *frequncy);
		int								GetResolutionIndex(DWORD width, DWORD height, DWORD bpp);
		int								GetFrequencyIndex(int res_index, DWORD frequency);
		bool							isViewCulling();

		// Sound
		float							GetMusicVolume();
		int								GetSoundVolume();
		void							SetMusicVolume(float fVolume);
		void							SetSoundVolumef(float fVolume);

		int								GetDistance();
#if defined(ENABLE_FOV_OPTION)
		// FoV
		float							GetFOV();
		void							SetFOV(float c_fValue);
#endif

#ifdef ENABLE_SHADOW_RENDER_QUALITY_OPTION
		int								GetShadowTargetLevel();
		void							SetShadowTargetLevel(unsigned int level);
		int								GetShadowQualityLevel();
		void							SetShadowQualityLevel(unsigned int level);
		int								GetRainLevel();
		void							SetRainLevel(unsigned int level);
#else
		int								GetShadowLevel() const;
		void							SetShadowLevel(unsigned int level);
#endif

#ifdef ENABLE_HWID_BAN
		DWORD							getVolumeHash();
		const char*						getCpuInfos();
		const char*						getMachineName();
		const char*						getBiosDate();
		const char*						getMainboardName();
		const char*						getGPUName();
		const char*						GetHWID();
		const char*						generateHash(const std::string& bytes);
#endif

#ifdef STONE_SCALE_SYSTEM
		void							SetStoneScale(float fScale);
		float							GetStoneScale();
#endif

		int GetPlayerInfo(int type);

	protected:
		TResolution						m_ResolutionList[RESOLUTION_MAX_NUM];
		int								m_ResolutionCount;

		TConfig							m_Config;
		TConfig							m_OldConfig;

		bool							m_isInterfaceConfig;
		PyObject *						m_poInterfaceHandler;
		TWindowStatus					m_WindowStatus[WINDOW_MAX_NUM];
};
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
