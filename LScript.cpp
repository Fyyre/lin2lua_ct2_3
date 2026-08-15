//#define _CRT_SECURE_NO_DEPRECATE 1
//#define _CRT_NONSTDC_NO_DEPRECATE 1
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

/* 
	USER CONFIG
*/
	
#define DEFAULT_BOTFILE		"default.lua"
#define DEFAULT_LOGFILE		"default.txt"

#define BASEKEY				VK_MENU		//,VK_SHIFT
#define RESTART_KEY			'r'			//BASEKEY + <your_key>
#define OPEN_LOG_KEY		'l'			//BASEKEY + <your_key>
#define OPEN_LUA_CONSOLE	'c'			//BASEKEY + <your_key>

/*
	USER CONFIG END
*/

#include "Inc.h"

#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <time.h>

#include "Hook.h"
#include "LIN2.2.1Specific.h"

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

extern User * PlayerUser;

#define EngineUFunction "Engine.UFunction"
#define EngineUObject   "Engine.UObject"
#define EngineUProperty "Engine.UProperty"
#define MaxFullNameSize 512
#define MaxNameSize     128
#define NoIndex         0x7FFFFFFF


__declspec(dllexport) void NullExport()
{

}


/******************************************************************************
*
* LogFile
*
******************************************************************************/
FILE* hLogFile;
char szLogFile[MAX_PATH];
bool bLogActive = false;

void Log(char * pFormat, ...)
{
	va_list     ArgList;
	char        Line[32];
	time_t      Time;
	struct tm * TM;

	if(hLogFile && bLogActive)
	{
		Time = time(0);
		TM = localtime(&Time);
		strftime(Line, sizeof(Line), "%H:%M:%S ", TM);
		fwrite(Line, strlen(Line), 1, hLogFile);

		va_start(ArgList, pFormat);
		vfprintf(hLogFile, pFormat, ArgList);
		va_end(ArgList);

		fprintf(hLogFile, "\n");
		fflush(hLogFile);
	}
}

extern void DumpMemoryImage(char * szFilename);
static int LuaMemoryImageDump(lua_State *L) 
{ 
	const char * pCode = lua_tostring(L, -1);

	DumpMemoryImage((char *)pCode);

	return(0);
} 

#include <math.h>
void HexDump(char * pData, double iLength){
	int  Index;
	int  Offset = 0;

	fprintf(hLogFile, "Address    Offset  0 1 2 3  4 5 6 7  8 9 a b  c d e f 10111213 14151617 18191a1b 1c1d1e1f  0   4   8   c   0   4   8   c  f\n");
	fprintf(hLogFile, "---------- ------|-------- -------- -------- -------- -------- -------- -------- -------- |--------------------------------\n");

	while (iLength > 0){
		fprintf(hLogFile, "0x%08x 0x%04x|", &pData[0], Offset);
		for (Index = 0; Index < 32; ++Index){
			(iLength > Index) ? fprintf(hLogFile, "%02x", pData[Index] & 0xff) : fprintf(hLogFile, "--");
			if (fmod((double)Index+1, 4) == 0) fprintf(hLogFile, " ");
		}

		fprintf(hLogFile, "|");
		for (Index = 0; Index < 32; ++Index){
			if (iLength > Index){
				char Char = pData[Index];
				(Char 
				&& (Char != 0x00) 
				&& (Char != 0xff)
				&& (Char != 0x2f) 
				&& (Char != '\b') // Backspace
				&& (Char != '\f') // Form feed (page feed)
				&& (Char != '\n') // Newline
				&& (Char != '\r') // Carriage return
				&& (Char != '\t') // Tab
				&& (Char != '\v') // Vertical tab
				&& (Char != '\a') // Bell
				) ? fprintf(hLogFile, "%c", pData[Index]) : fprintf(hLogFile, ".");
			}
			else fprintf(hLogFile, " ");
		}

		iLength -= 32;
		pData += 32;
		Offset += 32;

		fprintf(hLogFile, "\n");
	}
	fflush(hLogFile);
}

static int LuaHexDump(lua_State *L) 
{ 
	void ** pAddress;
	double iLength;

	pAddress = (void **) lua_touserdata(L, 1); 
	iLength = lua_tonumber(L, 2); 
	HexDump((char *)*pAddress, iLength);

	return(0);
} 

#pragma warning (disable : 4244)
int LuaDwordDump(lua_State *L) 
{ 
	void ** pAddress;
	int iOffset;
	DWORD Return;

	pAddress = (void **) lua_touserdata(L, 1); 
	iOffset = lua_tonumber(L, 2); 
	//iReturn = (DWORD)pAddress[iOffset];
	//fprintf(hLogFile, "%08x\n", pAddress[iOffset]);

    DWORD * pData = (DWORD *)*pAddress;
	Return = pData[iOffset];
	lua_pushnumber(L, Return);

	return(1);
} 
#pragma warning (default : 4244)

//TArray<UObject *> *    pAActorArray;
TArray<FNameEntry *> * pFNameEntryArray;
TArray<UObject *> *    pUObjectArray;
UObject **             pUObjectHash;

UViewport *            pViewport;

UClass *               pActorClass;
UClass *               pClassClass;
UClass *               pFunctionClass;
UClass *               pStructClass;

UClass *               pArrayPropertyClass;
UClass *               pBoolPropertyClass;
UClass *               pBytePropertyClass;
UClass *               pClassPropertyClass;
UClass *               pDelegatePropertyClass;
UClass *               pFixedArrayPropertyClass;
UClass *               pFloatPropertyClass;
UClass *               pIntPropertyClass;
UClass *               pMapPropertyClass;
UClass *               pNamePropertyClass;
UClass *               pObjectPropertyClass;
UClass *               pPointerPropertyClass;
UClass *               pPropertyClass;
UClass *               pStructPropertyClass;
UClass *               pStrPropertyClass;
UClass *               pInterfacePropertyClass;
UClass *               pComponentPropertyClass;

void (__stdcall *      pFString_ConstructPChar)(char *);
void (__stdcall *      pFString_Destruct)(void);


UObject * FindActorByFullName(const char * pFullName) 
{
	char       Name[MaxFullNameSize];
	int        ObjectCount;
	UObject *  pObject;
	UObject ** ppObject;


	ObjectCount = pViewport->Actor->XLevel->ActorArray.Count;
	ppObject = (UObject **)pViewport->Actor->XLevel->ActorArray.pArray;
	while (ObjectCount)
	{
		pObject = *ppObject;
		if (pObject)
		{
			pObject->GetFullName(Name);
			if (!strcmp(Name, pFullName))
				return(pObject);
		}

		++ppObject;
		--ObjectCount;
	}

	return(0);
}


UObject * FindObjectByFullName(const char * pFullName) 
{
	char       Name[MaxFullNameSize];
	int        ObjectCount;
	UObject *  pObject;
	UObject ** ppObject;


	ObjectCount = pUObjectArray->Count;
	ppObject = (UObject **)pUObjectArray->pArray;
	while (ObjectCount)
	{
		pObject = *ppObject;
		if (pObject)
		{
			pObject->GetFullName(Name);
			if (!strcmp(Name, pFullName))
				return(pObject);
		}

		++ppObject;
		--ObjectCount;
	}

	return(0);
}

WCHAR * GetFName(FName Name)
{
	if ((!pFNameEntryArray) || (Name >= pFNameEntryArray->Count) || !pFNameEntryArray->pArray[Name])
		return(L"[empty]");

	return(pFNameEntryArray->pArray[Name]->Name);
}


void UObject::GetCPPName(char * pBuffer)
{
	if (pClass == pStructClass)
	{
		*pBuffer = 'F';
	}
	else
	{
		UClass * pNextClass;

		*pBuffer = 'U';

		if (pClass == pClassClass)
		        pNextClass = (UClass *)this;
		else
			pNextClass = this->pClass;
		while (pNextClass)
		{
			if (pNextClass == pActorClass)
			{
				*pBuffer = 'A';
				break;
			}

			if (pNextClass == pNextClass->pSuper)
				break;

			pNextClass = (UClass *)pNextClass->pSuper;
		}
	}

	sprintf(&pBuffer[1], "%S", GetFName(Name));
}


void AddPath(UObject * pObject, char * pBuffer)
{
	if (pObject->pOuter)
		AddPath(pObject->pOuter, pBuffer);

	sprintf(pBuffer, "%S.", GetFName(pObject->Name));
}


void UObject::GetFullName(char * pBuffer)
{
	sprintf(pBuffer, "%S ", GetFName(this->pClass->Name));
	if (this->pOuter)
		AddPath(this->pOuter, &pBuffer[strlen(pBuffer)]);
	sprintf(&pBuffer[strlen(pBuffer)], "%S", GetFName(this->Name));
}


void UObject::GetName(char * pBuffer)
{
	sprintf(pBuffer, "%S", GetFName(Name));
}


int UObject::IsA(UClass * pClass)
{
	UClass * pNextClass;


	pNextClass = this->pClass;
	while (pNextClass)
	{
		if (pNextClass == pClass)
			return(1);

		if (pNextClass == pNextClass->pSuper)
			return(0);

		pNextClass = (UClass *)pNextClass->pSuper;
	}

	return(0);
}


void AppendType(char * pString, UProperty * pProperty)
{
	if (pProperty->IsA(pArrayPropertyClass))
	{
		strcat(pString, "TArray<");
		AppendType(pString, (UProperty *)pProperty->pRelatedClass);
		strcat(pString, ">");
	}

	else if (pProperty->IsA(pBoolPropertyClass))
	{
		strcat(pString, "bool");
	}

	else if (pProperty->IsA(pBytePropertyClass))
	{
		strcat(pString, "byte");
	}

	else if (pProperty->IsA(pClassPropertyClass))
	{
		strcat(pString, "UClass *");
	}

	else if (pProperty->IsA(pDelegatePropertyClass))
	{
		strcat(pString, "delegate");
	}

	else if (pProperty->IsA(pFixedArrayPropertyClass))
	{
		strcat(pString, "fixed");
	}

	else if (pProperty->IsA(pFloatPropertyClass))
	{
		strcat(pString, "float");
	}

	else if (pProperty->IsA(pIntPropertyClass))
	{
		strcat(pString, "int");
	}

	else if (pProperty->IsA(pMapPropertyClass))
	{
		strcat(pString, "map");
	}

	else if (pProperty->IsA(pNamePropertyClass))
	{
		strcat(pString, "FName");
	}

	else if (pProperty->IsA(pObjectPropertyClass))
	{
		pProperty->pRelatedClass->GetCPPName(&pString[strlen(pString)]);
		strcat(pString, " *");
	}

	else if (pProperty->IsA(pPointerPropertyClass))
	{
		strcat(pString, "void *");
	}

	else if (pProperty->IsA(pStrPropertyClass))
	{
		strcat(pString, "FString");
	}

	else if (pProperty->IsA(pStructPropertyClass))
	{
		pProperty->pRelatedClass->GetCPPName(&pString[strlen(pString)]);
	}

	else if (pProperty->IsA(pInterfacePropertyClass))
	{
		pProperty->pRelatedClass->GetCPPName(&pString[strlen(pString)]);
	}

	else if (pProperty->IsA(pComponentPropertyClass))
	{
		strcat(pString, "component");
	}

	else
	{
		strcat(pString, "[unknown]");
	}
}


