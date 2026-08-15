#include "Inc.h"

#include "LScript.h"
#include "CDetour.h"
#include "LIN2.2.1Specific.h"

HMODULE dCore = NULL;
HMODULE dEngine = NULL;
HMODULE dNWindow = NULL;
extern User * PlayerUser;

CDetour UInteractionMasterProcessPostRenderDetour;
void __cdecl DetouredUInteractionMasterProcessPostRender(UObject * pCanvas)
{
	void * pThis;
	__asm mov pThis,ecx;

	__asm pushad
	//Log("UInteractionMaster::MasterProcessPostRender");
	//Log("-------------------------------------------");
	//Log("pThis        0x%08x", pThis);
	//Log("pCanvas      0x%08x", pCanvas);
	//Log("Called From  0x%08x", UInteractionMasterProcessPostRenderDetour.GetRetAddress());

	//Call the original post render (done automatically by CDetour unless specified otherwise)
	//Call Lua PostRender
	PostRender(pCanvas);
	__asm popad
}

CDetour UInteractionMasterProcessPreRenderDetour;
void __cdecl DetouredUInteractionMasterProcessPreRender(UObject * pCanvas)
{
	void * pThis;
	__asm mov pThis, ecx;

	__asm pushad
	//Log("UInteractionMaster::MasterProcessPreRender");
	//Log("------------------------------------------");
	//Log("pThis        0x%08x", pThis);
	//Log("pCanvas      0x%08x", pCanvas);
	//Log("Called From  0x%08x", UInteractionMasterProcessPreRenderDetour.GetRetAddress());

	//Call Lua PreRender
	PreRender(pCanvas);
	//Call the original Pre render (done automatically by CDetour unless specified otherwise)
	__asm popad
}

CDetour UGameEngineTickDetour;
void __cdecl DetouredUGameEngineTick(float DeltaTime)
{
	void * pThis;
	__asm mov pThis,ecx;

	__asm pushad

	//Log("GameEngine::Tick");
	//Log("----------------");
	//Log("pThis        0x%08x", pThis);
	//Log("DeltaTime    %f", DeltaTime);
	//Log("Called From  0x%08x", UGameEngineTickDetour.GetRetAddress());

	//Call Lua Tick
	Tick(DeltaTime);
	__asm popad
}

CDetour UInteractionMasterProcessKeyEventDetour;
int __cdecl DetouredUInteractionMasterProcessKeyEvent(DWORD Key, DWORD Action, float Value)
{
	void * pThis;
	__asm mov pThis,ecx;

	__asm pushad
	/*
	Log("UInteractionMaster::MasterProcessKeyEvent");
	Log("-----------------------------------------");
	Log("pUInteractionMaster 0x%08x", pThis);
	Log("Key                 %ld", Key);
	Log("Action              %ld", Action);
	Log("Value               %f", Value);
	Log("Called From         0x%08x", UInteractionMasterProcessKeyEventDetour.GetRetAddress());
	*/
	int    Result;
	//Call Lua KeyEvent
	Result = KeyEvent(Key, Action, Value);
	//Call the original 
	if (Result) {UInteractionMasterProcessKeyEventDetour.Ret(false);}
	__asm popad
	return(Result);
};

