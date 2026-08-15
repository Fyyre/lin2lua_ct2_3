#include "Inc.h"

void DumpMemoryImage(char * szFilename)
{
	FILE * pFile;
	HMODULE hModule = NULL;
	char szDumpedDllName[FILENAME_MAX];
	char szText[1024];

	hModule = GetModuleHandleA( szFilename );
    if( NULL == hModule ){
        fputs("Error Retrieving Process Module Handle\n", stdout);
        return;
    }

    PIMAGE_DOS_HEADER pDOSHeader = NULL;
    pDOSHeader = static_cast<PIMAGE_DOS_HEADER>( (PVOID)hModule );
    if( pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE ){
        fputs("Error - File is not EXE Format\n", stdout);
        return;
    }

    PIMAGE_NT_HEADERS pNTHeader = NULL;
    pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>((PBYTE)hModule + pDOSHeader->e_lfanew );
    if( pNTHeader->Signature != IMAGE_NT_SIGNATURE ){
        fputs("Error - File is not PE Format\n", stdout);
        return;
    }

    PIMAGE_FILE_HEADER pFileHeader = NULL;
    pFileHeader = reinterpret_cast<PIMAGE_FILE_HEADER>((PBYTE)&pNTHeader->FileHeader );

    PIMAGE_OPTIONAL_HEADER pOptionalHeader = NULL;
    pOptionalHeader = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>((PBYTE)&pNTHeader->OptionalHeader );

    if( IMAGE_NT_OPTIONAL_HDR32_MAGIC != pNTHeader->OptionalHeader.Magic ){
        fputs("Error - File is not 32 bit\n", stdout);
        return;
    }

    PIMAGE_SECTION_HEADER pSectionHeader = NULL;
    pSectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>((PBYTE)&pNTHeader->OptionalHeader + pNTHeader->FileHeader.SizeOfOptionalHeader );

	sprintf(szText, " \"%20.20s\"        0x%08x\n", szFilename, hModule);
    fputs(szText, stdout);
    UINT nSectionCount = pNTHeader->FileHeader.NumberOfSections;
    CHAR szSectionName[ IMAGE_SIZEOF_SHORT_NAME + 1 ];
    szSectionName[ IMAGE_SIZEOF_SHORT_NAME ] = '\0';

	DWORD pVirtualAddress = 0;
	PVOID pMemoryCodeStart = 0;
	PVOID pMemoryCodeEnd = 0;

    for( UINT i = 0; i < nSectionCount; i++ ){
		memcpy( szSectionName, pSectionHeader->Name, IMAGE_SIZEOF_SHORT_NAME );

		pVirtualAddress = pSectionHeader->VirtualAddress;
		sprintf(szText, " Virtual Address: 0x%08x\n", pVirtualAddress);
	    fputs(szText, stdout);
		pMemoryCodeStart = (PVOID)((PBYTE)hModule + pSectionHeader->VirtualAddress );
		sprintf(szText, " \"%s\" Start: 0x%08x\n", szSectionName, pMemoryCodeStart);
	    fputs(szText, stdout);
		sprintf(szText, " \"%s\"  Size: 0x%08x\n", szSectionName, pSectionHeader->Misc.VirtualSize);
	    fputs(szText, stdout);
		pMemoryCodeEnd = (PVOID)((PBYTE)pMemoryCodeStart + pSectionHeader->Misc.VirtualSize );
		sprintf(szText, " \"%s\"   End: 0x%08x\n", szSectionName, pMemoryCodeEnd);
	    fputs(szText, stdout);

		/*
		if (pMemoryCodeEnd == (DWORD *)0x055c5000){
			fputs("Dumping ...\n", stdout);
			fwrite (hModule , 1 , 0x155000 , pFile );
		}
		*/
		/*
		sprintf(szDumpedDllName, "Dumped_%i_%s", i, szFilename);
		pFile = fopen (szDumpedDllName , "wb" );

		DWORD oldProtection;
		VirtualProtect(pMemoryCodeStart, pSectionHeader->Misc.VirtualSize, PAGE_EXECUTE_READWRITE, &oldProtection);
		fwrite (pMemoryCodeStart, 1 , pSectionHeader->Misc.VirtualSize, pFile);
		VirtualProtect(pMemoryCodeStart, pSectionHeader->Misc.VirtualSize, oldProtection, &oldProtection);

		fclose (pFile);
		*/

        pSectionHeader++;
    }

	sprintf(szDumpedDllName, "Dumped_%s", szFilename);
	pFile = fopen (szDumpedDllName , "wb" );

	if (pMemoryCodeEnd != 0)
	{
		size_t count = (DWORD)pMemoryCodeEnd - (DWORD)hModule;
		sprintf(szText, "Size: 0x%08x\n", count);
	    fputs(szText, stdout);
		fputs("Dumping ...\n", stdout);
		fwrite (hModule , 1 , count , pFile );
	}

	fclose (pFile);

	/*
	Engine.MemoryImageDump("l2.exe")
	Engine.MemoryImageDump("core.dll")
	Engine.MemoryImageDump("engine.dll")
	Engine.MemoryImageDump("nwindow.dll")
	*/

	/*

10b02000-10900000
Engine.MemoryImageDump("L2.exe")
 "              L2.exe"        0x10900000
 Virtual Address: 0x00001000
 "   " Start: 0x10901000
 "   "  Size: 0x000ab000
 "   "   End: 0x109ac000
 Virtual Address: 0x000ac000
 ".rsrc" Start: 0x109ac000
 ".rsrc"  Size: 0x00002326
 ".rsrc"   End: 0x109ae326
 Virtual Address: 0x78efc7af
 "������nx" Start: 0x897fc7af
 "������nx"  Size: 0x000011e9
 "������nx"   End: 0x897fd998
 Virtual Address: 0x000b0000
 "Themida " Start: 0x109b0000
 "Themida "  Size: 0x00152000
 "Themida "   End: 0x10b02000
Size: 0x00080800 should be202000
Dumping ...
	*/

	//size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream ); <cstdio> 
	/*
	ptr - Pointer to the array of elements to be written. 
	size - Size in bytes of each element to be written. 
	count - Number of elements, each one with a size of size bytes. 
	stream - Pointer to a FILE object that specifies an output stream. 
	*/



}
