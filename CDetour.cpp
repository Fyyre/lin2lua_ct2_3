#include "Inc.h"

#include "CDetour.h"
#include "hde32.h"

//#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
//#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1

//extern void Log(char * pFormat, ...);

#pragma pack(1)
struct _CDetour_nrc_pre_gate_1_s {
	BYTE op_c605[2];		//mov byte ptr
	BYTE *ret_bool;
	BYTE one;
	
	BYTE op_8f05[2];		//pop dword ptr ds:[
	DWORD *ra_saver;
	
	BYTE op_ff15[2];		//call
	DWORD *pfunc_to_call;
	
	BYTE op_ff35[2];		//push dword ptr ds:[
	DWORD *ra_saver2;
	
	BYTE op_803d[2];		//cmp byte ptr
	BYTE *ret_bool2;
	BYTE one2;
	
	BYTE op_75;				//jne
	BYTE ow_len_plus_6;
};

struct _CDetour_rc_pre_gate_1_s {
	BYTE op_803d[2];		//cmp byte ptr
	BYTE *rcf;
	BYTE one;
	
	BYTE op_74;				//je
	BYTE ofs_ow_start;

	BYTE op_c605[2];		//mov byte ptr
	BYTE *rcf2;
	BYTE one2;

	BYTE op_c605_2[2];		//mov byte ptr
	BYTE *ret_bool;
	BYTE one3;
	
	BYTE op_8f05[2];		//pop dword ptr ds:[
	DWORD *ra_saver;
	
	BYTE op_ff15[2];		//call
	DWORD *pfunc_to_call;
	
	BYTE op_ff35[2];		//push dword ptr ds:[
	DWORD *ra_saver2;
	
	BYTE op_c605_3[2];		//mov byte ptr
	BYTE *rcf3;
	BYTE zero;

	BYTE op_803d_2[2];		//cmp byte ptr
	BYTE *ret_bool2;
	BYTE one4;
	
	BYTE op_75;				//jne
	BYTE ow_len_plus_6;
};

struct _CDetour_pre_gate_2_s {
	BYTE op_68;				//push
	DWORD return_addr;
	
	BYTE op_c3c2[2];		//ret; ret <#>
	WORD ret_num;
};
#pragma pack()

CDetour::CDetour()
{
	Deconstruct();
}

CDetour::~CDetour()
{
	Deconstruct();
}

void CDetour::Deconstruct()
{
	m_bReady = false;

	if(m_bApplied)
		Remove();

	if(m_pOverwrittenOpsBuf)
		delete m_pOverwrittenOpsBuf;

	if(m_pPatchBuf)
		delete m_pPatchBuf;

	if(m_pGate)
		delete m_pGate;

	m_dwAddr = 0;
	m_iNumArgs = 0;
	m_dwFuncToCall = 0;
	m_iPatchType = 0;
	m_bRecursionCheck = false;
	
	m_pOverwrittenOpsBuf = (BYTE*)0;
	m_iOverwrittenOpsBufLen = 0;

	m_pPatchBuf = (BYTE*)0;
	m_iPatchBufLen = 0;

	m_pGate = (BYTE*)0;
	m_iGateLen = 0;

	m_bReady = false;
	m_bApplied = false;

	m_dwRetAddress = 0;
	m_bReturnToOriginal = false;
	m_bRecursionFlag = false;
}

