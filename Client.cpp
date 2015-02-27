#include "stdafx.h"
#include "Sdk.h"
#include "CPlayer.h"
#include "Client.h"
#include "CColor.h"
#include "CCVar.h"
#include "CVisual.h"
#include "CSoundESP.h"
#include "Maths.h"
#include "CAimbot.h"
#include "SecureEngine.h"
#include "ScreenshotHook.h"
#include "HardwareID.h"
#include "COverlay.h"

CSoundEsp l_cSoundEsp;
bool g_isIngame = false;
bool l_initFailed = false;
char g_szPath[MAX_PATH];
int g_iSecurityLevel = 0;
HANDLE hOverlayThread = NULL;

int iTranslateKey( char* szKey )
{
	if( !strcmp(szKey, XStr( /*mouse1*/ 0x02, 0x06, 0x00, 0x6D6E7770, 0x61340000 ).c()) )
		return VK_LBUTTON;
	else if( !strcmp(szKey, XStr( /*mouse2*/ 0x02, 0x06, 0x00, 0x6D6E7770, 0x61370000 ).c()) )
		return VK_RBUTTON;
	else if( !strcmp(szKey, XStr( /*mouse3*/ 0x02, 0x06, 0x00, 0x6D6E7770, 0x61360000 ).c()) )
		return VK_MBUTTON;
	else if( !strcmp(szKey, XStr( /*mouse4*/ 0x02, 0x06, 0x00, 0x6D6E7770, 0x61310000 ).c()) )
		return 0x05;
	else if( !strcmp(szKey, XStr( /*mouse5*/ 0x02, 0x06, 0x00, 0x6D6E7770, 0x61300000 ).c()) )
		return 0x06;
	else if( !strcmp(szKey, XStr( /*shift*/ 0x02, 0x05, 0x00, 0x73696B65, 0x70000000 ).c()) )
		return VK_SHIFT;
	else if( !strcmp(szKey, XStr( /*lshift*/ 0x02, 0x06, 0x00, 0x6C726A6A, 0x62710000 ).c()) )
		return VK_LSHIFT;
	else if( !strcmp(szKey, XStr( /*rshift*/ 0x02, 0x06, 0x00, 0x72726A6A, 0x62710000 ).c()) )
		return VK_RSHIFT;
	else if( !strcmp(szKey, XStr( /*alt*/ 0x01, 0x03, 0x00, 0x616D7600 ).c()) )
		return VK_MENU;
	else if( !strcmp(szKey, XStr( /*lalt*/ 0x01, 0x04, 0x00, 0x6C606E77 ).c()) )
		return VK_LMENU;
	else if( !strcmp(szKey, XStr( /*ralt*/ 0x01, 0x04, 0x00, 0x72606E77 ).c()) )
		return VK_RMENU;
	else if( !strcmp(szKey, XStr( /*ctrl*/ 0x01, 0x04, 0x00, 0x6375706F ).c()) )
		return VK_CONTROL;
	else if( !strcmp(szKey, XStr( /*lctrl*/ 0x02, 0x05, 0x00, 0x6C627671, 0x68000000 ).c()) )
		return VK_LCONTROL;
	else if( !strcmp(szKey, XStr( /*rctrl*/ 0x02, 0x05, 0x00, 0x72627671, 0x68000000 ).c()) )
		return VK_RCONTROL;
	else if( !strcmp(szKey, XStr( /*tab*/ 0x01, 0x03, 0x00, 0x74606000 ).c()) )
		return VK_TAB;
	else if( !strcmp(szKey, XStr( /*up*/ 0x01, 0x02, 0x00, 0x75710000 ).c()) )
		return VK_UP;
	else if( !strcmp(szKey, XStr( /*down*/ 0x01, 0x04, 0x00, 0x646E756D ).c()) )
		return VK_DOWN;
	else if( !strcmp(szKey, XStr( /*left*/ 0x01, 0x04, 0x00, 0x6C646477 ).c()) )
		return VK_LEFT;
	else if( !strcmp(szKey, XStr( /*right*/ 0x02, 0x05, 0x00, 0x7268656B, 0x70000000 ).c()) )
		return VK_RIGHT;
	else if( !strcmp(szKey, XStr( /*insert*/ 0x02, 0x06, 0x00, 0x696F7166, 0x76710000 ).c()) )
		return VK_INSERT;
	else if( !strcmp(szKey, XStr( /*delete*/ 0x02, 0x06, 0x00, 0x64646E66, 0x70600000 ).c()) )
		return VK_DELETE;
	else if( !strcmp(szKey, XStr( /*home*/ 0x01, 0x04, 0x00, 0x686E6F66 ).c()) )
		return VK_HOME;
	else if( !strcmp(szKey, XStr( /*end*/ 0x01, 0x03, 0x00, 0x656F6600 ).c()) )
		return VK_END;
	else if( !strcmp(szKey, XStr( /*pageup*/ 0x02, 0x06, 0x00, 0x70606566, 0x71750000 ).c()) )
		return VK_PRIOR;
	else if( !strcmp(szKey, XStr( /*pagedn*/ 0x02, 0x06, 0x00, 0x70606566, 0x606B0000 ).c()) )
		return VK_NEXT;
	else if( !strcmp(szKey, XStr( /*backsp*/ 0x02, 0x06, 0x00, 0x62606168, 0x77750000 ).c()) )
		return VK_BACK;
	else if( !strcmp(szKey, XStr( /*enter*/ 0x02, 0x05, 0x00, 0x656F7666, 0x76000000 ).c()) )
		return VK_RETURN;
	else if( !strcmp(szKey, XStr( /*pause*/ 0x02, 0x05, 0x00, 0x70607770, 0x61000000 ).c()) )
		return VK_PAUSE;
	else if( !strcmp(szKey, XStr( /*numlock*/ 0x02, 0x07, 0x00, 0x6E746F6F, 0x6B666D00 ).c()) )
		return VK_NUMLOCK;
	else if( !strcmp(szKey, XStr( /*space*/ 0x02, 0x05, 0x00, 0x73716360, 0x61000000 ).c()) )
		return VK_SPACE;
	else if( !strcmp(szKey, XStr( /*numpad0*/ 0x02, 0x07, 0x00, 0x6E746F73, 0x65613600 ).c()) )
		return VK_NUMPAD0;
	else if( !strcmp(szKey, XStr( /*numpad1*/ 0x02, 0x07, 0x00, 0x6E746F73, 0x65613700 ).c()) )
		return VK_NUMPAD1;
	else if( !strcmp(szKey, XStr( /*numpad2*/ 0x02, 0x07, 0x00, 0x6E746F73, 0x65613400 ).c()) )
		return VK_NUMPAD2;
	else if( !strcmp(szKey, XStr( /*numpad3*/ 0x02, 0x07, 0x00, 0x6E746F73, 0x65613500 ).c()) )
		return VK_NUMPAD3;
	else if( !strcmp(szKey, XStr( /*numpad4*/ 0x02, 0x07, 0x00, 0x6E746F73, 0x65613200 ).c()) )
		return VK_NUMPAD4;
	else if( !strcmp(szKey, XStr( /*numpad5*/ 0x02, 0x07, 0x00, 0x6E746F73, 0x65613300 ).c()) )
		return VK_NUMPAD5;
	else if( !strcmp(szKey, XStr( /*numpad6*/ 0x02, 0x07, 0x00, 0x6E746F73, 0x65613000 ).c()) )
		return VK_NUMPAD6;
	else if( !strcmp(szKey, XStr( /*numpad7*/ 0x02, 0x07, 0x00, 0x6E746F73, 0x65613100 ).c()) )
		return VK_NUMPAD7;
	else if( !strcmp(szKey, XStr( /*numpad8*/ 0x02, 0x07, 0x00, 0x6E746F73, 0x65613E00 ).c()) )
		return VK_NUMPAD8;
	else if( !strcmp(szKey, XStr( /*numpad9*/ 0x02, 0x07, 0x00, 0x6E746F73, 0x65613F00 ).c()) )
		return VK_NUMPAD9;
	else if( !strcmp(szKey, XStr( /*f1*/ 0x01, 0x02, 0x00, 0x66300000 ).c()) )
		return VK_F1;
	else if( !strcmp(szKey, XStr( /*f2*/ 0x01, 0x02, 0x00, 0x66330000 ).c()) )
		return VK_F2;
	else if( !strcmp(szKey, XStr( /*f3*/ 0x01, 0x02, 0x00, 0x66320000 ).c()) )
		return VK_F3;
	else if( !strcmp(szKey, XStr( /*f4*/ 0x01, 0x02, 0x00, 0x66350000 ).c()) )
		return VK_F4;
	else if( !strcmp(szKey, XStr( /*f5*/ 0x01, 0x02, 0x00, 0x66340000 ).c()) )
		return VK_F5;
	else if( !strcmp(szKey, XStr( /*f6*/ 0x01, 0x02, 0x00, 0x66370000 ).c()) )
		return VK_F6;
	else if( !strcmp(szKey, XStr( /*f7*/ 0x01, 0x02, 0x00, 0x66360000 ).c()) )
		return VK_F7;
	else if( !strcmp(szKey, XStr( /*f8*/ 0x01, 0x02, 0x00, 0x66390000 ).c()) )
		return VK_F8;
	else if( !strcmp(szKey, XStr( /*f9*/ 0x01, 0x02, 0x00, 0x66380000 ).c()) )
		return VK_F9;
	else if( !strcmp(szKey, XStr( /*f10*/ 0x01, 0x03, 0x00, 0x66303200 ).c()) )
		return VK_F10;
	else if( !strcmp(szKey, XStr( /*f11*/ 0x01, 0x03, 0x00, 0x66303300 ).c()) )
		return VK_F11;
	else if( !strcmp(szKey, XStr( /*f12*/ 0x01, 0x03, 0x00, 0x66303000 ).c()) )
		return VK_F12;
	else if( !strcmp(szKey, XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c()) )
		return 49;
	else if( !strcmp(szKey, XStr( /*2*/ 0x01, 0x01, 0x00, 0x32000000 ).c()) )
		return 50;
	else if( !strcmp(szKey, XStr( /*3*/ 0x01, 0x01, 0x00, 0x33000000 ).c()) )
		return 51;
	else if( !strcmp(szKey, XStr( /*4*/ 0x01, 0x01, 0x00, 0x34000000 ).c()) )
		return 52;
	else if( !strcmp(szKey, XStr( /*5*/ 0x01, 0x01, 0x00, 0x35000000 ).c()) )
		return 53;
	else if( !strcmp(szKey, XStr( /*6*/ 0x01, 0x01, 0x00, 0x36000000 ).c()) )
		return 54;
	else if( !strcmp(szKey, XStr( /*7*/ 0x01, 0x01, 0x00, 0x37000000 ).c()) )
		return 55;
	else if( !strcmp(szKey, XStr( /*8*/ 0x01, 0x01, 0x00, 0x38000000 ).c()) )
		return 56;
	else if( !strcmp(szKey, XStr( /*9*/ 0x01, 0x01, 0x00, 0x39000000 ).c()) )
		return 57;
	else if( !strcmp(szKey, XStr( /*a*/ 0x01, 0x01, 0x00, 0x61000000 ).c()) )
		return 65;
	else if( !strcmp(szKey, XStr( /*b*/ 0x01, 0x01, 0x00, 0x62000000 ).c()) )
		return 66;
	else if( !strcmp(szKey, XStr( /*c*/ 0x01, 0x01, 0x00, 0x63000000 ).c()) )
		return 67;
	else if( !strcmp(szKey, XStr( /*d*/ 0x01, 0x01, 0x00, 0x64000000 ).c()) )
		return 68;
	else if( !strcmp(szKey, XStr( /*e*/ 0x01, 0x01, 0x00, 0x65000000 ).c()) )
		return 69;
	else if( !strcmp(szKey, XStr( /*f*/ 0x01, 0x01, 0x00, 0x66000000 ).c()) )
		return 70;
	else if( !strcmp(szKey, XStr( /*g*/ 0x01, 0x01, 0x00, 0x67000000 ).c()) )
		return 71;
	else if( !strcmp(szKey, XStr( /*h*/ 0x01, 0x01, 0x00, 0x68000000 ).c()) )
		return 72;
	else if( !strcmp(szKey, XStr( /*i*/ 0x01, 0x01, 0x00, 0x69000000 ).c()) )
		return 73;
	else if( !strcmp(szKey, XStr( /*j*/ 0x01, 0x01, 0x00, 0x6A000000 ).c()) )
		return 74;
	else if( !strcmp(szKey, XStr( /*k*/ 0x01, 0x01, 0x00, 0x6B000000 ).c()) )
		return 75;
	else if( !strcmp(szKey, XStr( /*l*/ 0x01, 0x01, 0x00, 0x6C000000 ).c()) )
		return 76;
	else if( !strcmp(szKey, XStr( /*m*/ 0x01, 0x01, 0x00, 0x6D000000 ).c()) )
		return 77;
	else if( !strcmp(szKey, XStr( /*n*/ 0x01, 0x01, 0x00, 0x6E000000 ).c()) )
		return 78;
	else if( !strcmp(szKey, XStr( /*o*/ 0x01, 0x01, 0x00, 0x6F000000 ).c()) )
		return 79;
	else if( !strcmp(szKey, XStr( /*p*/ 0x01, 0x01, 0x00, 0x70000000 ).c()) )
		return 80;
	else if( !strcmp(szKey, XStr( /*q*/ 0x01, 0x01, 0x00, 0x71000000 ).c()) )
		return 81;
	else if( !strcmp(szKey, XStr( /*r*/ 0x01, 0x01, 0x00, 0x72000000 ).c()) )
		return 82;
	else if( !strcmp(szKey, XStr( /*s*/ 0x01, 0x01, 0x00, 0x73000000 ).c()) )
		return 83;
	else if( !strcmp(szKey, XStr( /*t*/ 0x01, 0x01, 0x00, 0x74000000 ).c()) )
		return 84;
	else if( !strcmp(szKey, XStr( /*u*/ 0x01, 0x01, 0x00, 0x75000000 ).c()) )
		return 85;
	else if( !strcmp(szKey, XStr( /*v*/ 0x01, 0x01, 0x00, 0x76000000 ).c()) )
		return 86;
	else if( !strcmp(szKey, XStr( /*w*/ 0x01, 0x01, 0x00, 0x77000000 ).c()) )
		return 87;
	else if( !strcmp(szKey, XStr( /*x*/ 0x01, 0x01, 0x00, 0x78000000 ).c()) )
		return 88;
	else if( !strcmp(szKey, XStr( /*y*/ 0x01, 0x01, 0x00, 0x79000000 ).c()) )
		return 89;
	else if( !strcmp(szKey, XStr( /*z*/ 0x01, 0x01, 0x00, 0x7A000000 ).c()) )
		return 90;
	else
		return NULL;
}

