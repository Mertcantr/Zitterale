#include "stdafx.h"
#include "CPlayer.h"
#include "Client.h"
#include "CColor.h"
#include "CCVar.h"
#include "CVisual.h"
#include "COverlay.h"
#include "CSoundESP.h"
#include "Maths.h"
#include "CAimbot.h"
#include "SecureEngine.h"

CSoundEsp::CSoundEsp( )
{
	VectorClear( m_fDrawPosition );
	m_currentDrawColor = NULL;
	m_drawHDC = NULL;
}

CSoundEsp::~CSoundEsp( )
{
	VectorClear( m_fDrawPosition );
	m_currentDrawColor = NULL;
	m_drawHDC = NULL;
}

void CSoundEsp::Init( void )
{
	RegisterVar(m_cvBox, XStr( /*esp_box*/ 0x02, 0x07, 0x00, 0x6572725C, 0x666A7E00 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvBoxSize, XStr( /*esp_boxsize*/ 0x03, 0x0B, 0x00, 0x6572725C, 0x666A7E74, 0x61736F00 ).c(), XStr( /*5*/ 0x01, 0x01, 0x00, 0x35000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvLambert, XStr( /*esp_lambert*/ 0x03, 0x0B, 0x00, 0x6572725C, 0x68646B65, 0x6D7B7E00 ).c(), XStr( /*0*/ 0x01, 0x01, 0x00, 0x30000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvHealth, XStr( /*esp_health*/ 0x03, 0x0A, 0x00, 0x6572725C, 0x6C60676B, 0x7C610000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvDistance, XStr( /*esp_distance*/ 0x03, 0x0C, 0x00, 0x6572725C, 0x606C7573, 0x6967696E ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvLanEsp, XStr( /*esp_lanesp*/ 0x03, 0x0A, 0x00, 0x6572725C, 0x68646862, 0x7B790000 ).c(), XStr( /*0*/ 0x01, 0x01, 0x00, 0x30000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvName, XStr( /*esp_name*/ 0x02, 0x08, 0x00, 0x6572725C, 0x6A646B62 ).c(), XStr( /*0*/ 0x01, 0x01, 0x00, 0x30000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvWeapon, XStr( /*esp_weapon*/ 0x03, 0x0A, 0x00, 0x6572725C, 0x73606777, 0x67670000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvSxeEsp, XStr( /*sxe_esp*/ 0x02, 0x07, 0x00, 0x7379675C, 0x61767600 ).c(), XStr( /*0*/ 0x01, 0x01, 0x00, 0x30000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvSxeEspTime, XStr( /*sxe_time*/ 0x02, 0x08, 0x00, 0x7379675C, 0x706C6B62 ).c(), XStr( /*0*/ 0x01, 0x01, 0x00, 0x30000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvDrawFov, XStr( /*aim_drawfov*/ 0x03, 0x0B, 0x00, 0x61686F5C, 0x60776770, 0x6E667C00 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvAvdraw, XStr( /*aim_avdraw*/ 0x03, 0x0A, 0x00, 0x61686F5C, 0x65736275, 0x697E0000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvOverlay, XStr( /*esp_overlay*/ 0x03, 0x0B, 0x00, 0x6572725C, 0x6B736375, 0x64687300 ).c(), XStr( /*0*/ 0x01, 0x01, 0x00, 0x30000000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvOverlayX, XStr( /*esp_overlay_x*/ 0x04, 0x0D, 0x00, 0x6572725C, 0x6B736375, 0x64687354, 0x74000000 ).c(), XStr( /*10*/ 0x01, 0x02, 0x00, 0x31310000 ).c(), ESP_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvOverlayY, XStr( /*esp_overlay_y*/ 0x04, 0x0D, 0x00, 0x6572725C, 0x6B736375, 0x64687354, 0x75000000 ).c(), XStr( /*10*/ 0x01, 0x02, 0x00, 0x31310000 ).c(), ESP_SECTION, TYPE_INTEGER);
}

void CSoundEsp::DrawFov( CPlayerObject *const pPlayer )
{
	if( pPlayer->bValid && pPlayer->pEnt && pPlayer->bGotHitbox && pPlayer->eTeam != g_cPlayers.GetLocalPlayer().eTeam )
	{
		float fFov = g_cAimbot.CalcFOV(GetDistance(g_cPlayers.GetLocalPlayer().vEyeOrigin, pPlayer->pEnt->origin));
		float fScreen[2];
		if( CalcScreen(pPlayer->vHitbox, fScreen) )
			g_cVisual.DrawBox( fScreen[0], fScreen[1], &g_colFovDraw, fFov );
	}
}

void CSoundEsp::CalculateRadarPoint( const float * fOrigin, int& iScreenX, int& iScreenY )
{
	float fDisX = fOrigin[0] - g_cPlayers.GetLocalPlayer().vEyeOrigin[0];
	float fDisY = fOrigin[1] - g_cPlayers.GetLocalPlayer().vEyeOrigin[1];


	Vector vViewAngles;
	g_cSecureEngine.Get()->GetViewAngles(vViewAngles);

	float fYaw = vViewAngles[1] * (M_PI / 180.0);
	float fScreenX = fDisX * sin( fYaw ) - fDisY * cos( fYaw );
	float fScreenY = fDisX *(-cos( fYaw )) - fDisY * sin( fYaw );
	float m_fRadarRange = 3500;

	if( fabs( fScreenX ) > m_fRadarRange || fabs( fScreenY ) > m_fRadarRange )
	{
		if( fScreenY > fScreenX )
		{
			if( fScreenY > -fScreenX )
			{
				fScreenX = m_fRadarRange * fScreenX / fScreenY;
				fScreenY = m_fRadarRange;
			}
			else
			{
				fScreenY = -m_fRadarRange * fScreenY / fScreenX;
				fScreenX = -m_fRadarRange;
			}
		}
		else
		{
			if( fScreenY > -fScreenX )
			{
				fScreenY = m_fRadarRange * fScreenY / fScreenX;
				fScreenX = m_fRadarRange;
			}
			else
			{
				fScreenX = -m_fRadarRange * fScreenX / fScreenY;
				fScreenY = -m_fRadarRange;
			}
		}
	}

	iScreenX = 100 + int(fScreenX / m_fRadarRange * float( 200 ));
	iScreenY = 100 + int(fScreenY / m_fRadarRange * float( 200 ));
}

void CSoundEsp::InitializeOverlay( void )
{
	if( m_cvOverlay.bValue )
	{
		m_overlay.InitOverlay(m_cvOverlayX.iValue, m_cvOverlayY.iValue, 200, 200);
	}
}

void CSoundEsp::DrawBorder( void )
{
	if( m_cvOverlay.bValue )
		m_overlay.DrawBorder();
}

void CSoundEsp::DrawOverlay( void )
{
	if( m_cvOverlay.bValue )
		m_overlay.RenderOverlay();
}

void CSoundEsp::HUD_Redraw( CPlayerObject *const pPlayer )
{
	// Don't draw on local player
	if( pPlayer->iIndex == g_cPlayers.GetLocalPlayer().iIndex )
		return;

	// Don't drow on non-alive players
	if( !pPlayer->bAlive )
		return;

	// Don't draw on players we're spectating
	if( SpectatingEntity(pPlayer) )
		return;

	if( pPlayer->bValid )
	{
		BackupSound(pPlayer->vSound, pPlayer->vSoundOld);
		VectorCopy(pPlayer->pEnt->origin, m_vOrigin);
	}
	else if( pPlayer->bGotSound ) // SoundESP
	{
		// Smooth FarESP (credits: ko1n)
		DWORD dwCurrent = pPlayer->vSound.dwTime;
		DWORD dwOld = pPlayer->vSoundOld.dwTime;
		DWORD dwNow = GetTickCount( );

		Vector vDeltaOrigin;
		VectorSubtract(pPlayer->vSound.vSoundOrigin, pPlayer->vSoundOld.vSoundOrigin, vDeltaOrigin);

		float fInterp = 1.0f;
		if( dwCurrent + (dwCurrent - dwOld) > dwNow )
		{
			fInterp = 1.0f - ((float)(dwCurrent + (dwCurrent - dwOld) - dwNow) / (float)(dwCurrent - dwOld));
		}

		VectorScale(vDeltaOrigin, fInterp, vDeltaOrigin);
		VectorAdd(pPlayer->vSound.vSoundOrigin, vDeltaOrigin, m_vOrigin);
	}
	else
	{
		return; // No position data available
	}

	// Set color specific on team
	switch( pPlayer->eTeam )
	{
	case TEAM_CT:
		m_currentDrawColor = &g_colCT;
		break;
	case TEAM_T:
		m_currentDrawColor = &g_colT;
		break;
	case TEAM_UNASSIGNED:
		m_currentDrawColor = &g_colUnrecognized;
		break;
	}

	// Check if visible and set visible color if true
	// and not in local players' team
	pPlayer->bVisible = PathFree( m_vOrigin );
	if( pPlayer->bVisible && pPlayer->eTeam != g_cPlayers.GetLocalPlayer().eTeam &&
		g_cPlayers.GetLocalPlayer().bAlive )
		m_currentDrawColor = &g_colVis;

	// Fix visible sound "corpses"
	if( pPlayer->bVisible && !pPlayer->bValid )
		pPlayer->bGotSound = false;

	if( m_cvOverlay.bValue )
	{
		int x, y;
		CalculateRadarPoint(m_vOrigin, x, y);
		m_overlay.DrawRadarPoint(x, y, m_currentDrawColor->iR, m_currentDrawColor->iG, m_currentDrawColor->iB);
		return;
	}

	// Calculate 2D coordinates and finally draw
	if( CalcScreen(m_vOrigin, m_fDrawPosition) )
	{
		float fDistance = GetDistance( g_cPlayers.GetLocalPlayer().vEyeOrigin, m_vOrigin );
		if( m_cvLanEsp.bValue )
		{
			DrawLanEsp( pPlayer );
			return; // Don't draw anything else
		}
		if( m_cvBox.bValue )
			DrawBox( pPlayer, fDistance );

		if( !m_cvSxeEsp.bValue ) // sXe detects items drawn on screen
		{
			if( m_cvHealth.bValue && SECURITY_LEVEL(1) )
				DrawHealth( pPlayer );
			if( m_cvLambert.bValue )
				Lambert( pPlayer );
			if( m_cvDrawFov.bValue )
				DrawFov( pPlayer );
			DrawText( pPlayer, fDistance );
			if( m_cvAvdraw.bValue )
				AvDraw( pPlayer );
		}
	}
}

void CSoundEsp::DrawBox( CPlayerObject *const pPlayer, float fDistance )
{
	if( fDistance < 1 )
		fDistance = 1;
	float fRadius = m_cvBoxSize.iValue * (75.0f / fDistance);

	if( m_cvSxeEsp.bValue )
	{
		if( pPlayer->m_cSxeTimer.Expired() )
		{
			g_cVisual.drawSxeBox( m_drawHDC, m_fDrawPosition[0], m_fDrawPosition[1], m_cvBoxSize.iValue*6, RGB(m_currentDrawColor->iR, m_currentDrawColor->iG, m_currentDrawColor->iB));
			if( m_cvSxeEspTime.iValue > 0 )
				pPlayer->m_cSxeTimer.Countdown(m_cvSxeEspTime.iValue);
		}

	}
	else
		g_cVisual.DrawBox( m_fDrawPosition[0], m_fDrawPosition[1], m_currentDrawColor, fRadius );


}

void CSoundEsp::Lambert( CPlayerObject *const pPlayer )
{
	if( pPlayer->bValid )
	{
		dlight_t *pLight = g_cSecureEngine.Get()->CL_AllocDlight( 0 );
		if( pLight )
		{
			pLight->color.r = m_currentDrawColor->iR;
			pLight->color.g = m_currentDrawColor->iG;
			pLight->color.b = m_currentDrawColor->iB;

			pLight->origin = pPlayer->pEnt->origin;
			pLight->radius = 45.0f;
			pLight->die = g_cSecureEngine.Get()->GetClientTime() + 0.1f;
		}
	}
}

void CSoundEsp::DrawHealth( CPlayerObject *const pPlayer )
{
	color_t colHealth = { 0, 255, 0, 255 }; // green
	if( pPlayer->iHealth < 60 && pPlayer->iHealth > 20 )
	{
		// Orange
		colHealth.iR = 255;
		colHealth.iG = 165;
		colHealth.iB = 0;
	}
	else if( pPlayer->iHealth <= 20 )
	{
		// Red
		colHealth.iR = 255;
		colHealth.iG = 0;
		colHealth.iB = 0;
	}
	g_cVisual.DrawHudStringFormat( m_fDrawPosition[0], m_fDrawPosition[1]-50, true,
		&colHealth, XStr( /*[%i]*/ 0x01, 0x04, 0x00, 0x5B246B5E ).c(), pPlayer->iHealth );
}

void CSoundEsp::DrawText( CPlayerObject *const pPlayer, float fDistance )
{
	int iY = m_fDrawPosition[1];

	if( pPlayer->bUpdated )
	{
		if( m_cvName.bValue )
		{
			char szName[32];
			int iLen = lstrlen(pPlayer->szName);
			BoundValue(iLen, 25, 1);
			strncpy_s( szName, sizeof(szName), pPlayer->szName, iLen );
			szName[iLen] = '\0';
			g_cVisual.DrawHudString( m_fDrawPosition[0], iY, true, m_currentDrawColor, szName );
		}

		if( m_cvWeapon.bValue )
		{
			iY -= 15;
			g_cVisual.DrawHudString( m_fDrawPosition[0], iY, true, m_currentDrawColor, pPlayer->szWeapon );
		}

		if( m_cvDistance.bValue )
		{
			iY -= 15;
			g_cVisual.DrawHudStringFormat( m_fDrawPosition[0], iY, true, m_currentDrawColor, XStr( /*- %.2f -*/ 0x02, 0x08, 0x00, 0x2D21272D, 0x3663262A ).c(), fDistance );
		}
	}
}

bool CSoundEsp::SpectatingEntity( CPlayerObject *const pPlayer )
{
	if( !g_cPlayers.GetLocalPlayer( ).bAlive && g_cPlayers.GetLocalPlayer( ).pEnt )
	{
		if( g_cPlayers.GetLocalPlayer( ).pEnt->curstate.iuser1 && g_cPlayers.GetLocalPlayer( ).pEnt->curstate.iuser2 == pPlayer->iIndex && pPlayer->bAlive )
			return true;
	}

	return false;
}

void CSoundEsp::FilterHitSound( CPlayerObject *const pPlayer, const char *szSound )
{
	int iSub = 0;

	srand( GetTickCount() );

	// flesh hit
	if( szSound[12] == 'f' )
	{
		iSub = rand()%50;
		BoundValue(iSub, 50, 25);
	}
	// kevlar hit
	else if( szSound[12] == 'k' )
	{
		iSub = rand()%35;
		BoundValue(iSub, 35, 15);
	}
	// helmet hit
	else if( szSound[12] == 'h' )
	{
		iSub = rand()%100;
		BoundValue(iSub, 100, 80);
	}

	pPlayer->iHealth -= iSub;
	BoundValue(pPlayer->iHealth, 100, 1);
}

void CSoundEsp::DrawLanEsp( CPlayerObject *const pPlayer )
{
	// Don't draw on players in my team
	if( pPlayer->eTeam == g_cPlayers.GetLocalPlayer().eTeam )
		return;

	// 105 105 105 696969 DimGrey  DimGrey
	g_cSecureEngine.Get()->pfnFillRGBA( m_fDrawPosition[0], m_fDrawPosition[1]-30, 3, 3, 105,
		105, 105, m_cvLanEsp.iValue );
}

void CSoundEsp::AvDraw( CPlayerObject *const pPlayer )
{
	if( !pPlayer->bValid || !pPlayer->bGotHitbox )
		return;

	float fDraw[2];
	if( CalcScreen(pPlayer->vHitbox, fDraw) )
	{
		g_cSecureEngine.Get()->pfnFillRGBA( fDraw[0]-1, fDraw[1]-1, 3, 3, m_currentDrawColor->iR,
			m_currentDrawColor->iG, m_currentDrawColor->iB, 255 );
	}
}

void CSoundEsp::AcquireHDC( void )
{
	if( m_cvSxeEsp.bValue )
		m_drawHDC = GetWindowDC(GetForegroundWindow());
}

void CSoundEsp::ReleaseHDC( void )
{
	if( m_drawHDC )
	{
		ReleaseDC(GetForegroundWindow(), m_drawHDC);
	}
}