bool CDetour::Detour(DWORD dwAddr, DWORD dwFuncToCall, int iNumArgs, bool bRecursionCheck, int iPatchType, int iOpsToOverwrite)
{
	Deconstruct();

	if (iPatchType == DETOUR_TYPE_OBS_RAND) {
		m_iPatchType = (rand() % (DetourRandTypeHigh - DetourRandTypeLow + 1) + DetourRandTypeLow);

	}
	else
		m_iPatchType = iPatchType;

	int iMinOverwrittenOpsLen = GetDetourLen(m_iPatchType);
	if (!iMinOverwrittenOpsLen)
		return false;

	int iCurOpLen = 0;

	if (iOpsToOverwrite != 0) {
		iCurOpLen = iOpsToOverwrite;

	}
	else
	{
		BYTE* pCurOp = (BYTE*)dwAddr;

		while (iCurOpLen < iMinOverwrittenOpsLen)
		{
			int i = hde32_oplength(pCurOp);
			if (i == 0 || i == -1)
				return false;

			iCurOpLen += i;
			pCurOp += i;

		}

		m_iOverwrittenOpsBufLen = iCurOpLen;

		/*
		m_pOverwrittenOpsBuf = new BYTE[m_iOverwrittenOpsBufLen];
		if(!m_pOverwrittenOpsBuf) {
			return false;
		}
		*/
		memset(&m_OverwrittenOpsBuf, 0, sizeof(m_OverwrittenOpsBuf));
		m_pOverwrittenOpsBuf = &m_OverwrittenOpsBuf[0];
		//Log("m_iOverwrittenOpsBufLen = %ld", m_iOverwrittenOpsBufLen);
		if (m_iOverwrittenOpsBufLen > sizeof(m_OverwrittenOpsBuf)) {
			//Log("Detour m_iOverwrittenOpsBufLen > sizeof(m_OverwrittenOpsBuf) failed (%ld > %ld)", m_iOverwrittenOpsBufLen , sizeof(m_OverwrittenOpsBuf));
			return false;
		}
		//Log("Detour m_iOverwrittenOpsBufLen > sizeof(m_OverwrittenOpsBuf) succeeded");

		if (!memcpy((void*)m_pOverwrittenOpsBuf, (void*)dwAddr, m_iOverwrittenOpsBufLen))
		{
			//Log("Detour memcpy failed");
			Deconstruct();
			return false;
		}
		//Log("Detour memcpy succeeded");

		m_dwAddr = dwAddr;
		m_iNumArgs = iNumArgs;
		m_dwFuncToCall = dwFuncToCall;
		m_bRecursionCheck = bRecursionCheck;

		m_iPatchBufLen = iCurOpLen;

		if (!PrepareGate()) {
			//Log("Detour PrepareGate failed");
			Deconstruct();
			return false;
		}
		//Log("Detour PrepareGate succeeded");

		if (!PreparePatch()) {
			//Log("Detour PreparePatch failed");
			Deconstruct();
			return false;
		}

		m_bReady = true;

		//Log("~Detour");
		return true;
	}
}

bool CDetour::Apply()
{
	if(!m_bReady || m_bApplied)
		return false;;

	DWORD dwOldProt;
	DWORD dwDummy;

	if(!VirtualProtect(GetCurrentProcess(), (void*)m_dwAddr, m_iPatchBufLen, PAGE_EXECUTE_READWRITE, &dwOldProt)){
//Log("Apply.VirtualProtect failed");
		return false;
	}
	
	if(!memcpy((void*)m_dwAddr, (void*)m_pPatchBuf, m_iPatchBufLen)){
//Log("Apply.memcpy failed");
		return false;
	}

	if(!VirtualProtect(GetCurrentProcess(), (void*)m_dwAddr, m_iPatchBufLen, dwOldProt, &dwDummy)){
//Log("Apply.VirtualProtect failed");
	}

	m_bApplied = true;

	return true;
}

bool CDetour::Remove()
{
	if(!m_bApplied)
		return false;

	DWORD dwOldProt;
	DWORD dwDummy;

	if(!VirtualProtect(GetCurrentProcess(), (void*)m_dwAddr, m_iOverwrittenOpsBufLen, PAGE_EXECUTE_READWRITE, &dwOldProt))
		return false;

	if(!memcpy((void*)m_dwAddr, (void*)m_pOverwrittenOpsBuf, m_iOverwrittenOpsBufLen))
		return false;

	VirtualProtect(GetCurrentProcess(), (void*)m_dwAddr, m_iOverwrittenOpsBufLen, dwOldProt, &dwDummy);

	m_bApplied = false;

	return true;
}

DWORD CDetour::GetRetAddress(){
	if(!m_bReady)
		return 0;

	return m_dwRetAddress;
}

void CDetour::Ret(bool bReturnToOriginal)
{
	m_bReturnToOriginal = bReturnToOriginal;
}

