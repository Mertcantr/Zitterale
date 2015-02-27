#pragma once

#include "FunctionStealer.h"

typedef cl_entity_s*(*GetLocalPlayer)(void);
typedef cl_entity_s*(*GetViewModel)(void);
typedef cl_entity_s*(*GetEntityByIndex)(int);
typedef void (*pfnGetPlayerInfo)(int, hud_player_info_t*);
typedef void (*EV_LocalPlayerViewheight)(float*);
typedef int (*WorldToScreen)(float*, float*);
typedef void ( *pfnFillRGBA )( int x, int y, int width, int height, int r, int g, int b, int a );
typedef int ( *pfnGetScreenInfo )( SCREENINFO *pscrinfo );
typedef void ( *pfnAngleVectors )( const float * vecAngles, float * forward, float * right, float * up );
typedef int	( *pfnDrawConsoleString )( int x, int y, char *string );
typedef void ( *pfnDrawSetTextColor )( float r, float g, float b );
typedef void ( *pfnDrawConsoleStringLen )(  const char *string, int *length, int *height );
typedef void ( *GetViewAngles )( float * );
typedef float ( *GetClientTime )( void );
typedef void ( *pfnGetScreenFade )( struct screenfade_s *fade );
typedef void ( *pfnSetScreenFade )( struct screenfade_s *fade );
typedef void ( *SetViewAngles )( float * );
typedef dlight_t *( *CL_AllocDlight )( int key );
typedef void ( *EV_SetTraceHull )( int hull );
typedef void ( *EV_PlayerTrace )( float *start, float *end, int traceFlags, int ignore_pe, struct pmtrace_s *tr );
typedef int	( *EV_IndexFromTrace)( struct pmtrace_s *pTrace );
typedef void( *Status )( struct net_status_s *status );
typedef void ( *pfnConsolePrint )( const char *string );
typedef float ****( *StudioGetBoneTransform )( void );
typedef struct model_s	*( *GetModelByIndex )( int index );
typedef int	( *pfnClientCmd )( char *szCmdString );
typedef SHORT (APIENTRY *GetAsyncKeyState_t)(INT 	ey	);

struct secure_engine
{
	struct cl_entity_s			*( *GetLocalPlayer )		( void );
	struct cl_entity_s			*( *GetViewModel )			( void );
	struct cl_entity_s			*( *GetEntityByIndex )		( int idx );
	void						( *pfnGetPlayerInfo )		( int ent_num, hud_player_info_t *pinfo );
	void						( *pfnFillRGBA )			( int x, int y, int width, int height, int r, int g, int b, int a );
	int							( *pfnGetScreenInfo ) 		( SCREENINFO *pscrinfo );
	void						( *pfnAngleVectors )		( const float * vecAngles, float * forward, float * right, float * up );
	int							( *pfnDrawConsoleString )	( int x, int y, char *string );
	void						( *pfnDrawSetTextColor )	( float r, float g, float b );
	void						( *pfnDrawConsoleStringLen )(  const char *string, int *length, int *height );
	void						( *GetViewAngles )			( float * );
	float						( *GetClientTime )			( void );
	void						( *pfnGetScreenFade )		( struct screenfade_s *fade );
	void						( *pfnSetScreenFade )		( struct screenfade_s *fade );
	void						( *SetViewAngles )			( float * );
	void						( *pfnConsolePrint )		( const char *string );
	int							( *pfnClientCmd )			( char *szCmdString );

	void	( *EV_LocalPlayerViewheight ) ( float * );
	int			( *WorldToScreen ) ( float *world, float *screen );
	dlight_t	*( *CL_AllocDlight )			( int key );
	void	( *EV_SetTraceHull ) ( int hull );
	void	( *EV_PlayerTrace ) ( float *start, float *end, int traceFlags, int ignore_pe, struct pmtrace_s *tr );
	int		( *EV_IndexFromTrace) ( struct pmtrace_s *pTrace );
	void		( *Status ) ( struct net_status_s *status );

	float			****( *StudioGetBoneTransform ) ( void );
	struct model_s	*( *GetModelByIndex )			( int index );

	SHORT  (APIENTRY *xGetAsyncKeyState)	(	INT 	Key	);
};

class CSecureEngine
{
public:
	bool Initialize( DWORD dwHwDllBase, cl_enginefuncs_s *pEngineFuncs, engine_studio_api_s *pStudioFuncs );
	secure_engine *Get( void );

private:
	CFunctionStealer m_cFuncStealer;
	bool m_bInitialized;
	secure_engine m_secureEngine;

};

extern CSecureEngine g_cSecureEngine;