void HUD_Reset( void )
{
	// We reset players health and alive status cause
	// new round has started
	for( int iCount = 0; iCount < MAX_VPLAYERS; iCount++ )
	{
		// Local player
		if( iCount == g_cPlayers.GetLocalPlayer().iIndex )
		{
			g_cPlayers.GetLocalPlayer().bAlive = true;
			continue;
		}

		CPlayerObject *pPlayer = g_cPlayers.GetPlayer(iCount);
		if( pPlayer )
		{
			pPlayer->bAlive = true;
			pPlayer->iHealth = 100;
			pPlayer->bGotSound = false;
		}
	}
}

CTimer tScreenTimer; // Not inside function because of __try
CTimer tPanicTimer;
bool l_bPanic = false, l_bAimbotPanic = false, l_bEspPanic = false;
CVarObject l_cvPanicKey, l_cvAimbotPanic, l_cvEspPanic, l_cvHardwareID;

void RegisterPanicVars()
{
	RegisterVar(l_cvPanicKey, XStr( /*panic_all*/ 0x03, 0x09, 0x00, 0x70606C6A, 0x675A676B, 0x64000000 ).c(), XStr( /*f11*/ 0x01, 0x03, 0x00, 0x66303300 ).c(), GLOBAL_SECTION, TYPE_BUTTON);
	RegisterVar(l_cvAimbotPanic, XStr( /*panic_aimbot*/ 0x03, 0x0C, 0x00, 0x70606C6A, 0x675A676E, 0x656B657F ).c(), XStr( /*f10*/ 0x01, 0x03, 0x00, 0x66303200 ).c(), GLOBAL_SECTION, TYPE_BUTTON);
	RegisterVar(l_cvEspPanic, XStr( /*panic_esp*/ 0x03, 0x09, 0x00, 0x70606C6A, 0x675A6374, 0x78000000 ).c(), XStr( /*f9*/ 0x01, 0x02, 0x00, 0x66380000 ).c(), GLOBAL_SECTION, TYPE_BUTTON);
	RegisterVar(l_cvHardwareID, XStr( /*misc_hardwareid*/ 0x04, 0x0F, 0x00, 0x6D687160, 0x5B6D6775, 0x6C7E6B79, 0x69646A00 ).c(), XStr( /*be1d1456*/ 0x02, 0x08, 0x00, 0x62643367, 0x35313331 ).c(), TYPE_NO_DISPLAY, TYPE_NO_DISPLAY);
}

