#pragma once

//Encargado de los patcher 
#define NEW_CHECK_VERSION 4545

#define REMOVE_MOB_COLLISION
//#define __AUTO_HUNT__										// Auto Caza

#define ENABLE_EVENT_BANNER_FLAG /// < Game event banner flags

#define __BL_KILL_BAR__

//#define WORLD_BOSS_YUMA
#define ENABLE_SELL_ITEM
#define STONE_SCALE_SYSTEM
#define QUICK_SELL_SYSTEM
//#define ENABLE_EXCHANGE_LOG
#define __ENABLE_CH3HP_PROTECTION__
#if defined(__ENABLE_CH3HP_PROTECTION__)
#define REGIST_WEBSITE_LINK "http://ddos.wox2.com:4545/"
#define REGIST_WEBSITE_LINK2 "http://ddos2.wox2.com:4545/"
#endif
//#define ENABLE_GRAPHIC_ON_OFF
#define ENABLE_DOG_MODE /// < Dog mode (all monsters with dog model)
#define BESTPRODUCTION_BOT_KONTROL_SYSTEM

#define ENABLE_EXPANDED_MONEY_TASKBAR						// Nueva ventana para ver la caida del yang
//#define ENABLE_GEM_SYSTEM									// Sistema de GAYA
#define RENEWAL_MISSION_BOOKS								// Sistema de Libros de misiones
#define ENABLE_HUNTING_SYSTEM								// Sistema de Caza
#define ENABLE_INFINITE_RAFINES								// Objetos infinitos de refinamiento
#define ENABLE_ATLAS_BOSS									// Ver los Jefes en el mapa
//#define ENABLE_EMOTICONS									// Emojis en el chat general
#define ENABLE_MULTI_LANGUAGE_SYSTEM						// Sistema de multi lenguaje 
#define ENABLE_EXTENDED_WHISPER_DETAILS						// Información de destino de susurro ampliada, creada principalmente para banderas del reino en varios idiomas, pero se puede utilizar para otra información.
#define ENABLE_MULTI_FARM_BLOCK								// Bloque Multi Farm para múltiples personajes
#define __MAINTENANCE__										// Modo mantenimiento y Loby (Dracarys)
#define ENABLE_AGGREGATE_MONSTER_EFFECT						// Efecto de Capa de valor
#define ENABLE_NPC_WEAR_ITEM								// NPC de Raza de los personajes para previsualizar equipamiento
#define ENABLE_EMOTION_HIDE_WEAPON							// Oculta las armas cuando haces una emocion
#define ENABLE_FOV_OPTION									// Vision periferica 
//#define ENABLE_FOG_FIX									// Nieblina (Bug)
#define ENABLE_DISABLE_SOFTWARE_TILING						// Deshabilitar el mosaico de software
#define ENABLE_SHADOW_RENDER_QUALITY_OPTION					// Optimiza las sombras
//#define __BL_SAVE_CAMERA_MODE__								// Gauarda la camara
#define ENABLE_SHOW_GUILD_LEADER							// Leader, CoLeader Gremios
#define BL_OFFLINE_MESSAGE									// Mensajes de fuera de linea (4 horas desaparecera)
#define ENABLE_MAP_LOCATION_APP_NAME						// Habilita el nombre del mapa en el titulo de la app
#define __BL_PICK_FILTER__									// Filtro para recoger objetos
#define ENABLE_AURA_SYSTEM									// Vestimenta de Aura
#define ENABLE_HIDE_COSTUME_SYSTEM							// Ocultar Atuendos
#define WJ_ENABLE_TRADABLE_ICON								// Efecto en los slots al comerciar 
#define METINSTONES_QUEUE									// Auto caza de metines
#define ENABLE_NEW_AUTOPOTION								// Auto Posiones
#define ENABLE_QUEST_RENEWAL								// Pagina de Quest Oficial
#define dracaryS_DUNGEON_LIB								// Librerias para las dungeons de (Dracarys)
#define ENABLE_REFINE_RENEWAL								// Mantener la ventana abierta cuando mejoras un objeto
#define ENABLE_ULTIMATE_REGEN								// Sistema de Regen (Dracarys)
#define ENABLE_TRACK_WINDOW									// Sistema de Mazmorras de (Dracarys) (Mas WorldBoss)
#define ENABLE_DEFENSAWESHIP								// La Hidra de (Dracarys)
#define ENABLE_EVENT_MANAGER								// Manager de eventos de (Dracarys)
#define ENABLE_NEW_DETAILS_GUI								// Ventana de informacion en character de (Dracarys)
#define __RENEWAL_NOTICE__									// Ventana del Notice o /n de GM en chat de (Dracarys)
#define ENABLE_AFFECT_BUFF_REMOVE							// Quitar al hacer clic en B
#define ENABLE_AFFECT_POLYMORPH_REMOVE						// Eliminar al hacer clic en Polimorfo en el icono superior izquierdo
#define ENABLE_CPP_PSM										// PlayerSettingsModule Módulo Velocidad del cliente (carga rápida)
#define LINK_IN_CHAT										// Poner un enlace en los chats https://www.google.com/
#define ENABLE_PLUS_ICON_ITEM								// Muestra el valor del artículo +0 - +200
#define ENABLE_SPECIAL_STORAGE								// Inventario Especial K (Materiales, Libros, Piedras, Cofres)
#define ENABLE_ELEMENT_ADD									// Sistema [Elementos] Oficial.
#define ENABLE_TALISMAN_EFFECT								// Efecto al equiparse un talisman
#define INGAME_WIKI											// WIKIPEDIA
#ifdef INGAME_WIKI											
//	#define INGAME_WIKI_WOLFMAN 							// Habilitar el Lobo en la wiki
#endif
#define ENABLE_ASLAN_BUFF_NPC_SYSTEM						// Sistema de chamana
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM							
	#define ASLAN_BUFF_NPC_ENABLE_EXPORT					
	#define ASLAN_BUFF_NPC_ENABLE_EXPORT_COST				
	#define ASLAN_BUFF_NPC_USE_SKILL_TECH_LEVEL				
