#ifndef _M_IX86
#error Sorry - CDetour only works on x86 processors
#endif

#pragma once

#define DETOUR_TYPE_NOT_SET				-1

#define DETOUR_TYPE_OBS_RAND			0

#define DETOUR_TYPE_JMP					1
#define DETOUR_TYPE_PUSH_RET			2

#define DETOUR_TYPE_NOP_JMP				3
#define DETOUR_TYPE_NOP_NOP_JMP			4

#define DETOUR_TYPE_STC_JC				5
#define DETOUR_TYPE_CLC_JNC				6

#define DETOUR_TYPE_OBS_ADD				7
#define DETOUR_TYPE_OBS_XOR				8
#define DETOUR_TYPE_OBS_STACKADD		9
#define DETOUR_TYPE_OBS_ROR				10
#define DETOUR_TYPE_OBS_ADDNOT			11

#define DetourRandTypeLow				DETOUR_TYPE_OBS_ADD
#define DetourRandTypeHigh				DETOUR_TYPE_OBS_ADDNOT

class CDetour {
public:
	CDetour();
	~CDetour();

	bool Detour(DWORD dwAddr, DWORD dwFuncToCall, int iNumArgs = 0, bool bRecursionCheck = true, int iPatchType = DETOUR_TYPE_OBS_RAND, int iOpsToOverwrite = 0);

	bool Apply();
	bool Remove();

	DWORD GetRetAddress();

	void Ret(bool bReturnToOriginal);

	static FARPROC GetProcAddress(HMODULE hModule, LPCSTR pProcName);
private:
	BOOL WINAPI CDetour::VirtualProtect( HANDLE hProcess, PVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect );

	void Deconstruct();

	bool PrepareGate();

	int GetDetourLen(int iPatchType);

	bool PreparePatch();

	DWORD m_dwAddr;
	int m_iNumArgs;
	DWORD m_dwFuncToCall;
	int m_iPatchType;
	bool m_bRecursionCheck;

	BYTE *m_pOverwrittenOpsBuf;
	BYTE m_OverwrittenOpsBuf[256];
	int m_iOverwrittenOpsBufLen;

	BYTE *m_pPatchBuf;
	BYTE m_PatchBuf[256];
	int m_iPatchBufLen;

	BYTE *m_pGate;
	BYTE m_Gate[256];
	int m_iGateLen;

	bool m_bReady;
	bool m_bApplied;

	DWORD m_dwRetAddress;
	bool m_bReturnToOriginal;
	BYTE m_bRecursionFlag;
};