void ConsoleDisplayError()
{
	g_cSecureEngine.Get()->pfnConsolePrint(XStr( /*Zitterale: Error while initializing...*/ 0x0A, 0x26, 0x00, 0x5A687677, 0x6177676B, 0x6D332A4E, 0x7E7F617D, 0x30667A7A, 0x7870367E, 0x76706E72, 0x7D717765, 0x494F450D, 0x0A0B0000 ).c());
}

void LoadCVars()
{
	Log(("Loading CVars from %s", g_szPath));
	strcat(g_szPath, XStr( /*config.cfg*/ 0x03, 0x0A, 0x00, 0x636E6C65, 0x6D622864, 0x6E6E0000 ).c());
	g_cCVar.Read(g_szPath);
}

void InitHardwareID( char *hardwareid, DWORD dwHardwareCrc )
{
	sprintf(hardwareid, XStr( /*%08x*/ 0x01, 0x04, 0x00, 0x25313A7B ).c(), dwHardwareCrc);
}

void WrongHardwareID( void )
{
	g_cSecureEngine.Get()->pfnConsolePrint(XStr( /*Wrong HardwareID!*/ 0x05, 0x11, 0x00, 0x57736D6D, 0x63254E66, 0x7A6D7D6A, 0x7E68474B, 0x31000000 ).c());
	g_isIngame = false;
}