bool CDetour::PrepareGate()
{
	int iGateBufLen;
	
	if(m_bRecursionCheck)
		iGateBufLen = sizeof(_CDetour_rc_pre_gate_1_s) + sizeof(_CDetour_pre_gate_2_s) + m_iOverwrittenOpsBufLen;
	else
		iGateBufLen = sizeof(_CDetour_nrc_pre_gate_1_s) + sizeof(_CDetour_pre_gate_2_s) + m_iOverwrittenOpsBufLen;
	
	/*
	m_pGate = new BYTE[iGateBufLen];
	if(!m_pGate)
		return false;
	*/

	memset(&m_Gate,0,sizeof(m_Gate));
	m_pGate = &m_Gate[0];

	if(iGateBufLen > sizeof(m_Gate))
	{
		//Log("PrepareGate iGateBufLen > sizeof(m_Gate) failed (%ld > %ld)", iGateBufLen, sizeof(m_Gate));
		return false;
	}
	
	_CDetour_pre_gate_2_s *preg2;
	BYTE *pOverwrittenOpsBuf;
	
	if (m_bRecursionCheck)
	{
		_CDetour_rc_pre_gate_1_s *preg1 = (_CDetour_rc_pre_gate_1_s*)(m_pGate);
		
		preg2 = (_CDetour_pre_gate_2_s*)(m_pGate + sizeof(_CDetour_rc_pre_gate_1_s) + m_iOverwrittenOpsBufLen);
		pOverwrittenOpsBuf = (BYTE*)(m_pGate + sizeof(_CDetour_rc_pre_gate_1_s));

		preg1->op_803d[0] = '\x80';
		preg1->op_803d[1] = '\x3D';
		preg1->op_74 = '\x74';
		preg1->op_c605[0] = '\xC6';
		preg1->op_c605[1] = '\x05';
		preg1->op_c605_2[0] = '\xC6';
		preg1->op_c605_2[1] = '\x05';
		preg1->op_8f05[0] = '\x8F';
		preg1->op_8f05[1] = '\x05';
		preg1->op_ff15[0] = '\xFF';
		preg1->op_ff15[1] = '\x15';
		preg1->op_ff35[0] = '\xFF';
		preg1->op_ff35[1] = '\x35';
		preg1->op_c605_3[0] = '\xC6';
		preg1->op_c605_3[1] = '\x05';
		preg1->op_803d_2[0] = '\x80';
		preg1->op_803d_2[1] = '\x3D';
		preg1->op_75 = '\x75';

		preg1->zero = 0x00;
		
		preg1->one = 0x01;
		preg1->one2 = 0x01;
		preg1->one3 = 0x01;
		preg1->one4 = 0x01;

		preg1->rcf = &m_bRecursionFlag;
		preg1->rcf2 = &m_bRecursionFlag;
		preg1->rcf3 = &m_bRecursionFlag;
		
		preg1->ofs_ow_start = (BYTE)(((DWORD)preg1 + sizeof(_CDetour_rc_pre_gate_1_s)) - ((DWORD)&preg1->ofs_ow_start)) - 1;
	
		preg1->ret_bool = (BYTE*)&m_bReturnToOriginal;
		preg1->ret_bool2 = (BYTE*)&m_bReturnToOriginal;

		preg1->ra_saver = &m_dwRetAddress;
		preg1->ra_saver2 = &m_dwRetAddress;

		preg1->ow_len_plus_6 = m_iOverwrittenOpsBufLen + 6;

		preg1->pfunc_to_call = &m_dwFuncToCall;


	}
	else
	{
		_CDetour_nrc_pre_gate_1_s *preg1 = (_CDetour_nrc_pre_gate_1_s*)(m_pGate);
		
		preg2 = (_CDetour_pre_gate_2_s*)(m_pGate + sizeof(_CDetour_nrc_pre_gate_1_s) + m_iOverwrittenOpsBufLen);
		pOverwrittenOpsBuf = (BYTE*)(m_pGate + sizeof(_CDetour_nrc_pre_gate_1_s));

		preg1->op_c605[0] = '\xC6';
		preg1->op_c605[1] = '\x05';
		preg1->op_8f05[0] = '\x8F';
		preg1->op_8f05[1] = '\x05';
		preg1->op_ff15[0] = '\xFF';
		preg1->op_ff15[1] = '\x15';
		preg1->op_ff35[0] = '\xFF';
		preg1->op_ff35[1] = '\x35';
		preg1->op_803d[0] = '\x80';
		preg1->op_803d[1] = '\x3D';
		preg1->op_75 = '\x75';

		preg1->one = 0x01;
		preg1->one2 = 0x01;

		preg1->ret_bool = (BYTE*)&m_bReturnToOriginal;
		preg1->ret_bool2 = (BYTE*)&m_bReturnToOriginal;

		preg1->ra_saver = &m_dwRetAddress;
		preg1->ra_saver2 = &m_dwRetAddress;

		preg1->ow_len_plus_6 = m_iOverwrittenOpsBufLen + 6;

		preg1->pfunc_to_call = &m_dwFuncToCall;
	}

	memcpy((void*)pOverwrittenOpsBuf, m_pOverwrittenOpsBuf, m_iOverwrittenOpsBufLen);

	preg2->op_68 = '\x68';
	preg2->op_c3c2[0] = '\xC3';
	preg2->op_c3c2[1] = '\xC2';

	preg2->ret_num = (WORD)m_iNumArgs * 4;
	
	preg2->return_addr = m_dwAddr + m_iPatchBufLen;

	return true;
}