void AppendValue(char * pString, UProperty * pProperty, char * pData, int Index)
{
	if (pProperty->IsA(pArrayPropertyClass))
	{
		TArray<void> * pValue;

		pValue = (TArray<void> *)(pData + pProperty->CStructOffset);
		if (!pValue->Count || !pValue->pArray)
		{
			sprintf(&pString[strlen(pString)], "[%d][%d][nil]", pValue->Count, pValue->Max);
		}
		else
		{
			sprintf(&pString[strlen(pString)], "[%d][%d]", pValue->Count, pValue->Max);
			AppendValue(pString, (UProperty *)pProperty->pRelatedClass, (char *)pValue->pArray, 0);
		}
	}

	else if (pProperty->IsA(pBoolPropertyClass))
	{
		int Value;

		Value = *(int *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		Value &= (DWORD)pProperty->pRelatedClass;
		if (Value)
			Value = 1;
		sprintf(&pString[strlen(pString)], "[%d]", Value);
	}

	else if (pProperty->IsA(pBytePropertyClass))
	{
		int Value;

		Value = *(BYTE *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		sprintf(&pString[strlen(pString)], "[%d]", Value);
	}

	else if (pProperty->IsA(pClassPropertyClass))
	{
		int Value;

		Value = *(int *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		sprintf(&pString[strlen(pString)], "[0x08%X]", Value);
	}

	else if (pProperty->IsA(pFloatPropertyClass))
	{
		float Value;

		Value = *(float *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		sprintf(&pString[strlen(pString)], "[%0.3f]", Value);
	}

	else if (pProperty->IsA(pIntPropertyClass))
	{
		int Value;

		Value = *(int *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		sprintf(&pString[strlen(pString)], "[%d]", Value);
	}

	else if (pProperty->IsA(pNamePropertyClass))
	{
		FName Value;

		Value = *(FName *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		sprintf(&pString[strlen(pString)], "[%S]", GetFName(Value));
	}

	else if (pProperty->IsA(pObjectPropertyClass))
	{
		UObject * pValue;

		pValue = *(UObject **)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		if (pValue)
			sprintf(&pString[strlen(pString)], "[%S]", GetFName(pValue->Name));
		else
			strcat(pString, "[nil]");
	}

	else if (pProperty->IsA(pPointerPropertyClass))
	{
		int Value;

		Value = *(int *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		sprintf(&pString[strlen(pString)], "[0x08%X]", Value);
	}

	else if (pProperty->IsA(pStrPropertyClass))
	{
		TArray<WCHAR> * Value;

		Value = (TArray<WCHAR> *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		if (Value->pArray)
			sprintf(&pString[strlen(pString)], "[%S]", Value->pArray);
		else
			strcat(pString, "[nil]");
	}

	else if (pProperty->IsA(pStructPropertyClass))
	{
		UProperty * pSubProp;

		pSubProp = (UProperty *)pProperty->pRelatedClass->pChildren;
		while (pSubProp)
		{
			if (pSubProp->IsA(pPropertyClass))
			{
				pSubProp->GetName(&pString[strlen(pString)]);
				AppendValue(pString, pSubProp, pData + pProperty->CStructOffset +
					(pProperty->ElementSize * Index), 0);
				strcat(pString, " ");
			}

			pSubProp = (UProperty *)pSubProp->pNext;
		}
	}

	else if (pProperty->IsA(pDelegatePropertyClass))
	{
		UObject * pValue;

		pValue = *(UObject **)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		if (pValue)
			sprintf(&pString[strlen(pString)], "[%S]", GetFName(pValue->Name));
		else
			strcat(pString, "[nil]");
	}

	else if (pProperty->IsA(pInterfacePropertyClass))
	{
		UObject * pValue;

		pValue = *(UObject **)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		if (pValue)
			sprintf(&pString[strlen(pString)], "[%S]", GetFName(pValue->Name));
		else
			strcat(pString, "[nil]");
	}
	//pFixedArrayPropertyClass
	//pMapPropertyClass
  //pComponentClass

}

void FStringConstructor(void * pThis, const char * pString)
{
	__asm mov  eax,pString;
	__asm push eax
	__asm mov  ecx,pThis;
	__asm call pFString_ConstructPChar;
}

void FStringDestructor(void * pThis) 
{
	__asm mov  ecx,pThis;
	__asm call pFString_Destruct;
}

void EngineInit(void)
{
	if (!pUObjectArray)
	{
		HMODULE hCore;
		hCore = GetModuleHandle("core.dll");

		pFNameEntryArray = (TArray<FNameEntry *> *)GetProcAddress(hCore, FNAMEENTRYARRAY);
		pFString_ConstructPChar = (void (__stdcall *)(char *))GetProcAddress(hCore, FSTRING_CONSTRUCTPCHAR);
		pFString_Destruct = (void (__stdcall *)(void))GetProcAddress(hCore, FSTRING_DESTRUCT);
		pUObjectArray = (TArray<UObject *> *)GetProcAddress(hCore, UOBJECTARRAY);
		pUObjectHash = (UObject **)GetProcAddress(hCore, UOBJECTHASH);
		pViewport = (UViewport *)FindObjectByFullName(WINDOWSVIEWPORT);

		pActorClass = (UClass *)FindObjectByFullName("Class Engine.Actor");
		pClassClass = (UClass *)FindObjectByFullName("Class Core.Class");
		pFunctionClass = (UClass *)FindObjectByFullName("Class Core.Function");
		pStructClass = (UClass *)FindObjectByFullName("Class Core.Struct");

		pArrayPropertyClass = (UClass *)FindObjectByFullName("Class Core.ArrayProperty");
		pBoolPropertyClass = (UClass *)FindObjectByFullName("Class Core.BoolProperty");
		pBytePropertyClass = (UClass *)FindObjectByFullName("Class Core.ByteProperty");
		pClassPropertyClass = (UClass *)FindObjectByFullName("Class Core.ClassProperty");
		pDelegatePropertyClass = (UClass *)FindObjectByFullName("Class Core.DelegateProperty");
		pFixedArrayPropertyClass = (UClass *)FindObjectByFullName("Class Core.FixedArrayProperty");
		pFloatPropertyClass = (UClass *)FindObjectByFullName("Class Core.FloatProperty");
		pIntPropertyClass = (UClass *)FindObjectByFullName("Class Core.IntProperty");
		pMapPropertyClass = (UClass *)FindObjectByFullName("Class Core.MapProperty");
		pNamePropertyClass = (UClass *)FindObjectByFullName("Class Core.NameProperty");
		pObjectPropertyClass = (UClass *)FindObjectByFullName("Class Core.ObjectProperty");
		pPointerPropertyClass = (UClass *)FindObjectByFullName("Class Core.PointerProperty");
		pPropertyClass = (UClass *)FindObjectByFullName("Class Core.Property");
		pStructPropertyClass = (UClass *)FindObjectByFullName("Class Core.StructProperty");
		pStrPropertyClass = (UClass *)FindObjectByFullName("Class Core.StrProperty");
		pInterfacePropertyClass  = (UClass *)FindObjectByFullName("Class Core.InterfaceProperty");
		pComponentPropertyClass  = (UClass *)FindObjectByFullName("Class Core.ComponentProperty");
	}
}


/******************************************************************************
*
* Lua Engine Functions
*
******************************************************************************/
struct UFunctionData
{
	UObject *   pObject;
	UFunction * pFunction;
};

struct UObjectData
{
	UObject *   pObject;
	UClass *    pClass;
	char *      pData;
	DWORD       ActorIndex;
};

struct UObjectProp
{
	UObject *   pObject;
	UProperty * pProperty;
	char *      pData;
};


HANDLE    hConsole;
HANDLE    hInputEvent;
char      Input[MaxFullNameSize];
char      Path[MAX_PATH];
bool      RestartLua;
HANDLE    StdErr;
HANDLE    StdIn;
HANDLE    StdOut;


int CloseConsole(lua_State * /*L*/) 
{
	if (hConsole)
	{
		SetStdHandle(STD_INPUT_HANDLE, StdIn);
		SetStdHandle(STD_OUTPUT_HANDLE, StdOut);
		SetStdHandle(STD_ERROR_HANDLE, StdErr);

		CloseHandle(hConsole);
		FreeConsole();

		hConsole = 0;
		SetEvent(hInputEvent);
	}

	return(0);
}

BOOL WINAPI ConsoleHandlerRoutine(DWORD	/*dwCtrlType*/)
{
	FreeConsole();
	return(1);
}

void DumpProperty(UProperty * pProperty, char * pData, char * pName)
{
	int Len;

	if (pProperty->IsA(pPropertyClass))
	{
		AppendType(pName, pProperty);

		strcat(pName, " ");
		Len = strlen(pName);
		if (Len < 40)
		{
			memset(&pName[Len], ' ', 40 - Len);
			pName[40] = 0;
		}

		pProperty->GetName(&pName[strlen(pName)]);

		if (pProperty->ElementCount > 1)
		{
			sprintf(&pName[strlen(pName)], "[%d]", 
				pProperty->ElementCount);
		}

		strcat(pName, " ");
		Len = strlen(pName);
		if (Len < 80)
		{
			memset(&pName[Len], ' ', 80 - Len);
			pName[80] = 0;
		}

		AppendValue(pName, pProperty, pData, 0);
	}
}

void DumpFunction(UFunction * pFunction, char * pName)
{
	UProperty *     pProperty;
	int             ParamIndex;

	if (pFunction->IsA(pFunctionClass))
	{
		//Return
		pProperty = (UProperty *)pFunction->pChildren;
		while (pProperty)
		{
			if (pProperty->IsA(pPropertyClass) && (pProperty->Flags & 0x80))
			{
				if (pProperty->Flags & 0x400)
				{
					AppendType(pName, pProperty);
					strcat(pName, " ");
				}
			}
			pProperty = (UProperty *)pProperty->pNext;
		}

		pFunction->GetName(&pName[strlen(pName)]);
		strcat(pName, "(");

		ParamIndex = 0;
		pProperty = (UProperty *)pFunction->pChildren;
		while (pProperty)
		{
			if (pProperty->IsA(pPropertyClass) && (pProperty->Flags & 0x80))
			{
				if (!(pProperty->Flags & 0x400))
				{
					if (!(pProperty->Flags & 0x10))
					{
						if (ParamIndex > 0) strcat(pName, ", ");
						AppendType(pName, pProperty);
						strcat(pName, " ");
						pProperty->GetName(&pName[strlen(pName)]);
					}
					++ParamIndex;
				}
			}

			pProperty = (UProperty *)pProperty->pNext;
		}
		strcat(pName, ")");
	}
}


int Dump(lua_State * L) 
{
	int           Found;
	DWORD         Index;
	char          Name[MaxFullNameSize*8];
	char *        pData;
	UClass *      pClass;
	UObject *     pObject;
	UObjectData * pObjectData;
	UObjectProp * pObjectProp;
	UProperty *   pProperty;

	//Plain data type.
	if (!lua_touserdata(L, 1))
	{
		pData = Name;
		if (lua_isnoneornil(L, 1))
			pData = "[nil]";
		else if (lua_isnumber(L, 1))
			sprintf(pData, "[%0.3f]", (float)lua_tonumber(L, 1));
		else
			sprintf(pData, "[%s]", lua_tostring(L, 1));

		fputs("\n", stdout);
		fputs(pData, stdout);
		fputs("\n\n", stdout);
		Log("Dump: %s", pData);
	}

	//Check for UObject
	lua_getfield(L, LUA_REGISTRYINDEX, EngineUObject);
	lua_getmetatable(L, 1);
	Found = lua_rawequal(L, -1, -2);
	lua_pop(L, 2);
	if (Found)
	{
		pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
		pObject = pObjectData->pObject;
		pData = pObjectData->pData;
		pClass = pObjectData->pClass;

		if ((char *)pObject == pData)
		{
			pObject->GetFullName(Name);
		}
		else
		{
			pClass->GetFullName(Name);
			if (pObject)
			{
				strcat(Name, " part of [");
				pObject->GetFullName(&Name[strlen(Name)]);
				strcat(Name, "]");
			}
		}

		fputs("\n", stdout);
		fputs(Name, stdout);
		fputs("\n", stdout);
		Log("\n\n\nDump: %s", Name);

		while (pClass)
		{
			strcpy(Name, "  ");
			pClass->GetCPPName(&Name[strlen(Name)]);
			strcat(Name, "  [");
			pClass->GetFullName(&Name[strlen(Name)]);
			strcat(Name, "]");
			fputs(Name, stdout);
			fputs("\n", stdout);
			Log("Dump: %s", Name);

			pProperty = (UProperty *)pClass->pChildren;
			while (pProperty)
			{
				if (pProperty->IsA(pPropertyClass))
				{
					strcpy(Name, "    ");

					DumpProperty(pProperty, pData, Name);

					fputs(Name, stdout);
					fputs("\n", stdout);
					Log("Dump: %s", Name);
				}
				/*
				Functions in UnrealScript are actually static members of the underlying class
				so it kind of makes more sense if they are listed via the DumpClass command.
				They can be invoked as long as the class itself has been loaded.

				if (pProperty->IsA(pFunctionClass))
				{
					strcpy(Name, "    ");

					DumpFunction((UFunction *)pProperty, Name);

					fputs(Name, stdout);
					fputs("\n", stdout);
					Log("Dump: %s", Name);
				}
				*/
				pProperty = (UProperty *)pProperty->pNext;
			}

			pClass = (UClass *)pClass->pSuper;
		}
		fputs("\n", stdout);

		return(0);
	}

	//Check for UProperty
	lua_getfield(L, LUA_REGISTRYINDEX, EngineUProperty);
	lua_getmetatable(L, 1);
	Found = lua_rawequal(L, -1, -2);
	lua_pop(L, 2);
	if (!Found)
		return(0);

	pObjectProp = (UObjectProp *)luaL_checkudata(L, 1, EngineUProperty);
	pObject = pObjectProp->pObject;
	pProperty = pObjectProp->pProperty;
	pData = pObjectProp->pData;

	if ((char *)pObject == pData)
	{
		pObject->GetFullName(Name);
	}
	else
	{
		pProperty->GetFullName(Name);
		if (pObject)
		{
			strcat(Name, " part of [");
			pObject->GetFullName(&Name[strlen(Name)]);
			strcat(Name, "]");
		}
	}

	fputs("\n", stdout);
	fputs(Name, stdout);
	fputs("\n", stdout);
	Log("\n\n\nDump: %s", Name);

	if (pProperty->ElementCount > 1)
	{
		strcpy(Name, "  ");

		AppendType(Name, pProperty);
		strcat(Name, " ");
		pProperty->GetName(&Name[strlen(Name)]);
		sprintf(&Name[strlen(Name)], "[%d]", pProperty->ElementCount);

		fputs(Name, stdout);
		fputs("\n", stdout);
		Log("Dump: %s", Name);

		for (Index = 0; Index < pProperty->ElementCount; ++Index)
		{
			sprintf(Name, "    [%d] ", Index);
			AppendValue(Name, pProperty, pData, Index);

			fputs(Name, stdout);
			fputs("\n", stdout);
			Log("Dump: %s", Name);
		}
	}

	else if (pProperty->IsA(pStructPropertyClass))
	{
		pData += pProperty->CStructOffset;
		pClass = pProperty->pRelatedClass;
		while (pClass)
		{
			strcpy(Name, "  ");
			pClass->GetCPPName(&Name[strlen(Name)]);
			fputs(Name, stdout);
			fputs("\n", stdout);
			Log("Dump: %s", Name);

			pProperty = (UProperty *)pClass->pChildren;
			while (pProperty)
			{
				if (pProperty->IsA(pPropertyClass))
				{
					strcpy(Name, "    ");

					DumpProperty(pProperty, pData, Name);

					fputs(Name, stdout);
					fputs("\n", stdout);
					Log("Dump: %s", Name);
				}

				pProperty = (UProperty *)pProperty->pNext;
			}

			pClass = (UClass *)pClass->pSuper;
		}
	}

	else if (pProperty->IsA(pArrayPropertyClass))
	{
		TArray<void> * pArray;


		pArray = (TArray<void> *)(pData + pProperty->CStructOffset); 
		strcpy(Name, "  ");

		AppendType(Name, pProperty);
		strcat(Name, " ");
		pProperty->GetName(&Name[strlen(Name)]);
		sprintf(&Name[strlen(Name)], "[%d][%d]", pArray->Count, pArray->Max);

		fputs(Name, stdout);
		fputs("\n", stdout);
		Log("Dump: %s", Name);

		pProperty = (UProperty *)pProperty->pRelatedClass;
		for (Index = 0; Index < pArray->Count; ++Index)
		{
			sprintf(Name, "    [%d] ", Index);

			pData = (char *)((DWORD)pArray->pArray + (Index * pProperty->ElementSize));
			AppendValue(Name, pProperty, pData, 0);

			fputs(Name, stdout);
			fputs("\n", stdout);
			Log("Dump: %s", Name);
		}
	}

	else
	{
		pClass = (UClass *)pProperty;
		while (pClass)
		{
			strcpy(Name, "  ");
			pClass->GetCPPName(&Name[strlen(Name)]);
			strcat(Name, "  [");
			pClass->GetFullName(&Name[strlen(Name)]);
			strcat(Name, "]");
			fputs(Name, stdout);
			fputs("\n", stdout);
			Log("Dump: %s", Name);

			pProperty = (UProperty *)pClass->pChildren;
			while (pProperty)
			{
				if (pProperty->IsA(pPropertyClass))
				{
					strcpy(Name, "    ");

					DumpProperty(pProperty, pData, Name);

					fputs(Name, stdout);
					fputs("\n", stdout);
					Log("Dump: %s", Name);
				}

				pProperty = (UProperty *)pProperty->pNext;
			}

			pClass = (UClass *)pClass->pSuper;
		}
	}
	fputs("\n", stdout);

	return(0);
}


int DumpClass(lua_State * L) 
{
	int           Found;
	char          Name[MaxFullNameSize*8];
	UClass *      pClass;
	UObjectData * pObjectData;
	UObjectProp * pObjectProp;

	//Plain data type.
	if (!lua_touserdata(L, 1))
		luaL_argcheck(L, 0, 1, "Expecting a UObject or UProperty.");

	//Check for UObject
	lua_getfield(L, LUA_REGISTRYINDEX, EngineUObject);
	lua_getmetatable(L, 1);
	Found = lua_rawequal(L, -1, -2);
	lua_pop(L, 2);
	if (Found)
	{
		pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
		if (!pObjectData->pObject)
			luaL_argcheck(L, 0, 1, "Expecting a UObject, UClass or UProperty.");

		if (pObjectData->pObject && pObjectData->pObject->IsA(pClassClass))
		{
			pClass = (UClass *)pObjectData->pObject;
		}
		else
		{
			pClass = pObjectData->pClass;
		}
	}

	//Check for UProperty
	else
	{
		lua_getfield(L, LUA_REGISTRYINDEX, EngineUProperty);
		lua_getmetatable(L, 1);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (!Found)
			return(0);

		pObjectProp = (UObjectProp *)luaL_checkudata(L, 1, EngineUProperty);
		pObjectProp->pProperty->GetFullName(Name);

		pClass = 0;
		if (pObjectProp->pProperty->IsA(pArrayPropertyClass) ||
			pObjectProp->pProperty->IsA(pStructPropertyClass))
		{
			pClass = pObjectProp->pProperty->pRelatedClass;
		}
	}

	while (pClass)
	{
		strcpy(Name, "  ");
		pClass->GetFullName(&Name[strlen(Name)]);
		fputs(Name, stdout);
		fputs("\n", stdout);
		Log("DumpClass: %s", Name);

		UProperty *pProperty = (UProperty *)pClass->pChildren;
		while (pProperty)
		{
			if (pProperty->IsA(pFunctionClass))
			{
				strcpy(Name, "    ");

				DumpFunction((UFunction *)pProperty, Name);

				fputs(Name, stdout);
				fputs("\n", stdout);
				Log("DumpClass: %s", Name);
			}
			pProperty = (UProperty *)pProperty->pNext;
		}

		pClass = (UClass *)pClass->pSuper;
	}
	return(0);
}

#pragma warning (disable : 4244)
int GetKeyState(lua_State * L) {
  if (lua_isnumber(L, 1)) {
    if (GetAsyncKeyState(lua_tonumber(L, 1)) & 0x8000) {
      lua_pushboolean(L, true);
    } else {
      lua_pushboolean(L, false);
    }
  }
  return(1);
}
#pragma warning (default : 4244)

/* 
 * Though this function works I'm not sure how usefull it is.
 * The primary problem is that when searching for all say:
 * 'Class Engine.Input' you naturally  search for FName 
 * 'Input' but the Object that you really want is
 * 'Input Transient.Input0' i.e. its FName is 'Input0'
*/

int FindFirst(lua_State * L) 
{
	FName         Name = NULL;
	UClass *      pClass = NULL;
	UObject *     pObject = NULL;
	UObjectData * pObjectData = NULL;


	if (lua_type(L, 1) == LUA_TUSERDATA)
	{
		pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
		if ((char *)pObjectData->pObject != pObjectData->pData)
			luaL_argcheck(L, 0, 1, "Expecting a UObject or FName, UClass");

		pObject = pObjectData->pObject;
		pClass = pObject->pClass;
		Name = pObject->Name;
	}
	else
	{
		Name = luaL_checkinteger(L, 1);
		pObjectData = (UObjectData *)luaL_checkudata(L, 2, EngineUObject);
		if ((char *)pObjectData->pObject != pObjectData->pData)
			luaL_argcheck(L, 0, 2, "Expecting a UObject or FName, UClass");
		if (!pObjectData->pObject->IsA(pClassClass))
			luaL_argcheck(L, 0, 2, "Expecting a UObject or FName, UClass");

		pClass = (UClass *)pObjectData->pObject;
	}

	//pObject = pUObjectHash[Name & 0xFFF];
    pObject = pUObjectHash[Name & 0xFFFF];
	while (pObject && !pObject->IsA(pClass))
	{
		pObject = pObject->pHash;
	}

	if (pObject)
	{
		pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
		pObjectData->pObject = pObject;
		pObjectData->pClass = pObject->pClass;
		pObjectData->pData = (char *)pObject;
		pObjectData->ActorIndex = (DWORD)-1;

		luaL_getmetatable(L, EngineUObject);
		lua_setmetatable(L, -2);
	}
	else
	{
		lua_pushnil(L);
	}

	return(1);
}


int FindFirstAActor(lua_State * L) 
{
	DWORD         Index;
	UClass *      pClass;
	UObject *     pObject;
	UObjectData * pObjectData;

	pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
	if ((char *)pObjectData->pObject != pObjectData->pData)
		luaL_argcheck(L, 0, 1, "Expecting a UClass");
	if (!pObjectData->pObject->IsA(pClassClass))
		luaL_argcheck(L, 0, 1, "Expecting a UClass");

	pClass = (UClass *)pObjectData->pObject;

	Index = 0;
	pObject = 0;
	while (Index < pViewport->Actor->XLevel->ActorArray.Count)
	{
		pObject = pViewport->Actor->XLevel->ActorArray.pArray[Index];
		if (pObject && (pObject->IsA(pClass)))
			break;

		++Index;
	}

	if (pObject && (Index < pViewport->Actor->XLevel->ActorArray.Count))
	{
		pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
		pObjectData->pObject = pObject;
		pObjectData->pClass = pObject->pClass;
		pObjectData->pData = (char *)pObject;
		pObjectData->ActorIndex = Index;

		luaL_getmetatable(L, EngineUObject);
		lua_setmetatable(L, -2);
	}
	else
	{
		lua_pushnil(L);
	}

	return(1);
}


int FindNext(lua_State * L) 
{
	UClass *      pClass;
	UObject *     pObject;
	UObjectData * pObjectData;

	pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
	if ((char *)pObjectData->pObject != pObjectData->pData)
		luaL_argcheck(L, 0, 1, "Expecting a UObject");

	pObject = pObjectData->pObject;
	pClass = pObject->pClass;

	pObject = pObject->pHash;
	while (pObject && (pObject->pClass != pClass))
		pObject = pObject->pHash;

	if (pObject)
	{
		pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
		pObjectData->pObject = pObject;
		pObjectData->pClass = pObject->pClass;
		pObjectData->pData = (char *)pObject;
		pObjectData->ActorIndex = (DWORD)-1;

		luaL_getmetatable(L, EngineUObject);
		lua_setmetatable(L, -2);
	}
	else
	{
		lua_pushnil(L);
	}

	return(1);
}


int FindNextAActor(lua_State * L) 
{
	DWORD         Index;
	UClass *      pClass;
	UObject *     pObject;
	UObjectData * pObjectData;

	pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
	if ((char *)pObjectData->pObject != pObjectData->pData)
		luaL_argcheck(L, 0, 1, "Expecting a UObject, UClass");

	Index = pObjectData->ActorIndex + 1;

	pObjectData = (UObjectData *)luaL_checkudata(L, 2, EngineUObject);
	if ((char *)pObjectData->pObject != pObjectData->pData)
		luaL_argcheck(L, 0, 2, "Expecting a UObject, UClass");
	if (!pObjectData->pObject->IsA(pClassClass))
		luaL_argcheck(L, 0, 2, "Expecting a UObject, UClass");

	pClass = (UClass *)pObjectData->pObject;

	pObject = 0;
	while (Index < pViewport->Actor->XLevel->ActorArray.Count)
	{
		pObject = pViewport->Actor->XLevel->ActorArray.pArray[Index];
		if (pObject && (pObject->IsA(pClass)))
			break;

		++Index;
	}

	if (pObject && (Index < pViewport->Actor->XLevel->ActorArray.Count))
	{
		pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
		pObjectData->pObject = pObject;
		pObjectData->pClass = pObject->pClass;
		pObjectData->pData = (char *)pObject;
		pObjectData->ActorIndex = Index;

		luaL_getmetatable(L, EngineUObject);
		lua_setmetatable(L, -2);
	}
	else
	{
		lua_pushnil(L);
	}

	return(1);
}


int FullName(lua_State * L) 
{
	int             Found;
	char            Name[MaxFullNameSize];
	UObjectData *   pObjectData;
	UFunctionData * pFunctionData;


	lua_getfield(L, LUA_REGISTRYINDEX, EngineUObject);
	lua_getmetatable(L, 1);
	Found = lua_rawequal(L, -1, -2);
	lua_pop(L, 2);
	if (Found)
	{
		pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
		if ((char *)pObjectData->pObject != pObjectData->pData)
			luaL_argcheck(L, 0, 1, "Expecting a UObject or UFunction");
		pObjectData->pObject->GetFullName(Name);
	}
	else
	{
		pFunctionData = (UFunctionData *)luaL_checkudata(L, 1, EngineUFunction);
		pFunctionData->pFunction->GetFullName(Name);
	}

	lua_pushstring(L, Name);

	return(1);
}


unsigned int __stdcall InputThread(void *)
{
	Input[0] = 0;
	ResetEvent(hInputEvent);
	while (hConsole)
	{
		fgets(&Input[1], sizeof(Input) - 1, stdin);
		Input[0] = 1;
		WaitForSingleObject(hInputEvent, INFINITE);
	}

	return(0);
}

int IsA(lua_State * L) 
{
	UClass *		pClass = NULL;
	UObjectData *	pClassData = NULL;
	UObject *		pObject = NULL;
	int				Found;

	lua_getfield(L, LUA_REGISTRYINDEX, EngineUObject);
	lua_getmetatable(L, 1);
	Found = lua_rawequal(L, -1, -2);
	lua_pop(L, 2);
	if (Found)
	{
		UObjectData * pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
		if ((char *)pObjectData->pObject != pObjectData->pData)
			luaL_argcheck(L, 0, 1, "Expecting a UObject or UFunction");
		pObject = pObjectData->pObject;
	}
	else
	{
		lua_getfield(L, LUA_REGISTRYINDEX, EngineUFunction);
		lua_getmetatable(L, 1);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);

		if (Found)
		{
			UFunctionData * pFunctionData = (UFunctionData *)luaL_checkudata(L, 1, EngineUFunction);
			pObject = pFunctionData->pObject;
		}
		else
		{
			lua_getfield(L, LUA_REGISTRYINDEX, EngineUProperty);
			lua_getmetatable(L, 1);
			Found = lua_rawequal(L, -1, -2);
			lua_pop(L, 2);

			if (Found)
			{
				UObjectProp * pObjectProp = (UObjectProp *)luaL_checkudata(L, 1, EngineUProperty);
				pObject = pObjectProp->pObject;
			}
			else
			{
				luaL_argcheck(L, 0, 1, "Expecting a UObject, UClass");
			}
		}
	}

	pClassData = (UObjectData *)luaL_checkudata(L, 2, EngineUObject);
	if ((char *)pClassData->pObject != pClassData->pData)
		luaL_argcheck(L, 0, 2, "Expecting a UObject, UClass");
	if (!pClassData->pObject->IsA(pClassClass))
		luaL_argcheck(L, 0, 2, "Expecting a UObject, UClass");

	pClass = (UClass *)pClassData->pObject;

	lua_pushboolean(L, pObject->IsA(pClass));


	return(1);
}


int OpenConsole(lua_State *	/*L*/) 
{
	int          ConsoleHandle;
	FILE *       File;
	unsigned int ID;


	if (hConsole)
		return(0);

	AllocConsole();
	SetConsoleTitle("Command Prompt");
	SetConsoleCtrlHandler(ConsoleHandlerRoutine, 1);


	//Fix up stdin/stdout/stderr
	StdIn = GetStdHandle(STD_INPUT_HANDLE);
	ConsoleHandle = _open_osfhandle((long)StdIn, _O_TEXT);
	File = _fdopen(ConsoleHandle, "r");
	*stdin = *File;
	setvbuf(stdin, NULL, _IONBF, 0);

	StdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	ConsoleHandle = _open_osfhandle((long)StdOut, _O_TEXT);
	File = _fdopen(ConsoleHandle, "w");
	*stdout = *File;
	setvbuf(stdout, NULL, _IONBF, 0);

	StdErr = GetStdHandle(STD_ERROR_HANDLE);
	ConsoleHandle = _open_osfhandle((long)StdErr, _O_TEXT);
	File = _fdopen(ConsoleHandle, "w");
	*stdout = *File;
	setvbuf(stdout, NULL, _IONBF, 0);

	fputs("\n> ", stdout);
	hConsole = (HANDLE)_beginthreadex(0, 0, InputThread, 0, 0, &ID);

	return(0);
}


int Load(lua_State * L) 
{
	char * pFileName;
	char   Script[MAX_PATH];

	strcpy(Script, Path);
	pFileName = (char *)luaL_checkstring(L, 1);
	strcat(Script, pFileName);

	fputs("Loading[", stdout);
	fputs(Script, stdout);
	fputs("]\n", stdout);
	Log("System: Loading[%s]", Script);
	if (luaL_dofile(L, Script))
	{
		const char * pError = lua_tostring(L, -1);

		fputs("Script Error: ", stdout);
		if (pError)
		{
			fputs(pError, stdout);
			Log("Script Error: %s", pError);
		}
		else
		{
			fputs("[unknown]", stdout);
			Log("Script Error: [unknown]");
		}

		fputs("\n", stdout);
	}
	fputs("\n", stdout);

	return(0);
}


int LogString(lua_State * L) 
{
	//Log("Script: %s", (char *)luaL_checkstring(L, 1));
	Log("%s", (char *)luaL_checkstring(L, 1));
	return(0);
}


int New(lua_State * L) 
{
	UObjectData * pObjectData;
	UObjectData * pNewData;
	UStruct *     pStruct;

	pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);

	if (!pObjectData->pObject->IsA(pStructClass))
		luaL_argcheck(L, 0, 1, "Expecting a Struct.");

	pStruct = (UStruct *)pObjectData->pObject;

	pNewData = (UObjectData *)lua_newuserdata(L, sizeof(*pNewData) +
		pStruct->Size);
	pNewData->pObject = 0;
	pNewData->pClass = (UClass *)pStruct;
	pNewData->pData = (char *)pNewData + sizeof(*pNewData);

	luaL_getmetatable(L, EngineUObject);
	lua_setmetatable(L, -2);

	memset(pNewData->pData, 0, pStruct->Size);

	return(1);
}



int RestartLogfile(lua_State *	/*L*/) 
{
	fclose(hLogFile);
	hLogFile = fopen(szLogFile, "w");
	return(0);
}

int Restart(lua_State *	/*L*/) 
{
	RestartLua = true;
	return(0);
}


/******************************************************************************
*
* Lua Common Functions to find UScript properties.
*
******************************************************************************/
/* In some games new FNames are constantly allocated for properties which can 
** thrash the cache and make it less efficient than the actor list. It is 
** therefore important to optimally set the the MAXPROPCACHE. 
*/

#define MAXPROPCACHE 1024

struct PropertyCacheStruct
{
	UClass *    pClass;
	UProperty * pProperty;
	char        Name[MaxNameSize];
};

PropertyCacheStruct * Cache[MAXPROPCACHE];
PropertyCacheStruct   NewCacheData[MAXPROPCACHE]; //fixed


void ResetPropertyCache(void)
{
	int Index;

	for (Index = 0; Index < MAXPROPCACHE; ++Index)
		Cache[Index] = &NewCacheData[Index];

	memset(NewCacheData, 0, sizeof(NewCacheData));
}

UProperty * FindPropertySlow(UClass * pClass, const char * pPropertyName)
{
	UField *    pField;
	UProperty * pProperty;
	WCHAR       WName[MaxNameSize];

	wsprintfW(WName, L"%S", pPropertyName);

	pProperty = 0;
	while (pClass && !pProperty)
	{
		pField = pClass->pChildren;
		while (pField)
		{
			if (pField->pClass == pFunctionClass)
			{
				if (!wcscmp(WName, GetFName(pField->Name)))
				{
					pProperty = (UProperty *)pField;
					break;
				}
			}

			else if (pField->IsA(pPropertyClass))
			{
				if (!wcscmp(WName, GetFName(pField->Name)))
				{
					pProperty = (UProperty *)pField;
					break;
				}
			}

			pField = pField->pNext;
		}

		pClass = (UClass *)pClass->pSuper;
	}

	return(pProperty);
}

UProperty * FindProperty(lua_State * L, UObjectData * pObject, UClass * pClass, const char * pPropertyName)
{
	int                    Index;
    PropertyCacheStruct *  pCache;
	UProperty *            pProperty;
    PropertyCacheStruct ** ppCache;

	//Search the cache.
	ppCache = Cache;
	for (Index = 0; Index < MAXPROPCACHE; ++Index)
	{
		// Cache via Objects Class
		if ((*ppCache)->pClass 
		&& ((*ppCache)->pClass == pClass) 
		&& !strcmp((*ppCache)->Name, pPropertyName))
		{
			//Log("System: Cache hit Property[%s] Class[%S] via Object", pPropertyName, (*ppCache)->Name);
			break;
		}
		// Cache via Class
		if ((*ppCache)->pClass 
		&& pObject
		&& pObject->pObject
		&& pObject->pObject->IsA(pClass)
		&& ((*ppCache)->pClass == pObject->pObject) 
		&& !strcmp((*ppCache)->Name, pPropertyName))
		{
			//Log("System: Cache hit Property[%s] Class[%S] via Class", pPropertyName, (*ppCache)->Name);
			break;
		}
		++ppCache;
	}

	//We don't have a cache hit, so look it up the old way.
	if ((Index >= MAXPROPCACHE) || !(*ppCache)->pClass)
	{
		pProperty = FindPropertySlow(pClass, pPropertyName);
		if (!pProperty)
		{
			if (pObject	&& pObject->pObject->IsA(pClass)) {
				pProperty = FindPropertySlow((UClass *)pObject->pObject, pPropertyName);
				if (pProperty && pProperty->IsA(pFunctionClass)) {
					Log("System: Caching Property[%s] Class[%S] via Class", pPropertyName, GetFName(pObject->pObject->Name));
					Index = MAXPROPCACHE - 1;
					pCache = Cache[Index];
					pCache->pClass = (UClass *)pObject->pObject;
				}
			}
			if (!pProperty || !pProperty->IsA(pFunctionClass)) {
				char Line[512];
				sprintf(Line, "[%s] is not a valid property of [%S]", pPropertyName, GetFName(pClass->Name));
				luaL_argcheck(L, pProperty, 2, Line);
				return(0);
			}
		} else {
			Log("System: Caching Property[%s] Class[%S] via Object", pPropertyName, GetFName(pClass->Name));
			Index = MAXPROPCACHE - 1;
			pCache = Cache[Index];
			pCache->pClass = pClass;
		}
		strcpy(pCache->Name, pPropertyName);
		pCache->pProperty = pProperty;
	}


	//Move the item to the top.
	pCache = Cache[Index];
	memmove(&Cache[1], &Cache[0], sizeof(Cache[0]) * Index);
	Cache[0] = pCache;

	return(pCache->pProperty);
}

/******************************************************************************
*
* Lua Common Functions to Get/Set properties and to create
* parameter blocks for calling UScript functions.
*
******************************************************************************/
void CreateReturn(lua_State * L, UProperty * pProperty, char * pParam)
{
	if (pProperty->IsA(pBoolPropertyClass))
	{
		lua_pushboolean(L, *(bool *)pParam);
	}

	else if (pProperty->IsA(pBytePropertyClass))
	{
		lua_pushinteger(L, *(BYTE *)pParam);
	}

	else if (pProperty->IsA(pClassPropertyClass) ||
		pProperty->IsA(pObjectPropertyClass) ||
    		pProperty->IsA(pInterfacePropertyClass))
	{
		UObjectData * pObjectData;


		pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
		pObjectData->pObject = *(UClass **)pParam;
		pObjectData->pClass = pObjectData->pObject->pClass;
		pObjectData->pData = (char *)pObjectData->pObject;
		pObjectData->ActorIndex = (DWORD)-1;

		luaL_getmetatable(L, EngineUObject);
		lua_setmetatable(L, -2);
	}

	else if (pProperty->IsA(pFloatPropertyClass))
	{
		lua_pushnumber(L, *(float *)pParam);
	}

	else if (pProperty->IsA(pIntPropertyClass))
	{
		lua_pushinteger(L, *(int *)pParam);
	}

	else if (pProperty->IsA(pNamePropertyClass))
	{
		lua_pushinteger(L, *(FName *)pParam);
	}

	else if (pProperty->IsA(pStrPropertyClass))
	{
		TArray<WCHAR> * Value;

		Value = (TArray<WCHAR> *)pParam;
		if (Value->pArray)
		{
			char * pName = new char[Value->Count];
			sprintf(pName, "%S", Value->pArray);
			lua_pushstring(L, pName);
			delete[] pName;
		}
		else
		{
			lua_pushnil(L);
		}
	}

	else if (pProperty->IsA(pStructPropertyClass))
	{
		UObjectData * pObjectData;


		pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData) +
			pProperty->ElementSize);
		pObjectData->pObject = 0;
		pObjectData->pClass = pProperty->pRelatedClass;
		pObjectData->pData = (char *)pObjectData + sizeof(*pObjectData);

		luaL_getmetatable(L, EngineUObject);
		lua_setmetatable(L, -2);

		memcpy(pObjectData->pData, pParam, pProperty->ElementSize);
	}

	else
	{
		luaL_argcheck(L, 0, 0, "Return property type not implemented.");
	}

	//pArrayPropertyClass
	//pDelegatePropertyClass;
	//pObjectPropertyClass;
	//pPointerPropertyClass;
	//pFixedArrayPropertyClass;
	//pMapPropertyClass;
}


void GetProperty(lua_State * L, UObject * pObject, UProperty * pProperty, 
	char * pData, int Index)
{
	UObjectData *   pObjectData;
	UObjectProp *   pObjectProp;
	UFunctionData * pFunctionData;

	if (!pProperty)
	{
		luaL_argcheck(L, 0, 2, "Property is NULL.");
		return;
	}

	if (pProperty->IsA(pFunctionClass))
	{
		pFunctionData = (UFunctionData *)lua_newuserdata(L, sizeof(*pFunctionData));
		pFunctionData->pObject = pObject;
		pFunctionData->pFunction = (UFunction *)pProperty;

		luaL_getmetatable(L, EngineUFunction);
		lua_setmetatable(L, -2);
		return;
	}

	if ((Index == NoIndex) && ((pProperty->ElementCount > 1) || 
		pProperty->IsA(pArrayPropertyClass)))
	{
		pObjectProp = (UObjectProp *)lua_newuserdata(L, sizeof(*pObjectProp));
		pObjectProp->pObject = pObject;
		pObjectProp->pProperty = pProperty;
		pObjectProp->pData = pData;

		luaL_getmetatable(L, EngineUProperty);
		lua_setmetatable(L, -2);
		return;
	}

  	//if its not an array, only then if elecount is 1 we have a problem
	if ((Index != NoIndex) && (pProperty->ElementCount == 1) && !pProperty->IsA(pArrayPropertyClass))
	{
		luaL_argcheck(L, 0, 2, "Property cannot be indexed.");
		return;
	}

	if (Index == NoIndex)
		Index = 0;
 	//if its not an array, only then if elecount is 1 we have a problem
	if (((Index < 0) || ((DWORD)Index >= pProperty->ElementCount))
     		&& !pProperty->IsA(pArrayPropertyClass))
	{
		luaL_argcheck(L, 0, 2, "Index out of range.");
		return;
	}


	if (pProperty->IsA(pArrayPropertyClass))
	{
		TArray<void> * pValue;

		pValue = (TArray<void> *)(pData + pProperty->CStructOffset); 
		if ((DWORD)Index >= pValue->Count)
		{
			luaL_argcheck(L, 0, 2, "Index out of range.");
			return;
		}

		pProperty = (UProperty *)pProperty->pRelatedClass;
		GetProperty(L, pObject, pProperty, (char *)((DWORD)pValue->pArray + 
			(Index * pProperty->ElementSize)), NoIndex);
	}

	else if (pProperty->IsA(pBoolPropertyClass))
	{
		int Value;

		Value = *(int *)(pData + pProperty->CStructOffset + 
			(pProperty->ElementSize * Index));
		Value &= (DWORD)pProperty->pRelatedClass;
		if (Value)
			Value = 1;
		lua_pushboolean(L, Value);
	}

	else if (pProperty->IsA(pBytePropertyClass))
	{
		int Value;

		Value = *(BYTE *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		lua_pushinteger(L, Value);
	}

	else if (pProperty->IsA(pClassPropertyClass) ||
		pProperty->IsA(pObjectPropertyClass) ||
    		pProperty->IsA(pInterfacePropertyClass))
	{
		UObject * pValue;

		pValue = *(UObject **)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));

		if (pValue)
		{
			pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
			pObjectData->pObject = pValue;
			pObjectData->pClass = pValue->pClass;
			pObjectData->pData = (char *)pValue;
			pObjectData->ActorIndex = (DWORD)-1;

			luaL_getmetatable(L, EngineUObject);
			lua_setmetatable(L, -2);
		}
		else
		{
			lua_pushnil(L);
		}
	}

	else if (pProperty->IsA(pFloatPropertyClass))
	{
		float Value;

		Value = *(float *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		lua_pushnumber(L, Value);
	}

	else if (pProperty->IsA(pIntPropertyClass))
	{
		int Value;

		Value = *(int *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		lua_pushinteger(L, Value);
	}

	else if (pProperty->IsA(pNamePropertyClass))
	{
		FName Value;

		Value = *(FName *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		lua_pushinteger(L, Value);
	}

	else if (pProperty->IsA(pPointerPropertyClass))
	{
		int Value;

		Value = *(int *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		lua_pushinteger(L, Value);
	}

	else if (pProperty->IsA(pStrPropertyClass))
	{
		TArray<WCHAR> * Value;

		Value = (TArray<WCHAR> *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		if (Value->pArray)
		{
			char * pName = new char[Value->Count];
			sprintf(pName, "%S", Value->pArray);
			lua_pushstring(L, pName);
			delete[] pName;
		}
		else
		{
			lua_pushnil(L);
		}
	}

	else if (pProperty->IsA(pStructPropertyClass))
	{
		pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
		pObjectData->pObject = pObject;
		pObjectData->pClass = pProperty->pRelatedClass;
		pObjectData->pData = pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index);
		pObjectData->ActorIndex = (DWORD)-1;

		luaL_getmetatable(L, EngineUObject);
		lua_setmetatable(L, -2);
	}

	//pDelegatePropertyClass
	//pFixedArrayPropertyClass
	//pMapPropertyClass
	else
	{
		luaL_argcheck(L, 0, 2, "Can't get this property type.");
	}
}


void InParam(lua_State * L, UProperty * pProperty, int ParamIndex, char * pParam)
{
	if (pProperty->IsA(pBoolPropertyClass))
	{
		int Value;

		luaL_checktype(L, ParamIndex, LUA_TBOOLEAN);
		Value = lua_toboolean(L, ParamIndex);
		*(int *)pParam = Value;
	}

	else if (pProperty->IsA(pBytePropertyClass))
	{
		int Value;

		Value = luaL_checkinteger(L, ParamIndex);
		*(BYTE *)pParam = (BYTE)Value;
	}


	else if (pProperty->IsA(pClassPropertyClass) ||
		pProperty->IsA(pObjectPropertyClass) ||
    		pProperty->IsA(pInterfacePropertyClass))
	{
		int           Found;
		UObjectData * pObjectData;
		UObjectProp * pObjectProp;


		lua_getfield(L, LUA_REGISTRYINDEX, EngineUObject);
		lua_getmetatable(L, ParamIndex);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (Found)
		{
			pObjectData = (UObjectData *)luaL_checkudata(L, ParamIndex, EngineUObject);
			*(UClass **)pParam = (UClass *)pObjectData->pData;

			return;
		}

		lua_getfield(L, LUA_REGISTRYINDEX, EngineUProperty);
		lua_getmetatable(L, ParamIndex);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (!Found)
			return;

		pObjectProp = (UObjectProp *)luaL_checkudata(L, ParamIndex, EngineUProperty);
		memcpy(pParam, pObjectProp->pData, 4);
	}

	else if (pProperty->IsA(pFloatPropertyClass))
	{
		float Value;

		Value = (float)luaL_checknumber(L, ParamIndex);
		*(float *)pParam = Value;
	}

	else if (pProperty->IsA(pIntPropertyClass))
	{
		int Value;

		Value = luaL_checkinteger(L, ParamIndex);
		*(int *)pParam = Value;
	}

	else if (pProperty->IsA(pNamePropertyClass))
	{
		FName Value;

		Value = luaL_checkinteger(L, ParamIndex);
		*(FName *)pParam = Value;
	}

	else if (pProperty->IsA(pStrPropertyClass))
	{
		const char * pValue;

		pValue = luaL_checkstring(L, ParamIndex);
		FStringConstructor(pParam, pValue);
	}

	else if (pProperty->IsA(pStructPropertyClass))
	{
		int           Found;
		UObjectData * pObjectData;
		UObjectProp * pObjectProp;


		lua_getfield(L, LUA_REGISTRYINDEX, EngineUObject);
		lua_getmetatable(L, ParamIndex);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (Found)
		{
			pObjectData = (UObjectData *)luaL_checkudata(L, ParamIndex, EngineUObject);
			memcpy(pParam, pObjectData->pData, pObjectData->pClass->Size);

			return;
		}

		lua_getfield(L, LUA_REGISTRYINDEX, EngineUProperty);
		lua_getmetatable(L, ParamIndex);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (!Found)
			return;

		pObjectProp = (UObjectProp *)luaL_checkudata(L, ParamIndex, EngineUProperty);
		memcpy(pParam, pObjectProp->pData, pProperty->ElementSize);
	}
}


void SetProperty(lua_State * L, UProperty * pProperty, char * pData, int Index)
{
	if ((Index == NoIndex) && (pProperty->ElementCount > 1))
	{
		luaL_argcheck(L, 0, 2, "Can't set multiple array elements.");
		return;
	}

	if ((Index != NoIndex) && (pProperty->ElementCount == 1) &&
		!pProperty->IsA(pArrayPropertyClass))
	{
		luaL_argcheck(L, 0, 2, "Property cannot be indexed.");
		return;
	}

	if (Index == NoIndex)
		Index = 0;

	if (((Index < 0) || ((DWORD)Index >= pProperty->ElementCount))
    		&& !pProperty->IsA(pArrayPropertyClass))
	{
		luaL_argcheck(L, 0, 2, "Index out of range.");
		return;
	}

	if (pProperty->IsA(pArrayPropertyClass))
	{
		TArray<void> * pArray;

		pArray = (TArray<void> *)(pData + pProperty->CStructOffset); 
		if ((DWORD)Index >= pArray->Count)
		{
			luaL_argcheck(L, 0, 2, "Index out of range.");
			return;
		}

		pProperty = (UProperty *)pProperty->pRelatedClass;
		SetProperty(L, pProperty, (char *)((DWORD)pArray->pArray + 
			(Index * pProperty->ElementSize)), NoIndex);
	}

	else if (pProperty->IsA(pBoolPropertyClass))
	{
		int Value;


		Value = (DWORD)pProperty->pRelatedClass;
		luaL_checktype(L, 3, LUA_TBOOLEAN);
		if (lua_toboolean(L, 3))
		{
			*(int *)(pData + pProperty->CStructOffset + 
				(pProperty->ElementSize * Index)) |= Value;
		}

		else
		{
			*(int *)(pData + pProperty->CStructOffset + 
				(pProperty->ElementSize * Index)) &= ~Value;
		}
	}

	else if (pProperty->IsA(pBytePropertyClass))
	{
		BYTE Value;

		Value = (BYTE)luaL_checkinteger(L, 3);
		*(BYTE *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index)) = Value;
	}

	else if (pProperty->IsA(pClassPropertyClass) ||
	    pProperty->IsA(pObjectPropertyClass) ||
      	    pProperty->IsA(pInterfacePropertyClass))
	{
		UObjectData * pValue;

		pValue = (UObjectData *)luaL_checkudata(L, 3, EngineUObject);
		*(UClass **)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index)) = (UClass *)pValue->pData;
	}

	else if (pProperty->IsA(pFloatPropertyClass))
	{
		float Value;

		Value = (float)luaL_checknumber(L, 3);
		*(float *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index)) = Value;
	}

	else if (pProperty->IsA(pIntPropertyClass))
	{
		int Value;

		Value = luaL_checkinteger(L, 3);
		*(int *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index)) = Value;
	}

	else if (pProperty->IsA(pNamePropertyClass))
	{
		FName Value;

		Value = (FName)luaL_checkinteger(L, 3);
		*(FName *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index)) = Value;
	}

	else if (pProperty->IsA(pPointerPropertyClass))
	{
		int Value;

		Value = luaL_checkinteger(L, 3);
		*(int *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index)) = Value;
	}

	else if (pProperty->IsA(pStrPropertyClass))
	{
		TArray<WCHAR> * Value;

		Value = (TArray<WCHAR> *)(pData + pProperty->CStructOffset +
			(pProperty->ElementSize * Index));
		if (Value->pArray)
			FStringDestructor(Value);

		if (!lua_isnil(L, 3))
			FStringConstructor(Value, luaL_checkstring(L, 3));
	}

	else if (pProperty->IsA(pStructPropertyClass))
	{
		UObjectData * pValue;

		pValue = (UObjectData *)luaL_checkudata(L, 3, EngineUObject);
		memcpy(pData + pProperty->CStructOffset + 
			(pProperty->ElementSize * Index), pValue->pData, 
			pProperty->ElementSize);
	}

	//pDelegatePropertyClass
	//pFixedArrayPropertyClass
	//pMapPropertyClass
	else
	{
		luaL_argcheck(L, 0, 2, "Can't set this property type.");
	}
}

