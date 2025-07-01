#pragma once
#include "../GameLib/ItemData.h"

struct SAffects
{
	enum
	{
		AFFECT_MAX_NUM = 32,
	};

	SAffects() : dwAffects(0) {}
	SAffects(const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}
	int operator = (const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}

	BOOL IsAffect(BYTE byIndex)
	{
		return dwAffects & (1 << byIndex);
	}

	void __SetAffects(const DWORD & c_rAffects)
	{
		dwAffects = c_rAffects;
	}

	DWORD dwAffects;
};

extern std::string g_strGuildSymbolPathName;

constexpr DWORD c_Name_Max_Length = 64;
constexpr DWORD c_FileName_Max_Length = 128;
constexpr DWORD c_Short_Name_Max_Length = 32;

constexpr DWORD c_Inventory_Page_Column = 5;
constexpr DWORD c_Inventory_Page_Row = 9;
constexpr DWORD c_Inventory_Page_Size = c_Inventory_Page_Column*c_Inventory_Page_Row; // x*y
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
constexpr DWORD c_Inventory_Page_Count = 4;
#else
constexpr DWORD c_Inventory_Page_Count = 2;
#endif
constexpr DWORD c_ItemSlot_Count = c_Inventory_Page_Size * c_Inventory_Page_Count;
constexpr DWORD c_Equipment_Count = 12;

constexpr DWORD c_Equipment_Start = c_ItemSlot_Count;

constexpr DWORD c_Equipment_Body	= c_Equipment_Start + CItemData::WEAR_BODY;
constexpr DWORD c_Equipment_Head	= c_Equipment_Start + CItemData::WEAR_HEAD;
constexpr DWORD c_Equipment_Shoes	= c_Equipment_Start + CItemData::WEAR_FOOTS;
constexpr DWORD c_Equipment_Wrist	= c_Equipment_Start + CItemData::WEAR_WRIST;
constexpr DWORD c_Equipment_Weapon	= c_Equipment_Start + CItemData::WEAR_WEAPON;
constexpr DWORD c_Equipment_Neck	= c_Equipment_Start + CItemData::WEAR_NECK;
constexpr DWORD c_Equipment_Ear		= c_Equipment_Start + CItemData::WEAR_EAR;
constexpr DWORD c_Equipment_Unique1	= c_Equipment_Start + CItemData::WEAR_UNIQUE1;
constexpr DWORD c_Equipment_Unique2	= c_Equipment_Start + CItemData::WEAR_UNIQUE2;
constexpr DWORD c_Equipment_Arrow	= c_Equipment_Start + CItemData::WEAR_ARROW;
constexpr DWORD c_Equipment_Shield	= c_Equipment_Start + CItemData::WEAR_SHIELD;

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
constexpr DWORD c_New_Equipment_Start = c_Equipment_Start + CItemData::WEAR_BELT;
constexpr DWORD c_New_Equipment_Count = 1;
constexpr DWORD c_Equipment_Belt  = c_Equipment_Start + CItemData::WEAR_BELT;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
enum ESkillColorLength
{
	MAX_SKILL_COUNT = 6,
	MAX_EFFECT_COUNT = 5,
	BUFF_BEGIN = MAX_SKILL_COUNT,
#ifdef ENABLE_WOLFMAN_CHARACTER
	MAX_BUFF_COUNT = 6,
#else
	MAX_BUFF_COUNT = 5,
#endif
};
#endif

#if defined(ENABLE_CONQUEROR_LEVEL)
enum ELevelTypes
{
	LEVEL_TYPE_BASE,
	LEVEL_TYPE_CONQUEROR,
	LEVEL_TYPE_MAX
};
#endif

enum EDragonSoulDeckType
{
	DS_DECK_1,
	DS_DECK_2,
	DS_DECK_MAX_NUM = 2,
};

enum EDragonSoulGradeTypes
{
	DRAGON_SOUL_GRADE_NORMAL,
	DRAGON_SOUL_GRADE_BRILLIANT,
	DRAGON_SOUL_GRADE_RARE,
	DRAGON_SOUL_GRADE_ANCIENT,
	DRAGON_SOUL_GRADE_LEGENDARY,
	DRAGON_SOUL_GRADE_MAX,
};

