#include "stdafx.h"
#include "FunctionStealer.h"
#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "BeaEngine.h"

PIMAGE_SECTION_HEADER CFunctionStealer::GetEnclosingSectionHeader( DWORD_PTR rva, PIMAGE_NT_HEADERS pNTHeader )
{
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
	unsigned int i;

	for( i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
	{
		// This 3 line idiocy is because Watcom's linker actually sets the
		// Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
		DWORD size = section->Misc.VirtualSize;
		if ( 0 == size )
			size = section->SizeOfRawData;

		// Is the RVA within this section?
		if ((rva >= section->VirtualAddress) &&
			(rva < (section->VirtualAddress + size)))
			return section;
	}

	return 0;
}

PVOID CFunctionStealer::GetPtrFromRVA( DWORD_PTR rva, PIMAGE_NT_HEADERS pNTHeader, PVOID imageBase )
{
	PIMAGE_SECTION_HEADER pSectionHdr;
	DWORD_PTR delta;

	pSectionHdr = GetEnclosingSectionHeader( rva, pNTHeader );
	if( !pSectionHdr )
		return 0;

	delta = (DWORD_PTR)(pSectionHdr->VirtualAddress-pSectionHdr->PointerToRawData);
	return (PVOID)(((DWORD_PTR)(imageBase)) + rva - delta);
}

PIMAGE_NT_HEADERS CFunctionStealer::GetNtHeader( DWORD_PTR ptrBase )
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNtHeader;

	pDosHeader = (PIMAGE_DOS_HEADER)ptrBase;
	if( !pDosHeader || pDosHeader->e_magic != IMAGE_DOS_SIGNATURE )
		return NULL;

	// Get NT header
	pNtHeader = MakePtr(PIMAGE_NT_HEADERS, ptrBase, pDosHeader->e_lfanew);
	if( !pNtHeader || pNtHeader->Signature != IMAGE_NT_SIGNATURE )
		return NULL;

	return pNtHeader;
}

DWORD CFunctionStealer::PredictFunctionSize( DWORD_PTR ptrFunctionVa )
{
	DISASM disasm;
	DWORD dwFunctionSize = 0;
	DWORD_PTR ptrSavedFunctionVA = ptrFunctionVa;
	disasm.Instruction.Opcode = 0;

	while( (disasm.Instruction.Opcode != 0xC3 && disasm.Instruction.Opcode != 0xCB) ) // ret instruction
	{
		disasm.EIP = (UIntPtr)ptrFunctionVa;
		Disasm(&disasm);
		ptrFunctionVa++;
	}

	return (DWORD)ptrFunctionVa-ptrSavedFunctionVA;
}

