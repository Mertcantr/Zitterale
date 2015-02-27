#pragma once

//PBYTE DetourPatch( PBYTE pTarget, PBYTE pDestination, DWORD dwLen );

class CDetour
{
public:
	CDetour();
	virtual ~CDetour();

	PBYTE DetourPatch( PBYTE pTarget, PBYTE pDestination, DWORD dwLen );
	PBYTE DetourPatch( PBYTE pTarget, PBYTE pDestination );
	void DetourRemove( void );
private:
	DWORD m_dwLen;
	PBYTE m_pTarget;
	PBYTE m_uTramp;
};
