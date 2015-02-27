#pragma once

#pragma warning(disable:4005) // macro re-definition

#include "HLSDK\Engine\wrect.h"
#include "HLSDK\Engine\cl_dll.h"
#include "HLSDK\Engine\cdll_int.h"
#include "HLSDK\Engine\const.h"
#include "HLSDK\Engine\progdefs.h"
#include "HLSDK\Engine\eiface.h"
#include "HLSDK\Engine\edict.h"
#include "HLSDK\Engine\studio.h"
#include "HLSDK\Engine\studio_event.h"
#include "HLSDK\Engine\entity_types.h"
#include "HLSDK\Engine\r_efx.h"
#include "HLSDK\Engine\net_api.h"
#include "HLSDK\Engine\demo_api.h"
#include "HLSDK\Engine\triangleapi.h"
#include "HLSDK\Engine\event_api.h"
#include "HLSDK\Engine\pm_defs.h"
#include "HLSDK\Engine\ref_params.h"
#include "HLSDK\Engine\r_studioint.h"
#include "HLSDK\Engine\com_model.h"
#include "HlSDK\Engine\parsemsg.h"
#include "HLSDK\Common\screenfade.h"
#include "HLSDK\Misc\defs.h"
#include "HLSDK\Misc\parsemsg.h"

typedef struct ExportTable_s
{
	int (*Initialize)(cl_enginefunc_t *, int);
	int (*HUD_Init)(void);
	int (*HUD_VidInit)(void);
	int (*HUD_Redraw)(float, int);
	int (*HUD_UpdateClientData)(client_data_t*, float);
	int (*HUD_Reset)(void);
	void (*HUD_PlayerMove)(struct playermove_s*, int);
	void (*HUD_PlayerMoveInit)(struct playermove_s*);
	char (*HUD_PlayerMoveTexture)(char *);
	void (*IN_ActivateMouse)(void);
	void (*IN_DeactivateMouse)(void);
	void (*IN_MouseEvent)(int mstate);
	void (*IN_ClearStates)(void);
	void (*IN_Accumulate)(void);
	void (*CL_CreateMove)(float, struct usercmd_s*, int);
	int (*CL_IsThirdPerson)(void);
	void (*CL_CameraOffset)(float *);
	struct kbutton_s *(*KB_Find)(const char*);
	void (*CAM_Think)(void);
	void (*V_CalcRefdef)(struct ref_params_s *pparams);
	int (*HUD_AddEntity)(int, struct cl_entity_s *, const char *);
	void (*HUD_CreateEntities)(void);
	void (*HUD_DrawNormalTriangles)(void);
	void (*HUD_DrawTransparentTriangles)(void);
	void (*HUD_StudioEvent)(const struct mstudioevent_s *, const struct cl_entity_s *);
	void (*HUD_PostRunCmd)(struct local_state_s*, struct local_state_s*, struct usercmd_s*, int, double, unsigned int);
	void (*HUD_Shutdown)(void);
	void (*HUD_TxferLocalOverrides)(struct entity_state_s *, const struct clientdata_s *);
	void (*HUD_ProcessPlayerState)(struct entity_state_s *, const struct entity_state_s *);
	void (*HUD_TxferPredictionData)(struct entity_state_s *, const struct entity_state_s *, struct clientdata_s *, const struct clientdata_s *, struct weapon_data_s *, const struct weapon_data_s *);
	void (*Demo_ReadBuffer)(int, unsigned char *);
	int (*HUD_ConnectionlessPacket)(struct netadr_s*, const char*, char*, int*);
	int (*HUD_GetHullBounds)(int, float*, float*);
	void (*HUD_Frame)(double time);
	int (*HUD_Key_Event)(int, int, const char *);
	void (*HUD_TempEntUpdate)(double, double, double, struct tempent_s **, struct tempent_s **, int(*Callback_AddVisibleEntity)(struct cl_entity_s*), void(*Callback_TempEntPlaySound)(struct tempent_s *pTemp, float damp ));
	struct cl_entity_s *(*HUD_GetUserEntity)(int index);
	int (*HUD_VoiceStatus)(int entindex, qboolean bTalking);
	int (*HUD_DirectorEvent) (unsigned char, unsigned int, unsigned int, unsigned int);
	int (*HUD_GetStudioModelInterface)(int, struct r_studio_interface_s**, struct engine_studio_api_s*);
	unsigned long (*ClientFactory )(void);
} ExportTable_t;

