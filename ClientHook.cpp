// Zitterale.cpp : Definiert die exportierten Funktionen f�r die DLL-Anwendung.
//

#include "stdafx.h"
#include "Sdk.h"
#include "ClientHook.h"
#include "Client.h"
#include "SoundHook.h"
#include "Usermessagehook.h"
#include "SecureEngine.h"
#include "Detour.h"

DWORD l_HUD_Redraw_Old = 0;
DWORD l_StudioRenderModel_Old = 0;
DWORD l_CreateMove_Old = 0;
DWORD l_uRedraw = 0;
DWORD l_uStudioRender = 0;
DWORD l_uCreateMove = 0;
DWORD l_uCreateMove_New = 0;
DWORD l_CreateMove_Backup = 0;
DWORD l_CalcRefDef_Old = 0;
DWORD l_CalcRefDef = 0;
DWORD l_origSvDataPtr = 0;
DWORD l_newSvDataPtr = 0;
usercmd_t *l_pCreateMoveCmd = NULL;
PSTUDIO_MODEL_RENDERER_FRAME *l_pStudioFrame = NULL;

__declspec( naked ) void _CreateMove_Ret()
{
	_asm
	{
		pushad
		mov eax, dword ptr ss:[esp+0x30]
		mov ebx, l_pCreateMoveCmd
		push ebx
		push eax
		call CL_CreateMove
		popad
		mov esi, l_pCreateMoveCmd
		jmp l_CreateMove_Old
	}
}

__declspec( naked ) void _CreateMove_Gate()
{
	_asm
	{
		//save usercmd_t*
		mov l_pCreateMoveCmd, esi

		pushad
		// check the ret address
		mov eax, dword ptr ss:[esp+0x20]
		cmp eax, dword ptr ds:[l_uCreateMove]
		jne cmend


		// backup original return address
		mov eax, dword ptr ss:[esp+0x3C]
		mov dword ptr ds:[l_CreateMove_Old], eax
		// exchange return address
		mov eax, dword ptr ds:[l_uCreateMove_New]
		mov dword ptr ss:[esp+0x3C], eax
cmend:
		popad
		jmp l_CreateMove_Backup
	}
}

__declspec( naked ) void _HUD_Redraw_Gate( )
{
	_asm
	{
		pushad
		mov eax, dword ptr ss:[esp+0x20] // adjust for pushad
		cmp eax, dword ptr ds:[l_uRedraw]
		jne redrawend
		call HUD_Redraw
redrawend:
		popad
		jmp l_HUD_Redraw_Old
	}

}


__declspec( naked ) void _StudioRenderModel_Gate( )
{
	_asm
	{
		mov l_pStudioFrame, ecx

		pushad
		mov eax, dword ptr ss:[esp+0x20] // adjust for pushad
		cmp eax, dword ptr ds:[l_uStudioRender]
		jne studioend
		mov ebx, l_pStudioFrame
		push ebx
		call StudioRenderModel
studioend:
		popad
		jmp l_StudioRenderModel_Old
	}
}

__declspec( naked ) void _CalcRefDefGate()
{
	_asm
	{
		pushad
		mov eax, dword ptr ss:[esp+0x20] // adjust for pushad
		cmp eax, dword ptr ds:[l_CalcRefDef]
		jne calcend

		mov ebx, dword ptr ss:[esp+0x24]
		push ebx
		call V_CalcRefDef
calcend:
		popad
		jmp l_CalcRefDef_Old
	}
}


__declspec( naked ) void _SetViewAnglesGate()
{
	_asm
	{
		pushad
		mov eax, dword ptr ss:[esp+0x24]
		mov ecx, l_origSvDataPtr
		push eax
		call ecx
		add esp, 4
		popad
		jmp l_newSvDataPtr
	}
}

