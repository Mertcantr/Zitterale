#pragma once


#define MIN_SMOOTH 1.0f
#define MAX_SMOOTH 10.0f
#define MAX_SHOOT_DISTANCE 8192
#define MAX_SHOOT_DISTANCE_HANDGUN 4096

class CAimbot
{
public:
	CAimbot( );

	void Init( void );
	void FindTarget( CPlayerObject *const pPlayer );
	void Think( void );
	void Cleanup( void );
	bool CalculateHitbox( PSTUDIO_MODEL_RENDERER_FRAME pRendererFrame );
	void CL_CreateMove( float frameTime, usercmd_t *pCmd );
	void SetSettingsBasedOnWeapon( void );
	bool IsHandGun( void );
	float CalcFOV( float fDistance );

private:
	void PredictTarget( void );
	void SmoothAngles( Vector vAimAngles );
	bool IsInFOV( float *fPosition, float fFov );
	float GetDistanceFromCursor( float *fScreen);
	void CalculateAngles( Vector &vOrigin, Vector &vAngles );
	bool CalculateTriggerBot( float frameTime, usercmd_t *pCmd );
	bool IsValidPlayer( CPlayerObject *const pPlayer );
	bool CanAim( void );
	void NoRecoil( float frameTime, float *flNewAngles );
	bool CalculateTriggerBotLevel2( void );
private:
	CPlayerObject *m_pCurrentTarget;
	float m_fCurrentTargetScreenPosition[2];
	float m_fCurrentTargetDistance;
	float m_fCurrentTargetRealDistance;
	CTimer m_cLockTimer;
	Vector m_vAimAngles;

	bool m_bAnglesCalculated;
	bool m_bGotTarget;

	// Weapon based CVar's
	CVarObject m_cvAimbot, m_cvTrigger, m_cvNoRecoil, m_cvHitbox, m_cvHeight, m_cvMaxDist, m_cvMinDist, m_cvFov, m_cvDistBasedFov, m_cvSmooth, m_cvAimbotGlobal;
	// Global CVar's
	CVarObject m_cvPredict, m_cvLock, m_cvAuto, m_cvSilent;

	// USP
	CVarObject m_uspCvAimbot, m_uspCvTrigger, m_uspCvNoRecoil, m_uspCvHitbox, m_uspCvHeight, m_uspCvMaxDist, m_uspCvMinDist, m_uspCvFov, m_uspCvDistBasedFov, m_uspCvSmooth;
	// Glock
	CVarObject m_glockCvAimbot, m_glockCvTrigger, m_glockCvNoRecoil, m_glockCvHitbox, m_glockCvHeight, m_glockCvMaxDist, m_glockCvMinDist, m_glockCvFov, m_glockCvDistBasedFov, m_glockCvSmooth;
	// Deagle
	CVarObject m_deagleCvAimbot, m_deagleCvTrigger, m_deagleCvNoRecoil, m_deagleCvHitbox, m_deagleCvHeight, m_deagleCvMaxDist, m_deagleCvMinDist, m_deagleCvFov, m_deagleCvDistBasedFov, m_deagleCvSmooth;
	// MP5
	CVarObject m_mp5CvAimbot, m_mp5CvTrigger, m_mp5CvNoRecoil, m_mp5CvHitbox, m_mp5CvHeight, m_mp5CvMaxDist, m_mp5CvMinDist, m_mp5CvFov, m_mp5CvDistBasedFov, m_mp5CvSmooth;
	// M4A1
	CVarObject m_m4a1CvAimbot, m_m4a1CvTrigger, m_m4a1CvNoRecoil, m_m4a1CvHitbox, m_m4a1CvHeight, m_m4a1CvMaxDist, m_m4a1CvMinDist, m_m4a1CvFov, m_m4a1CvDistBasedFov, m_m4a1CvSmooth;
	// AK
	CVarObject m_akCvAimbot, m_akCvTrigger, m_akCvNoRecoil, m_akCvHitbox, m_akCvHeight, m_akCvMaxDist, m_akCvMinDist, m_akCvFov, m_akCvDistBasedFov, m_akCvSmooth;
	// AWP
	CVarObject m_awpCvAimbot, m_awpCvTrigger, m_awpCvNoRecoil, m_awpCvHitbox, m_awpCvHeight, m_awpCvMaxDist, m_awpCvMinDist, m_awpCvFov, m_awpCvDistBasedFov, m_awpCvSmooth;
	// All other weapons
	CVarObject m_otherCvAimbot, m_otherCvTrigger, m_otherCvNoRecoil, m_otherCvHitbox, m_otherCvHeight, m_otherCvMaxDist, m_otherCvMinDist, m_otherCvFov, m_otherCvDistBasedFov, m_otherCvSmooth;
};

extern CAimbot g_cAimbot;