DWORD_PTR CFunctionStealer::GetFile( char *szFile )
{
	HANDLE hFile = CreateFile(szFile,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return NULL;

	DWORD nSize = GetFileSize(hFile, NULL);
	if( !nSize )
	{
		CloseHandle(hFile);
		return NULL;
	}

	m_peFile = (PUCHAR)VirtualAlloc( NULL, nSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE );
	if( !m_peFile )
	{
		CloseHandle(hFile);
		return NULL;
	}

	DWORD nBytes;
	if( !ReadFile( hFile, m_peFile, nSize, (LPDWORD)&nBytes, FALSE ) )
	{
		CloseHandle(hFile);
		VirtualFree((PVOID)m_peFile, 0, MEM_RELEASE);
	}

	CloseHandle(hFile);
	return (DWORD_PTR)m_peFile;
}

void CFunctionStealer::FreeFile( void )
{
	VirtualFree((PVOID)m_peFile, 0, MEM_RELEASE);
}

DWORD_PTR CFunctionStealer::StealFunction( DWORD_PTR ptrModuleBase, DWORD_PTR ptrFunctionVa )
{
	DWORD_PTR ptrFunctionRVA = ptrFunctionVa-ptrModuleBase;
	DWORD dwFunctionSize = PredictFunctionSize(ptrFunctionVa)+0xFF; // To adjust for functions that have more than one ret
	char szModulePath[MAX_PATH];

	if( !ptrModuleBase || !ptrFunctionVa || !dwFunctionSize )
	{
		Log(("StealFunction -> Wrong parameters"));
		return NULL;
	}

	Log(("Stealing Function at %08x Modulebase %08x RVA %08x Predicted size %x", ptrFunctionVa, ptrModuleBase, ptrFunctionRVA, dwFunctionSize));

	if( !GetModuleFileName((HMODULE)ptrModuleBase, szModulePath, sizeof(szModulePath)) )
		return NULL;

	Log(("Module file name %s", szModulePath));

	PIMAGE_NT_HEADERS pNtHeader = GetNtHeader(ptrModuleBase);
	if( !pNtHeader)
		return NULL;

	DWORD_PTR ptrFile = GetFile(szModulePath);
	if( !ptrFile )
		return NULL;

	DWORD_PTR ptrNewFunction = (DWORD_PTR)VirtualAlloc( NULL, dwFunctionSize, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE );
	if( !ptrNewFunction )
		return NULL;

	// Fix relocations
	PIMAGE_BASE_RELOCATION pRelocation = (PIMAGE_BASE_RELOCATION)GetPtrFromRVA(pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress, pNtHeader, (PVOID)ptrFile);
	if( pRelocation )
	{
		SIZE_T relocSize = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
		PIMAGE_BASE_RELOCATION pRelocationEnd = MakePtr(PIMAGE_BASE_RELOCATION, pRelocation, relocSize);
		DWORD_PTR Delta = MakeDelta(DWORD_PTR, ptrModuleBase, pNtHeader->OptionalHeader.ImageBase);

		while( pRelocation < pRelocationEnd && pRelocation->SizeOfBlock > 0 )
		{
			PBYTE RelocBase = (PBYTE)GetPtrFromRVA(pRelocation->VirtualAddress, pNtHeader, (PVOID)ptrFile);
			DWORD numRelocs = (DWORD)(pRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
			PWORD pRelocData = (PWORD)(pRelocation + 1);
			DWORD i = 0;

			for( i = 0; i < numRelocs; ++i )
			{
				BYTE RelocType = *pRelocData >> 12;
				WORD Offset = *pRelocData & 0xFFF;

				switch (RelocType)
				{
				case IMAGE_REL_BASED_ABSOLUTE:
					break;

				case IMAGE_REL_BASED_HIGHLOW:
					*(DWORD32*)(RelocBase + Offset) += (DWORD32)(Delta);
					break;

				case IMAGE_REL_BASED_DIR64:
					*(DWORD64*)(RelocBase + Offset) += (DWORD64)(Delta);
					break;
				}


				pRelocData++;
			}
			pRelocation = (PIMAGE_BASE_RELOCATION)pRelocData;
		}
	}

	memcpy( (PVOID)ptrNewFunction, (PVOID)GetPtrFromRVA(ptrFunctionRVA, pNtHeader, (PVOID)ptrFile), dwFunctionSize );

	// Fix calls/jmps
	DWORD_PTR ptrFunctionEnd = ptrNewFunction+dwFunctionSize;
	DWORD_PTR ptrCurrent = ptrNewFunction;
	int len = 0;

	DISASM disasm;
	ZeroMemory(&disasm, sizeof(DISASM));
	disasm.EIP = (UIntPtr)ptrCurrent;

	while( ptrCurrent < ptrFunctionEnd )
	{
		len = Disasm(&disasm);

		if( disasm.Instruction.Opcode == 0xE8 || disasm.Instruction.Opcode == 0xE9 )
		{
			*(DWORD*)((DWORD)ptrCurrent+0x1) -= ptrNewFunction-ptrFunctionVa;
		}

		if( len != UNKNOWN_OPCODE )
		{
			ptrCurrent += len;
			disasm.EIP = (UIntPtr)ptrCurrent;
		}
		else
		{
			Log(("Error while fixing calls. Invalid opcode."));
			break;
		}

	}

	Log(("New Function at %08x", ptrNewFunction));

	FreeFile();

	return ptrNewFunction;
}
