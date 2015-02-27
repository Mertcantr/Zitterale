#pragma once

#include <ddraw.h>
#include <GdiPlus.h>
#include <MMSystem.h>
#include <aviriff.h>

class COverlay
{
public:
	bool InitOverlay( int x, int y, int w, int h );
	bool ConvertSurfaceToYUY2( IDirectDrawSurface7* pSourceSurface, IDirectDrawSurface7* pDestSurface );
	bool ConvertSurfaceToUYVY( IDirectDrawSurface7* pSourceSurface, IDirectDrawSurface7* pDestSurface );
	bool RenderOverlay( );
	void FillRGB( int x, int y, int w, int h, BYTE r, BYTE g, BYTE b );
	void SetPixel( int x, int y, BYTE r, BYTE g, BYTE b );
	void Print( int x, int y, COLORREF FontColor, const char* fmt, ... );
	void Resize( int w, int h );
	void Move( int x, int y );
	void DrawRadarPoint( int x,int y, int r, int g, int b );
	void DrawBorder( void );
	LPDIRECTDRAWSURFACE7 m_SurfaceDraw;

private:
	RECT m_RectSource;
	RECT m_RectDestination;
	IDirectDraw7* m_pDirectDraw;
	DDSURFACEDESC2 m_SurfaceDescriptor;
	LPDIRECTDRAWSURFACE7 m_SurfaceOverlay;
	LPDIRECTDRAWSURFACE7 m_SurfacePrimary;
	LPDIRECTDRAWSURFACE7 m_SurfaceOffscreen;
	ULONG m_uPixelFormat;
	bool m_bUpdate;
};
