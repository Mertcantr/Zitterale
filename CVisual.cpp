#include "stdafx.h"
#include "CColor.h"
#include "CPlayer.h"
#include "Client.h"
#include "CCVar.h"
#include "CVisual.h"
#include "Maths.h"
#include "SecureEngine.h"

CVisual g_cVisual;

CVisual::CVisual()
{
	m_vDrawString.clear( );
}

CVisual::~CVisual()
{
	vector<drawstring_t*>::iterator ic;
	for( ic = m_vDrawString.begin( ); ic != m_vDrawString.end( ); ic++ )
		delete (*ic);

	m_vDrawString.clear( );
}

void CVisual::Init( void )
{
	RegisterVar(m_cvBoxthickness, XStr( /*misc_boxthickness*/ 0x05, 0x11, 0x00, 0x6D687160, 0x5B67697F, 0x7C616368, 0x67636B7C, 0x63000000 ).c(), XStr( /*2*/ 0x01, 0x01, 0x00, 0x32000000 ).c(), VISUAL_SECTION, TYPE_INTEGER);
	RegisterVar(m_cvFlashpercent, XStr( /*misc_flashpercent*/ 0x05, 0x11, 0x00, 0x6D687160, 0x5B636A66, 0x7B617A6E, 0x7E6E6B61, 0x64000000 ).c(), XStr( /*80*/ 0x01, 0x02, 0x00, 0x38310000 ).c(), VISUAL_SECTION, TYPE_INTEGER);
}

void CVisual::drawSxeBox( HDC DCcs, int x, int y, int size, COLORREF colors )
{
	int l1;
	int linesize = size/2;

	for (l1=0;l1<size;l1++)
		SetPixel(DCcs,(x-linesize)+l1,y-linesize,colors);
	for(l1=0;l1<size;l1++)
		SetPixel(DCcs,(x-linesize)+l1,y+linesize,colors);
	for(l1=0;l1<size;l1++)
		SetPixel(DCcs,x-linesize,(y-linesize)+l1,colors);
	for (l1=0;l1<size;l1++)
		SetPixel(DCcs,x+linesize,(y-linesize)+l1,colors);
}


void CVisual::PercentNoFlash( void )
{
	screenfade_t sScreenfade;
	g_cSecureEngine.Get()->pfnGetScreenFade( &sScreenfade );
	sScreenfade.fadealpha = 2.55 * m_cvFlashpercent.iValue;
	g_cSecureEngine.Get()->pfnSetScreenFade( &sScreenfade );
}

void CVisual::DrawBox( int iX, int iY, color_t *pColor, int iRadius )
{
	if( !pColor )
		return;

	int iNewRadius = iRadius * 2;
	int iHeight = m_cvBoxthickness.iValue; // Thickness
	g_cSecureEngine.Get()->pfnFillRGBA( iX-iRadius+iHeight, iY-iRadius, iNewRadius-iHeight, iHeight,
		pColor->iR, pColor->iG, pColor->iB, pColor->iA );
	g_cSecureEngine.Get()->pfnFillRGBA( iX-iRadius, iY-iRadius, iHeight,
		iNewRadius,
		pColor->iR, pColor->iG, pColor->iB, pColor->iA );
	g_cSecureEngine.Get()->pfnFillRGBA( iX-iRadius, iY+iRadius,
		iNewRadius, iHeight,
		pColor->iR, pColor->iG, pColor->iB, pColor->iA );
	g_cSecureEngine.Get()->pfnFillRGBA( iX+iRadius, iY-iRadius,
		iHeight, iNewRadius+iHeight, pColor->iR,
		pColor->iG, pColor->iB, pColor->iA );
}

void CVisual::DrawFilledBox( int iX, int iY, int iW, int iH,
				   color_t *pColor, color_t *pFillColor )
{
	if( !pColor || !pFillColor )
		return;

	g_cSecureEngine.Get()->pfnFillRGBA( iX, iY, iW, 3, pColor->iR, pColor->iG, pColor->iB, pColor->iA );
	g_cSecureEngine.Get()->pfnFillRGBA( iX, iY, 3, iH, pColor->iR, pColor->iG, pColor->iB, pColor->iA );
	g_cSecureEngine.Get()->pfnFillRGBA( iX+iW, iY, 3, iH, pColor->iR, pColor->iG, pColor->iB, pColor->iA );
	g_cSecureEngine.Get()->pfnFillRGBA( iX, iY+iH, iW+3, 3, pColor->iR, pColor->iG, pColor->iB, pColor->iA );
	g_cSecureEngine.Get()->pfnFillRGBA( iX, iY, iW+3, 3, pColor->iR, pColor->iG, pColor->iB, pColor->iA );
	g_cSecureEngine.Get()->pfnFillRGBA( iX, iY, iW, iH, pFillColor->iR, pFillColor->iG, pFillColor->iB,
		pFillColor->iA );
}

void CVisual::DrawHudString( int iX, int iY, bool bCenter, color_t *pColor,
				   char *szString )
{
	if( !pColor )
		return;
	int iLen;
	g_cSecureEngine.Get()->pfnDrawSetTextColor( pColor->iR/255.0f,
		pColor->iG/255.0f, pColor->iB/255.0f );
	if( bCenter )
	{
		int iTemp;
		g_cSecureEngine.Get()->pfnDrawConsoleStringLen( szString, &iLen,
			&iTemp );
		iX -= iLen / 2;
	}
	g_cSecureEngine.Get()->pfnDrawConsoleString( iX, iY, szString );
}

void CVisual::DrawHudStringFormat( int iX, int iY, bool bCenter, color_t *pColor,
						 const char *szFormat, ... )
{
	char szBuf[1024];

	va_list va_alist;
	va_start( va_alist, szFormat );
	_vsnprintf_s( szBuf, sizeof(szBuf), _TRUNCATE, szFormat, va_alist );
	va_end( va_alist );

	DrawHudString( iX, iY, bCenter, pColor, szBuf );
}

void CVisual::AddCenterDrawString( const char *szString, DWORD dwVisibleTime )
{
	drawstring_t *pDraw = new drawstring_t;
	if( pDraw )
	{
		lstrcpy(pDraw->szString, szString);
		pDraw->dwVisibleTime = dwVisibleTime;
		pDraw->dwStartTime = 0;
		m_vDrawString.push_back(pDraw);
	}
	else
		Log(("Error allocating memory for center draw string!"));
}

void CVisual::HUD_Redraw( void )
{
	static bool bGot = false;
	static vector<drawstring_t*>::iterator ic;
	static drawstring_t *pDraw = NULL;

	if( !bGot)
	{
		// If empty return
		if( m_vDrawString.empty() )
			return;

		for( ic = m_vDrawString.begin(); ic != m_vDrawString.end(); ic++ )
		{
			if( *ic )
			{
				pDraw = *ic;
				bGot = true;
				break;
			}
		}
	}

	if( pDraw && bGot )
	{
		// Draw the item as long it's wanted
		if( pDraw->dwStartTime == 0 )
			pDraw->dwStartTime = GetTickCount();
		if( (pDraw->dwStartTime+pDraw->dwVisibleTime) >= GetTickCount() )
		{
			DrawHudString(g_iScreenCenterX, g_iScreenCenterY, true, &g_colHudString, pDraw->szString);
		}
		// After time is over erase the item
		else
		{
			bGot = false;
			m_vDrawString.erase(ic);
		}
	}
}
