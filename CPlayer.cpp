#include "stdafx.h"
#include "CPlayer.h"
#include "SecureEngine.h"
#include "Client.h"

CPlayer g_cPlayers;

// Interface to get player information and set it

CPlayer::CPlayer( )
{
	m_vPlayers.clear( );
}

CPlayer::~CPlayer( )
{
	DestroyPlayers( );
}

bool CPlayer::InitializePlayers( void )
{
	m_cLocalPlayer.iIndex = -1;
	m_cLocalPlayer.bAlive = false;
	m_cLocalPlayer.eTeam = TEAM_UNASSIGNED;
	m_cLocalPlayer.pEnt = NULL;
	strcpy( m_cLocalPlayer.szWeapon, XStr( /*<< unknown >>*/ 0x04, 0x0D, 0x00, 0x3C3D2276, 0x6A6E6868, 0x7F672A35, 0x32000000 ).c() );
	Log(("InitializePlayers called"));

	// Loop through max. available players
	for( int iCount = 0; iCount < MAX_VPLAYERS; iCount++ )
	{
		// Construct an object and list item for each player
		CPlayerObject *pPlayerObject = new CPlayerObject;
		if( !pPlayerObject )
		{
			return false;
		}

		pPlayerObject->bAlive = false;
		pPlayerObject->bVisible = false;
		pPlayerObject->bGotHitbox = false;
		pPlayerObject->bGotSound = false;
		pPlayerObject->bValid = false;
		pPlayerObject->bUpdated = false;
		pPlayerObject->eTeam = TEAM_UNASSIGNED;
		pPlayerObject->iHealth = 0;
		pPlayerObject->iIndex = iCount;
		pPlayerObject->pEnt = NULL;
		VectorClear( pPlayerObject->vHitbox );
		strcpy( pPlayerObject->szName, XStr( /*<< unknown >>*/ 0x04, 0x0D, 0x00, 0x3C3D2276, 0x6A6E6868, 0x7F672A35, 0x32000000 ).c() );
		strcpy( pPlayerObject->szWeapon, XStr( /*<< unknown >>*/ 0x04, 0x0D, 0x00, 0x3C3D2276, 0x6A6E6868, 0x7F672A35, 0x32000000 ).c() );
		m_vPlayers.push_back(pPlayerObject);
	}

	return true;
}

void CPlayer::UpdateLocalPlayer( void )
{
	if( !g_cSecureEngine.Get()->GetLocalPlayer() )
		return;

	m_cLocalPlayer.iIndex = g_cSecureEngine.Get()->GetLocalPlayer()->index;
	m_cLocalPlayer.pEnt = g_cSecureEngine.Get()->GetLocalPlayer( );
	GetStrippedWeaponName( m_cLocalPlayer.szWeapon, m_cLocalPlayer.pEnt );
	VectorCopy(m_cLocalPlayer.pEnt->origin, m_cLocalPlayer.vOrigin);
	g_cSecureEngine.Get()->EV_LocalPlayerViewheight(m_cLocalPlayer.vEyeOrigin);
	VectorAdd(m_cLocalPlayer.pEnt->origin, m_cLocalPlayer.vEyeOrigin, m_cLocalPlayer.vEyeOrigin);

	if( g_iSecurityLevel > 1 )
	{
		cl_entity_s *pEnt = g_cSecureEngine.Get()->GetLocalPlayer();
		if( pEnt && pEnt->player && !pEnt->curstate.spectator && pEnt->curstate.solid  && !(pEnt->curstate.effects &EF_NODRAW) )
			m_cLocalPlayer.bAlive = true;
		else
			m_cLocalPlayer.bAlive = false;
	}

	hud_player_info_t tInfo;
	g_cSecureEngine.Get()->pfnGetPlayerInfo( m_cLocalPlayer.iIndex, &tInfo );
	if( tInfo.model )
		m_cLocalPlayer.eTeam = GetTeamFromModel(tInfo.model);
}