void DelayedHookEngine(void);
CDetour UserUserDetour;
void __cdecl DetouredUserUser()
{
	void * pThis;
	__asm mov pThis,ecx;

	__asm pushad
	
	//Log("User::User");
	//Log("----------");
	//Log("pUser       0x%08x", pThis);
	//Log("Called From 0x%08x = %x", UserUserDetour.GetRetAddress(), UserUserDetour.GetRetAddress()-(DWORD)dEngine);
	//DumpUser((User *)pThis); -- All 0's

	/*
	IL
	dEngine + 1332C5 Player
	dEngine + 139BA2 NPCs
	dEngine + 35787B Junk
    CT1
	24959d5 1659D5
	249dc1b 16DC1B
	26d18f2 3A18F2

	CT1.5
	=====
10:39:53 HMODULE dEngine  = 0x00420000
10:39:53 User::User detour 00482260
10:46:59 pUser       0x212a3800
10:46:59 Called From 0x007b684a = 39684A
10:41:45 pUser       0x1ea0b800
10:41:45 Called From 0x007b5ccc = 395CCC
10:41:46 pUser       0x00123dbc
10:41:46 Called From 0x0061c698 = 1FC698

	CT2
	===
11:02:33 HMODULE dEngine  = 0x00280000
11:02:33 User::User detour 002d74e0
11:03:11 pUser       0x1683d400
11:03:11 Called From 0x0061a64a = 39a64a
11:03:11 pUser       0x167a0c00
11:03:11 Called From 0x00619acc = 399acc
11:04:10 pUser       0x00123f28
11:04:10 Called From 0x0049957e = 21957e
*/

/* May 2009
23:11:50 System: Caching Property[SetDrawColor] Class[Canvas] via Object
23:11:50 System: Caching Property[DrawText] Class[Canvas] via Object
23:12:24 pUser       0x0012efa4
23:12:24 Called From 0x006123fa = 3923fa
23:12:59 pUser       0x0ced8c00
23:12:59 Called From 0x08ad19c7 = 88519c7
23:13:00 pUser       0x0012f048
23:13:00 Called From 0x00617062 = 397062
23:13:26 pUser       0x11f73800
23:13:26 Called From 0x0061bbda = 39bbda
23:13:30 pUser       0x11f72800
23:13:30 Called From 0x0061bbda = 39bbda
23:13:31 pUser       0x11f72400
23:13:31 Called From 0x0061b05c = 39b05c
23:13:35 pUser       0x0c020000
23:13:35 Called From 0x0061b05c = 39b05c
23:13:35 pUser       0x0c2cfc00
23:13:35 Called From 0x0061b05c = 39b05c
23:13:35 pUser       0x0c2cf800
23:13:35 Called From 0x0061b05c = 39b05c
23:13:48 pUser       0x100ec000
*/

	//if (UserUserDetour.GetRetAddress() == ((DWORD)dEngine + 0x1332C5)) //	CT1 3 Feb 2008
	//if (UserUserDetour.GetRetAddress() == ((DWORD)dEngine + 0x1659D5))
	//if (UserUserDetour.GetRetAddress() == ((DWORD)dEngine + 0x39684A)) // CT 1.5
	//if (UserUserDetour.GetRetAddress() == ((DWORD)dEngine + 0x39a64a)) // CT 2 31 Aug 2008
	//if (UserUserDetour.GetRetAddress() == ((DWORD)dEngine + 0x39bbda)) // Gracia pt 2 May 2009
    if (UserUserDetour.GetRetAddress() == ((DWORD)dEngine + 0x3C070A)) // Gracia Final Revision 83
    {
		PlayerUser = (User *)pThis;
		Log("System: PlayerUser [0x%08x]", pThis);
	}

	DelayedHookEngine();

	__asm popad
}


extern void * pUGameEngine;

class FString
{
public:
	TCHAR * pArray;
	DWORD  Count;
	DWORD  Max;
};

#include "FIFOBuffer.h"
class FIFO TextBuffer;