int CDetour::GetDetourLen(int iPatchType)
{
	switch(iPatchType){
	case DETOUR_TYPE_JMP:
		return 5;

	case DETOUR_TYPE_PUSH_RET:
	case DETOUR_TYPE_NOP_JMP:
		return 6;
	
	case DETOUR_TYPE_NOP_NOP_JMP:
	case DETOUR_TYPE_STC_JC:
	case DETOUR_TYPE_CLC_JNC:
		return 7;

	case DETOUR_TYPE_OBS_ADD:
		return 12;

	case DETOUR_TYPE_OBS_STACKADD:
	case DETOUR_TYPE_OBS_ROR:
		return 13;

	case DETOUR_TYPE_OBS_XOR:
	case DETOUR_TYPE_OBS_ADDNOT:
		return 14;
	
	default:
		return 0;
	}
}

bool CDetour::PreparePatch()
{
	/*
	m_pPatchBuf = new BYTE[m_iPatchBufLen];
	if(!m_pPatchBuf)
		return false;
	*/

	memset(&m_PatchBuf,0,sizeof(m_PatchBuf));
	m_pPatchBuf = &m_PatchBuf[0];

	if(m_iPatchBufLen > sizeof(m_PatchBuf))
	{
		return false;
	}

	int iTmpRnd = (rand() * 0xFF) + rand();
	BYTE bTmpRnd = (BYTE)iTmpRnd;
	
	DWORD dwAddrToCall = (DWORD)m_pGate;

	memset((void*)m_pPatchBuf, 0x90, m_iPatchBufLen);

	switch(m_iPatchType)
	{
	case DETOUR_TYPE_JMP:
		m_pPatchBuf[0] = '\xE9';
		*(DWORD*)&m_pPatchBuf[1] = dwAddrToCall - (DWORD)m_dwAddr - 5;
		
		return true;

	case DETOUR_TYPE_PUSH_RET:
		m_pPatchBuf[0] = '\x68';
		*(DWORD*)&m_pPatchBuf[1] = dwAddrToCall;
		m_pPatchBuf[5] = '\xC3';

		return true;

	case DETOUR_TYPE_NOP_JMP:
		m_pPatchBuf[0] = '\x90';
		m_pPatchBuf[1] = '\xE9';
		*(DWORD*)&m_pPatchBuf[2] = dwAddrToCall - (DWORD)m_dwAddr - 6;
		
		return true;
	
	case DETOUR_TYPE_NOP_NOP_JMP:
		m_pPatchBuf[0] = '\x90';
		m_pPatchBuf[1] = '\x90';
		m_pPatchBuf[2] = '\xE9';
		*(DWORD*)&m_pPatchBuf[3] = dwAddrToCall - (DWORD)m_dwAddr - 7;
		
		return true;
	
	case DETOUR_TYPE_STC_JC:
		m_pPatchBuf[0] = '\xF9';
		m_pPatchBuf[1] = '\x0F';
		m_pPatchBuf[2] = '\x82';
		*(DWORD*)&m_pPatchBuf[3] = dwAddrToCall - (DWORD)m_dwAddr - 7;

		return true;

	case DETOUR_TYPE_CLC_JNC:
		m_pPatchBuf[0] = '\xF8';
		m_pPatchBuf[1] = '\x0F';
		m_pPatchBuf[2] = '\x83';
		*(DWORD*)&m_pPatchBuf[3] = dwAddrToCall - (DWORD)m_dwAddr - 7;

		return true;

	case DETOUR_TYPE_OBS_ADD:
		m_pPatchBuf[0] = '\xB8'; //mov eax
		*(DWORD*)&m_pPatchBuf[1] = iTmpRnd;
		m_pPatchBuf[5] = '\x05'; //add eax
		*(int*)&m_pPatchBuf[6] = dwAddrToCall - iTmpRnd;
		m_pPatchBuf[10] = '\xFF'; //jmp eax
		m_pPatchBuf[11] = '\xE0';
		
		return true;

	case DETOUR_TYPE_OBS_XOR:
		m_pPatchBuf[0] = '\x33'; //xor eax, eax
		m_pPatchBuf[1] = '\xC0';
		m_pPatchBuf[2] = '\x2D'; //sub eax
		*(int*)&m_pPatchBuf[3] = (int)iTmpRnd;
		m_pPatchBuf[7] = '\x35'; //xor eax
		*(DWORD*)&m_pPatchBuf[8] = dwAddrToCall ^ (-iTmpRnd);
		m_pPatchBuf[12] = '\xFF'; //jmp eax
		m_pPatchBuf[13] = '\xE0';

		return true;

	case DETOUR_TYPE_OBS_STACKADD:
		m_pPatchBuf[0] = '\x68'; //push
		*(DWORD*)&m_pPatchBuf[1] = (DWORD)iTmpRnd;
		m_pPatchBuf[5] = '\x81'; //xor dword ptr [esp]
		m_pPatchBuf[6] = '\x34';
		m_pPatchBuf[7] = '\x24';
		*(DWORD*)&m_pPatchBuf[8] = dwAddrToCall ^ iTmpRnd;
		m_pPatchBuf[12] = '\xC3'; //ret
		
		return true;

	case DETOUR_TYPE_OBS_ROR:
		while(!(bTmpRnd % 32))
			bTmpRnd = (BYTE)rand();
		
		__asm{
			pushad
			mov cl, bTmpRnd
			mov eax, dwAddrToCall
			rol eax, cl
			mov dword ptr dwAddrToCall, eax
			popad
		}

		m_pPatchBuf[0] = '\x51'; //push ecx
		m_pPatchBuf[1] = '\xB1'; //mov cl, 
		m_pPatchBuf[2] = bTmpRnd;
		m_pPatchBuf[3] = '\xB8'; //mov eax
		*(DWORD*)&m_pPatchBuf[4] = dwAddrToCall;
		m_pPatchBuf[8] = '\xD3'; //ror eax, cl
		m_pPatchBuf[9] = '\xC8';
		m_pPatchBuf[10] = '\x59'; //pop ecx
		m_pPatchBuf[11] = '\xFF'; //jmp eax
		m_pPatchBuf[12] = '\xE0';

		return true;

	case DETOUR_TYPE_OBS_ADDNOT:
		m_pPatchBuf[0] = '\xB8'; //mov eax
		*(DWORD*)&m_pPatchBuf[1] = iTmpRnd;
		m_pPatchBuf[5] = '\x05'; //add eax
		*(int*)&m_pPatchBuf[6] = (~dwAddrToCall) - iTmpRnd;
		m_pPatchBuf[10] = '\xF7'; //not eax
		m_pPatchBuf[11] = '\xD0';
		m_pPatchBuf[12] = '\xFF'; //jmp eax
		m_pPatchBuf[13] = '\xE0';

		return true;

	}

	return false;
}

