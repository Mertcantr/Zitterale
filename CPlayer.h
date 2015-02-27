#pragma once

#define MAX_VPLAYERS 33

enum ePlayerTeam
{
	TEAM_CT = 2,
	TEAM_T,
	TEAM_UNASSIGNED
};

#define BackupSound(a, b) { VectorCopy(a.vSoundOrigin, b.vSoundOrigin); b.dwTime = a.dwTime; }
#define ClearSound(a) { VectorClear(a.vSoundOrigin); a.dwTime = 0; }

static int iSequenceInfoTable[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 2, 0, 1, 2, 0, 1, 2, 0,
	1, 2, 0, 1, 1, 2, 0, 1, 1, 2,
	0, 1, 2, 0, 1, 2, 0, 1, 2, 0,
	1, 2, 0, 1, 2, 0, 1, 2, 0, 1,
	2, 0, 1, 2, 0, 0, 0, 4, 0, 4,
	0, 5, 0, 5, 0, 0, 1, 1, 2, 0,
	1, 1, 2, 0, 1, 0, 1, 0, 1, 2,
	0, 1, 2, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3
};

typedef struct soundinfo_s
{
	Vector vSoundOrigin;
	DWORD dwTime;
} soundinfo_t;

class CPlayerObject
{
public:
	explicit CPlayerObject( ) { pEnt = NULL; }
	virtual ~CPlayerObject( ) { pEnt = NULL; }
	int iIndex;
	cl_entity_s *pEnt;
	char szName[128];
	char szWeapon[64];
	ePlayerTeam eTeam;
	bool bVisible;
	bool bAlive;
	bool bValid;
	bool bUpdated;
	bool bGotSound;
	bool bGotHitbox;
	int iHealth;
	soundinfo_t vSound;
	soundinfo_t vSoundOld;
	Vector vHitbox;
	CTimer m_cSoundTimer;
	CTimer m_cSxeTimer;
};

typedef struct player_list_s
{
	player_list_s *pNext;
	CPlayerObject *pObject;
} player_list_t;

class CLocalPlayer
{
public:
	int iIndex;
	cl_entity_s *pEnt;
	char szWeapon[64];
	bool bAlive;
	ePlayerTeam eTeam;
	Vector vEyeOrigin;
	Vector vOrigin;
	Vector vPunchAngle;
	bool IsReloading( void )
	{
		if( pEnt )
		{
			int iSequenceType = iSequenceInfoTable[pEnt->curstate.sequence];
			if( iSequenceType == 2 || iSequenceType == 5 )
				return true;
		}
		return false;
	}
};

class CPlayer
{
public:
	CPlayer( );
	virtual ~CPlayer( );
	bool InitializePlayers( void );
	void UpdatePlayer( CPlayerObject *const curPlayer );
	CPlayerObject *const GetPlayer( int iIdx );
	CLocalPlayer &GetLocalPlayer( void );
	void DestroyPlayers( void );
	void UpdateLocalPlayer( void );
private:
	bool CheckValidEntity( cl_entity_s *pEnt );
	void GetStrippedWeaponName( char *szOut, cl_entity_s *pEnt );
	ePlayerTeam GetTeamFromModel( const char *model );
	CLocalPlayer m_cLocalPlayer;
	vector<CPlayerObject*> m_vPlayers;
};

extern CPlayer g_cPlayers;
