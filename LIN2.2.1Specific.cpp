#include "Inc.h"

#include "LIN2.2.1Specific.h"

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

User * PlayerUser;
extern FLandMarkAddLandMark_typedef pReal_FLandMarkAddLandMark;
extern UNetworkHandlerMTL_typedef pReal_UNetworkHandlerMTL;
extern void Log(char * pFormat, ...);
void * pUGameEngine = NULL;

int MoveToLocation(lua_State *L) {
	void ** ppPawn = NULL;
	FVector To;
	To.X = To.Y = To.Z = 0.0;
	FVector From;
	From.X = From.Y = From.Z = 0.0;
	void ** ppTerrain = NULL;
	int Ordinal = 0;
	void ** ppUNetworkHandler = NULL;
	if (pReal_UNetworkHandlerMTL != NULL) {
		ppPawn = (void **) lua_touserdata(L, 1); 
		if (lua_isnumber(L, 2))	{
			To.X = (float)lua_tonumber(L, 2);
		}
		if (lua_isnumber(L, 3))	{
			To.Y = (float)lua_tonumber(L, 3);
		}
		if (lua_isnumber(L, 4))	{
			To.Z = (float)lua_tonumber(L, 4);
		}
		if (lua_isnumber(L, 5))	{
			From.X = (float)lua_tonumber(L, 5);
		}
		if (lua_isnumber(L, 6))	{
			From.Y = (float)lua_tonumber(L, 6);
		}
		if (lua_isnumber(L, 7))	{
			From.Z = (float)lua_tonumber(L, 7);
		}
		ppTerrain = (void **) lua_touserdata(L, 8); 
		ppUNetworkHandler = (void **) lua_touserdata(L, 9); 
	    DWORD * pPawn = (DWORD *)*ppPawn;
	    DWORD * pTerrain = (DWORD *)*ppTerrain;
	    DWORD * pUNetworkHandler = (DWORD *)*ppUNetworkHandler;
		Log("UNetworkHandler::MTL(0x%08x, To(%f, %f, %f), From(%f, %f, %f), 0x%08x, %ld, 0x%08x"
		, pPawn, To.X, To.Y, To.Z, From.X, From.Y, From.Z, pTerrain, Ordinal, pUNetworkHandler);
		if (pUNetworkHandler != NULL && pPawn != NULL && pTerrain != NULL)
		{
			__asm mov  eax, Ordinal;
			__asm push eax;
			__asm mov  eax, pTerrain;
			__asm push eax;
			__asm mov  eax, From.Z;
			__asm push eax;
			__asm mov  eax, From.Y;
			__asm push eax;
			__asm mov  eax, From.X;
			__asm push eax;
			__asm mov  eax, To.Z;
			__asm push eax;
			__asm mov  eax, To.Y;
			__asm push eax;
			__asm mov  eax, To.X;
			__asm push eax;
			__asm mov  eax, pPawn;
			__asm push eax;
			__asm mov ecx, pUNetworkHandler;
			__asm call pReal_UNetworkHandlerMTL;

		}
	}
  	return 1;
}

int FLandMarkAddLandMark (lua_State *L) {
	FVector Vector;
	Vector.X = Vector.Y = Vector.Z = 0.0;
	int Ordinal = 0;
	if (pReal_FLandMarkAddLandMark != NULL) {
		if (lua_isnumber(L, 1))	{
			Vector.X = (float)lua_tonumber(L, 1);
		}
		if (lua_isnumber(L, 2))	{
			Vector.Y = (float)lua_tonumber(L, 2);
		}
		if (lua_isnumber(L, 3))	{
			Vector.Z = (float)lua_tonumber(L, 3);
		}
		Log("FLandMarkAddLandMark: Vector(%f, %f, %f) %ld %ld", Vector.X, Vector.Y, Vector.Z, Ordinal, pUGameEngine);
		if (Vector.X != 0.0 && Vector.Y != 0.0 && Vector.Z != 0.0 && pUGameEngine != NULL)
		{
			__asm xor  eax,eax;
			__asm push eax;
			__asm mov  eax,Vector.Z;
			__asm push eax;
			__asm mov  eax,Vector.Y;
			__asm push eax;
			__asm mov  eax,Vector.X;
			__asm push eax;
			__asm mov  eax,Vector.Z;
			__asm push eax;
			__asm mov  eax,Vector.Y;
			__asm push eax;
			__asm mov  eax,Vector.X;
			__asm push eax;
			__asm mov ecx, pUGameEngine;
			__asm call pReal_FLandMarkAddLandMark;

		}
	}
  	return 1;
}

int PlayerMagicCastingSpeed (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->MagicCastingSpeed);
	else lua_pushnil(L);
  	return 1;
}

int PlayerAttackSpeed (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->PhysicalAttackSpeed);
	else lua_pushnil(L);
  	return 1;
}

int PlayerNPCID (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->NPCID);
	else lua_pushnil(L);
  	return 1;
}

int PlayerNotTransformed (lua_State *L) {
	if (PlayerUser) lua_pushboolean(L, PlayerUser->bTransformed);
	else lua_pushnil(L);
  	return 1;
}

int PlayerSTR (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->Str);
	else lua_pushnil(L);
  	return 1;
}
int PlayerDEX (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->Dex);
	else lua_pushnil(L);
  	return 1;
}
int PlayerCON (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->Con);
	else lua_pushnil(L);
  	return 1;
}
int PlayerMEN (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->Men);
	else lua_pushnil(L);
  	return 1;
}
int PlayerINT (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->Int);
	else lua_pushnil(L);
  	return 1;
}
int PlayerWIT (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->Wit);
	else lua_pushnil(L);
  	return 1;
}

