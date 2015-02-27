#include "stdafx.h"
#include "CPlayer.h"
#include "Usermessagehook.h"
#include "Client.h"

DWORD l_oldTeamInfo = 0;

// stdcall so we don't have to clean up stack
void _stdcall TeamInfo( const char *pszName, int iSize, void *pBuf )
{
	BEGIN_READ( pBuf, iSize );

	int iIndex = READ_BYTE( );
	const char *szTeam = READ_STRING( );

	if( !g_isIngame )
		return;

	ePlayerTeam eTeam = TEAM_UNASSIGNED;

	if( !lstrcmp(szTeam, XStr( /*TERRORIST*/ 0x03, 0x09, 0x00, 0x54445051, 0x4B574F54, 0x5C000000 ).c()) )
	{
		eTeam = TEAM_T;
	}
	else if( !lstrcmp(szTeam, XStr( /*CT*/ 0x01, 0x02, 0x00, 0x43550000 ).c()) )
	{
		eTeam = TEAM_CT;
	}
	else
	{
		eTeam = TEAM_UNASSIGNED;
	}

	if( iIndex != g_cPlayers.GetLocalPlayer().iIndex )
	{
		CPlayerObject *pPlayer = g_cPlayers.GetPlayer(iIndex);
		if( pPlayer )
			pPlayer->eTeam = eTeam;
	}
	else
		g_cPlayers.GetLocalPlayer().eTeam = eTeam;
}

__declspec( naked ) void _TeamInfo_Gate( void )
{
	_asm
	{
		pushad
		mov eax, dword ptr ss:[esp+0x24] // pszName
		mov ebx, dword ptr ss:[esp+0x28] // iSize
		mov ecx, dword ptr ss:[esp+0x2C] // pBuf
		push ecx
		push ebx
		push eax
		call TeamInfo
		popad
		jmp l_oldTeamInfo
	}

}

bool HookUsermessage( DWORD dwClientDll )
{
	/* TeamInfo
	0F7CE8D0    8B0D C4098C0F   mov     ecx, dword ptr ds:[F8C09C4] <- This is the pointer we'll exchange which is dereferenced three times and calculated plus 0x3C     ; client.0F87C1C0
	0F7CE8D6    85C9            test    ecx, ecx
	0F7CE8D8    74 15           je      short client.0F7CE8EF
	0F7CE8DA    8B5424 0C       mov     edx, dword ptr ss:[esp+C]
	0F7CE8DE    8B01            mov     eax, dword ptr ds:[ecx] <- we have to dereference it two times
	0F7CE8E0    52              push    edx
	0F7CE8E1    8B5424 0C       mov     edx, dword ptr ss:[esp+C]
	0F7CE8E5    52              push    edx
	0F7CE8E6    8B5424 0C       mov     edx, dword ptr ss:[esp+C]
	0F7CE8EA    52              push    edx
	0F7CE8EB    FF50 3C         call    dword ptr ds:[eax+3C] <- We have to add 0x3C to dereferenced value
	0F7CE8EE    C3              retn
	0F7CE8EF    33C0            xor     eax, eax
	0F7CE8F1    C3              retn
	*/
	DWORD OldProtect = 0;
	DWORD dwCallAddr = dwClientDll+0x4E8D2;
	DWORD dwDataPtr = ***(DWORD***)(dwCallAddr);
	dwDataPtr += 0x3C;
	Log(("TeamInfo CallAddr %08x DataPtr Addr %08x", dwCallAddr, dwDataPtr));

	// TODO: Seems detectable right now
	l_oldTeamInfo = *(DWORD*)dwDataPtr;
	VirtualProtect((PVOID)dwDataPtr, sizeof(DWORD), PAGE_READWRITE, &OldProtect);
	//*(DWORD*)dwDataPtr = (DWORD)_TeamInfo_Gate;
	VirtualProtect((PVOID)dwDataPtr, sizeof(DWORD), OldProtect, &OldProtect);


	return true;
}
