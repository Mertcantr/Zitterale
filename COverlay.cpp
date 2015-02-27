#include "stdafx.h"
#include "COverlay.h"
#pragma comment(lib, "dxguid.lib")


/*
*	~GetError
*
*	- [DEBUG] used to show errors of ddraw functions
*/

void GetError( HRESULT hRes )
{
	switch( hRes )
	{
	case DDERR_DEVICEDOESNTOWNSURFACE:
		Log(("DDERR_DEVICEDOESNTOWNSURFACE"));
		break;
	case DDERR_GENERIC:
		Log(("DDERR_GENERIC"));
		break;
	case DDERR_HEIGHTALIGN:
		Log(("DDERR_HEIGHTALIGN"));
		break;
	case DDERR_INVALIDOBJECT:
		Log(("DDERR_INVALIDOBJECT"));
		break;
	case DDERR_INVALIDPARAMS:
		Log(("DDERR_INVALIDPARAMS"));
		break;
	case DDERR_INVALIDRECT:
		Log(("DDERR_INVALIDRECT"));
		break;
	case DDERR_NOSTRETCHHW:
		Log(("DDERR_NOSTRETCHHW"));
		break;
	case DDERR_NOTAOVERLAYSURFACE:
		Log(("DDERR_NOTAOVERLAYSURFACE"));
		break;
	case DDERR_OUTOFCAPS:
		Log(("DDERR_OUTOFCAPS"));
		break;
	case DDERR_SURFACELOST:
		Log(("DDERR_SURFACELOST"));
		break;
	case DDERR_UNSUPPORTED:
		Log(("DDERR_UNSUPPORTED"));
		break;
	case DDERR_INVALIDSURFACETYPE:
		Log(("DDERR_INVALIDSURFACETYPE"));
		break;
	case DDERR_DCALREADYCREATED:
		Log(("DDERR_DCALREADYCREATED"));
		break;
	case DDERR_WASSTILLDRAWING:
		Log(("DDERR_WASSTILLDRAWING"));
		break;
	case DDERR_INCOMPATIBLEPRIMARY:
		Log(("INCOMPATIBLEPRIMARY"));
		break;
	case DDERR_INVALIDCAPS:
		Log(("INVALIDCAPS"));
		break;
	case DDERR_INVALIDPIXELFORMAT:
		Log(("INVALIDPIXELFORMAT"));
		break;
	case DDERR_OUTOFMEMORY:
		Log(("OUTOFMEMORY"));
		break;
	case DDERR_OUTOFVIDEOMEMORY:
		Log(("OUTOFVIDEOMEMORY"));
		break;
	case DDERR_PRIMARYSURFACEALREADYEXISTS:
		Log(("PRIMARYSURFACEALREADYEXISTS"));
		break;
	case DDERR_NOFLIPHW:
		Log(("NOFLIPHW"));
		break;
	case DDERR_NOOVERLAYHW:
		Log(("NOOVERLAYHW"));
		break;
	case DDERR_NOCOOPERATIVELEVELSET:
		Log(("NOCOOPERATIVELEVELSET"));
		break;
	case DDERR_NOALPHAHW:
		Log(("NOALPHAHW"));
		break;
	default:
		Log(("Unkonwn error :("));
		break;
	}
}