typedef struct STUDIO_MODEL_RENDERER_FRAME_
{
	double dummy;
	double m_clTime;
	double m_clOldTime;
	int m_fDoInterp;
	int m_fGaitEstimation;
	int m_nFrameCount;
	cvar_t *m_pCvarHiModels;
	cvar_t *m_pCvarDeveloper;
	cvar_t *m_pCvarDrawEntities;
	cl_entity_t *m_pCurrentEntity;
	model_t *m_pRenderModel;
	player_info_t *m_pPlayerInfo;
	int m_nPlayerIndex;
	float m_flGaitMovement;
	studiohdr_t *m_pStudioHeader;
	mstudiobodyparts_t *m_pBodyPart;
	mstudiomodel_t *m_pSubModel;
	int m_nTopColor;
	int m_nBottomColor;
	model_t *m_pChromeSprite;
	int m_nCachedBones;
	char m_nCachedBoneNames[MAXSTUDIOBONES][32];
	float m_rgCachedBoneTransform[MAXSTUDIOBONES][3][4];
	float m_rgCachedLightTransform[MAXSTUDIOBONES][3][4];
	float m_fSoftwareXScale, m_fSoftwareYScale;
	float m_vUp[3];
	float m_vRight[3];
	float m_vNormal[3];
	float m_vRenderOrigin[3];
	int *m_pStudioModelCount;
	int *m_pModelsDrawn;
	float (*m_protationmatrix)[3][4];
	float (*m_paliastransform)[3][4];
	float (*m_pbonetransform)[MAXSTUDIOBONES][3][4];
	float (*m_plighttransform)[MAXSTUDIOBONES][3][4];
} STUDIO_MODEL_RENDERER_FRAME, *PSTUDIO_MODEL_RENDERER_FRAME;

typedef struct STUDIO_MODEL_RENDERER_
{
	void (*CStudioModelRenderer)(void);
	void (*Init)(void);
	int (*StudioDrawModel) (int flags);
	int (*StudioDrawPlayer)(int flags, struct entity_state_s *pplayer);
	mstudioanim_t (*StudioGetAnim)( model_t *m_pSubModel, mstudioseqdesc_t *pseqdesc );
	void (*StudioSetUpTransform) (int trivial_accept);
	void (*StudioSetupBones) ( void );
	void (*StudioCalcAttachments) ( void );
	void (*StudioSaveBones)( void );
	void (*StudioMergeBones)( model_t *m_pSubModel );
	float (*StudioEstimateInterpolant)( void );
	float (*StudioEstimateFrame)( mstudioseqdesc_t *pseqdesc );
	void (*StudioFxTransform)( cl_entity_t *ent, float transform[3][4] );
	void (*StudioSlerpBones)( vec4_t q1[], float pos1[][3], vec4_t q2[], float pos2[][3], float s );
	void (*StudioCalcBoneAdj) ( float dadt, float *adj, const byte *pcontroller1, const byte *pcontroller2, byte mouthopen );
	void (*StudioCalcBoneQuaterion)( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *q );
	void (*StudioCalcBonePosition)( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *pos );
	void (*StudioCalcRotations)( float pos[][3], vec4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f );
	void (*StudioRenderModel) ( void );
	void (*StudioRenderFinal) (void);
	void (*StudioRenderFinal_Software) ( void );
	void (*StudioRenderFinal_Hardware) ( void );
	void (*StudioPlayerBlend) ( mstudioseqdesc_t *pseqdesc, int *pBlend, float *pPitch );
	void (*StudioEstimateGait) ( entity_state_t *pplayer );
	void (*StudioProcessGait) ( entity_state_t *pplayer );
} STUDIO_MODEL_RENDERER, *PSTUDIO_MODEL_RENDERER;