DWORD WINAPI OverlayThread( PVOID unused )
{
	__try
	{
	//	l_cSoundEsp.InitializeOverlay();
	//	while( true )
	//	{
			l_cSoundEsp.DrawBorder();
			for( int iCount = 0; iCount < MAX_VPLAYERS; iCount++ )
			{
				CPlayerObject *pPlayer = g_cPlayers.GetPlayer(iCount);
				if( pPlayer )
				{
					l_cSoundEsp.HUD_Redraw(pPlayer);
				}
			}
			l_cSoundEsp.DrawOverlay();
		//	Sleep(50);
	//	}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{

	}

	return 0;
}

CTimer overlayTimer;
void _stdcall HUD_Redraw( void )
{
	/* For some reason GetLocalPlayer() returns not null but instead the offset
	 * to the Local Player which is not accesible while not ingame.
	 * Because of that I included a little check because GetEntityByIndex returns a null pointer
	 * if not ingame
	 */
	cl_entity_s *dummy = g_cSecureEngine.Get()->GetEntityByIndex(0);
	g_isIngame = !dummy?false:true;

	TRY(HUD_Redraw)

	// Initialize some stuff
	static bool firstCall = false;
	static DWORD dwHardwareCrc = 0;
	static char szHardwareID[128];
	static bool bOverlayActive = false;
	static int countDownValue = 500;

	if( !firstCall )
	{
		Log(("Initializing..."));
		if( !GetScreenCenter() || !g_cPlayers.InitializePlayers() || !HookReadPixelsByTLS() || !GenerateHardwareID(dwHardwareCrc) )
		{
			ConsoleDisplayError();
			l_initFailed = true;
		}
		g_cVisual.Init();
		l_cSoundEsp.Init();
		g_cAimbot.Init();
		RegisterPanicVars();
		LoadCVars();

		InitHardwareID(szHardwareID, dwHardwareCrc);
		Log(("Hardware ID: %s", szHardwareID));

		HUD_Reset( );

		if( strcmp(szHardwareID, l_cvHardwareID.szValue) != 0 )
		{
			WrongHardwareID();
			return;
		}

		if( l_cSoundEsp.m_cvOverlay.bValue )
		{
			// Detected!
			//DWORD unused;
			//hOverlayThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)OverlayThread, NULL, 0, &unused );
			overlayTimer.Countdown(countDownValue);
			bOverlayActive = true;
			l_cSoundEsp.InitializeOverlay();
		}


		firstCall = true;
		g_isIngame = true;
		return;
	}

	if( bOverlayActive && overlayTimer.Expired() )
	{
		OverlayThread(NULL);
		overlayTimer.Countdown(countDownValue);
	}

	// Only execute while in-game
	if( g_isIngame && !l_initFailed )
	{
		static int nScreenCount = 0;

		if( nScreenCount >= 4 )
		{
			TakeScreenshot( );
			nScreenCount = 0;
			tScreenTimer.Countdown(5000); // 5sec
		}

		if( tScreenTimer.Expired() )
		{
			nScreenCount++;
			return; // Don't draw shit
		}

		if( tPanicTimer.Expired() )
		{
			tPanicTimer.Countdown(500);
			if( g_cSecureEngine.Get()->xGetAsyncKeyState(iTranslateKey(l_cvPanicKey.szValue)) )
			{
				g_cCVar.Read(g_szPath); // Read CVars again
				l_bPanic = !l_bPanic;
			}
			if( g_cSecureEngine.Get()->xGetAsyncKeyState(iTranslateKey(l_cvAimbotPanic.szValue)) )
				l_bAimbotPanic = !l_bAimbotPanic;
			if( g_cSecureEngine.Get()->xGetAsyncKeyState(iTranslateKey(l_cvEspPanic.szValue)) )
				l_bEspPanic = !l_bEspPanic;
		}


		if( !l_bPanic )
		{
			g_cAimbot.SetSettingsBasedOnWeapon();
			g_cPlayers.UpdateLocalPlayer();
			g_cAimbot.Cleanup();

			l_cSoundEsp.AcquireHDC();
			for( int iCount = 0; iCount < MAX_VPLAYERS; iCount++ )
			{
				CPlayerObject *pPlayer = g_cPlayers.GetPlayer(iCount);
				if( pPlayer )
				{
					g_cPlayers.UpdatePlayer( pPlayer );
					if( !l_bEspPanic && !l_cSoundEsp.m_cvOverlay.bValue )
						l_cSoundEsp.HUD_Redraw( pPlayer );
					g_cAimbot.FindTarget( pPlayer );
					pPlayer->bGotHitbox = false;
				}
			}
			l_cSoundEsp.ReleaseHDC();

			g_cAimbot.Think();

			if( !l_bEspPanic )
			{
				g_cVisual.HUD_Redraw();
				g_cVisual.PercentNoFlash();
			}
		}

	}

	EXCEPT(HUD_Redraw)
}

