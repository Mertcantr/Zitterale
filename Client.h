#pragma once

void _stdcall HUD_Redraw( void );
void _stdcall SoundEvent( int entity, char *szSound, float *flPosition );
void _stdcall StudioRenderModel( PSTUDIO_MODEL_RENDERER_FRAME pRendererFrame );
void _stdcall CL_CreateMove( float frametime, usercmd_t *pCmd );
void _stdcall V_CalcRefDef( ref_params_t *refParams );

extern bool g_isIngame;
extern char g_szPath[MAX_PATH];
extern int g_iSecurityLevel;

#define SECURITY_LEVEL(x) (g_iSecurityLevel==(x))