enum EDragonSoulStepTypes
{
	DRAGON_SOUL_STEP_LOWEST,
	DRAGON_SOUL_STEP_LOW,
	DRAGON_SOUL_STEP_MID,
	DRAGON_SOUL_STEP_HIGH,
	DRAGON_SOUL_STEP_HIGHEST,
	DRAGON_SOUL_STEP_MAX,
};

#ifdef WJ_ENABLE_TRADABLE_ICON
enum ETopWindowTypes
{
	ON_TOP_WND_NONE,
	ON_TOP_WND_SHOP,
	ON_TOP_WND_EXCHANGE,
	ON_TOP_WND_SAFEBOX,
	ON_TOP_WND_PRIVATE_SHOP,
	ON_TOP_WND_ITEM_COMB,
	ON_TOP_WND_PET_FEED,

	ON_TOP_WND_MAX,
};
#endif

#ifdef ENABLE_COSTUME_SYSTEM
	const DWORD c_Costume_Slot_Start	= c_Equipment_Start + CItemData::WEAR_COSTUME_BODY; //180+19=199
	const DWORD	c_Costume_Slot_Body		= c_Costume_Slot_Start + CItemData::COSTUME_BODY; //199+0=199
	const DWORD	c_Costume_Slot_Hair		= c_Costume_Slot_Start + CItemData::COSTUME_HAIR; //199+1=200
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Mount	= c_Costume_Slot_Start + CItemData::COSTUME_MOUNT; //199+2=201
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Acce		= c_Costume_Slot_Start + CItemData::COSTUME_ACCE; //199+3=202
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Weapon	= c_Equipment_Start + CItemData::WEAR_COSTUME_WEAPON; // c_Costume_Slot_End + 1; //180+24=204
#endif
#ifdef ENABLE_PENDANT_SYSTEM
	constexpr DWORD c_Equipment_Pendant = c_Equipment_Start + CItemData::WEAR_PENDANT;
#endif
#ifdef ENABLE_GLOVE_SYSTEM
	constexpr DWORD c_Equipment_Glove = c_Equipment_Start + CItemData::WEAR_GLOVE;
#endif
#ifdef ENABLE_AURA_SYSTEM
	const DWORD c_Costume_Slot_Aura		= c_Equipment_Start + CItemData::WEAR_COSTUME_AURA; //199+5=205
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM) || defined(ENABLE_ACCE_COSTUME_SYSTEM)
	const DWORD c_Costume_Slot_Count	= 4;
#elif defined(ENABLE_MOUNT_COSTUME_SYSTEM)
	const DWORD c_Costume_Slot_Count	= 3;
#else
	const DWORD c_Costume_Slot_Count	= 2;
#endif

	const DWORD c_Costume_Slot_End		= c_Costume_Slot_Start + c_Costume_Slot_Count;

#endif

#ifdef ENABLE_SHINING_SYSTEM
	const DWORD c_New_Equipment_End = c_Costume_Slot_Aura+1;
	const DWORD c_Shining_Slot_Start = c_New_Equipment_End;
	const DWORD c_Shining_Slot_Count = 3;
	const DWORD c_Shining_Slot_End = c_Shining_Slot_Start + c_Shining_Slot_Count;
#endif


const DWORD c_Wear_Max = CItemData::WEAR_MAX_NUM;
const DWORD c_DragonSoul_Equip_Start = c_ItemSlot_Count + c_Wear_Max;
const DWORD c_DragonSoul_Equip_Slot_Max = 6;
const DWORD c_DragonSoul_Equip_End = c_DragonSoul_Equip_Start + c_DragonSoul_Equip_Slot_Max * DS_DECK_MAX_NUM;

const DWORD c_DragonSoul_Equip_Reserved_Count = c_DragonSoul_Equip_Slot_Max * 3;

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	const DWORD c_Belt_Inventory_Slot_Start = c_DragonSoul_Equip_End + c_DragonSoul_Equip_Reserved_Count;
	const DWORD c_Belt_Inventory_Width = 4;
	const DWORD c_Belt_Inventory_Height= 4;
	const DWORD c_Belt_Inventory_Slot_Count = c_Belt_Inventory_Width * c_Belt_Inventory_Height;
	const DWORD c_Belt_Inventory_Slot_End = c_Belt_Inventory_Slot_Start + c_Belt_Inventory_Slot_Count;

	const DWORD c_Inventory_Count	= c_Belt_Inventory_Slot_End;
