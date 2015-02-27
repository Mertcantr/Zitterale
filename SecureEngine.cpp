#include "stdafx.h"
#include "SecureEngine.h"

CSecureEngine g_cSecureEngine;

bool CSecureEngine::Initialize( DWORD dwHwDllBase, cl_enginefuncs_s *pEngineFuncs, engine_studio_api_s *pStudioFuncs )
{
	#define StealEngine(x)  m_secureEngine.##x = (x)m_cFuncStealer.StealFunction(dwHwDllBase, (DWORD_PTR)pEngineFuncs->##x); \
							if( !m_secureEngine.##x ) \
							return false; \
							Log(("Stolen %s", #x))

	#define StealEngineS(x, y)  m_secureEngine.##x = (x)m_cFuncStealer.StealFunction(dwHwDllBase, (DWORD_PTR)y); \
								if( !m_secureEngine.##x ) \
								return false; \
								Log(("Stolen %s", #x))

	#define StealStudio(x)  m_secureEngine.##x = (x)m_cFuncStealer.StealFunction(dwHwDllBase, (DWORD_PTR)pStudioFuncs->##x); \
							if( !m_secureEngine.##x ) \
							return false; \
							Log(("Stolen %s", #x))

	m_bInitialized = false;

	StealEngine(GetLocalPlayer);
	StealEngine(GetViewModel);
	StealEngine(GetEntityByIndex);
	StealEngine(pfnGetPlayerInfo);
	StealEngine(pfnFillRGBA);
	StealEngine(pfnGetScreenInfo);
	StealEngine(pfnAngleVectors);
	StealEngine(pfnDrawConsoleString);
	StealEngine(pfnDrawConsoleStringLen);
	StealEngine(pfnDrawSetTextColor);
	StealEngine(GetViewAngles);
	StealEngine(GetClientTime);
	StealEngine(pfnGetScreenFade);
	StealEngine(pfnSetScreenFade);
	StealEngine(SetViewAngles);
	StealEngine(pfnConsolePrint);
	StealEngine(pfnClientCmd);

	StealEngineS(EV_LocalPlayerViewheight, pEngineFuncs->pEventAPI->EV_LocalPlayerViewheight);
	StealEngineS(WorldToScreen, pEngineFuncs->pTriAPI->WorldToScreen);
	StealEngineS(CL_AllocDlight, pEngineFuncs->pEfxAPI->CL_AllocDlight);
	StealEngineS(EV_PlayerTrace, pEngineFuncs->pEventAPI->EV_PlayerTrace);
	StealEngineS(EV_SetTraceHull, pEngineFuncs->pEventAPI->EV_SetTraceHull);
	StealEngineS(EV_IndexFromTrace, pEngineFuncs->pEventAPI->EV_IndexFromTrace);
	StealEngineS(Status, pEngineFuncs->pNetAPI->Status);

	StealStudio(StudioGetBoneTransform);
	StealStudio(GetModelByIndex);

	/* GetAsyncKeyState syscall in user32.dll */
	HMODULE hDll = GetModuleHandle("user32.dll");
	DWORD dwGetKeyAddr = ((DWORD)GetProcAddress(hDll, "GetAsyncKeyState"))+0xB2;
	m_secureEngine.xGetAsyncKeyState = (GetAsyncKeyState_t)dwGetKeyAddr;
	Log(("xGetAsyncKeyState at %08x", (DWORD)m_secureEngine.xGetAsyncKeyState));

	m_bInitialized = true;
	return m_bInitialized;
}

secure_engine * CSecureEngine::Get( void )
{
	if( m_bInitialized )
		return &m_secureEngine;

	return NULL;
}