CDetour xdetour;
bool HookClient( void )
{
	Log(("HookClient() Build on %s", __TIMESTAMP__));

	DWORD OldProtect = 0;
	DWORD HwDll = (DWORD)GetModuleHandle(XStr( /*hw.dll*/ 0x02, 0x06, 0x00, 0x68762C67, 0x68690000 ).c());
	DWORD ClientDll = (DWORD)GetModuleHandle(XStr( /*client.dll*/ 0x03, 0x0A, 0x00, 0x636D6B66, 0x6A712863, 0x64650000 ).c());
	DWORD EngFuncs = HwDll+0x166A98;

	char szIniPath[MAX_PATH];
	strcpy(szIniPath, g_szPath);
	strcat(szIniPath, "security.ini");
	Log(("Ini at %s", szIniPath));
	g_iSecurityLevel = GetPrivateProfileInt("zitterale", "level", 2, szIniPath);;
	Log(("Security Level %i", g_iSecurityLevel));

	/* 05CE99F7    68 B829E505     push    hw.05E529B8
		* Before call to HUD_GetStudioModelInterface */
	DWORD EngStudio = *(DWORD*)(HwDll+0x199F8);

	Log(("Enginefuncs at 0x%08x", EngFuncs));
	cl_enginefuncs_s *pEngFuncs = (cl_enginefuncs_s *)EngFuncs;
	Log(("Engstudio at 0x%08x", EngStudio));
	engine_studio_api_s *pStudioFuncs = (engine_studio_api_s *)EngStudio;

	g_cSecureEngine.Initialize(HwDll, pEngFuncs, pStudioFuncs);


	ULONG uToPatch = *(ULONG*) ( ( (ULONG) pEngFuncs->GetViewAngles ) + 0x7 );
	Log(("uToPatch %08x %08x", uToPatch, (DWORD)pEngFuncs->GetViewAngles));

	DWORD HudRedrawPtr = HwDll+0xABF2;
	DWORD DataPtr = ***((DWORD***)(HudRedrawPtr));
	DataPtr += 0x18;
	l_uRedraw = HwDll+0x18FA5;
	Log(("HUD_Redraw Call address %08x DataPtr address %08x Ret addr %08x", HudRedrawPtr, DataPtr, l_uRedraw));

	l_HUD_Redraw_Old = *(DWORD*)DataPtr;
	VirtualProtect((PVOID)DataPtr, sizeof(DWORD), PAGE_READWRITE, &OldProtect);
	*(DWORD*)DataPtr = (DWORD)_HUD_Redraw_Gate;
	VirtualProtect((PVOID)DataPtr, sizeof(DWORD), OldProtect, &OldProtect);

	/* 0FE4A0E3    FF15 EC8DF10F   call    dword ptr ds:[FF18DEC]           ; hw.062BE600 */
	DWORD StudioRenderModel = ClientDll+0x6A0E5;
	DWORD SrDataPtr = *((DWORD*)StudioRenderModel);
	l_uStudioRender = StudioRenderModel+0x4;
	Log(("StudioRenderModel Call address %08x DataPtr address %08x Ret Addr %08x", StudioRenderModel, SrDataPtr, l_uStudioRender));

	l_StudioRenderModel_Old = *(DWORD*)SrDataPtr;
	VirtualProtect((PVOID)SrDataPtr, sizeof(DWORD), PAGE_READWRITE, &OldProtect);
	*(DWORD*)SrDataPtr = (DWORD)_StudioRenderModel_Gate;
	VirtualProtect((PVOID)SrDataPtr, sizeof(DWORD), OldProtect, &OldProtect);

	/*
	0FE18F7E    8B7424 1C       mov     esi, dword ptr ss:[esp+1C] // usercmd_t* in CL_CreateMove
	0FE194D9    FF15 281CEE0F   call    dword ptr ds:[FEE1C28]           ; hw.06B793F0 <- Call we want to use to exploit return address
	0018F09C   0FE194DF  RETURN to client.0FE194DF from hw.06B793F0 <- Stack view on call
	0018F0B8   06B78D9A  RETURN to hw.06B78D9A // Return address of CreateMove
	-- Difference = 1C
	*/
	DWORD CreateMoveCall = ClientDll+0x594DB;
	DWORD CmDataPtr = *((DWORD*)CreateMoveCall);
	l_uCreateMove = CreateMoveCall+0x4;
	l_uCreateMove_New = (DWORD)_CreateMove_Ret;

	l_CreateMove_Backup = *((DWORD*)CmDataPtr);
	Log(("CreateMoveCall %08x CmDataPtr %08x CreateMove Ret %08x Backup %08x", CreateMoveCall, CmDataPtr, l_uCreateMove, l_CreateMove_Backup));

	// Only do this for non-EAC
	if( g_iSecurityLevel < 3 )
	{
		Log(("Hooking CreateMove"));
		VirtualProtect((PVOID)CmDataPtr, sizeof(DWORD), PAGE_READWRITE, &OldProtect);
		*(DWORD*)CmDataPtr = (DWORD)_CreateMove_Gate;
		VirtualProtect((PVOID)CmDataPtr, sizeof(DWORD), OldProtect, &OldProtect);
	}

	// Patch the SetViewAngles calls....
	DWORD dwSetViewAnglesCalls = ClientDll+0x58FB7;
	DWORD SvDataPtr = *((DWORD*)dwSetViewAnglesCalls);
	l_origSvDataPtr = *(DWORD*)SvDataPtr;
	DWORD origEngFuncFunc = (DWORD)pEngFuncs->SetViewAngles;
	l_newSvDataPtr = (DWORD)g_cSecureEngine.Get()->SetViewAngles;
	Log(("SetViewAnglesCalls %08x Orig %08x Orig from Engfuncs %08x New %08x", dwSetViewAnglesCalls, l_origSvDataPtr, origEngFuncFunc, l_newSvDataPtr ));

	if( l_origSvDataPtr != origEngFuncFunc )
	{
		Log(("Exchanging pointer for Set/GetViewangles..."));
		g_cSecureEngine.Get()->SetViewAngles = (SetViewAngles)l_origSvDataPtr;
		g_cSecureEngine.Get()->GetViewAngles = (GetViewAngles)l_CreateMove_Backup;
	}


	// Do not hook this stuff if security level is greater than 1
	if( SECURITY_LEVEL(1) )
	{
		/*
		06AA8CF0    A1 6CE3C807     mov     eax, dword ptr ds:[7C8E36C] <- Address to exchange
		06AA8CF5    85C0            test    eax, eax
		06AA8CF7    74 08           je      short hw.06AA8D01
		06AA8CF9    8B4C24 04       mov     ecx, dword ptr ss:[esp+4]
		06AA8CFD    51              push    ecx
		06AA8CFE    FFD0            call    eax                              ; client.V_CalcRefdef
		06AA8D00    59              pop     ecx <- Our expected ret
		06AA8D01    C3              retn
		*/
		DWORD CalcRefDefCall = HwDll+0x18CF1;
		DWORD CRDataPtr = *((DWORD*)CalcRefDefCall);
		l_CalcRefDef = CalcRefDefCall+0xF;

		l_CalcRefDef_Old = *(DWORD*)CRDataPtr;
		VirtualProtect((PVOID)CRDataPtr, sizeof(DWORD), PAGE_READWRITE, &OldProtect);
		*(DWORD*)CRDataPtr = (DWORD)_CalcRefDefGate;
		VirtualProtect((PVOID)CRDataPtr, sizeof(DWORD), OldProtect, &OldProtect);
		Log(("CalcRefDefCall %08x CRDataPtr %08x Retaddr %08x Orig %08x", CalcRefDefCall, CRDataPtr, l_CalcRefDef, l_CalcRefDef_Old));


		//if( !HookUsermessage(ClientDll) )
		//	return false;

		if( !HookSound(HwDll) )
			return false;
	}



	return true;
}