#else
	const DWORD c_Inventory_Count	= c_DragonSoul_Equip_End;
#endif

const DWORD c_DragonSoul_Inventory_Start = 0;
const DWORD c_DragonSoul_Inventory_Box_Size = 32;
const DWORD c_DragonSoul_Inventory_Count = CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Box_Size;
const DWORD c_DragonSoul_Inventory_End = c_DragonSoul_Inventory_Start + c_DragonSoul_Inventory_Count;

#ifdef ENABLE_SPECIAL_STORAGE
const DWORD c_Special_Inventory_Page_Size = 5*9;
const DWORD c_Special_Inventory_Page_Count = 4;
const DWORD c_Special_ItemSlot_Count = c_Special_Inventory_Page_Size * c_Special_Inventory_Page_Count;
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM 
const DWORD c_Buff_Equipment_Slot_Start = 0;
const DWORD c_Buff_Equipment_Slot_End = c_Buff_Equipment_Slot_Start + CItemData::BUFF_WINDOW_SLOT_MAX_NUM;
#endif

enum ESlotType
{
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_SKILL,
	SLOT_TYPE_EMOTION,
	SLOT_TYPE_SHOP,
	SLOT_TYPE_EXCHANGE_OWNER,
	SLOT_TYPE_EXCHANGE_TARGET,
	SLOT_TYPE_QUICK_SLOT,
	SLOT_TYPE_SAFEBOX,
	SLOT_TYPE_PRIVATE_SHOP,
	SLOT_TYPE_MALL,
	SLOT_TYPE_DRAGON_SOUL_INVENTORY,
#ifdef ENABLE_SWITCHBOT
	SLOT_TYPE_SWITCHBOT,
#endif

#ifdef ENABLE_SPECIAL_STORAGE
	SLOT_TYPE_UPGRADE_INVENTORY,
	SLOT_TYPE_BOOK_INVENTORY,
	SLOT_TYPE_STONE_INVENTORY,
	SLOT_TYPE_CHEST_INVENTORY,
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	SLOT_TYPE_BUFF_EQUIPMENT,	// SLOT_TYPE_BUFF_EQUIPMENT
#endif

#ifdef ENABLE_AURA_SYSTEM
	SLOT_TYPE_AURA,
#endif

	SLOT_TYPE_MAX,
};

#ifdef ENABLE_AURA_SYSTEM
const BYTE c_AuraMaxLevel = 250;

enum EAuraRefineInfoSlot
{
	AURA_REFINE_INFO_SLOT_CURRENT,
	AURA_REFINE_INFO_SLOT_NEXT,
	AURA_REFINE_INFO_SLOT_EVOLVED,
	AURA_REFINE_INFO_SLOT_MAX
};

enum EAuraWindowType
{
	AURA_WINDOW_TYPE_ABSORB,
	AURA_WINDOW_TYPE_GROWTH,
	AURA_WINDOW_TYPE_EVOLVE,
	AURA_WINDOW_TYPE_MAX,
};

enum EAuraSlotType
{
	AURA_SLOT_MAIN,
	AURA_SLOT_SUB,
	AURA_SLOT_RESULT,
	AURA_SLOT_MAX
};

enum EAuraRefineInfoType
{
	AURA_REFINE_INFO_STEP,
	AURA_REFINE_INFO_LEVEL_MIN,
	AURA_REFINE_INFO_LEVEL_MAX,
	AURA_REFINE_INFO_NEED_EXP,
	AURA_REFINE_INFO_MATERIAL_VNUM,
	AURA_REFINE_INFO_MATERIAL_COUNT,
	AURA_REFINE_INFO_NEED_GOLD,
	AURA_REFINE_INFO_EVOLVE_PCT,
	AURA_REFINE_INFO_MAX
};
#endif

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,
#ifdef ENABLE_SWITCHBOT
	SWITCHBOT,
#endif
#ifdef ENABLE_SPECIAL_STORAGE
	UPGRADE_INVENTORY,
	BOOK_INVENTORY,
	STONE_INVENTORY,
	CHEST_INVENTORY,
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	BUFF_EQUIPMENT,
#endif
#ifdef ENABLE_AURA_SYSTEM
	AURA_REFINE,
#endif
	GROUND,
	WINDOW_TYPE_MAX,
};