/*
*	InitOverlay
*
*	- Creates the surfaces for the overlay
*/
typedef HRESULT (WINAPI *DirectDrawCreateEx_t)(GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter);
DirectDrawCreateEx_t nDirectDrawCreateEx;
bool COverlay::InitOverlay( int x, int y, int w, int h )
{
	HRESULT		hResult			= NULL;
	DDCAPS		DeviceCaps		= {0};
	ULONG		uPixelFormat[2]	= { mmioFOURCC('Y','U','Y','2'), mmioFOURCC('U','Y','V','Y') };
	DDSCAPS2	caps			= {0};

	m_RectSource.left = 0;
	m_RectSource.top = 0;
	m_RectSource.right = w;
	m_RectSource.bottom = h;

	m_RectDestination.left = x;
	m_RectDestination.top = y;
	m_RectDestination.right = x + w;
	m_RectDestination.bottom = y + h;

	HMODULE hDdraw = LoadLibrary("ddraw.dll");

	if(!hDdraw)
	{
		Log(("Failed to load ddraw.dll %d", GetLastError()));
		return false;
	}

	nDirectDrawCreateEx = (DirectDrawCreateEx_t) GetProcAddress(hDdraw, "DirectDrawCreateEx");

	if(!nDirectDrawCreateEx)
	{
		Log(("Failed to get address of DirectDrawCreateEx %d", GetLastError()));
		return false;
	}


	hResult = nDirectDrawCreateEx( 0, (PVOID*) &m_pDirectDraw, IID_IDirectDraw7, NULL );
	if( hResult != DD_OK )
	{
		Log(("[COverlay] DirectDrawCreateEx failed!" ));
		return FALSE;
	}

	hResult = m_pDirectDraw->SetCooperativeLevel( 0, DDSCL_NORMAL );
	if( hResult != DD_OK )
	{
		Log(("[COverlay] SetCooperativeLevel failed!" ));
		return FALSE;
	}

	RtlZeroMemory( &DeviceCaps, sizeof( DDCAPS ) );
	DeviceCaps.dwSize = sizeof( DDCAPS );

	hResult = m_pDirectDraw->GetCaps( &DeviceCaps, NULL );
	if( hResult != DD_OK )
	{
		Log(("[COverlay] GetCaps failed!\n" ));
		return FALSE;
	}


	// Check which modi are supported!
	if( ( DeviceCaps.dwCaps & DDCAPS_OVERLAY ) != DDCAPS_OVERLAY )
	{
		Log(( "[COverlay] Overlay not supported!" ));
		return FALSE;
	}


	// Create primary surface!
	RtlZeroMemory( &m_SurfaceDescriptor, sizeof( DDSURFACEDESC2 ) );
	m_SurfaceDescriptor.dwSize = sizeof( DDSURFACEDESC2 );
	m_SurfaceDescriptor.dwFlags = DDSD_CAPS;
	m_SurfaceDescriptor.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	hResult = m_pDirectDraw->CreateSurface( &m_SurfaceDescriptor, &m_SurfacePrimary, NULL );
	if( hResult != DD_OK )
	{
		Log(("[COverlay] CreateSurface - Primary failed!" ));
		return FALSE;
	}


	// Create overlay surface
	RtlZeroMemory( &m_SurfaceDescriptor, sizeof( DDSURFACEDESC2 ) );
	m_SurfaceDescriptor.dwSize = sizeof( DDSURFACEDESC2 );
	m_SurfaceDescriptor.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_BACKBUFFERCOUNT;
	m_SurfaceDescriptor.dwHeight = m_RectSource.bottom;
	m_SurfaceDescriptor.dwWidth = m_RectSource.right;
	m_SurfaceDescriptor.dwBackBufferCount = 1;
	m_SurfaceDescriptor.ddpfPixelFormat.dwSize = sizeof( DDPIXELFORMAT );
	m_SurfaceDescriptor.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
	m_SurfaceDescriptor.ddsCaps.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

	for( int i = 0; i < 2; i++ )
	{
		m_SurfaceDescriptor.ddpfPixelFormat.dwFourCC = uPixelFormat[i];
		hResult = m_pDirectDraw->CreateSurface( &m_SurfaceDescriptor, &m_SurfaceOverlay, NULL );
		if( hResult == DD_OK )
		{
			m_uPixelFormat = uPixelFormat[i];
			break;
		}
	}

	if( hResult != DD_OK )
	{
		GetError(hResult);
		Log(("[COverlay] CreateSurface - Overlay failed!" ));
		return FALSE;
	}


	// create draw surface
	RtlZeroMemory( &m_SurfaceDescriptor, sizeof( DDSURFACEDESC2 ) );
	m_SurfaceDescriptor.dwSize = sizeof( DDSURFACEDESC2 );
	m_SurfaceDescriptor.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	m_SurfaceDescriptor.dwHeight = m_RectSource.bottom;
	m_SurfaceDescriptor.dwWidth = m_RectSource.right;

	m_SurfaceDescriptor.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

	hResult = m_pDirectDraw->CreateSurface( &m_SurfaceDescriptor, &m_SurfaceDraw, NULL );
	if( hResult != DD_OK )
	{
		Log(("[COverlay] CreateSurface - Clean failed!" ));
		return FALSE;
	}


	// Create offscreen surface
	RtlZeroMemory( &caps, sizeof( DDSCAPS2 ) );
	caps.dwCaps = DDSCAPS_BACKBUFFER;

	m_SurfaceOverlay->GetAttachedSurface( &caps, &m_SurfaceOffscreen );
	if( hResult != DD_OK )
	{
		Log(("[COverlay] CreateSurface - Offscreen failed!" ));
		return FALSE;
	}

	m_bUpdate = TRUE;

	DDCOLORKEY ddColorKey;

	memset( &ddColorKey, 0, sizeof( DDCOLORKEY ) );
	ddColorKey.dwColorSpaceHighValue = 0x00000000;
	ddColorKey.dwColorSpaceLowValue  = 0x00000000;

	m_SurfaceOverlay->SetColorKey( DDCKEY_SRCOVERLAY, & ddColorKey );

	//printf( "[COverlay] Surfaces are ready!\n" );
	return TRUE;
}


