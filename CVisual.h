#pragma once

typedef struct drawstring_s
{
	char szString[1024];
	DWORD dwStartTime;
	DWORD dwVisibleTime;
} drawstring_t;

class CVisual
{
public:
	explicit CVisual( );
	virtual ~CVisual( );

	void Init( void );
	void PercentNoFlash( void );
	void DrawBox( int iX, int iY, color_t *pColor, int iRadius );
	void DrawFilledBox( int iX, int iY, int iW, int iH,
		color_t *pColor, color_t *pFillColor );
	void DrawHudString( int iX, int iY, bool bCenter, color_t *pColor,
		char *szString );
	void DrawHudStringFormat( int iX, int iY, bool bCenter, color_t *pColor,
		const char *szFormat, ... );
	void HUD_Redraw( void );
	void AddCenterDrawString( const char *szString, DWORD dwVisibleTime );
	void drawSxeBox( HDC DCcs, int x, int y, int size, COLORREF colors );
private:
	CVarObject m_cvFlashpercent, m_cvBoxthickness;
	vector<drawstring_s*> m_vDrawString;
};

extern CVisual g_cVisual;
