#include "stdafx.h"
#include "CCVar.h"

CCVarHandler g_cCVar;


CCVarHandler::CCVarHandler( )
{
	m_vCVarList.clear( );
}

CCVarHandler::~CCVarHandler( )
{
	m_vCVarList.clear( );
}

bool CCVarHandler::RegisterCVar( CVarObject *const pObject, const char *szName, const char *szStandardValue, const char *szDescription )
{
	if( !pObject || !szName || !szStandardValue )
		return false;

	Log(("RegisterCvar 0x%08x %s %s", pObject, szName, szStandardValue));

	strcpy( pObject->szName, szName );
	strcpy( pObject->szValue, szStandardValue );
	pObject->iValue = atoi(szStandardValue);
	pObject->fValue = atof(szStandardValue);
	if( pObject->fValue > 0.0f || pObject->iValue > 0 )
		pObject->bValue = true;
	else
		pObject->bValue = false;
	m_vCVarList.push_back( pObject );
	return true;
}

bool CCVarHandler::SetCVar( CVarObject *const pObject, const char *szValue )
{
	if( !pObject || !szValue )
		return false;

	strcpy( pObject->szValue, szValue );
	pObject->iValue = atoi(szValue);
	pObject->fValue = atof(szValue);
	if( pObject->fValue > 0.0f || pObject->iValue > 0 )
		pObject->bValue = true;
	else
		pObject->bValue = false;
	return true;
}

bool CCVarHandler::SetCVar( CVarObject *const pObject, float fValue )
{
	if( !pObject )
		return false;

	sprintf_s( pObject->szValue, sizeof(pObject->szValue), "%f", fValue );
	pObject->iValue = (int)fValue;
	pObject->fValue = fValue;
	if( pObject->fValue > 0.0f || pObject->iValue > 0 )
		pObject->bValue = true;
	else
		pObject->bValue = false;
	return true;
}

bool CCVarHandler::SetCVar( CVarObject *const pObject, int iValue )
{
	if( !pObject )
		return false;

	sprintf_s( pObject->szValue, sizeof(pObject->szValue), "%d", iValue );
	pObject->iValue = iValue;
	pObject->fValue = (int)iValue;
	if( pObject->fValue > 0.0f || pObject->iValue > 0 )
		pObject->bValue = true;
	else
		pObject->bValue = false;
	return true;
}

CVarObject *const CCVarHandler::FindCVarByName( const char *szName )
{
	if( !szName )
		return false;

	vector<CVarObject*>::iterator ic;
	for( ic = m_vCVarList.begin( ); ic != m_vCVarList.end(); ic++ )
	{
		if( !lstrcmp((*ic)->szName, szName) )
			return (*ic);
	}

	return NULL;
}

bool CCVarHandler::Read( const char *szFile )
{
	if( !szFile )
		return false;

	Log(("Attempting to read cvars from %s", szFile));

	char szBuffer[4096];
	char szName[256], szValue[256];
	std::ifstream ifs;
	ifs.open( szFile );

	while( ifs )
	{
		ifs.getline( szBuffer, sizeof(szBuffer));
		if( !ifs )
			break;
		if( szBuffer )
		{
			sscanf( szBuffer, "%s %s", &szName, &szValue );
			CVarObject *const pCVar = FindCVarByName( szName );
			if( !pCVar )
				continue;
			Log(("Setting CVar %s to %s", szName, szValue));
			SetCVar( pCVar, szValue );
		}
	}

	return true;
}

//bool CCVarHandler::Write( const char *szFile )
//{
//	if( !szFile )
//		return false;
//
//	FILE *fFile = NULL;
//	fopen_s( &fFile, szFile, "w+" );
//	if( !fFile )
//		return false;
//
//	fputs( "// Zitterale configuration file\r\n", fFile );
//	std::vector<CVarObject*>::iterator ic;
//
//	for( ic = m_vCVarList.begin(); ic != m_vCVarList.end(); ic++ )
//	{
//		Log(("Writing CVar %s %s", (*ic)->szName, (*ic)->szValue));
//		fprintf_s( fFile, "%s %s\n", (*ic)->szName, (*ic)->szValue );
//	}
//
//	fclose( fFile );
//
//	return false;
//}
//