/*
*	ConvertSurfaceToYUY2
*
*	- Surface conversion form RGB to YUY2
*/

bool COverlay::ConvertSurfaceToYUY2( IDirectDrawSurface7* pSourceSurface, IDirectDrawSurface7* pDestSurface )
{
	HRESULT			hResult				= NULL;
	PBYTE			pSurfaceMap			= NULL;
	PBYTE			pOverlayMap			= NULL;
	DDSURFACEDESC2	OverlayDescriptor	= {0};
	BYTE			RGB[6]				= {0};
	int				x					= 0;
	int				y					= 0;
	int				w					= 0;
	int				h					= 0;

	if( !pSourceSurface ||!pDestSurface )
		return FALSE;

	RtlZeroMemory( &m_SurfaceDescriptor, sizeof( DDSURFACEDESC2 ) );
	m_SurfaceDescriptor.dwSize = sizeof( DDSURFACEDESC2 );

	hResult = pSourceSurface->Lock( NULL, &m_SurfaceDescriptor, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL );
	if( hResult != DD_OK )
	{
		//printf("[COverlay] Surface - Lock failed!\n" );
		return FALSE;
	}

	pSurfaceMap = (PBYTE) m_SurfaceDescriptor.lpSurface;
	if( !pSurfaceMap )
	{
		//printf("[COverlay] pSurfaceMap == NULL \n" );
		hResult = pSourceSurface->Unlock( NULL );
		return FALSE;
	}

	RtlZeroMemory( &OverlayDescriptor, sizeof( DDSURFACEDESC2 ) );
	OverlayDescriptor.dwSize = sizeof( DDSURFACEDESC2 );

	hResult = pDestSurface->Lock( NULL, &OverlayDescriptor, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL );
	if( hResult != DD_OK )
	{
		//printf("[COverlay] SurfaceOverlay - Lock failed!\n" );
		return FALSE;
	}

	pOverlayMap = (PBYTE) OverlayDescriptor.lpSurface;
	if( !pOverlayMap )
	{
		//printf("[COverlay] pSurfaceMap == NULL \n" );
		hResult = pDestSurface->Unlock( NULL );
		return FALSE;
	}


	w = m_SurfaceDescriptor.dwWidth;
	h = m_SurfaceDescriptor.dwHeight;

	if( !w || !h )
	{
		//printf("[COverlay] Height or width are zero! \n" );
		hResult = pSourceSurface->Unlock( NULL );
		return FALSE;
	}

	for(y = 0;y<h;y++)
	{
		for(x = 0;x<w;x += 2)
		{
			RGB[2] = pSurfaceMap[x*2+0];
			RGB[1] = pSurfaceMap[x*2+1];
			RGB[0] = pSurfaceMap[x*2+2];
			RGB[5] = pSurfaceMap[x*2+4];
			RGB[4] = pSurfaceMap[x*2+5];
			RGB[3] = pSurfaceMap[x*2+6];

			*(pOverlayMap++) = *(pSurfaceMap++) = (BYTE)(0.257*RGB[0]+0.504*RGB[1]+0.098*RGB[2]+16);
			*(pOverlayMap++) = *(pSurfaceMap++) = (BYTE)(-0.148*RGB[0]-0.291*RGB[1]+0.439*RGB[2]+128);
			*(pOverlayMap++) = *(pSurfaceMap++) = (BYTE)(0.257*RGB[3]+0.504*RGB[4]+0.098*RGB[5]+16);
			*(pOverlayMap++) = *(pSurfaceMap++) = (BYTE)(0.439*RGB[3]-0.368*RGB[4]-0.071*RGB[5]+128);
		}
		pSurfaceMap += m_SurfaceDescriptor.lPitch-w*2;
		pOverlayMap += OverlayDescriptor.lPitch - w*2;
	}

	hResult = pSourceSurface->Unlock( NULL );
	if( hResult != DD_OK )
	{
		//printf("[COverlay] Final unlock failed!\n" );
		return FALSE;
	}

	hResult = pDestSurface->Unlock( NULL );
	if( hResult != DD_OK )
	{
		//printf("[COverlay] Final unlock failed!\n" );
		return FALSE;
	}

	return TRUE;
}


/*
*	ConvertSurfaceToUYVY
*
*	- Surface conversion form RGB to UYVY
*/