void ValidateParam(lua_State * L, UProperty * pProperty, int ParamIndex)
{
	if (pProperty->IsA(pBoolPropertyClass))
	{
		luaL_checktype(L, ParamIndex, LUA_TBOOLEAN);
	}

	else if (pProperty->IsA(pBytePropertyClass))
	{
		luaL_checkinteger(L, ParamIndex);
	}

	else if (pProperty->IsA(pClassPropertyClass))
	{
		int           Found;
		UObjectData * pObjectData;
		UObjectProp * pObjectProp;


		lua_getfield(L, LUA_REGISTRYINDEX, EngineUObject);
		lua_getmetatable(L, ParamIndex);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (Found)
		{
			pObjectData = (UObjectData *)luaL_checkudata(L, ParamIndex, EngineUObject);
			if (!pObjectData->pObject || 
				!pObjectData->pObject->IsA(pClassClass) ||
				((char *)pObjectData->pObject != pObjectData->pData))
			{
				luaL_argcheck(L, 0, ParamIndex, "Expecting a UClass.");
			}

			return;
		}

		lua_getfield(L, LUA_REGISTRYINDEX, EngineUProperty);
		lua_getmetatable(L, ParamIndex);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (!Found)
			return;

		pObjectProp = (UObjectProp *)luaL_checkudata(L, ParamIndex, EngineUProperty);
		if (!pObjectProp->pProperty->IsA(pClassPropertyClass))
			luaL_argcheck(L, 0, ParamIndex, "Expecting a Class.");
	}

	else if (pProperty->IsA(pFloatPropertyClass))
	{
		luaL_checknumber(L, ParamIndex);
	}

	else if (pProperty->IsA(pIntPropertyClass))
	{
		luaL_checkinteger(L, ParamIndex);
	}

	else if (pProperty->IsA(pNamePropertyClass))
	{
		luaL_checkinteger(L, ParamIndex);
	}

	else if (pProperty->IsA(pObjectPropertyClass)
    		|| pProperty->IsA(pInterfacePropertyClass))
	{
		int           Found;
		UObjectData * pObjectData;
		UObjectProp * pObjectProp;

		lua_getfield(L, LUA_REGISTRYINDEX, EngineUObject);
		lua_getmetatable(L, ParamIndex);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (Found)
		{
			pObjectData = (UObjectData *)luaL_checkudata(L, ParamIndex, EngineUObject);
			if (!pObjectData->pObject || 
				((char *)pObjectData->pObject != pObjectData->pData))
			{
				luaL_argcheck(L, 0, ParamIndex, "Expecting a UObject.");
			}

			return;
		}

		lua_getfield(L, LUA_REGISTRYINDEX, EngineUProperty);
		lua_getmetatable(L, ParamIndex);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (!Found)
			return;

		pObjectProp = (UObjectProp *)luaL_checkudata(L, ParamIndex, EngineUProperty);
		if (!pObjectProp->pProperty->IsA(pObjectPropertyClass))
			luaL_argcheck(L, 0, ParamIndex, "Expecting a UObject.");
	}

	else if (pProperty->IsA(pStrPropertyClass))
	{
		luaL_checkstring(L, ParamIndex);
	}

	else if (pProperty->IsA(pStructPropertyClass))
	{
		int           Found;
		UObjectData * pObjectData;
		UObjectProp * pObjectProp;


		lua_getfield(L, LUA_REGISTRYINDEX, EngineUObject);
		lua_getmetatable(L, ParamIndex);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (Found)
		{
			pObjectData = (UObjectData *)luaL_checkudata(L, ParamIndex, EngineUObject);
			if (!pObjectData->pClass->IsA(pStructClass))
				luaL_argcheck(L, 0, ParamIndex, "Expecting a Struct.");

			return;
		}

		lua_getfield(L, LUA_REGISTRYINDEX, EngineUProperty);
		lua_getmetatable(L, ParamIndex);
		Found = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		if (!Found)
			return;

		pObjectProp = (UObjectProp *)luaL_checkudata(L, ParamIndex, EngineUProperty);
		if (!pObjectProp->pProperty->IsA(pStructPropertyClass))
			luaL_argcheck(L, 0, ParamIndex, "Expecting a Struct.");
	}

	//pArrayPropertyClass
	//pDelegatePropertyClass
	//pFixedArrayPropertyClass
	//pMapPropertyClass
	//pPointerPropertyClass
	else
	{
		luaL_argcheck(L, 0, ParamIndex, "Property type not implemented.");
	}
}