enum EDSInventoryMaxNum
{
	DS_INVENTORY_MAX_NUM = c_DragonSoul_Inventory_Count,
	DS_REFINE_WINDOW_MAX_NUM = 15,
};

#pragma pack (push, 1)
#define WORD_MAX 0xffff

#ifdef ENABLE_SWITCHBOT
enum ESwitchbotValues
{
	SWITCHBOT_SLOT_COUNT = 5,
	SWITCHBOT_ALTERNATIVE_COUNT = 2,
	MAX_NORM_ATTR_NUM = 5,
};

enum EAttributeSet
{
	ATTRIBUTE_SET_WEAPON,
	ATTRIBUTE_SET_BODY,
	ATTRIBUTE_SET_WRIST,
	ATTRIBUTE_SET_FOOTS,
	ATTRIBUTE_SET_NECK,
	ATTRIBUTE_SET_HEAD,
	ATTRIBUTE_SET_SHIELD,
	ATTRIBUTE_SET_EAR,
	ATTRIBUTE_SET_MAX_NUM,
};
#endif

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
    SItemPos ()
    {
		window_type =     INVENTORY;
		cell = WORD_MAX;
    }
	SItemPos (BYTE _window_type, WORD _cell)
    {
        window_type = _window_type;
        cell = _cell;
    }

  //  int operator=(const int _cell)
  //  {
		//window_type = INVENTORY;
  //      cell = _cell;
  //      return cell;
  //  }
	bool IsValidCell()
	{
		switch (window_type)
		{
		case INVENTORY:
			return cell < c_Inventory_Count;
			break;
		case EQUIPMENT:
			return cell < c_DragonSoul_Equip_End;
			break;
		case DRAGON_SOUL_INVENTORY:
			return cell < (DS_INVENTORY_MAX_NUM);
			break;

#ifdef ENABLE_SWITCHBOT
		case SWITCHBOT:
			return cell < SWITCHBOT_SLOT_COUNT;
			break;
#endif

#ifdef ENABLE_SPECIAL_STORAGE
		case UPGRADE_INVENTORY:
			return cell < c_Special_ItemSlot_Count;
			break;
		case BOOK_INVENTORY:
			return cell < c_Special_ItemSlot_Count;
			break;
		case STONE_INVENTORY:
			return cell < c_Special_ItemSlot_Count;
			break;
		case CHEST_INVENTORY:
			return cell < c_Special_ItemSlot_Count;
			break;
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		case BUFF_EQUIPMENT:
			return cell < (CItemData::BUFF_WINDOW_SLOT_MAX_NUM);
			break;
#endif

		default:
			return false;
		}
	}
	bool IsEquipCell()
	{
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
			return (c_Equipment_Start + c_Wear_Max > cell) && (c_Equipment_Start <= cell);
			break;

		case BELT_INVENTORY:
		case DRAGON_SOUL_INVENTORY:
			return false;

#ifdef ENABLE_SPECIAL_STORAGE
		case UPGRADE_INVENTORY:
		case BOOK_INVENTORY:
		case STONE_INVENTORY:
		case CHEST_INVENTORY:
#endif

			break;

		default:
			return false;
		}
	}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	bool IsBeltInventoryCell()
	{
		bool bResult = c_Belt_Inventory_Slot_Start <= cell && c_Belt_Inventory_Slot_End > cell;
		return bResult;
	}
#endif

	bool IsNPOS()
	{
		return (window_type == RESERVED_WINDOW && cell == WORD_MAX);
	}

	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}

	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;

#ifdef ENABLE_EXTENDED_BATTLE_PASS
typedef struct SExtBattlePassRewardItem
{
	DWORD dwVnum;
	BYTE bCount;
} TExtBattlePassRewardItem;

typedef struct SExtBattlePassMissionInfo
{
	BYTE bMissionIndex;
	BYTE bMissionType;
	DWORD dwMissionInfo[3];
	TExtBattlePassRewardItem aRewardList[3];
} TExtBattlePassMissionInfo;

typedef struct SExtBattlePassRanking
{
	BYTE bPos;
	char playerName[24 + 1];
	DWORD dwFinishTime;
} TExtBattlePassRanking;
#endif
const TItemPos NPOS(RESERVED_WINDOW, WORD_MAX);
#pragma pack(pop)