bool COverlay::ConvertSurfaceToUYVY( IDirectDrawSurface7* pSourceSurface, IDirectDrawSurface7* pDestSurface )
{
	HRESULT			hResult				= NULL;
	PBYTE			pSurfaceMap			= NULL;
	PBYTE			pOverlayMap			= NULL;
	DDSURFACEDESC2	OverlayDescriptor	= {0};
	BYTE			RGB[6]				= {0};
	int				x					= 0;
	int				y					= 0;
	int				w					= 0;
	int				h					= 0;

	if( !pSourceSurface ||!pDestSurface )
		return FALSE;

	RtlZeroMemory( &m_SurfaceDescriptor, sizeof( DDSURFACEDESC2 ) );
	m_SurfaceDescriptor.dwSize = sizeof( DDSURFACEDESC2 );

	hResult = pSourceSurface->Lock( NULL, &m_SurfaceDescriptor, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL );
	if( hResult != DD_OK )
	{
		//printf("[COverlay] Surface - Lock failed!\n" );
		return FALSE;
	}

	pSurfaceMap = (PBYTE) m_SurfaceDescriptor.lpSurface;
	if( !pSurfaceMap )
	{
		//printf("[COverlay] pSurfaceMap == NULL \n" );
		hResult = pSourceSurface->Unlock( NULL );
		return FALSE;
	}

	RtlZeroMemory( &OverlayDescriptor, sizeof( DDSURFACEDESC2 ) );
	OverlayDescriptor.dwSize = sizeof( DDSURFACEDESC2 );

	hResult = pDestSurface->Lock( NULL, &OverlayDescriptor, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL );
	if( hResult != DD_OK )
	{
		//printf("[COverlay] SurfaceOverlay - Lock failed!\n" );
		return FALSE;
	}

	pOverlayMap = (PBYTE) OverlayDescriptor.lpSurface;
	if( !pOverlayMap )
	{
		//printf("[COverlay] pSurfaceMap == NULL \n" );
		hResult = pDestSurface->Unlock( NULL );
		return FALSE;
	}


	w = m_SurfaceDescriptor.dwWidth;
	h = m_SurfaceDescriptor.dwHeight;

	if( !w || !h )
	{
		//printf("[COverlay] Height or width are zero! \n" );
		hResult = pSourceSurface->Unlock( NULL );
		return FALSE;
	}

	for(y = 0;y<h;y++)
	{
		for(x = 0;x<w;x += 2)
		{
			RGB[2] = pSurfaceMap[x*2+0];
			RGB[1] = pSurfaceMap[x*2+1];
			RGB[0] = pSurfaceMap[x*2+2];
			RGB[5] = pSurfaceMap[x*2+4];
			RGB[4] = pSurfaceMap[x*2+5];
			RGB[3] = pSurfaceMap[x*2+6];

			*(pOverlayMap++) = *(pSurfaceMap++) = (BYTE)(-0.148*RGB[0]-0.291*RGB[1]+0.439*RGB[2]+128);
			*(pOverlayMap++) = *(pSurfaceMap++) = (BYTE)(0.257*RGB[0]+0.504*RGB[1]+0.098*RGB[2]+16);
			*(pOverlayMap++) = *(pSurfaceMap++) = (BYTE)(0.439*RGB[3]-0.368*RGB[4]-0.071*RGB[5]+128);
			*(pOverlayMap++) = *(pSurfaceMap++) = (BYTE)(0.257*RGB[3]+0.504*RGB[4]+0.098*RGB[5]+16);
		}
		pSurfaceMap += m_SurfaceDescriptor.lPitch-w*2;
		pOverlayMap += OverlayDescriptor.lPitch - w*2;
	}

	hResult = pSourceSurface->Unlock( NULL );
	if( hResult != DD_OK )
	{
		//printf("[COverlay] Final unlock failed!\n" );
		return FALSE;
	}

	hResult = pDestSurface->Unlock( NULL );
	if( hResult != DD_OK )
	{
		//printf("[COverlay] Final unlock failed!\n" );
		return FALSE;
	}

	return TRUE;
}


/*
*	RenderOverlay
*
*	- Renders the current surface to the overlay
*/