void CPlayer::UpdatePlayer( CPlayerObject *const curPlayer )
{
	if( !curPlayer || curPlayer->iIndex == m_cLocalPlayer.iIndex )
		return;

	curPlayer->pEnt = g_cSecureEngine.Get()->GetEntityByIndex( curPlayer->iIndex );
	curPlayer->bValid = CheckValidEntity( curPlayer->pEnt );
	if( curPlayer->bValid )
	{
		hud_player_info_t tInfo;
		g_cSecureEngine.Get()->pfnGetPlayerInfo( curPlayer->iIndex, &tInfo );
		strcpy( curPlayer->szName, tInfo.name );
		GetStrippedWeaponName( curPlayer->szWeapon, curPlayer->pEnt );
		if( tInfo.model )
			curPlayer->eTeam = GetTeamFromModel(tInfo.model);
		curPlayer->bUpdated = true;
	}
	if( curPlayer->m_cSoundTimer.Expired() )
		curPlayer->bGotSound = false;

}

CPlayerObject *const CPlayer::GetPlayer( int iIdx )
{
	if( iIdx >= 0 && iIdx < MAX_VPLAYERS )
	{
		return m_vPlayers[iIdx];
	}

	return NULL;
}

CLocalPlayer &CPlayer::GetLocalPlayer( void )
{
	return m_cLocalPlayer;
}

void CPlayer::DestroyPlayers( void )
{
	Log(("Destroying players"));
	vector<CPlayerObject*>::iterator ic;
	for( ic = m_vPlayers.begin(); ic != m_vPlayers.end(); ic++ )
		delete (*ic);
	m_vPlayers.clear();
}

bool CPlayer::CheckValidEntity( cl_entity_s *pEnt )
{
	if( pEnt && pEnt->player && !pEnt->curstate.spectator && pEnt->curstate.solid
		&& pEnt != g_cSecureEngine.Get()->GetLocalPlayer() && !(pEnt->curstate.effects &EF_NODRAW)
		&& !(pEnt->curstate.messagenum < g_cSecureEngine.Get()->GetLocalPlayer()->curstate.messagenum) )
		return true;
	return false;
}

ePlayerTeam CPlayer::GetTeamFromModel( const char *model )
{
	// Terror team models
	if (strstr(model, XStr( /*arctic*/ 0x02, 0x06, 0x00, 0x61736177, 0x6D660000 ).c())		||
		strstr(model, XStr( /*guerilla*/ 0x02, 0x08, 0x00, 0x67746771, 0x6D696A66 ).c())	||
		strstr(model, XStr( /*leet*/ 0x01, 0x04, 0x00, 0x6C646777 ).c())		||
		strstr(model, XStr( /*terror*/ 0x02, 0x06, 0x00, 0x74647071, 0x6B770000 ).c()))
		return TEAM_T;

	// Counter team models
	else if (strstr(model, XStr( /*gign*/ 0x01, 0x04, 0x00, 0x6768656D ).c())	||
		strstr(model, XStr( /*gsg9*/ 0x01, 0x04, 0x00, 0x6772653A ).c())		||
		strstr(model, XStr( /*sas*/ 0x01, 0x03, 0x00, 0x73607100 ).c())		||
		strstr(model, XStr( /*urban*/ 0x02, 0x05, 0x00, 0x75736062, 0x6A000000 ).c())		||
		strstr(model, XStr( /*vip*/ 0x01, 0x03, 0x00, 0x76687200 ).c()))
		return TEAM_CT;

	return TEAM_UNASSIGNED;
}

void CPlayer::GetStrippedWeaponName( char *szOut, cl_entity_s *pEnt )
{
	char szWeapon[64];
	model_s *pModel = NULL;

	try
	{
		pModel = g_cSecureEngine.Get()->GetModelByIndex(pEnt->curstate.weaponmodel);
	}
	catch( ... )
	{
		return;
	}

	char *szName = pModel->name;
	if( !szName )
		return;
	size_t iLen = lstrlen(szName);
	if( iLen > 48 || iLen < 10 )
		return;

	if( strstr(szName, XStr( /*shield*/ 0x02, 0x06, 0x00, 0x73696B66, 0x68610000 ).c()) )
	{
		lstrcpy( szWeapon, szName+23 );
		iLen -= 23;
	}
	else
	{
		lstrcpy( szWeapon, szName+9 );
		iLen -= 9;
	}

	szWeapon[iLen-4] = '\0';
	strcpy( szOut, szWeapon );
}