CDetour UUIScripteventOnEventDetour;
void __cdecl DetouredUUIScripteventOnEvent(int a_EventID, class FString const &a_Param)
{
	void * pThis;
	__asm mov pThis,ecx;

	__asm pushad
	
	//Log("UUIScript::eventOnEvent");
	//Log("-----------------------");
	//Log("pUGameEngine    0x%08x", pThis);
	//Log("a_Param.pArray  %ws", a_Param.pArray);
	//Log("a_Param.Count   0x%08x", a_Param.Count);
	//Log("a_Param.Max     0x%08x", a_Param.Max);
	//Log("Called From     0x%08x", UUIScripteventOnEventDetour.GetRetAddress());

	//Log("UUIScript::eventOnEvent(%ld, '%ws') from 0x%08x", a_EventID, a_Param.pArray, UUIScripteventOnEventDetour.GetRetAddress());

	if ((a_EventID != 190)	// UUIScript::eventOnEvent(190, 'ServerID=268500804 CurrentHP=2604') from 0x07590bd7
	&& (a_EventID != 150)	// UUIScript::eventOnEvent(150, '(null)') from 0x07590bd7
	&& (a_EventID != 160)	// UUIScript::eventOnEvent(160, '(null)') from 0x07580bd7
	&& (a_EventID != 180)	// UUIScript::eventOnEvent(180, '(null)') from 0x07590bd7
	&& (a_EventID != 200)	// UUIScript::eventOnEvent(200, 'ServerID=268673083 MaxHP=1338') from 0x07590bd7
	&& (a_EventID != 210)	// UUIScript::eventOnEvent(210, 'ServerID=268500804 CurrentMP=707') from 0x07590bd7
	&& (a_EventID != 230)	// UUIScript::eventOnEvent(230, 'ServerID=268500804 CurrentCP=2091') from 0x07590bd7
	&& (a_EventID != 240)	// UUIScript::eventOnEvent(240, 'ServerID=268500804 MaxCP=2091') from 0x07590bd7
	&& (a_EventID != 550)	// UUIScript::eventOnEvent(550, '(null)') from 0x07590bd7
	&& (a_EventID != 830)	// UUIScript::eventOnEvent(830, 'RecipeID=143') from 0x07580bd7
	&& (a_EventID != 980)	// UUIScript::eventOnEvent(980, '(null)') from 0x07590bd7
	&& (a_EventID != 1710)	// UUIScript::eventOnEvent(1710, '(null)') from 0x07580bd7
	&& (a_EventID != 1830)	// UUIScript::eventOnEvent(1830, '(null)') from 0x07580bd7
	&& (a_EventID != 1840)	// UUIScript::eventOnEvent(1840, '(null)') from 0x07580bd7
	&& (a_EventID != 1870)	// UUIScript::eventOnEvent(1870, '(null)') from 0x07580bd7
	&& (a_EventID != 2920)	// UUIScript::eventOnEvent(2920, 'SourceType=0 TooltipType=Text Text=Noble Ant') from 0x07590bd7
	) {
		if (a_EventID == 540) {
			//HandleChatmessage(param))
			//Log("HandleChatmessage('%ws') from 0x%08x", a_Param.pArray, UUIScripteventOnEventDetour.GetRetAddress());

			TextBuffer.queuew(a_Param.pArray);
		} else {
/*
2600 Initialise Inventory
10:07:28 UUIScript::eventOnEvent(2600, 'classID=736 name=Scroll of Escape iconName=icon.etc_scroll_of_return_i00 description=A scroll of enchantment that relocates you to the nearest village. itemType=5 serverID=268685458 itemNum=20 slotBitType=0 enchanted=0 blessed=0 damaged=0 equipped=0 price=0 reserved=-1 defaultPrice=0 refineryOp1=0 refineryOp2=0 currentDurability=-1 weight=120 materialType=18 durability=0 crystalType=0 consumeType=2 ItemSubType=1') from 0x07560bd7
2610 Update Inventory
10:07:30 UUIScript::eventOnEvent(2610, 'type=update classID=8830 name=Ballistics: Katana iconName=icon.time_weapon_katana_i00 description=This Shadow Weapon contains the mirrored power of a Katana. It cannot be traded, dropped or given other functions. itemType=0 serverID=268533893 itemNum=1 slotBitType=128 enchanted=0 blessed=0 damaged=0 equipped=1 price=0 reserved=-1 defaultPrice=0 refineryOp1=0 refineryOp2=0 currentDurability=183 weight=480 materialType=51 weaponType=1 physicalDamage=122 magicalDamage=68 shieldDefense=0 shieldDefenseRate=0 durability=300 crystalType=2 randomDamage=10 critical=8 hitModify=0 attackSpeed=379 mpConsume=0 avoidModify=0 soulshotCount=2 spiritshotCount=2 ItemSubType=1') from 0x07560bd7
2110 Freight List
10:07:46 UUIScript::eventOnEvent(2110, 'classID=5183 name=Hatchling's Ophidian Plate iconName=icon.etc_pet_armor_i05 description=Can be worn by double-clicking. Exclusively used by a hatchling. itemType=7 serverID=0 itemNum=1 slotBitType=1024 enchanted=0 blessed=0 damaged=0 equipped=0 price=0 reserved=268672985 defaultPrice=0 refineryOp1=0 refineryOp2=0 currentDurability=-1 weight=160 materialType=19 durability=-1 crystalType=0 armorType=1 avoidModify=0 physicalDefense=46 magicalDefense=46 mpBonus=0 ItemSubType=1') from 0x07560bd7
2110 Warehoue list
10:08:30 UUIScript::eventOnEvent(2110, 'classID=7808 name=Purple Colored Lure - For Beginners iconName=icon.etc_bait_i01 description=A lure used by beginners. Since only the fish intended for beginners take the bait, a player can easily learn how to fish. This bait is preferred by fat fish. itemType=5 serverID=0 itemNum=19 slotBitType=256 enchanted=0 blessed=0 damaged=0 equipped=0 price=0 reserved=268730242 defaultPrice=0 refineryOp1=0 refineryOp2=0 currentDurability=-1 weight=5 materialType=53 durability=0 crystalType=0 consumeType=2 ItemSubType=16 arrow=1') from 0x07560bd7
2100 Adena
10:08:39 UUIScript::eventOnEvent(2100, 'category=1 type=deposit adena=2991330') from 0x07560bd7

11:56:14 UUIScript::eventOnEvent(2610, 'type=delete classID=5969 name=Ancient White Papyrus iconName=icon.etc_paper_white_i00 description=A papyrus scroll found in the Tower of Insolence. If you double-click it, you can read its contents. itemType=5 serverID=268690144 itemNum=0 slotBitType=0 enchanted=0 blessed=0 damaged=0 equipped=0 price=0 reserved=-1 defaultPrice=0 refineryOp1=0 refineryOp2=0 currentDurability=-1 weight=0 materialType=53 durability=0 crystalType=0 consumeType=2 ItemSubType=0') from 0x07560bd7
11:56:14 UUIScript::eventOnEvent(2610, 'type=delete classID=5969 name=Ancient White Papyrus iconName=icon.etc_paper_white_i00 description=A papyrus scroll found in the Tower of Insolence. If you double-click it, you can read its contents. itemType=5 serverID=268690144 itemNum=0 slotBitType=0 enchanted=0 blessed=0 damaged=0 equipped=0 price=0 reserved=-1 defaultPrice=0 refineryOp1=0 refineryOp2=0 currentDurability=-1 weight=0 materialType=53 durability=0 crystalType=0 consumeType=2 ItemSubType=0') from 0x07560bd7
11:58:46 UUIScript::eventOnEvent(2610, 'type=add classID=5550 name=Durable Metal Plate iconName=icon.etc_squares_wood_i00 description=Material used by a Dwarf to make an item. It can be sold at a regular store. itemType=5 serverID=268619975 itemNum=188 slotBitType=0 enchanted=0 blessed=0 damaged=0 equipped=0 price=0 reserved=-1 defaultPrice=0 refineryOp1=0 refineryOp2=0 currentDurability=-1 weight=2 materialType=53 durability=0 crystalType=0 consumeType=2 ItemSubType=6') from 0x07560bd7
11:58:46 UUIScript::eventOnEvent(2610, 'type=add classID=2132 name=Gemstone B iconName=icon.etc_bead_green_i00 itemType=5 serverID=268624747 itemNum=357 slotBitType=0 enchanted=0 blessed=0 damaged=0 equipped=0 price=0 reserved=-1 defaultPrice=0 refineryOp1=0 refineryOp2=0 currentDurability=-1 weight=2 materialType=53 durability=0 crystalType=0 consumeType=2 ItemSubType=6') from 0x07560bd7
20:03:18 UUIScript::eventOnEvent(2610, 'type=update classID=6645 name=Beast Soulshot iconName=icon.etc_beast_soul_shot_i00 description=The light of a spirit is bestowed upon a pet/servitor and temporarily increases the power of attack. The number of soulshots consumption can be checked in the Pet/Servitor Info window. (Used in the master's inventory.) itemType=5 serverID=268518801 itemNum=29 slotBitType=0 enchanted=0 blessed=0 damaged=0 equipped=0 price=0 reserved=-1 defaultPrice=0 refineryOp1=0 refineryOp2=0 currentDurability=-1 weight=1 materialType=18 durability=0 crystalType=0 consumeType=2 ItemSubType=0') from 0x07560bd7
20:03:18 UUIScript::eventOnEvent(2610, 'type=update classID=1466 name=Soulshot: A-grade iconName=icon.etc_spirit_bullet_silver_i00 description=The power of a higher-level spirit is bestowed upon a weapon, temporarily increasing power of attack. Used with an A-grade weapon. itemType=5 serverID=268597587 itemNum=5091 slotBitType=0 enchanted=0 blessed=0 damaged=0 equipped=0 price=0 reserved=-1 defaultPrice=0 refineryOp1=0 refineryOp2=0 currentDurability=-1 weight=2 materialType=18 durability=0 crystalType=4 consumeType=2 ItemSubType=0') from 0x07560bd7

*/
			if ((a_EventID == 2600)
			|| (a_EventID == 2610)
			|| (a_EventID == 2100)
			|| (a_EventID == 2110)
			|| (a_EventID == 2090))
			{
				//Log("UUIScript::eventOnEvent(%ld, '%ws') from 0x%08x", a_EventID, a_Param.pArray, UUIScripteventOnEventDetour.GetRetAddress());
			}

			if ((a_EventID == 2530) || (a_EventID == 2540)
			|| (a_EventID == 2550) || (a_EventID == 2560))
			{
				Log("  MultiSellWnd.OnEvent(%ld, \"%ws\")", a_EventID, a_Param.pArray);
			}
		}
	}
	
	__asm popad
}