bool COverlay::RenderOverlay( )
{
	HRESULT hResult = NULL;
	bool	bResult = FALSE;


	switch( m_uPixelFormat )
	{
	case mmioFOURCC('Y','U','Y','2'):
		bResult = ConvertSurfaceToYUY2( m_SurfaceDraw, m_SurfaceOffscreen );
		break;
	case mmioFOURCC('U','Y','V','Y'):
		bResult = ConvertSurfaceToUYVY( m_SurfaceDraw, m_SurfaceOffscreen );
		break;
	}

	if( !bResult )
	{
		Log(("[COverlay] Unable convert surface to target pixel format!\n" ));
		return FALSE;
	}

	m_SurfaceOverlay->Flip( 0, DDFLIP_WAIT );

	if( m_bUpdate )
	{
		hResult = m_SurfaceOverlay->UpdateOverlay( &m_RectSource, m_SurfacePrimary, &m_RectDestination, DDOVER_SHOW, NULL );
		if( hResult != DD_OK )
		{
			Log(("[COverlay] UpdateOverlay failed! -> " ));
			GetError( hResult );
			return FALSE;
		}

		m_bUpdate = FALSE;
	}

	return TRUE;
}

/*
*	FillRGB
*
*	- Fill a rectangle with a color
*/

void COverlay::FillRGB( int x, int y, int w, int h, BYTE r, BYTE g, BYTE b )
{
	DDBLTFX fx		= {0};
	RECT	rect	= {0};

	rect.left = x;
	rect.top = y;
	rect.right = x + w;
	rect.bottom = y + h;

	if( rect.left < 0 || rect.top < 0 ||
		rect.right > m_RectSource.right || rect.bottom > m_RectSource.bottom )
	{
		return;
	}

	RtlZeroMemory( &fx, sizeof( DDBLTFX ) );
	fx.dwSize = sizeof( DDBLTFX );
	fx.dwFillColor = RGB( b, g, r );

	m_SurfaceDraw->Blt( &rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &fx );
}


/*
*	SetPixel
*
*	- Fill a pixel with a color
*	* Note: SLOW as FUCK!
*/

void COverlay::SetPixel( int x, int y, BYTE r, BYTE g, BYTE b )
{
	HRESULT hResult = NULL;
	HDC		hDC		= NULL;

	hResult = m_SurfaceDraw->GetDC( &hDC );
	if( hResult != DD_OK )
		return;

	SetPixelV( hDC, x, y, RGB( r, g, b ) );

	m_SurfaceDraw->ReleaseDC( hDC );
}

void COverlay::DrawRadarPoint( int x,int y, int r, int g, int b )
{
	COLORREF color;
	color = RGB(r,g,b);

	HDC hdc_destination;
	m_SurfaceDraw->GetDC( &hdc_destination );

	SetPixelV(hdc_destination,x,y,color);
	SetPixelV(hdc_destination,x - 1,y -1,color);
	SetPixelV(hdc_destination,x + 1,y + 1,color);
	SetPixelV(hdc_destination,x + 1,y - 1,color);
	SetPixelV(hdc_destination,x - 1,y + 1,color);
	SetPixelV(hdc_destination,x + 1,y,color);
	SetPixelV(hdc_destination,x - 1,y,color);
	SetPixelV(hdc_destination,x,y + 1,color);
	SetPixelV(hdc_destination,x,y - 1,color);

	m_SurfaceDraw->ReleaseDC( hdc_destination );
}

void COverlay::DrawBorder( )
{
	HRESULT hResult = NULL;
	HDC		hDC		= NULL;

	// you SHOULD do a fill of the whole overlay size.. otherwise you'll may get awful artifacts
	FillRGB( 0, 0, m_RectSource.right, m_RectSource.bottom, 0, 0, 0 );


	// when drawing large static things.. skip the nice functions of this class and
	// draw with GDI between surface GetDC and ReleaseDC to bypass the
	// repetitive surface locking and unlocking.. otherwise you'll REALLY slow down the whole thing.

	m_SurfaceDraw->GetDC( &hDC );
	for( int i = 0; i < 200; i++ )
	{
		SetPixelV( hDC, i, 0, RGB( 250, 250, 250 ) );
		SetPixelV( hDC, 0, i, RGB( 250, 250, 250 ) );

		SetPixelV( hDC, i, 199, RGB( 250, 250, 250 ) );
		SetPixelV( hDC, 199, i, RGB( 250, 250, 250 ) );

		SetPixelV( hDC, i, 100, RGB( 250, 250, 250 ) );

		if( i < 101 )
		{
			SetPixelV( hDC, i, i, RGB( 250, 250, 250 ) );
			SetPixelV( hDC, 200 - i, i, RGB( 250, 250, 250 ) );
		}
		else
		{
			SetPixelV( hDC, 100, i, RGB( 250, 250, 250 ) );
		}
	}
	m_SurfaceDraw->ReleaseDC( hDC );
}