#endif
#define ENABLE_ITEMSHOP										// ItemShop Ingame
#define ENABLE_PREMIUM_SYSTEM								// Sistema VIP
#ifdef ENABLE_PREMIUM_SYSTEM								
#define ENABLE_NO_KINGDOM_FLAG_ABOVE_PREMIUM				
#define ENABLE_PREMIUM_EFFECT_ABOVE_HEAD					
#define ENABLE_PREMIUM_PREFIX								
#endif
#define ENABLE_SEND_TARGET_INFO_EXTENDED					// Ver el % de caida de un objeto en el drop
#define ENABLE_SHOW_CHEST_DROP_SYSTEM						// Ver contenido de cofres (adaptado al inventario especial)


#define ENABLE_HWID_BAN								// 
	#ifdef ENABLE_HWID_BAN
	#	define ENABLE_MULTIFARM_BLOCK						// 
// you should enable at least 3 conponents - (cpu, gpu, mainboard_name are recommended)
//	#	define HDD_COMPONENT								// 
	#	define CPU_COMPONENT								// 
//	#	define PC_NAME_COMPONENT							// 
//	#	define BIOS_DATE_COMPONENT							// 
	#	define MAINBOARD_NAME_COMPONENT						// 
	#	define GPU_NAME_COMPONENT							// 
	#	define HWID "2A4872A8E1205070CB1C89E8194884B0EB44BE50B9F45F30D1E81FFC23D89900"
	#endif
#define ENABLE_MULTISHOP									// Comprar con items
#define ENABLE_ANTI_EXP										// Anti-Exp
#define ENABLE_CUBE_RENEWAL_WORLDARD						// Sistema de Cube renewal (Copia Bonus)
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	//#define ENABLE_CUBE_RENEWAL_GEM_WORLDARD
	#define ENABLE_CUBE_RENEWAL_COPY_WORLDARD
#endif
#define ENABLE_MESSENGER_TEAM								// Staff en la ventana de los amigos
#define ENABLE_OFFLINESHOP_SYSTEM							// Sistema de Tienda Offline (Dracarys)
#ifdef ENABLE_OFFLINESHOP_SYSTEM							// 
	#define ENABLE_SHOP_SEARCH_SYSTEM						// Sistema de Buscador de tiendas Dracarys)
#endif
#define NEW_BONUS											// Nuevos Bonus metines y jefes
#define ENABLE_WAR_MAP_PVP_TACT								// 
#define ENABLE_DEATHMATCH_SYSTEM							// 
#define ENABLE_EXPRESSING_EMOTION							// 
#define ENABLE_BUTTON_EVENTS								// 
#define __ENABLE_SHAMAN_ATTACK_FIX__						// 




