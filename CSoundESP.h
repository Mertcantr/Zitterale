#pragma once

#include "COverlay.h"

class CSoundEsp
{
public:
	CSoundEsp();
	virtual ~CSoundEsp();

	void Init( void );
	void HUD_Redraw( CPlayerObject *const pPlayer );
	void FilterHitSound( CPlayerObject *const pPlayer, const char *szSound );
	void AcquireHDC( void );
	void ReleaseHDC( void );
	void InitializeOverlay( void );
	void DrawOverlay( void );
	void DrawBorder( void );

	CVarObject m_cvOverlay;

private:
	void CalculateRadarPoint( const float * fOrigin, int& iScreenX, int& iScreenY );
	void DrawFov( CPlayerObject *const pPlayer );
	void DrawBox( CPlayerObject *const pPlayer, float fDistance );
	void DrawText( CPlayerObject *const pPlayer, float fDistance );
	void DrawHealth( CPlayerObject *const pPlayer );
	void Lambert( CPlayerObject *const pPlayer );
	void AvDraw( CPlayerObject *const pPlayer );
	bool SpectatingEntity( CPlayerObject *const pPlayer );
	void DrawLanEsp( CPlayerObject *const pPlayer );
	float m_fDrawPosition[2]; // Current drawing position
	color_t *m_currentDrawColor;
	vec3_t m_vOrigin;
	HDC m_drawHDC;
	COverlay m_overlay;

	// CVar's
	CVarObject m_cvBox, m_cvBoxSize, m_cvLambert, m_cvHealth, m_cvName, m_cvWeapon, m_cvDistance, m_cvAvdraw, m_cvLanEsp, m_cvDrawFov, m_cvSxeEsp, m_cvSxeEspTime, m_cvOverlayY, m_cvOverlayX;
};