/******************************************************************************
*
* Lua AActors Functions
*
******************************************************************************/
int AActorsIndex(lua_State * L) 
{
	DWORD         Index;
	UObject *     pObject;
	UObjectData * pObjectData;


	if (lua_type(L, 2) == LUA_TSTRING)
	{
		pObject = FindActorByFullName(luaL_checkstring(L, 2));
		if (pObject)
		{
			pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
			pObjectData->pObject = pObject;
			pObjectData->pClass = pObject->pClass;
			pObjectData->pData = (char *)pObject;
			pObjectData->ActorIndex = (DWORD)-1;

			luaL_getmetatable(L, EngineUObject);
			lua_setmetatable(L, -2);
		}
		else
		{
			lua_pushnil(L);
		}
	}

	else
	{
		Index = luaL_checkinteger(L, 2);
		if (Index < pViewport->Actor->XLevel->ActorArray.Count)
		{
			if (pViewport->Actor->XLevel->ActorArray.pArray[Index])
			{
				pObject = pViewport->Actor->XLevel->ActorArray.pArray[Index];

				pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
				pObjectData->pObject = pObject;
				pObjectData->pClass = pObject->pClass;
				pObjectData->pData = (char *)pObject;
				pObjectData->ActorIndex = (DWORD)-1;

				luaL_getmetatable(L, EngineUObject);
				lua_setmetatable(L, -2);
			}
			else
			{
				lua_pushnil(L);
			}
		}
		else
		{
			luaL_argcheck(L, 0, 2, "Index out of range.");
		}
	}

	return(1);
}