///////////////////////////////
//	Matt Pietrek's MakePtr macro, and my MakeDelta macro
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))
#define MakeDelta(cast, x, y) (cast) ( (DWORD_PTR)(x) - (DWORD_PTR)(y))

/**************************************************************************** 
* 
* GetLocalProcAddress(hDll, pProcName) : pProc 
*   hDll      - Module handle of the dll. 
*   pProcName - Name or ordinal of the proc. 
*   pProc     - Address of the function. 
* 
* GetProcAddress is just like GetProcAddress. 
* 
****************************************************************************/ 
FARPROC CDetour::GetProcAddress(HMODULE hModule, LPCSTR pProcName) 
{ 
    DWORD                    Index; 
    DWORD                    MaxAdd; 
    DWORD                    MinAdd; 
    char *                   pChar; 
    PIMAGE_DOS_HEADER        pDOSHeader; 
    PIMAGE_EXPORT_DIRECTORY  pExportDir; 
    DWORD *                  pDWORD; 
    PIMAGE_NT_HEADERS        pNTHeader; 
    FARPROC                  pProc; 
    WORD *                   pWORD; 

    //Validate the input. 
    if (!hModule || !pProcName)  return(0); 
    try 
    { 
        //Get the EAT. 
        pDOSHeader = (PIMAGE_DOS_HEADER)hModule; 
        if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE) return(0); 

		pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDOSHeader, pDOSHeader->e_lfanew);
        if (pNTHeader->Signature != IMAGE_NT_SIGNATURE) return(0); 

        if (!pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress) return(0); 

		pExportDir = MakePtr(PIMAGE_EXPORT_DIRECTORY, pDOSHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

        //Search the EAT. 
        if ((DWORD)pProcName & 0xFFFF0000)	// By name
        { 
			pDWORD = MakePtr(LPDWORD, pDOSHeader, pExportDir->AddressOfNames);

            for (Index = 0; Index < pExportDir->NumberOfNames; ++Index) 
            { 
				pChar = MakePtr(char *, pDOSHeader, pDWORD[Index]);
                if (!stricmp(pProcName, pChar)) break; 
            } 
            if (Index >= pExportDir->NumberOfNames) return(0); 

			pWORD = MakePtr(LPWORD, pDOSHeader, pExportDir->AddressOfNameOrdinals);

            Index = pWORD[Index] + pExportDir->Base; 
        } 
        else	// By ordinal
        { 
			Index = (DWORD)pProcName; 
			if (Index >= pExportDir->NumberOfNames) return(0); 
		} 

        //Return the offset to the function. 
		pDWORD  = MakePtr(LPDWORD, pDOSHeader, pExportDir->AddressOfFunctions);
		pProc  = MakePtr(FARPROC, pDOSHeader, pDWORD[Index - pExportDir->Base]);

        //Check to see if this is a forwarder.  A forwarder points to 
        //DLLName.FunctionName\0 
		MinAdd = (DWORD)pDOSHeader + (DWORD)pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		MaxAdd = MinAdd + (DWORD)pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
        if (((DWORD)pProc >= MinAdd) && ((DWORD)pProc <= MaxAdd)) 
        { 
            char   Line[MAX_PATH]; 
            char * pChar; 

            strcpy(Line, (char *)pProc); 
            pChar = Line; 
            while (*pChar && (*pChar != '.')) ++pChar; 
            if (!*pChar) return(0); 

            *pChar = 0; 
            ++pChar; 

            hModule = GetModuleHandleA(Line); 
            if (!hModule) return(0); 

			pProc = ::GetProcAddress(hModule, pChar); 
        } 
        return(pProc); 
    } 

    catch (...) 
    { 
        return(0); 
    } 
} 

BOOL WINAPI CDetour::VirtualProtect( HANDLE hProcess, PVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect )
{
	long Status;

	HMODULE hNtDll = GetModuleHandleA("ntdll.dll");
	if (!hNtDll)	return false;

	//Status = NtProtectVirtualMemory( hProcess,&lpAddress, &dwSize, flNewProtect, lpflOldProtect);
	typedef LONG (WINAPI NTVPM)(HANDLE, PVOID, PULONG, ULONG, PULONG);
	NTVPM *lpfnNtProtectVirtualMemory = (NTVPM *)GetProcAddress(hNtDll, "NtProtectVirtualMemory");
	if (lpfnNtProtectVirtualMemory!=NULL)
	{
		Status = (*lpfnNtProtectVirtualMemory)( hProcess,&lpAddress, &dwSize, flNewProtect, lpflOldProtect);
	}

    if (Status >= 0) 
	{
        return true;
    }
    else 
	{
        return false;
    }
}