const DWORD c_QuickBar_Line_Count = 3;
const DWORD c_QuickBar_Slot_Count = 12;

const float c_Idle_WaitTime = 5.0f;

const int c_Monster_Race_Start_Number = 6;
const int c_Monster_Model_Start_Number = 20001;

const float c_fAttack_Delay_Time = 0.2f;
const float c_fHit_Delay_Time = 0.1f;
const float c_fCrash_Wave_Time = 0.2f;
const float c_fCrash_Wave_Distance = 3.0f;

const float c_fHeight_Step_Distance = 50.0f;

enum
{
	DISTANCE_TYPE_FOUR_WAY,
	DISTANCE_TYPE_EIGHT_WAY,
	DISTANCE_TYPE_ONE_WAY,
	DISTANCE_TYPE_MAX_NUM,
};

const float c_fMagic_Script_Version = 1.0f;
const float c_fSkill_Script_Version = 1.0f;
const float c_fMagicSoundInformation_Version = 1.0f;
const float c_fBattleCommand_Script_Version = 1.0f;
const float c_fEmotionCommand_Script_Version = 1.0f;
const float c_fActive_Script_Version = 1.0f;
const float c_fPassive_Script_Version = 1.0f;

// Used by PushMove
const float c_fWalkDistance = 175.0f;
const float c_fRunDistance = 310.0f;

#define FILE_MAX_LEN 128

enum
{
#ifdef ENABLE_EXTEND_SOCKET_5
	ITEM_SOCKET_SLOT_MAX_NUM = 6,
#else
	ITEM_SOCKET_SLOT_MAX_NUM = 3,
#endif
	// refactored attribute slot begin
	ITEM_ATTRIBUTE_SLOT_NORM_NUM	= 5,
	ITEM_ATTRIBUTE_SLOT_RARE_NUM	= 2,

	ITEM_ATTRIBUTE_SLOT_NORM_START	= 0,
	ITEM_ATTRIBUTE_SLOT_NORM_END	= ITEM_ATTRIBUTE_SLOT_NORM_START + ITEM_ATTRIBUTE_SLOT_NORM_NUM,

	ITEM_ATTRIBUTE_SLOT_RARE_START	= ITEM_ATTRIBUTE_SLOT_NORM_END,
	ITEM_ATTRIBUTE_SLOT_RARE_END	= ITEM_ATTRIBUTE_SLOT_RARE_START + ITEM_ATTRIBUTE_SLOT_RARE_NUM,

	ITEM_ATTRIBUTE_SLOT_MAX_NUM		= ITEM_ATTRIBUTE_SLOT_RARE_END, // 7
	// refactored attribute slot end
};

#pragma pack(push)
#pragma pack(1)

typedef struct SQuickSlot
{
	BYTE Type;
	BYTE Position;
} TQuickSlot;

typedef struct TPlayerItemAttribute
{
    BYTE        bType;
    short       sValue;
} TPlayerItemAttribute;

typedef struct packet_item
{
    DWORD       vnum;
    uint16_t        count;
	DWORD		flags;
	DWORD		anti_flags;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TItemData;

typedef struct packet_shop_item
{
    DWORD       vnum;
    int64_t       price;
    uint16_t        count;
#ifdef ENABLE_MULTISHOP
	DWORD		wPriceVnum;
	DWORD		wPrice;
#endif
	BYTE		display_pos;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TShopItemData;

#pragma pack(pop)

inline float GetSqrtDistance(int ix1, int iy1, int ix2, int iy2) // By sqrt
{
	float dx, dy;

	dx = float(ix1 - ix2);
	dy = float(iy1 - iy2);

	return sqrtf(dx*dx + dy*dy);
}

#ifdef ENABLE_AURA_SYSTEM
int* GetAuraRefineInfo(BYTE bLevel);
#endif

// DEFAULT_FONT
void DefaultFont_Startup();
void DefaultFont_Cleanup();
void DefaultFont_SetName(const char * c_szFontName);
CResource* DefaultFont_GetResource();
CResource* DefaultItalicFont_GetResource();
// END_OF_DEFAULT_FONT

void SetGuildSymbolPath(const char * c_szPathName);
const char * GetGuildSymbolFileName(DWORD dwGuildID);
BYTE SlotTypeToInvenType(BYTE bSlotType);
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