int AActorsLen(lua_State * L) 
{
	lua_pushinteger(L, pViewport->Actor->XLevel->ActorArray.Count);

	return(1);
}



/******************************************************************************
*
* Lua FNames Functions
*
******************************************************************************/
int FNamesIndex(lua_State * L) 
{
	DWORD  Index;
	char   Name[MaxNameSize];
	WCHAR  WName[MaxNameSize];

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		wsprintfW(WName, L"%S", luaL_checkstring(L, 2));
		for (Index = 0; Index < pFNameEntryArray->Count; ++Index)
		{
			if (pFNameEntryArray->pArray[Index])
			{
				if (!wcscmp(WName, pFNameEntryArray->pArray[Index]->Name))
					break;
			}
		}

		if (Index < pFNameEntryArray->Count)
			lua_pushinteger(L, Index);
		else
			lua_pushnil(L);
	}

	else
	{
		Index = luaL_checkinteger(L, 2);
		if (Index < pFNameEntryArray->Count)
		{
			if (pFNameEntryArray->pArray[Index])
			{
				sprintf(Name, "%S", GetFName(Index));
				lua_pushstring(L, Name);
			}
			else
			{
				lua_pushnil(L);
			}
		}
		else
		{
			luaL_argcheck(L, 0, 2, "Index out of range.");
		}
	}

	return(1);
}


