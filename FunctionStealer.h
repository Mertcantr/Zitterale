#pragma once

#define MakePtr(cast, ptr, addValue)	(cast)((DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))
#define MakeDelta(cast, x, y)			(cast)((DWORD_PTR)(x) - (DWORD_PTR)(y))

class CFunctionStealer
{
public:
	DWORD_PTR StealFunction( DWORD_PTR ptrModuleBase, DWORD_PTR ptrFunctionVa );
	DWORD_PTR StealFunction( DWORD_PTR ptrModuleBase, DWORD_PTR ptrFunctionVa, DWORD dwSizeVariable );
	static PVOID GetPtrFromRVA( DWORD_PTR rva, PIMAGE_NT_HEADERS pNTHeader, PVOID imageBase );
	static PIMAGE_SECTION_HEADER GetEnclosingSectionHeader( DWORD_PTR rva, PIMAGE_NT_HEADERS pNTHeader );
	static PIMAGE_NT_HEADERS GetNtHeader( DWORD_PTR ptrBase );

private:
	DWORD PredictFunctionSize( DWORD_PTR ptrFunctionVa );
	DWORD_PTR GetFile( char *szFile );
	void FreeFile( void );
	PUCHAR m_peFile;
};
