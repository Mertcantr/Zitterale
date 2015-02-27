#pragma once


class XStr
{
public:
	XStr( int hashlen, int strlen, int key, ... );
	virtual ~XStr( );
	char * c();
private:
	DWORD_PTR * m_charList;
	char * m_retBuf;
	int m_strLength;
	int m_hashLength;
	int m_key;
};