int FNamesLen(lua_State * L) 
{
	lua_pushinteger(L, pFNameEntryArray->Count);

	return(1);
}


/******************************************************************************
*
* Lua UFunction Functions
*
******************************************************************************/
int UFunctionCall(lua_State * L) 
{
	WORD            OldNativeIndex;
	UFunction *     pFunction;
	UFunctionData * pFunctionData;
	UObject *       pObject;
	char *          pParam;
	char *          pParamBlock;
	DWORD *         pProcessEvent;
	UProperty *     pProperty;
	int             ParamBlockSize;
	int             ParamIndex;
	int             Results;

	Results = 0;
	try
	{
		pFunctionData = (UFunctionData *)luaL_checkudata(L, 1, EngineUFunction);

		//Validate the parameters.
		ParamIndex = 2;
		ParamBlockSize = 0;
		pProperty = (UProperty *)pFunctionData->pFunction->pChildren;
		while (pProperty)
		{
			if (pProperty->IsA(pPropertyClass) && (pProperty->Flags & 0x80)) //CPF_Parm
			{
				ParamBlockSize += pProperty->ElementSize;

				if (!(pProperty->Flags & 0x400)) //CPF_ReturnParm
				{
					if (!(pProperty->Flags & 0x10) || (ParamIndex <= lua_gettop(L))) //CPF_OptionalParm
						ValidateParam(L, pProperty, ParamIndex);

					++ParamIndex;
				}
			}

			pProperty = (UProperty *)pProperty->pNext;
		}


		//Fill out the parameter block.
		pParamBlock = new char[ParamBlockSize];
		memset(pParamBlock, 0, ParamBlockSize);

		try
		{
			pParam = pParamBlock;
			ParamIndex = 2;
			pProperty = (UProperty *)pFunctionData->pFunction->pChildren;
			while (pProperty)
			{
				if (pProperty->IsA(pPropertyClass) && (pProperty->Flags & 0x80)) //CPF_Parm
				{
					if (!(pProperty->Flags & 0x400)) //CPF_ReturnParm
					{
						if (!(pProperty->Flags & 0x10) || (ParamIndex <= lua_gettop(L))) //CPF_OptionalParm
							InParam(L, pProperty, ParamIndex, pParam);

						++ParamIndex;
					}

					pParam += pProperty->ElementSize;
				}

				pProperty = (UProperty *)pProperty->pNext;
			}


			//Call the function.
			OldNativeIndex = pFunctionData->pFunction->NativeIndex;
			pFunctionData->pFunction->NativeIndex = 0;

			pObject = pFunctionData->pObject;
			pFunction = pFunctionData->pFunction;
			pProcessEvent = pObject->pVMT[4];

			__asm xor  eax,eax;
			__asm push eax;
			__asm mov  eax,pParamBlock;
			__asm push eax;
			__asm mov  eax,pFunction;
			__asm push eax;
			__asm mov  ecx,pObject;
			__asm call pProcessEvent;

			pFunctionData->pFunction->NativeIndex = OldNativeIndex;


			//Push the return value.
			pParam = pParamBlock;
			pProperty = (UProperty *)pFunctionData->pFunction->pChildren;
			while (pProperty)
			{
				if (pProperty->IsA(pPropertyClass) && (pProperty->Flags & 0x80)) //CPF_Parm
				{
					if (pProperty->Flags & 0x400) //CPF_ReturnParm
					{
						++Results;
						CreateReturn(L, pProperty, pParam);
					}

					pParam += pProperty->ElementSize;
				}

				pProperty = (UProperty *)pProperty->pNext;
			}


			//Push the out parameters, in order
			pParam = pParamBlock;
			pProperty = (UProperty *)pFunctionData->pFunction->pChildren;
			while (pProperty)
			{
				if (pProperty->IsA(pPropertyClass) && (pProperty->Flags & 0x80)) //CPF_Parm
				{
					if (pProperty->Flags & 0x400) //CPF_ReturnParm
					{
					}

					else if (pProperty->Flags & 0x100) //CPF_OutParm
					{
						++Results;
						CreateReturn(L, pProperty, pParam);
					}
          //Since FMalloc.Realloc is complex, use own destructor when it was an inparam
  				if (pProperty->IsA(pStrPropertyClass) && !(pProperty->Flags & 0x10) && !(pProperty->Flags & 0x400)) //CPF_OptionalParm
						FStringDestructor(pParam);

					pParam += pProperty->ElementSize;
				}

				pProperty = (UProperty *)pProperty->pNext;
			}
		}
		catch (...)
		{
			luaL_argcheck(L, 0, 1, "GPF in Call2");
		}

		delete[] pParamBlock;
	}
	catch(...)
	{
		luaL_argcheck(L, 0, 1, "GPF in Call");
		ResetPropertyCache();
	}

	return(Results);
}


int UFunctionIndex(lua_State * L) 
{
	DWORD           Index;
	UFunctionData * pFunctionData;


	pFunctionData = (UFunctionData *)luaL_checkudata(L, 1, EngineUFunction);
	Index = luaL_checkinteger(L, 2);
	if (Index < pFunctionData->pFunction->Script.Count)
		lua_pushinteger(L, pFunctionData->pFunction->Script.pArray[Index]);
	else
		luaL_argcheck(L, 0, 2, "Index out of range.");

	return(1);
}


int UFunctionNewIndex(lua_State * L) 
{
	DWORD           Index;
	UFunctionData * pFunctionData;
	DWORD           Value;


	pFunctionData = (UFunctionData *)luaL_checkudata(L, 1, EngineUFunction);
	Index = luaL_checkinteger(L, 2);
	Value = luaL_checkinteger(L, 3);
	if (Index < pFunctionData->pFunction->Script.Count)
		pFunctionData->pFunction->Script.pArray[Index] = (BYTE)Value;
	else
		luaL_argcheck(L, 0, 2, "Index out of range.");

	return(0);
}


int UFunctionLen(lua_State * L) 
{
	UFunctionData * pFunctionData;


	pFunctionData = (UFunctionData *)luaL_checkudata(L, 1, EngineUFunction);
	lua_pushinteger(L, pFunctionData->pFunction->Script.Count);

	return(1);
}


/******************************************************************************
*
* Lua UObject Functions
*
******************************************************************************/
int UObjectIndex(lua_State * L) 
{
	UProperty *   pProperty;
	const char *  pPropertyName;
	UObjectData * pObjectData;


	pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
	pPropertyName = lua_tostring(L, 2);

	pProperty = FindProperty(L, pObjectData, pObjectData->pClass, pPropertyName);
	GetProperty(L, pObjectData->pObject, pProperty, pObjectData->pData, NoIndex);

	return(1);
}


int UObjectNewIndex(lua_State * L) 
{
	UObjectData * pObjectData;
	UProperty *   pProperty;
	const char *  pPropertyName;


	pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
	pPropertyName = lua_tostring(L, 2);

	pProperty = FindProperty(L, pObjectData, pObjectData->pClass, pPropertyName);
	SetProperty(L, pProperty, pObjectData->pData, NoIndex);

	return(0);
}


int UObjectLen(lua_State * L) 
{
	UObjectData * pObjectData;

	pObjectData = (UObjectData *)luaL_checkudata(L, 1, EngineUObject);
	if ((char *)pObjectData->pObject != pObjectData->pData)
		luaL_argcheck(L, 0, 1, "Expecting a UObject");

	lua_pushinteger(L, pObjectData->pObject->ObjectInternal);

	return(1);
}


/******************************************************************************
*
* Lua UObjects Functions
*
******************************************************************************/
int UObjectsIndex(lua_State * L) 
{
	DWORD           Index;
	UFunctionData * pFunctionData;
	UObject *       pObject;
	UObjectData *   pObjectData;


	pObject = 0;
	if (lua_type(L, 2) == LUA_TSTRING)
	{
		pObject = FindObjectByFullName(luaL_checkstring(L, 2));
	}

	else
	{
		Index = luaL_checkinteger(L, 2);
		if (Index < pUObjectArray->Count)
			pObject = pUObjectArray->pArray[Index];
		else
			luaL_argcheck(L, 0, 2, "Index out of range.");
	}

	if (pObject)
	{
		if (pObject->IsA(pFunctionClass))
		{
			pFunctionData = (UFunctionData *)lua_newuserdata(L, sizeof(*pFunctionData));
			pFunctionData->pObject = pObject;
			pFunctionData->pFunction = (UFunction *)pObject;

			luaL_getmetatable(L, EngineUFunction);
			lua_setmetatable(L, -2);
		}

		else
		{
			pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
			pObjectData->pObject = pObject;
			pObjectData->pClass = pObject->pClass;
			pObjectData->pData = (char *)pObject;
			pObjectData->ActorIndex = (DWORD)-1;

			luaL_getmetatable(L, EngineUObject);
			lua_setmetatable(L, -2);
		}
	}
	else
	{
		lua_pushnil(L);
	}

	return(1);
}


int UObjectsLen(lua_State * L) 
{
	lua_pushinteger(L, pUObjectArray->Count);

	return(1);
}


/******************************************************************************
*
* Lua UProperty Functions
*
******************************************************************************/
int UPropertyIndex(lua_State * L) 
{
	int           Index;
	UObjectProp * pObjectProp;
	UProperty *   pProperty;
	const char *  pPropertyName;

	pObjectProp = (UObjectProp *)luaL_checkudata(L, 1, EngineUProperty);

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		pPropertyName = lua_tostring(L, 2);

		pProperty = FindProperty(L, NULL, pObjectProp->pProperty->pRelatedClass, 
			pPropertyName);
		GetProperty(L, pObjectProp->pObject, pProperty, 
			pObjectProp->pData + pObjectProp->pProperty->CStructOffset, NoIndex);
	}

	else
	{
		Index = luaL_checkinteger(L, 2);
		GetProperty(L, pObjectProp->pObject, pObjectProp->pProperty, 
			pObjectProp->pData, Index);
	}

	return(1);
}


int UPropertyNewIndex(lua_State * L) 
{
	int           Index;
	UObjectProp * pObjectProp;
	UProperty *   pProperty;
	const char *  pPropertyName;

	pObjectProp = (UObjectProp *)luaL_checkudata(L, 1, EngineUProperty);

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		pPropertyName = lua_tostring(L, 2);

		pProperty = FindProperty(L, NULL, pObjectProp->pProperty->pRelatedClass, 
			pPropertyName);
		SetProperty(L, pProperty, pObjectProp->pData + 
			pObjectProp->pProperty->CStructOffset, NoIndex);
	}

	else
	{
		Index = luaL_checkinteger(L, 2);
		SetProperty(L, pObjectProp->pProperty, pObjectProp->pData, Index);
	}

	return(0);
}

int UPropertyLen(lua_State * L) 
{
	UObjectProp * pObjectProp = (UObjectProp *)luaL_checkudata(L, 1, EngineUProperty);

  if (pObjectProp->pProperty->IsA(pArrayPropertyClass)) {
    lua_pushinteger(L, ((TArray<void> *)(pObjectProp->pData + pObjectProp->pProperty->CStructOffset))->Count);
  } else {
    lua_pushinteger(L, pObjectProp->pProperty->ElementCount);
  }

	return(1);
}

/******************************************************************************
*
* Lua Engine Library
*
******************************************************************************/
lua_State* L;


const struct luaL_reg lmLib[] = 
{
	{"CloseConsole", CloseConsole},
	{"Dump", Dump},
	{"DumpClass", DumpClass},
	{"FindFirst", FindFirst},
	{"FindFirstAActor", FindFirstAActor},
	{"FindNext", FindNext},
	{"FindNextAActor", FindNextAActor},
	{"FullName", FullName},
	{"IsA", IsA},
	{"Load", Load},
	{"Log", LogString},
	{"New", New},
	{"OpenConsole", OpenConsole},
	{"Restart", Restart},
	{"RestartLogfile", RestartLogfile},
	{"HexDump", LuaHexDump},
	{"DwordDump", LuaDwordDump},
	{"MemoryImageDump", LuaMemoryImageDump},
	{"GetKeyState", GetKeyState},
	{NULL, NULL}
};