void DelayedHookEngine(void)
{
	DWORD OldCallFunction;
	bool bStatus;

	if (!dNWindow) {
		dNWindow = GetModuleHandleA("nwindow.dll");
		if (dNWindow) {
			Log("HMODULE dNWindow = 0x%08x", dNWindow);

			// This is a simple hook where the export is not a JMP
			OldCallFunction = (DWORD)GetProcAddress(dNWindow, "?eventOnEvent@UUIScript@@QAEXHABVFString@@@Z");
			Log("UUIScript::eventOnEvent detour %08x", OldCallFunction);
			if (OldCallFunction)
			{
				bStatus = UUIScripteventOnEventDetour.Detour((DWORD)OldCallFunction, (DWORD)DetouredUUIScripteventOnEvent, 2, true);
				Log("UUIScript::eventOnEvent detour %s", bStatus ? "constructed" : "construction failed");
				bStatus = UUIScripteventOnEventDetour.Apply();
				Log("UUIScript::eventOnEvent detour %s", bStatus ? "applied" : "not applied");
			}
		}
	}
}

FLandMarkAddLandMark_typedef pReal_FLandMarkAddLandMark = NULL;
UNetworkHandlerMTL_typedef pReal_UNetworkHandlerMTL = NULL;

void HookEngine(void)
{
	DWORD OldCallFunction;
	bool bStatus;
	dCore = GetModuleHandleA("Core.dll");
	dEngine = GetModuleHandleA("Engine.dll");

	Log("HMODULE dCore    = 0x%08x", dCore);
	Log("HMODULE dEngine  = 0x%08x", dEngine);

	pReal_UNetworkHandlerMTL = (UNetworkHandlerMTL_typedef)CDetour::GetProcAddress(dEngine, "?MTL@UNetworkHandler@@UAEXPAVAActor@@VFVector@@10H@Z");

	OldCallFunction = (DWORD)GetProcAddress(dEngine, "??0User@@QAE@XZ");
	Log("User::User detour %08x", OldCallFunction);
	if (OldCallFunction)
	{
		bStatus = UserUserDetour.Detour((DWORD)OldCallFunction, (DWORD)DetouredUserUser, 0, true);
		Log("User::User detour %s", bStatus ? "constructed" : "construction failed");
		bStatus = UserUserDetour.Apply();
		Log("User::User detour %s", bStatus ? "applied" : "not applied");
	}

	OldCallFunction = (DWORD)CDetour::GetProcAddress(dEngine, "?Tick@UGameEngine@@UAEXM@Z");
	Log("UGameEngine::Tick detour %08x", OldCallFunction);
	if (OldCallFunction)
	{
		bStatus = UGameEngineTickDetour.Detour((DWORD)OldCallFunction, (DWORD)DetouredUGameEngineTick, 1, true);
		Log("UGameEngine::Tick detour %s", bStatus ? "constructed" : "construction failed");
		bStatus = UGameEngineTickDetour.Apply();
		Log("UGameEngine::Tick detour %s", bStatus ? "applied" : "not applied");
	}

	OldCallFunction = (DWORD)GetProcAddress(dEngine, "?MasterProcessKeyEvent@UInteractionMaster@@QAEHW4EInputKey@@W4EInputAction@@M@Z");
	Log("UInteractionMaster::MasterProcessKeyEvent detour %08x", OldCallFunction);
	if (OldCallFunction)
	{
		bStatus = UInteractionMasterProcessKeyEventDetour.Detour((DWORD)OldCallFunction, (DWORD)DetouredUInteractionMasterProcessKeyEvent, 3, true);
		Log("UInteractionMaster::MasterProcessKeyEvent detour %s", bStatus ? "constructed" : "construction failed");
		bStatus = UInteractionMasterProcessKeyEventDetour.Apply();
		Log("UInteractionMaster::MasterProcessKeyEvent detour %s", bStatus ? "applied" : "not applied");
	}

	OldCallFunction = (DWORD)GetProcAddress(dEngine, "?MasterProcessPostRender@UInteractionMaster@@QAEXPAVUCanvas@@@Z");
	Log("UInteractionMaster::MasterProcessPostRender detour %08x", OldCallFunction);
	if (OldCallFunction)
	{
		bStatus = UInteractionMasterProcessPostRenderDetour.Detour((DWORD)OldCallFunction, (DWORD)DetouredUInteractionMasterProcessPostRender, 1, true);
		Log("UInteractionMaster::MasterProcessPostRender detour %s", bStatus ? "constructed" : "construction failed");
		bStatus = UInteractionMasterProcessPostRenderDetour.Apply();
		Log("UInteractionMaster::MasterProcessPostRender detour %s", bStatus ? "applied" : "not applied");
	}

	OldCallFunction = (DWORD)GetProcAddress(dEngine, "?MasterProcessPreRender@UInteractionMaster@@QAEXPAVUCanvas@@@Z");
	Log("UInteractionMaster::MasterProcessPreRender detour %08x", OldCallFunction);
	if (OldCallFunction)
	{
		bStatus = UInteractionMasterProcessPreRenderDetour.Detour((DWORD)OldCallFunction, (DWORD)DetouredUInteractionMasterProcessPreRender, 1, true);
		Log("UInteractionMaster::MasterProcessPreRender detour %s", bStatus ? "constructed" : "construction failed");
		bStatus = UInteractionMasterProcessPreRenderDetour.Apply();
		Log("UInteractionMaster::MasterProcessPreRender detour %s", bStatus ? "applied" : "not applied");
	}

}