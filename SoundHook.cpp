#include "stdafx.h"
#include "Sdk.h"
#include "ClientHook.h"
#include "Client.h"
#include "SoundHook.h"

DWORD l_orig_Sound = 0;

// Credits to XEPT
// TODO: Create own way...

__declspec(naked) void SVC_Sound_RET()
{
	_asm PUSHAD;
	_asm MOV EAX, DWORD PTR SS:[ESP - 0x7C];
	_asm MOV EDX, DWORD PTR SS:[ESP - 0x74];
	_asm MOV ECX, DWORD PTR SS:[ESP - 0x70];
	_asm PUSH ECX;
	_asm PUSH EDX;
	_asm PUSH EAX;
	_asm CALL SoundEvent;
	_asm POPAD;
	_asm RET
}


__declspec(naked) void SVC_Sound_Gate()
{
	_asm PUSH SVC_Sound_RET
	_asm JMP l_orig_Sound;
}

bool HookSound( DWORD dwAddress )
{
	DWORD oldProtect;
	DWORD SoundPtr = ( *(ULONG*) (dwAddress + 0x2A85D) ) + 0x48;
	Log(("SoundPtr %08x", SoundPtr));

	l_orig_Sound = *(DWORD*)SoundPtr;
	VirtualProtect((PVOID)SoundPtr, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
	*(DWORD*)SoundPtr = (DWORD)SVC_Sound_Gate;
	VirtualProtect((PVOID)SoundPtr, sizeof(DWORD), oldProtect, &oldProtect);
	return true;
}