int lua_openEngine(lua_State *L) 
{
	luaL_openlib(L, "lm", lmLib, 0);

	lua_pushstring(L, "AActors");
	lua_newuserdata(L, 1);
	luaL_newmetatable(L, "Engine.AActors");
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, AActorsIndex);
	lua_settable(L, -3);
	lua_pushstring(L, "__len");
	lua_pushcfunction(L, AActorsLen);
	lua_settable(L, -3);
	lua_setmetatable(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "FNames");
	lua_newuserdata(L, 1);
	luaL_newmetatable(L, "Engine.FNames");
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, FNamesIndex);
	lua_settable(L, -3);
	lua_pushstring(L, "__len");
	lua_pushcfunction(L, FNamesLen);
	lua_settable(L, -3);
	lua_setmetatable(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "UObjects");
	lua_newuserdata(L, 1);
	luaL_newmetatable(L, "Engine.UObjects");
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, UObjectsIndex);
	lua_settable(L, -3);
	lua_pushstring(L, "__len");
	lua_pushcfunction(L, UObjectsLen);
	lua_settable(L, -3);
	lua_setmetatable(L, -2);
	lua_settable(L, -3);

	luaL_newmetatable(L, EngineUFunction);
	lua_pushstring(L, "__call");
	lua_pushcfunction(L, UFunctionCall);
	lua_settable(L, -3);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, UFunctionIndex);
	lua_settable(L, -3);
	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, UFunctionNewIndex);
	lua_settable(L, -3);
	lua_pushstring(L, "__len");
	lua_pushcfunction(L, UFunctionLen);
	lua_settable(L, -3);
	lua_pop(L, 1);

	luaL_newmetatable(L, EngineUObject);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, UObjectIndex);
	lua_settable(L, -3);
	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, UObjectNewIndex);
	lua_settable(L, -3);
	lua_pushstring(L, "__len");
	lua_pushcfunction(L, UObjectLen);
	lua_settable(L, -3);
	lua_pop(L, 1);

	luaL_newmetatable(L, EngineUProperty);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, UPropertyIndex);
	lua_settable(L, -3);
	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, UPropertyNewIndex);
	lua_settable(L, -3);
	lua_pushstring(L, "__len");
	lua_pushcfunction(L, UPropertyLen);
	lua_settable(L, -3);
	lua_pop(L, 1);

	return(1);
}


void ShutdownLua(void)
{
	if (L)
	{
		Log("[*]: Lua System Shutdown");
		lua_close(L);
		L = 0;
	}
}


void InitLua(void)
{
	if (!L)
	{
		ResetPropertyCache();
		char szBotPath[MAX_PATH];

		Log("[*]: Lua System Enabled");
		fputs("Lua Restarted\n", stdout);

		if(hLogFile != NULL && bLogActive) //added
		{
			//fclose(hLogFile);
			//hLogFile = fopen(szLogFile, "w");
		}

		L = lua_open();
		luaL_openlibs(L);
		lua_openEngine(L);
		lua_LineageIIopen(L);
		lua_gc(L, LUA_GCRESTART, 0);

		lua_pushcfunction(L, Load);
		
		sprintf(szBotPath, "%s%s", Path, DEFAULT_BOTFILE);
		lua_pushstring(L, DEFAULT_BOTFILE);
		lua_pcall(L, 1, 0, 0);
		
		fputs("\n> ", stdout);
	}
}


/******************************************************************************
*
* Lua Callbacks.
*
******************************************************************************/
int KeyEvent(DWORD Key, DWORD Action, float Value)
{
	int Result;

	Result = 0;
	try
	{
		if (L)
		{
			lua_settop(L, 0);
			lua_getglobal(L, "KeyEvent");
			if (!lua_isnil(L, -1))
			{
				lua_pushinteger(L, Key);
				lua_pushinteger(L, Action);
				lua_pushnumber(L, Value);

				if (lua_pcall(L, 3, 1, 0))
				{
					const char * pError = lua_tostring(L, -1);
					if (pError)
						Log("KeyEvent Error: %s", pError);
				}

				if (!lua_isboolean(L, -1))
				{
					Log("KeyEvent Error: KeyEvent needs to return a boolean.");
				}
				else
				{
					Result = lua_toboolean(L, -1);
				}
			}
		}
	}
	catch(...)
	{
		ResetPropertyCache();
	}

	return(Result);
}


void PostRender(UObject * pCanvas)
{
	UObjectData * pObjectData;


	try
	{
		if (L)
		{
			lua_settop(L, 0);
			lua_getglobal(L, "PostRender");
			if (!lua_isnil(L, -1))
			{
				if (pCanvas)
				{
					pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
					pObjectData->pObject = pCanvas;
					pObjectData->pClass = pCanvas->pClass;
					pObjectData->pData = (char *)pCanvas;
					pObjectData->ActorIndex = (DWORD)-1;

					luaL_getmetatable(L, EngineUObject);
					lua_setmetatable(L, -2);
				}
				else
				{
					lua_pushnil(L);
				}

				if (lua_pcall(L, 1, 0, 0))
				{
					const char * pError = lua_tostring(L, -1);
					if (pError)
						Log("PostRender Error: %s", pError);
				}
			}
		}
	}
	catch(...)
	{
		ResetPropertyCache();
	}

}


void PreRender(UObject * pCanvas)
{
	UObjectData * pObjectData;


	try
	{
		if (L)
		{
			lua_settop(L, 0);
			lua_getglobal(L, "PreRender");
			if (!lua_isnil(L, -1))
			{
				if (pCanvas)
				{
					pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
					pObjectData->pObject = pCanvas;
					pObjectData->pClass = pCanvas->pClass;
					pObjectData->pData = (char *)pCanvas;
					pObjectData->ActorIndex = (DWORD)-1;

					luaL_getmetatable(L, EngineUObject);
					lua_setmetatable(L, -2);
				}
				else
				{
					lua_pushnil(L);
				}

				if (lua_pcall(L, 1, 0, 0))
				{
					const char * pError = lua_tostring(L, -1);
					if (pError)
						Log("PreRender Error: %s", pError);
				}
			}
		}
	}
	catch(...)
	{
		ResetPropertyCache();
	}
}

void DumpUser(User * PlayerUser)
{
	if (PlayerUser) {
		Log("PlayerUser->SummonedID = %i", PlayerUser->SummonedID);	
		Log("PlayerUser->PetID = %i", PlayerUser->PetID);
		Log("PlayerUser->Unknown3 = %i", PlayerUser->Unknown3);
		Log("PlayerUser->bCanBeAttacked = %i", PlayerUser->bCanBeAttacked);
		Log("PlayerUser->NPCID = %i", PlayerUser->NPCID);
		Log("PlayerUser->Class = %i", PlayerUser->Class);
		Log("PlayerUser->Name = %ws", PlayerUser->Name);
		Log("PlayerUser->Race = %i", PlayerUser->Race);
		Log("PlayerUser->Gender = %i", PlayerUser->Gender);
		Log("PlayerUser->ClassType = %i", PlayerUser->ClassType);
		Log("PlayerUser->Level = %i", PlayerUser->Level);					//58
		//Log("PlayerUser->Exp = %016I64x", PlayerUser->Exp);
		Log("PlayerUser->ExpHigh = %i", PlayerUser->ExpHigh);
		Log("PlayerUser->ExpLow = %i", PlayerUser->ExpLow);
		Log("PlayerUser->Str = %i", PlayerUser->Str);					//64
		Log("PlayerUser->Dex = %i", PlayerUser->Dex);					//68
		Log("PlayerUser->Con = %i", PlayerUser->Con);					//6C
		Log("PlayerUser->Int = %i", PlayerUser->Int);					//70
		Log("PlayerUser->Wit = %i", PlayerUser->Wit);					//74
		Log("PlayerUser->Men = %i", PlayerUser->Men);					//78
		Log("PlayerUser->MaxHP = %i", PlayerUser->MaxHP);					//7C
		Log("PlayerUser->HP = %i", PlayerUser->HP);						//80
		Log("PlayerUser->MaxMP = %i", PlayerUser->MaxMP);					//84
		Log("PlayerUser->MP = %i", PlayerUser->MP);						//88
		Log("PlayerUser->CarryWeight = %i", PlayerUser->CarryWeight);			//8C
		Log("PlayerUser->AttackRange = %i", PlayerUser->AttackRange);			//90
		Log("PlayerUser->bTransformed = %i", PlayerUser->bTransformed);
		Log("PlayerUser->EQUIPITEM_Underwear = %i", PlayerUser->EQUIPITEM_Underwear);
		Log("PlayerUser->EQUIPITEM_LEar = %i", PlayerUser->EQUIPITEM_LEar);
		Log("PlayerUser->EQUIPITEM_REar = %i", PlayerUser->EQUIPITEM_REar);
		Log("PlayerUser->EQUIPITEM_Neck = %i", PlayerUser->EQUIPITEM_Neck);
		Log("PlayerUser->EQUIPITEM_RFinger = %i", PlayerUser->EQUIPITEM_RFinger);
		Log("PlayerUser->EQUIPITEM_LFinger = %i", PlayerUser->EQUIPITEM_LFinger);
		Log("PlayerUser->EQUIPITEM_Head = %i", PlayerUser->EQUIPITEM_Head);
		Log("PlayerUser->EQUIPITEM_RHand = %i", PlayerUser->EQUIPITEM_RHand);
		Log("PlayerUser->EQUIPITEM_LHand = %i", PlayerUser->EQUIPITEM_LHand);
		Log("PlayerUser->EQUIPITEM_Gloves = %i", PlayerUser->EQUIPITEM_Gloves);
		Log("PlayerUser->EQUIPITEM_Chest = %i", PlayerUser->EQUIPITEM_Chest);
		Log("PlayerUser->EQUIPITEM_Legs = %i", PlayerUser->EQUIPITEM_Legs);
		Log("PlayerUser->EQUIPITEM_Feet = %i", PlayerUser->EQUIPITEM_Feet);
		Log("PlayerUser->Unknown51 = %i", PlayerUser->Unknown51);
		Log("PlayerUser->Unknown52 = %i", PlayerUser->Unknown52);
		Log("PlayerUser->Unknown53 = %i", PlayerUser->Unknown53);
		Log("PlayerUser->Unknown54 = %i", PlayerUser->Unknown54);
		Log("PlayerUser->Unknown55 = %i", PlayerUser->Unknown55);
		Log("PlayerUser->Unknown56 = %i", PlayerUser->Unknown56);
		Log("PlayerUser->Unknown57 = %i", PlayerUser->Unknown57);
		Log("PlayerUser->Unknown58 = %i", PlayerUser->Unknown58);
		Log("PlayerUser->Unknown59 = %i", PlayerUser->Unknown59);
		Log("PlayerUser->Unknown60 = %i", PlayerUser->Unknown60);
		Log("PlayerUser->Unknown61 = %i", PlayerUser->Unknown61);
		Log("PlayerUser->Unknown62 = %i", PlayerUser->Unknown62);
		Log("PlayerUser->Unknown63 = %i", PlayerUser->Unknown63);
		Log("PlayerUser->Unknown64 = %i", PlayerUser->Unknown64);
		Log("PlayerUser->Unknown65 = %i", PlayerUser->Unknown65);
		Log("PlayerUser->Unknown66 = %i", PlayerUser->Unknown66);
		Log("PlayerUser->Unknown67 = %i", PlayerUser->Unknown67);
		Log("PlayerUser->Unknown68 = %i", PlayerUser->Unknown68);
		Log("PlayerUser->Unknown69 = %i", PlayerUser->Unknown69);
		Log("PlayerUser->Unknown70 = %i", PlayerUser->Unknown70);
		Log("PlayerUser->Unknown71 = %i", PlayerUser->Unknown71);
		Log("PlayerUser->Unknown72 = %i", PlayerUser->Unknown72);
		Log("PlayerUser->Unknown73 = %i", PlayerUser->Unknown73);
		Log("PlayerUser->Unknown74 = %i", PlayerUser->Unknown74);
		Log("PlayerUser->Unknown75 = %i", PlayerUser->Unknown75);
		Log("PlayerUser->Unknown76 = %i", PlayerUser->Unknown76);
		Log("PlayerUser->Unknown77 = %i", PlayerUser->Unknown77);
		Log("PlayerUser->Unknown78 = %i", PlayerUser->Unknown78);
		Log("PlayerUser->Unknown79 = %i", PlayerUser->Unknown79);
		Log("PlayerUser->Unknown80 = %i", PlayerUser->Unknown80);
		Log("PlayerUser->Unknown81 = %i", PlayerUser->Unknown81);
		Log("PlayerUser->Unknown82 = %i", PlayerUser->Unknown82);
		Log("PlayerUser->Unknown83 = %i", PlayerUser->Unknown83);
		Log("PlayerUser->Unknown84 = %i", PlayerUser->Unknown84);
		Log("PlayerUser->Unknown85 = %i", PlayerUser->Unknown85);
		Log("PlayerUser->Unknown86 = %i", PlayerUser->Unknown86);
		Log("PlayerUser->Unknown87 = %i", PlayerUser->Unknown87);
		Log("PlayerUser->Unknown88 = %i", PlayerUser->Unknown88);
		Log("PlayerUser->Unknown89 = %i", PlayerUser->Unknown89);
		Log("PlayerUser->Unknown90 = %i", PlayerUser->Unknown90);
		Log("PlayerUser->Unknown91 = %i", PlayerUser->Unknown91);
		Log("PlayerUser->Unknown92 = %i", PlayerUser->Unknown92);
		Log("PlayerUser->Unknown93 = %i", PlayerUser->Unknown93);
		Log("PlayerUser->Unknown94 = %i", PlayerUser->Unknown94);
		Log("PlayerUser->Unknown95 = %i", PlayerUser->Unknown95);
		Log("PlayerUser->Unknown96 = %i", PlayerUser->Unknown96);
		Log("PlayerUser->Unknown97 = %i", PlayerUser->Unknown97);
		Log("PlayerUser->Unknown98 = %i", PlayerUser->Unknown98);
		Log("PlayerUser->Unknown99 = %i", PlayerUser->Unknown99);
		Log("PlayerUser->NickColor = 0x%08x", PlayerUser->NickColor);			//190
		Log("PlayerUser->Guilty = %i", PlayerUser->Guilty);					//194
		Log("PlayerUser->CriminalRate = %i", PlayerUser->CriminalRate);			//198
		Log("PlayerUser->Unknown103 = %i", PlayerUser->Unknown103);
		Log("PlayerUser->Unknown104 = %i", PlayerUser->Unknown104);
		Log("PlayerUser->Unknown105 = %i", PlayerUser->Unknown105);
		Log("PlayerUser->Unknown106 = %i", PlayerUser->Unknown106);
		Log("PlayerUser->Unknown107 = %i", PlayerUser->Unknown107);
		Log("PlayerUser->Unknown108 = %i", PlayerUser->Unknown108);
		Log("PlayerUser->Unknown109 = %i", PlayerUser->Unknown109);
		Log("PlayerUser->Unknown110 = %i", PlayerUser->Unknown110);				//1B8 64 on MakeZ
		Log("PlayerUser->Unknown111 = 0x%08x", PlayerUser->Unknown111);				//1BC c0000000 on MakeZ float
		Log("PlayerUser->Unknown112 = 0x%08x", PlayerUser->Unknown112);				//1C0 caa1fb3f on MakeZ
		Log("PlayerUser->Unknown113 = 0x%08x", PlayerUser->Unknown113);				//1C4 00000060 on MakeZ
		Log("PlayerUser->Unknown114 = 0x%08x", PlayerUser->Unknown114);				//1C8 5555f13f on MakeZ
		Log("PlayerUser->Unknown115 = %f", PlayerUser->Unknown115);				//1CC 00001041 on MakeZ float
		Log("PlayerUser->Unknown116 = %f", PlayerUser->Unknown116);				//1D0 00009041 on MakeZ float
		Log("PlayerUser->Unknown117 = %i", PlayerUser->Unknown117);				//1D4	initialised to 0
		Log("PlayerUser->Unknown118 = %i", PlayerUser->Unknown118);
		Log("PlayerUser->Unknown119 = %i", PlayerUser->Unknown119);
		Log("PlayerUser->Unknown120 = %i", PlayerUser->Unknown120);
		Log("PlayerUser->Unknown121 = %i", PlayerUser->Unknown121);
		Log("PlayerUser->Unknown122 = %i", PlayerUser->Unknown122);
		Log("PlayerUser->Unknown123 = %i", PlayerUser->Unknown123);
		Log("PlayerUser->Unknown124 = %i", PlayerUser->Unknown124);
		Log("PlayerUser->Unknown125 = %i", PlayerUser->Unknown125);
		Log("PlayerUser->Unknown126 = %i", PlayerUser->Unknown126);
		Log("PlayerUser->Unknown127 = %i", PlayerUser->Unknown127);
		Log("PlayerUser->Unknown128 = %i", PlayerUser->Unknown128);
		Log("PlayerUser->Pawn = 0x%08x", PlayerUser->Pawn);			//204
		Log("PlayerUser->Unknown130 = %i", PlayerUser->Unknown130);
		Log("PlayerUser->Unknown131 = %i", PlayerUser->Unknown131);
		//Log("PlayerUser->Unknown132 = %i", PlayerUser->Unknown132);
		Log("PlayerUser->CarryingWeight = %i", PlayerUser->CarryingWeight);
		Log("PlayerUser->SP = %i", PlayerUser->SP);
		Log("PlayerUser->HitRate = %i", PlayerUser->HitRate);
		Log("PlayerUser->CriticalRate = %i", PlayerUser->CriticalRate);
		Log("PlayerUser->PhysicalAttack = %i", PlayerUser->PhysicalAttack);
		Log("PlayerUser->PhysicalAttackSpeed = %i", PlayerUser->PhysicalAttackSpeed);
		Log("PlayerUser->PhysicalDefence = %i", PlayerUser->PhysicalDefence);
		Log("PlayerUser->PhysicalAvoid = %i", PlayerUser->PhysicalAvoid);
		Log("PlayerUser->MagicalAttack = %i", PlayerUser->MagicalAttack);
		Log("PlayerUser->MagicDefense = %i", PlayerUser->MagicDefense);
		Log("PlayerUser->MagicCastingSpeed = %i", PlayerUser->MagicCastingSpeed);
		Log("PlayerUser->Unknown144 = %i", PlayerUser->Unknown144);
		Log("PlayerUser->Unknown145 = %i", PlayerUser->Unknown145);
		Log("PlayerUser->Unknown146 = %i", PlayerUser->Unknown146);
		Log("PlayerUser->Unknown147 = %i", PlayerUser->Unknown147);
		Log("PlayerUser->Unknown148 = %i", PlayerUser->Unknown148);
		Log("PlayerUser->NickName = %ws", PlayerUser->NickName);				//254
		Log("PlayerUser->PledgeID = %i", PlayerUser->PledgeID);				//284
		Log("PlayerUser->Unknown162 = %i", PlayerUser->Unknown162);
		Log("PlayerUser->Unknown163 = %i", PlayerUser->Unknown163);
		Log("PlayerUser->Unknown164 = %i", PlayerUser->Unknown164);
		Log("PlayerUser->Pledge = %i", PlayerUser->Pledge);					//294
		Log("PlayerUser->Unknown166 = %i", PlayerUser->Unknown166);
		Log("PlayerUser->SurrenderWarID = %i", PlayerUser->SurrenderWarID);			//29C
		Log("PlayerUser->Unknown168 = %i", PlayerUser->Unknown168);
		Log("PlayerUser->Unknown169 = %i", PlayerUser->Unknown169);
		Log("PlayerUser->Unknown170 = %i", PlayerUser->Unknown170);
		Log("PlayerUser->Unknown171 = %i", PlayerUser->Unknown171);
		Log("PlayerUser->PrivateStoreState = %i", PlayerUser->PrivateStoreState);		//2B0
		Log("PlayerUser->PKCount    = %i", PlayerUser->PKCount);
		Log("PlayerUser->PvPCount   = %i", PlayerUser->PvPCount);
		Log("PlayerUser->Unknown175 = %i", PlayerUser->Unknown175);
		Log("PlayerUser->ActiveClassType = %i", PlayerUser->ActiveClassType);
		Log("PlayerUser->MaxCP = %i", PlayerUser->MaxCP);					//2C4
		Log("PlayerUser->CP = %i", PlayerUser->CP);						//2C8
		Log("PlayerUser->Unknown179 = %i", PlayerUser->Unknown179);
		Log("PlayerUser->Unknown180 = %i", PlayerUser->Unknown180);
		Log("PlayerUser->Unknown181 = %i", PlayerUser->Unknown181);
		Log("PlayerUser->Unknown182 = %i", PlayerUser->Unknown182);
		Log("PlayerUser->Unknown183 = %i", PlayerUser->Unknown183);
		Log("PlayerUser->Unknown184 = %i", PlayerUser->Unknown184);
		Log("PlayerUser->Unknown185 = %i", PlayerUser->Unknown185);
		Log("PlayerUser->Unknown186 = %i", PlayerUser->Unknown186);
		Log("PlayerUser->Unknown187 = %i", PlayerUser->Unknown187);
		Log("PlayerUser->Unknown188 = %i", PlayerUser->Unknown188);
		Log("PlayerUser->Unknown189 = %i", PlayerUser->Unknown189);
		Log("PlayerUser->Unknown190 = %i", PlayerUser->Unknown190);
		Log("PlayerUser->Unknown191 = %i", PlayerUser->Unknown191);
		Log("PlayerUser->Unknown192 = %i", PlayerUser->Unknown192);
		Log("PlayerUser->Unknown193 = %i", PlayerUser->Unknown193);
		Log("PlayerUser->UniqueNameColor = 0x%08x", PlayerUser->UniqueNameColor);	//308	initialised to 0FFFFFFFFh
		Log("PlayerUser->Unknown195 = %i", PlayerUser->Unknown195);
		Log("PlayerUser->Unknown196 = %i", PlayerUser->Unknown196);
		
		HexDump((char *)PlayerUser, 197*4);
	}
}