#define ENABLE_DUNGEON_EVENT_TIMER							// 
#define ENABLE_DUNGEON_COOL_TIME							// 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// SISTEMAS DE YOHARA //////////////////////////////////////////////////////////////////////////////////////
#define ENABLE_CONQUEROR_LEVEL									// Sistema [Yohara] Oficial.
#define ENABLE_9THSKILL											// Sistema [Yohara - 9ª Habilidad -] Oficial.
#define ENABLE_9TH_SKILL											// Sistema [Yohara - 9ª Habilidad -] Oficial.
#define ENABLE_IMPROVED_LOGOUT_POINTS							// Sistema [Puntos Yohara] Oficial.
//#define ENABLE_LIMIT_LEVEL_YOHARA								// Sistema [Yohara - Nivel -] Oficial.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







//////////FIX//////////
#define FIX_TEXTURES_BLURRING_OUT
#define LIMIT_WINDOWS_RECT //Enable limit rect movable windows 


//////////////////////////////////////////////////////////////////////////
// ### Default Ymir Macros ###
#define LOCALE_SERVICE_EUROPE
#define ENABLE_COSTUME_SYSTEM
#define ENABLE_ENERGY_SYSTEM
//#define ENABLE_DRAGON_SOUL_SYSTEM
#define ENABLE_NEW_EQUIPMENT_SYSTEM
// ### Default Ymir Macros ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New From LocaleInc ###
#define ENABLE_PACK_GET_CHECK
#define ENABLE_CANSEEHIDDENTHING_FOR_GM
#define ENABLE_PROTOSTRUCT_AUTODETECT
#define ENABLE_PLAYER_PER_ACCOUNT5
#define ENABLE_LEVEL_IN_TRADE
#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_LVL115_ARMOR_EFFECT
#define ENABLE_SLOT_WINDOW_EX
#define ENABLE_TEXT_LEVEL_REFRESH
#define ENABLE_USE_COSTUME_ATTR
#define ENABLE_DISCORD_RPC
#define ENABLE_PET_SYSTEM_EX
#define ENABLE_LOCALE_EX
#define ENABLE_NO_DSS_QUALIFICATION
//#define ENABLE_NO_SELL_PRICE_DIVIDED_BY_5
#define ENABLE_PENDANT_SYSTEM
#define ENABLE_GLOVE_SYSTEM
//#define ENABLE_MOVE_CHANNEL
#define ENABLE_QUIVER_SYSTEM
#define ENABLE_4TH_AFF_SKILL_DESC
#define ENABLE_LOCALE_COMMON

#define WJ_SHOW_MOB_INFO
#ifdef WJ_SHOW_MOB_INFO
#define ENABLE_SHOW_MOBAIFLAG
#define ENABLE_SHOW_MOBLEVEL
#endif

// ### New From LocaleInc ###
//////////////////////////////////////////////////////////////////////////

#define ENABLE_SWITCHBOT
#define ENABLE_BIYOLOG
#define ENABLE_VIEW_TARGET_PLAYER_HP
#define ENABLE_VIEW_TARGET_DECIMAL_HP
#define ENABLE_SHINING_SYSTEM // Shining effects

#define ENABLE_RENDER_TARGET
#define ENABLE_SEND_TARGET_INFO
#define ENABLE_TITLE_SYSTEM
#define ENABLE_CHANNEL_SWITCHER
#define ENABLE_SKILL_COLOR_SYSTEM // Skill color
#define ENABLE_CONFIG_MODULE // Enable configuration module for saving settings
#define ENABLE_EXTENDED_BATTLE_PASS

//////////////////////////////////////////////////////////////////////////
// ### From GameLib ###
#define ENABLE_WOLFMAN_CHARACTER
#ifdef ENABLE_WOLFMAN_CHARACTER
#define DISABLE_WOLFMAN_ON_CREATE
#endif
// #define ENABLE_MAGIC_REDUCTION_SYSTEM
#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_WEAPON_COSTUME_SYSTEM
// ### From GameLib ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New System Defines - Extended Version ###

// if is define ENABLE_ACCE_COSTUME_SYSTEM the players can use shoulder sash
// if you want to use object scaling function you must defined ENABLE_OBJ_SCALLING
#define ENABLE_ACCE_COSTUME_SYSTEM
#define ENABLE_OBJ_SCALLING
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS

// if you want use SetMouseWheelScrollEvent or you want use mouse wheel to move the scrollbar
#define ENABLE_MOUSEWHEEL_EVENT

//if you want to see highlighted a new item when dropped or when exchanged
#define ENABLE_HIGHLIGHT_NEW_ITEM

