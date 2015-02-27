#pragma once

class CVarObject
{
public:
	CVarObject &operator =( const CVarObject &rhs )
	{
		strcpy(szName, rhs.szName);
		strcpy(szValue, rhs.szValue);
		bValue = rhs.bValue;
		iValue = rhs.iValue;
		fValue = rhs.fValue;
		return *this;
	}
	char szName[256];
	char szValue[256];
	char szDesc[256];
	bool bValue;
	int iValue;
	float fValue;
};

class CCVarHandler
{
public:
	CCVarHandler();
	virtual ~CCVarHandler();

	bool RegisterCVar( CVarObject *const pObject, const char *szName, const char *szStandardValue, const char *szDescription );
	bool SetCVar( CVarObject *const pObject, const char *szValue );
	bool SetCVar( CVarObject *const pObject, int iValue );
	bool SetCVar( CVarObject *const pObject, float fValue );
	CVarObject *const FindCVarByName( const char *szName );
	bool Read( const char *szFile );
	bool Write( const char *szFile );
private:
	std::vector<CVarObject*> m_vCVarList;
};

#define RegisterVar( a, b, c, d, e ) g_cCVar.RegisterCVar( &a, b, c, "" );

extern CCVarHandler g_cCVar;
