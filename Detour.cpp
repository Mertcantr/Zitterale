#include "stdafx.h"
#include "ADE32.h"
#include "Detour.h"

CDetour::CDetour()
{
	m_uTramp = NULL;
	m_pTarget = NULL;
	m_dwLen = NULL;
}

CDetour::~CDetour()
{
	DetourRemove();
	m_uTramp = NULL;
	m_pTarget = NULL;
	m_dwLen = NULL;
}

void CDetour::DetourRemove( void )
{
	DWORD dwOld;

	VirtualProtect( (PVOID)m_pTarget, m_dwLen, PAGE_EXECUTE_READWRITE, &dwOld );
	m_uTramp -= m_dwLen;
	for( DWORD i = 0; i < m_dwLen; i++ )
		m_pTarget[i] = m_uTramp[i];
	free( m_uTramp );
	VirtualProtect( (PVOID)m_pTarget, m_dwLen, dwOld, &dwOld );
}

PBYTE CDetour::DetourPatch( PBYTE pTarget, PBYTE pDestination, DWORD dwLen )
{
	__try
	{
		BYTE uDetour[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
		BYTE uNop = 0x90;
		DWORD dwOld;

		m_dwLen = dwLen;
		m_pTarget = pTarget;
		m_uTramp = (BYTE*)malloc(dwLen+5);
		memcpy( m_uTramp, pTarget, dwLen );
		m_uTramp += dwLen;

		*(DWORD*)(&uDetour[1]) = (DWORD)(pTarget+dwLen - m_uTramp) - 5;
		for( int i = 0; i < 5; i++ )
			m_uTramp[i] = uDetour[i];

		VirtualProtect( pTarget, dwLen, PAGE_EXECUTE_READWRITE, &dwOld );
		*(DWORD*)(&uDetour[1]) = (DWORD)(pDestination - pTarget) - 5;
		for( int i = 0; i < 5; i++ )
			pTarget[i] = uDetour[i];
		for( DWORD i = 5; i < dwLen; i++ )
			pTarget[i] = uNop;
		VirtualProtect( pTarget, dwLen, dwOld, &dwOld );

		return (m_uTramp-dwLen);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return NULL;
	}
}

PBYTE CDetour::DetourPatch( PBYTE pTarget, PBYTE pDestination )
{
	BYTE *uOpcode = pTarget;
	DWORD dwLen = NULL;

	dwLen = (DWORD)oplen(uOpcode);
	uOpcode += dwLen;
	while( dwLen < 5 )
	{
		int iLen = oplen(uOpcode);
		if( !iLen )
			return NULL;

		dwLen += iLen;
		uOpcode += iLen;
	}

	return DetourPatch( pTarget, pDestination, dwLen );
}




//PBYTE DetourPatch( PBYTE pTarget, PBYTE pDestination, DWORD dwLen )
//{
//	__try
//	{
//		BYTE uDetour[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
//		BYTE uNop = 0x90;
//		BYTE *uTramp = (BYTE*)malloc(dwLen+5);
//		DWORD dwOld;
//
//		// Save instructions
//		memcpy( uTramp, pTarget, dwLen );
//		uTramp += dwLen;
//
//		// Stamp in reentry address
//		*(DWORD*)(&uDetour[1]) = (DWORD)(pTarget+dwLen - uTramp) - 5;
//
//		// Set the jump back
//		for( int i = 0; i < 5; i++ )
//			uTramp[i] = uDetour[i];
//
//		// Apply the patch and point a jmp to pDestination
//		VirtualProtect( pTarget, dwLen, PAGE_EXECUTE_READWRITE, &dwOld );
//		*(DWORD*)(&uDetour[1]) = (DWORD)(pDestination - pTarget) - 5;
//		for( int i = 0; i < 5; i++ )
//			pTarget[i] = uDetour[i];
//		for( DWORD i = 5; i < dwLen; i++ )
//			pTarget[i] = uNop;
//		VirtualProtect( pTarget, dwLen, dwOld, &dwOld );
//
//		// Return the address to our trampoline which executes missing instructions
//		// and jumps back
//		return (uTramp-dwLen);
//	}
//	__except(EXCEPTION_EXECUTE_HANDLER)
//	{
//		return NULL;
//	}
//}