// it shows emojis in the textlines
#define ENABLE_EMOJI_SYSTEM

// effects while hidden won't show up
#define __ENABLE_STEALTH_FIX__

// circle dots in minimap instead of squares
#define ENABLE_MINIMAP_WHITEMARK_CIRCLE

// for debug: print received packets
// #define ENABLE_PRINT_RECV_PACKET_DEBUG

// ### New System Defines - Extended Version ###

//////////////////////////////////////////////////////////////////////////
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe

//News fixme
//@fixme1000; UserInterface;InstanceBaseEffect.cpp	quitar bandera de reino en NPCs
//@fixme1001; MilesLib;SoundManager.cpp	fix no hay sonido luego de teletransportarse
//@fixme1003; GameLib;ActorInstanceBattle.cpp	shake stone damage
//@fixme1004; GameLib;ActorInstanceBattle.cpp	return in m_wcurMotionMode == 9
//@fixme1005; PRTerrainLib;TextureSet.cpp	return !pResource
//@fixme1006; UserInterface;PythonNetworkStreamPhaseGameActor.cpp	update part in select phase
//@fixme1007; UserInterface;PythonSkill.cpp	el valor del cooltime en el tooltip de las habilidades ahora se actualiza con la velocidad de hechizo
//@mod1000; EterPythonLib;PythonGraphic.cpp	85% screenshot jpg quality to 100%
//@mod1001; UserInterface;PythonChat.cpp	GM whisper default font

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* JTX | MR WOLF  --->  WORK START
⠀⠀⠀⠀⠀⣀⣠⣤⣤⣤⣤⣄⣀⠀⠀⠀⠀⠀
⠀⠀⢀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⡀⠀⠀                   _  __
⠀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⢿⣿⣷⡀⠀		            | |/ _|
⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⠀⣴⢿⣿⣧⠀	  __      _____ | | |_
⣿⣿⣿⣿⣿⡿⠛⣩⠍⠀⠀⠀⠐⠉⢠⣿⣿⡇    \ \ /\ / / _ \| |  _|
⣿⡿⠛⠋⠉⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿     \ V  V / (_) | | |
⢹⣿⣤⠄⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⡏      \_/\_/ \___/|_|_|
⠀⠻⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⣿⣿⣿⠟⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⠟⠁⠀⠀⠀
*/

#define DIRECTX9													// Select DX9

#ifdef DIRECTX9
#define D3DLIGHT8					D3DLIGHT9
#define D3DMATERIAL8				D3DMATERIAL9
#define IDirect3DVertexBuffer8		IDirect3DVertexBuffer9
#define LPDIRECT3DTEXTURE8			LPDIRECT3DTEXTURE9
#define LPDIRECT3DSURFACE8			LPDIRECT3DSURFACE9
#define D3DVIEWPORT8				D3DVIEWPORT9
#define LPDIRECT3DDEVICE8			LPDIRECT3DDEVICE9
#define LPDIRECT3DVERTEXBUFFER8		LPDIRECT3DVERTEXBUFFER9
#define LPDIRECT3DINDEXBUFFER8		LPDIRECT3DINDEXBUFFER9
#define DXLOCKTYPE					(void**)
#define D3DVERTEXELEMENT8			D3DVERTEXELEMENT9
#define LPDIRECT3DBASETEXTURE8		LPDIRECT3DBASETEXTURE9
#define LPDIRECT3DPIXELSHADER8		LPDIRECT3DPIXELSHADER9
#define D3DADAPTER_IDENTIFIER8		D3DADAPTER_IDENTIFIER9
#define IDirect3D8					IDirect3D9
#define IDirect3DDevice8			IDirect3DDevice9
#define D3DCAPS8					D3DCAPS9
#define LPDIRECT3D8					LPDIRECT3D9
#define D3DCAPS2_CANRENDERWINDOWED	DDCAPS2_CANRENDERWINDOWED
#define IDirect3DTexture8			IDirect3DTexture9
#define Direct3DCreate8				Direct3DCreate9
#define IDirect3DSurface8			IDirect3DSurface9
#else
#define DXLOCKTYPE					(BYTE**)
#endif

#define JETTYX_TRANSPARENCY_BUILDING_FIX
#define JTX_GPU_LEAF_PLACEMENT
#define JTX_GPU_TREE_WIND
#define JTX_TREE_DYNAMIC_LIGHT
#define JETTYX_TRANSPARENCY_BUILDING_FIX

// JTX | MR WOLF  --->  WORK END
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