int PlayerHP (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->HP);
	else lua_pushnil(L);
  	return 1;
}

int PlayerMP (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->MP);
	else lua_pushnil(L);
  	return 1;
}

int PlayerMaxHP (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->MaxHP);
	else lua_pushnil(L);
  	return 1;
}

int PlayerMaxMP (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->MaxMP);
	else lua_pushnil(L);
  	return 1;
}

int PlayerCP (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->CP);
	else lua_pushnil(L);
  	return 1;
}

int PlayerMaxCP (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->MaxCP);
	else lua_pushnil(L);
  	return 1;
}
int PlayerLevel (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->Level);
	else lua_pushnil(L);
  	return 1;
}

int PlayerSP (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->SP);
	else lua_pushnil(L);
  	return 1;
}

int PlayerCarryWeight (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->CarryWeight);
	else lua_pushnil(L);
  	return 1;
}

int PlayerExpHigh (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->ExpHigh);
	else lua_pushnil(L);
  	return 1;
}
int PlayerExpLow (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->ExpLow);
	else lua_pushnil(L);
  	return 1;
}


int PlayerCarryingWeight (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->CarryingWeight);
	else lua_pushnil(L);
  	return 1;
}

int PlayerAttackRange (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->AttackRange);
	else lua_pushnil(L);
  	return 1;
}

int PlayerActiveClassType (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->ActiveClassType);
	else lua_pushnil(L);
  	return 1;
}

int PlayerClass (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->Class);
	else lua_pushnil(L);
  	return 1;
}

int PlayerPawn (lua_State *L) {
	if (PlayerUser) lua_pushlightuserdata(L, PlayerUser->Pawn);
	else lua_pushnil(L);
  	return 1;
	
}

int PlayerClassType (lua_State *L) {
	if (PlayerUser) lua_pushnumber(L, PlayerUser->ClassType);
	else lua_pushnil(L);
  	return 1;
}

int PlayerGender (lua_State *L) {
	if (PlayerUser) {
		if (PlayerUser->Gender) {
			lua_pushstring(L, "F");
		} else {
			lua_pushstring(L, "M");
		}
	}
	else lua_pushnil(L);
  	return 1;
}

int PlayerRace (lua_State *L) {
	if (PlayerUser) {
		switch (PlayerUser->Race)
		{
		case 0:
			lua_pushstring(L, "Human");
			break;
		case 1:
			lua_pushstring(L, "Elf");
			break;
		case 2:
			lua_pushstring(L, "Dark Elf");
			break;
		case 3:
			lua_pushstring(L, "Orc");
			break;
		case 4:
			lua_pushstring(L, "Dwarf");
			break;
		case 5:
			lua_pushstring(L, "Kamael");
			break;
		default:
			lua_pushnil(L);
		}
	}
	else lua_pushnil(L);

  	return 1;
}

int PlayerName (lua_State *L) {
	if (PlayerUser) {
		char * pName = new char[512];
		sprintf(pName, "%ws", PlayerUser->Name);
		lua_pushstring(L, pName);
		delete[] pName;
	}
	else lua_pushnil(L);
  	return 1;
}

#include "FIFOBuffer.h"
extern class FIFO TextBuffer;

int PopChatMessage (lua_State *L) {
	if (!TextBuffer.isempty()) {
		lua_pushstring(L, TextBuffer.dequeue());
	} else {
		lua_pushnil(L);
	}
  	return 1;
}

const struct luaL_reg LineageII[] = {
	{"PlayerMagicCastingSpeed",   	PlayerMagicCastingSpeed},
	{"PlayerAttackSpeed",   		PlayerAttackSpeed},
	{"PlayerNotTransformed",		PlayerNotTransformed},
	{"PlayerNPCID",			PlayerNPCID},
	{"PlayerSTR",   		PlayerSTR},
	{"PlayerDEX",   		PlayerDEX},
	{"PlayerCON",   		PlayerCON},
	{"PlayerMEN",   		PlayerMEN},
	{"PlayerINT",   		PlayerINT},
	{"PlayerWIT",   		PlayerWIT},
	{"PlayerHP",   			PlayerHP},
	{"PlayerMaxHP",  		PlayerMaxHP},
	{"PlayerCP",   			PlayerCP},
	{"PlayerMaxCP",  		PlayerMaxCP},
	{"PlayerMP",   			PlayerMP},
	{"PlayerMaxMP",   		PlayerMaxMP},
	{"PlayerLevel",   		PlayerLevel},
	{"PlayerSP",   			PlayerSP},
	{"PlayerCarryWeight",   PlayerCarryWeight},
	{"PlayerCarryingWeight",PlayerCarryingWeight},
	{"PlayerAttackRange",   PlayerAttackRange},
	{"PlayerName",			PlayerName},
	{"PlayerClass",   		PlayerClass},
	{"PlayerActiveClassType",   PlayerActiveClassType},
	{"PlayerPawn",   		PlayerPawn},
	{"PlayerGender",		PlayerGender},
	{"PlayerRace",   		PlayerRace},
	{"PlayerExpHigh",   	PlayerExpHigh},
	{"PlayerExpLow",   		PlayerExpLow},
	{"PlayerClassType",		PlayerClassType},
	{"PopChatMessage",		PopChatMessage},
	{"FLandMarkAddLandMark",FLandMarkAddLandMark},	
	{"MoveToLocation",MoveToLocation},	
	{NULL, NULL}
};

int lua_LineageIIopen (lua_State *L) {
	luaL_openlib(L, "LineageII", LineageII, 0);

	return(1);
}