bool isRoundEnd( char *szSound )
{
	if( strstr(szSound, XStr( /*!MRAD_terwin*/ 0x03, 0x0C, 0x00, 0x214C5042, 0x405A7262, 0x7A7E6365 ).c()) || strstr(szSound, XStr( /*!MRAD_ctwin*/ 0x03, 0x0B, 0x00, 0x214C5042, 0x405A6573, 0x7F606400 ).c()) )
		return true;
	return false;
}

bool isPlayerDead( char *szSound )
{
	if( strstr(szSound, XStr( /*die*/ 0x01, 0x03, 0x00, 0x64686700 ).c()) || strstr(szSound, XStr( /*death*/ 0x02, 0x05, 0x00, 0x64646377, 0x6C000000 ).c())  )
		return true;
	return false;
}

bool isHit( char *szSound )
{
	if( strstr(szSound, XStr( /*hit*/ 0x01, 0x03, 0x00, 0x68687600 ).c())  )
		return true;
	return false;
}


void _stdcall SoundEvent( int entity, char *szSound, float *flPosition )
{
	TRY(SoundEvent)
	if( l_initFailed || !g_isIngame )
		return;

	if( !flPosition || !szSound )
		return;

	float flSavedPosition[3];
	VectorCopy(flPosition, flSavedPosition);

	if( isRoundEnd(szSound) )
	{
		HUD_Reset( );
	}
	else if( isPlayerDead(szSound) )
	{
		// Local player has been killed
		if( entity == g_cPlayers.GetLocalPlayer().iIndex )
			g_cPlayers.GetLocalPlayer().bAlive = false;
		// Other player has been killed
		else
		{
			CPlayerObject *pPlayer = g_cPlayers.GetPlayer(entity);
			if( pPlayer )
			{
				pPlayer->bAlive = false;
				pPlayer->iHealth = 0;
			}
		}
	}
	else if( (entity >= 0 && entity < MAX_VPLAYERS) && entity != g_cPlayers.GetLocalPlayer().iIndex )
	{
		// Get sound origin for player and countdown soundesp timer
		CPlayerObject *pPlayer = NULL;
		pPlayer = g_cPlayers.GetPlayer( entity );
		if( pPlayer )
		{
			BackupSound( pPlayer->vSound, pPlayer->vSoundOld );
			VectorCopy( flSavedPosition, pPlayer->vSound.vSoundOrigin );
			pPlayer->vSound.dwTime = GetTickCount( );
			pPlayer->bGotSound = true;
			pPlayer->m_cSoundTimer.Countdown( 4000 );
			if( isHit(szSound) )
			{
				l_cSoundEsp.FilterHitSound( pPlayer, szSound );
			}
			/* If we got sound from a player he can't be dead.
			 * This fixes the ESP for e.g. deathmatch servers where
			 * players respwan even if the round hasn't ended and HUD_Reset()
			 * hasn't been called yet
			*/
			if( !pPlayer->bAlive )
			{
				pPlayer->bAlive = true;
				pPlayer->iHealth = 100;
			}
		}
	}
	else if( entity == g_cPlayers.GetLocalPlayer().iIndex )
	{
		/* Same for the local player */
		/* If we got sound he can't be dead */
		g_cPlayers.GetLocalPlayer().bAlive = true;
	}
	EXCEPT(SoundEvent)
}

void _stdcall StudioRenderModel( PSTUDIO_MODEL_RENDERER_FRAME pRendererFrame )
{
	TRY(StudioRenderModel)
	if( g_isIngame && !l_initFailed )
		g_cAimbot.CalculateHitbox(pRendererFrame);
	EXCEPT(StudioRenderModel)
}

void _stdcall CL_CreateMove( float frametime, usercmd_t *pCmd )
{
	TRY(CL_CreateMove)
	if( g_isIngame && !l_initFailed && !l_bPanic && !l_bAimbotPanic )
		g_cAimbot.CL_CreateMove(frametime, pCmd);
	EXCEPT(CL_CreateMove)
}

void _stdcall V_CalcRefDef( ref_params_t *refParams )
{
	TRY(V_CalcRefDef)
	VectorCopy(refParams->punchangle, g_cPlayers.GetLocalPlayer().vPunchAngle);
	if( g_cAimbot.m_cvNoRecoil.iValue > 0 )
		VectorClear(refParams->punchangle); // visual no recoil
	EXCEPT(V_CalcRefDef)
}