void Tick(float DeltaTime)
{
	UObjectData * pObjectData;

	//Reload Lua Script
	EngineInit();
	if (RestartLua)
	{
		RestartLua = 0;
		ShutdownLua();
	}
	InitLua();

	char szWindowText[50];
	HWND fgw;

	if((fgw = GetForegroundWindow()) != NULL)
	{
		GetWindowText(fgw, szWindowText, sizeof(szWindowText));

   		if(strstr(szWindowText, WINDOWTEXT))
		{
			if ((GetAsyncKeyState(BASEKEY) & 0x8000) &&
				(GetAsyncKeyState((BYTE)VkKeyScan(RESTART_KEY)) & 0x8000))
			{
				RestartLua = true;
				Sleep(250);
			}

			if ((GetAsyncKeyState(BASEKEY) & 0x8000) &&
				(GetAsyncKeyState((BYTE)VkKeyScan(OPEN_LOG_KEY)) & 0x8000) &&
				bLogActive)
			{
				ShellExecute(0, "open", "notepad.exe", szLogFile, "", 1);
				Sleep(250);
			}

			// In Tick for dumping the game when you want
			if ((GetAsyncKeyState(BASEKEY) & 0x8000) &&
				(GetAsyncKeyState((BYTE)VkKeyScan('g')) & 0x8000))
			{
				//HMODULE hModule = LoadLibraryA("C:\\UT\\SDKGenerator\\SDKGenerator.dll");
				////HMODULE hModule = LoadLibraryA("C:\\UT\\LinTest\\Release\\Lin.dll");
				//Log("hModule = %ld", hModule);

				DumpUser(PlayerUser);

				/*
				list<void *>::iterator it;
				for (it=NPCUserList.begin(); it!=NPCUserList.end(); it++) {
					User * NPCUser;
					NPCUser = (User *)(*it);
					if (NPCUser 
					&& NPCUser->SummonedID == 0 
					&& PlayerUser->Pawn
					&& PlayerUser->UniqueNameColor == 0xffffffff 
					&& PlayerUser->NPCID <= 35566 
					&& PlayerUser->NPCID >= 0) {
						DumpUser(NPCUser);
					}
				}
				*/
			}

			//Toggle the console
			static int ConsoleOneShot;
			if ((GetAsyncKeyState(BASEKEY) & 0x8000) &&
				(GetAsyncKeyState((BYTE)VkKeyScan(OPEN_LUA_CONSOLE)) & 0x8000))
			{
				if (!ConsoleOneShot)
				{
					ConsoleOneShot = 1;
					if (hConsole)
						CloseConsole(L);
					else
						OpenConsole(L);
				}
			}
			else
			{
				ConsoleOneShot = 0;
			}
		}
	}
	
	if (hConsole && Input[0])
	{
		try
		{
			if (L)
			{
				lua_settop(L, 0);
				if (luaL_dostring(L, &Input[1]))
				{
					const char * pError = lua_tostring(L, -1);
					fputs("Console Error: ", stdout);
					if (pError)
					{
						fputs(pError, stdout);
						Log("Console Error: %s", pError);
					}
					else
					{
						fputs("[unknown]", stdout);
						Log("Console Error: [unknown]");
					}

					fputs("\n\n", stdout);
				}

				fputs("> ", stdout);

				Input[0] = 0;
				SetEvent(hInputEvent);
			}
		}
		catch(...)
		{
			ResetPropertyCache();
		}
	}

	try
	{
		if (L)
		{
			lua_settop(L, 0);
			lua_getglobal(L, "Tick");
			if (!lua_isnil(L, -1))
			{
				lua_pushnumber(L, DeltaTime);

				if (pViewport)
				{
			       	//if (!pAActorArray) {
					//	pAActorArray = &(((ULevel*) FindObjectByFullName(LEVEL_NAME))->ActorArray);
				    //}

					pObjectData = (UObjectData *)lua_newuserdata(L, sizeof(*pObjectData));
					pObjectData->pObject = pViewport;
					pObjectData->pClass = pViewport->pClass;
					pObjectData->pData = (char *)pViewport;
					pObjectData->ActorIndex = (DWORD)-1;

					luaL_getmetatable(L, EngineUObject);
					lua_setmetatable(L, -2);
				}
				else
				{
					lua_pushnil(L);
				}

				if (lua_pcall(L, 2, 0, 0))
				{
					const char * pError = lua_tostring(L, -1);
					if (pError)
						Log("Tick Error: %s", pError);
				}
			}
		}
	}
	catch(...)
	{
		ResetPropertyCache();
	}
}

/******************************************************************************
*
* DLL Attach/Detach
*
******************************************************************************/
void ProcessAttach(HINSTANCE hInstance)
{
	char FileName[MAX_PATH];
	char* pChar;
	SYSTEMTIME st;

	GetSystemTime(&st);

	//get base path
	GetModuleFileName(hInstance, FileName, sizeof(FileName));

	//Strip our file from the path.
	strcpy(Path, FileName);
	pChar = strrchr(Path, '\\');
	if (pChar)
		pChar[1] = 0;

	//Start a log file.
	if(strlen(DEFAULT_LOGFILE) > 0)
	{
		sprintf(szLogFile, "%s%04d%02d%02d%02d%02d%02d_%s", Path, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, DEFAULT_LOGFILE);
		hLogFile = fopen(szLogFile, "w");
		bLogActive = true;
		
		Log("[*]: System Online");
	}

	//Create the event.
	hInputEvent = CreateEvent(0, 0, 0, 0);
}

void ProcessDetach()
{
	CloseConsole(0);
	CloseHandle(hInputEvent);
	ShutdownLua();

	Log("[*]: System offline");
	
	if(hLogFile)
		fclose(hLogFile);
}

/******************************************************************************
*
* DLL initialization code.
*
******************************************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, PVOID )
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			ProcessAttach(hInstance);
			HookEngine();
			Log("HMODULE hInstance = 0x%08x", hInstance);
		}
		break;
		
		case DLL_PROCESS_DETACH:
			ProcessDetach();
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}
