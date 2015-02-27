// dllmain.cpp : Definiert den Einstiegspunkt f�r die DLL-Anwendung.
#include "stdafx.h"
#include "Clienthook.h"
#include "Client.h"
#include <stdio.h>

extern HANDLE hOverlayThread;

void StripPath( char *szPath )
{
	size_t iLen = strlen(szPath);
	for( size_t i = 0; i < iLen; i++ )
	{
		if( szPath[iLen-i] == '\\' )
		{
			szPath[(iLen-i)+1] = '\0';
			return;
		}
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetModuleFileName(hModule, g_szPath, sizeof(g_szPath));
		StripPath(g_szPath);
		return HookClient()?TRUE:FALSE;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		TerminateThread(hOverlayThread, 0);
		break;
	}
	return TRUE;
}
