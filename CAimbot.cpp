#include "stdafx.h"
#include "CPlayer.h"
#include "Client.h"
#include "CColor.h"
#include "CCVar.h"
#include "CVisual.h"
#include "Maths.h"
#include "CAimbot.h"
#include "SecureEngine.h"

CAimbot g_cAimbot;

CAimbot::CAimbot( )
	: m_bGotTarget(false), m_pCurrentTarget(NULL), m_fCurrentTargetDistance(0), m_bAnglesCalculated(false)
{

}

void CAimbot::Init( void )
{
	// Global CVar's
	RegisterVar(m_cvAimbotGlobal, XStr( /*aim_global_active*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x63696965, 0x6965556A, 0x6F796779, 0x75000000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Deactivates aim and triggerbot on all weapons if set to 0
	RegisterVar(m_cvLock, XStr( /*aim_lock*/ 0x02, 0x08, 0x00, 0x61686F5C, 0x686A656C ).c(), XStr( /*2*/ 0x01, 0x01, 0x00, 0x32000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Lock on target for n miliseconds
	RegisterVar(m_cvAuto, XStr( /*aim_auto*/ 0x02, 0x08, 0x00, 0x61686F5C, 0x65707268 ).c(), XStr( /*0*/ 0x01, 0x01, 0x00, 0x30000000 ).c(), AIM_SECTION, TYPE_INTEGER); // If set to "1" aimbot will always aim, set to "0" only aims when shooting. If using aim_trigger this should be 0.
	RegisterVar(m_cvSilent, XStr( /*aim_silent*/ 0x03, 0x0A, 0x00, 0x61686F5C, 0x776C6A62, 0x667D0000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // Real silent mode (aim without changing your current view) !Enable when playing with EAC!
	RegisterVar(m_cvPredict, XStr( /*aim_predict*/ 0x03, 0x0B, 0x00, 0x61686F5C, 0x74776363, 0x616A7E00 ).c(), XStr( /*3*/ 0x01, 0x01, 0x00, 0x33000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Latency based precit value

	// Weapon based CVar's

	// USP
	RegisterVar(m_uspCvTrigger, XStr( /*aim_usp_trigger*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x71767658, 0x7C7B636C, 0x6B687C00 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // Activate triggerbot
	RegisterVar(m_uspCvAimbot, XStr( /*aim_usp_active*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x71767658, 0x696A7E62, 0x7A680000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Aimbot active?
	RegisterVar(m_uspCvFov, XStr( /*aim_usp_fov*/ 0x03, 0x0B, 0x00, 0x61686F5C, 0x71767658, 0x6E667C00 ).c(), XStr( /*30*/ 0x01, 0x02, 0x00, 0x33310000 ).c(), AIM_SECTION, TYPE_INTEGER); // FOV
	RegisterVar(m_uspCvDistBasedFov, XStr( /*aim_usp_distbasedfov*/ 0x05, 0x14, 0x00, 0x61686F5C, 0x71767658, 0x6C60797F, 0x6E6C7D6A, 0x74777D65 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Distance based FOV
	RegisterVar(m_uspCvMaxDist, XStr( /*aim_usp_maxdist*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x71767658, 0x6568726F, 0x657E7A00 ).c(), XStr( /*100*/ 0x01, 0x03, 0x00, 0x31313200 ).c(), AIM_SECTION, TYPE_INTEGER); // Maximum real 3D distance
	RegisterVar(m_uspCvMinDist, XStr( /*aim_usp_mindist*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x71767658, 0x6560646F, 0x657E7A00 ).c(), XStr( /*5*/ 0x01, 0x01, 0x00, 0x35000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Minimum real 3D distance
	RegisterVar(m_uspCvHitbox, XStr( /*aim_usp_hitbox*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x71767658, 0x60607E69, 0x63750000 ).c(), XStr( /*9*/ 0x01, 0x01, 0x00, 0x39000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox (9 = head)
	RegisterVar(m_uspCvHeight, XStr( /*aim_usp_height*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x71767658, 0x606C636C, 0x64790000 ).c(), XStr( /*10*/ 0x01, 0x02, 0x00, 0x31310000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox height adjustment
	RegisterVar(m_uspCvSmooth, XStr( /*aim_usp_smooth*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x71767658, 0x7B646564, 0x78650000 ).c(), XStr( /*5.0*/ 0x01, 0x03, 0x00, 0x352F3200 ).c(), AIM_SECTION, TYPE_FLOAT); // Smooth aim angles
	RegisterVar(m_uspCvNoRecoil, XStr( /*aim_usp_norecoil*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x71767658, 0x6666786E, 0x6F626763 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // No recoil

	// Glock
	RegisterVar(m_glockCvTrigger, XStr( /*aim_glock_trigger*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x63696964, 0x63567E79, 0x656A696A, 0x62000000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // Activate triggerbot
	RegisterVar(m_glockCvAimbot, XStr( /*aim_glock_active*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x63696964, 0x63566B68, 0x7864786A ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Aimbot active?
	RegisterVar(m_glockCvFov, XStr( /*aim_glock_fov*/ 0x04, 0x0D, 0x00, 0x61686F5C, 0x63696964, 0x63566C64, 0x7A000000 ).c(), XStr( /*30*/ 0x01, 0x02, 0x00, 0x33310000 ).c(), AIM_SECTION, TYPE_INTEGER); // FOV
	RegisterVar(m_glockCvDistBasedFov, XStr( /*aim_glock_distbasedfov*/ 0x06, 0x16, 0x00, 0x61686F5C, 0x63696964, 0x63566E62, 0x7F796C6E, 0x63747675, 0x7B630000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Distance based FOV
	RegisterVar(m_glockCvMaxDist, XStr( /*aim_glock_maxdist*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x63696964, 0x6356676A, 0x7469677C, 0x64000000 ).c(), XStr( /*100*/ 0x01, 0x03, 0x00, 0x31313200 ).c(), AIM_SECTION, TYPE_INTEGER); // Maximum real 3D distance
	RegisterVar(m_glockCvMinDist, XStr( /*aim_glock_mindist*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x63696964, 0x63566762, 0x6269677C, 0x64000000 ).c(), XStr( /*5*/ 0x01, 0x01, 0x00, 0x35000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Minimum real 3D distance
	RegisterVar(m_glockCvHitbox, XStr( /*aim_glock_hitbox*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x63696964, 0x63566262, 0x786F6177 ).c(), XStr( /*9*/ 0x01, 0x01, 0x00, 0x39000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox (9 = head)
	RegisterVar(m_glockCvHeight, XStr( /*aim_glock_height*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x63696964, 0x6356626E, 0x656A667B ).c(), XStr( /*10*/ 0x01, 0x02, 0x00, 0x31310000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox height adjustment
	RegisterVar(m_glockCvSmooth, XStr( /*aim_glock_smooth*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x63696964, 0x63567966, 0x63627A67 ).c(), XStr( /*5.0*/ 0x01, 0x03, 0x00, 0x352F3200 ).c(), AIM_SECTION, TYPE_FLOAT); // Smooth aim angles
	RegisterVar(m_glockCvNoRecoil, XStr( /*aim_glock_norecoil*/ 0x05, 0x12, 0x00, 0x61686F5C, 0x63696964, 0x63566464, 0x7E686D60, 0x797D0000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // No recoil

	// Deagle
	RegisterVar(m_deagleCvTrigger, XStr( /*aim_deagle_trigger*/ 0x05, 0x12, 0x00, 0x61686F5C, 0x60606760, 0x646C557F, 0x7E646968, 0x75630000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // Activate triggerbot
	RegisterVar(m_deagleCvAimbot, XStr( /*aim_deagle_active*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x60606760, 0x646C556A, 0x6F796779, 0x75000000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Aimbot active?
	RegisterVar(m_deagleCvFov, XStr( /*aim_deagle_fov*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x60606760, 0x646C556D, 0x637B0000 ).c(), XStr( /*30*/ 0x01, 0x02, 0x00, 0x33310000 ).c(), AIM_SECTION, TYPE_INTEGER); // FOV
	RegisterVar(m_deagleCvDistBasedFov, XStr( /*aim_deagle_distbasedfov*/ 0x06, 0x17, 0x00, 0x61686F5C, 0x60606760, 0x646C556F, 0x657E7A6D, 0x71627777, 0x727A6000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Distance based FOV
	RegisterVar(m_deagleCvMaxDist, XStr( /*aim_deagle_maxdist*/ 0x05, 0x12, 0x00, 0x61686F5C, 0x60606760, 0x646C5566, 0x6D756A66, 0x63650000 ).c(), XStr( /*100*/ 0x01, 0x03, 0x00, 0x31313200 ).c(), AIM_SECTION, TYPE_INTEGER); // Maximum real 3D distance
	RegisterVar(m_deagleCvMinDist, XStr( /*aim_deagle_mindist*/ 0x05, 0x12, 0x00, 0x61686F5C, 0x60606760, 0x646C5566, 0x65636A66, 0x63650000 ).c(), XStr( /*5*/ 0x01, 0x01, 0x00, 0x35000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Minimum real 3D distance
	RegisterVar(m_deagleCvHitbox, XStr( /*aim_deagle_hitbox*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x60606760, 0x646C5563, 0x65796C60, 0x68000000 ).c(), XStr( /*9*/ 0x01, 0x01, 0x00, 0x39000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox (9 = head)
	RegisterVar(m_deagleCvHeight, XStr( /*aim_deagle_height*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x60606760, 0x646C5563, 0x69646967, 0x64000000 ).c(), XStr( /*10*/ 0x01, 0x02, 0x00, 0x31310000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox height adjustment
	RegisterVar(m_deagleCvSmooth, XStr( /*aim_deagle_smooth*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x60606760, 0x646C5578, 0x6162617B, 0x78000000 ).c(), XStr( /*5.0*/ 0x01, 0x03, 0x00, 0x352F3200 ).c(), AIM_SECTION, TYPE_FLOAT); // Smooth aim angles
	RegisterVar(m_deagleCvNoRecoil, XStr( /*aim_deagle_norecoil*/ 0x05, 0x13, 0x00, 0x61686F5C, 0x60606760, 0x646C5565, 0x637F6B6C, 0x7F787E00 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // No recoil

	// MP5
	RegisterVar(m_mp5CvTrigger, XStr( /*aim_mp5_trigger*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x69753358, 0x7C7B636C, 0x6B687C00 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // Activate triggerbot
	RegisterVar(m_mp5CvAimbot, XStr( /*aim_mp5_active*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x69753358, 0x696A7E62, 0x7A680000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Aimbot active?
	RegisterVar(m_mp5CvFov, XStr( /*aim_mp5_fov*/ 0x03, 0x0B, 0x00, 0x61686F5C, 0x69753358, 0x6E667C00 ).c(), XStr( /*30*/ 0x01, 0x02, 0x00, 0x33310000 ).c(), AIM_SECTION, TYPE_INTEGER); // FOV
	RegisterVar(m_mp5CvDistBasedFov, XStr( /*aim_mp5_distbasedfov*/ 0x05, 0x14, 0x00, 0x61686F5C, 0x69753358, 0x6C60797F, 0x6E6C7D6A, 0x74777D65 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Distance based FOV
	RegisterVar(m_mp5CvMaxDist, XStr( /*aim_mp5_maxdist*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x69753358, 0x6568726F, 0x657E7A00 ).c(), XStr( /*100*/ 0x01, 0x03, 0x00, 0x31313200 ).c(), AIM_SECTION, TYPE_INTEGER); // Maximum real 3D distance
	RegisterVar(m_mp5CvMinDist, XStr( /*aim_mp5_mindist*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x69753358, 0x6560646F, 0x657E7A00 ).c(), XStr( /*5*/ 0x01, 0x01, 0x00, 0x35000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Minimum real 3D distance
	RegisterVar(m_mp5CvHitbox, XStr( /*aim_mp5_hitbox*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x69753358, 0x60607E69, 0x63750000 ).c(), XStr( /*9*/ 0x01, 0x01, 0x00, 0x39000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox (9 = head)
	RegisterVar(m_mp5CvHeight, XStr( /*aim_mp5_height*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x69753358, 0x606C636C, 0x64790000 ).c(), XStr( /*10*/ 0x01, 0x02, 0x00, 0x31310000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox height adjustment
	RegisterVar(m_mp5CvSmooth, XStr( /*aim_mp5_smooth*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x69753358, 0x7B646564, 0x78650000 ).c(), XStr( /*5.0*/ 0x01, 0x03, 0x00, 0x352F3200 ).c(), AIM_SECTION, TYPE_FLOAT); // Smooth aim angles
	RegisterVar(m_mp5CvNoRecoil, XStr( /*aim_mp5_norecoil*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x69753358, 0x6666786E, 0x6F626763 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // No recoil

	// M4A1
	RegisterVar(m_m4a1CvTrigger, XStr( /*aim_m4a1_trigger*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x69316736, 0x577D7862, 0x6B6A6B7D ).c(), XStr( /*0*/ 0x01, 0x01, 0x00, 0x30000000 ).c(), AIM_SECTION, TYPE_INTGER); // Activate triggerbot
	RegisterVar(m_m4a1CvAimbot, XStr( /*aim_m4a1_active*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x69316736, 0x5768697F, 0x657B6B00 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Aimbot active?
	RegisterVar(m_m4a1CvFov, XStr( /*aim_m4a1_fov*/ 0x03, 0x0C, 0x00, 0x61686F5C, 0x69316736, 0x576F657D ).c(), XStr( /*30*/ 0x01, 0x02, 0x00, 0x33310000 ).c(), AIM_SECTION, TYPE_INTEGER); // FOV
	RegisterVar(m_m4a1CvDistBasedFov, XStr( /*aim_m4a1_distbasedfov*/ 0x06, 0x15, 0x00, 0x61686F5C, 0x69316736, 0x576D6378, 0x786F6F7C, 0x7575747C, 0x62000000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Distance based FOV
	RegisterVar(m_m4a1CvMaxDist, XStr( /*aim_m4a1_maxdist*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x69316736, 0x57646B73, 0x68647D7B ).c(), XStr( /*100*/ 0x01, 0x03, 0x00, 0x31313200 ).c(), AIM_SECTION, TYPE_INTEGER); // Maximum real 3D distance
	RegisterVar(m_m4a1CvMinDist, XStr( /*aim_m4a1_mindist*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x69316736, 0x57646365, 0x68647D7B ).c(), XStr( /*5*/ 0x01, 0x01, 0x00, 0x35000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Minimum real 3D distance
	RegisterVar(m_m4a1CvHitbox, XStr( /*aim_m4a1_hitbox*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x69316736, 0x5761637F, 0x6E627600 ).c(), XStr( /*9*/ 0x01, 0x01, 0x00, 0x39000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox (9 = head)
	RegisterVar(m_m4a1CvHeight, XStr( /*aim_m4a1_height*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x69316736, 0x57616F62, 0x6B657A00 ).c(), XStr( /*10*/ 0x01, 0x02, 0x00, 0x31310000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox height adjustment
	RegisterVar(m_m4a1CvSmooth, XStr( /*aim_m4a1_smooth*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x69316736, 0x577A6764, 0x63796600 ).c(), XStr( /*5.0*/ 0x01, 0x03, 0x00, 0x352F3200 ).c(), AIM_SECTION, TYPE_FLOAT); // Smooth aim angles
	RegisterVar(m_m4a1CvNoRecoil, XStr( /*aim_m4a1_norecoil*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x69316736, 0x57676579, 0x696E6166, 0x7C000000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // No recoil

	// AK
	RegisterVar(m_akCvTrigger, XStr( /*aim_ak_trigger*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x656E5973, 0x7A606D6C, 0x697F0000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // Activate triggerbot
	RegisterVar(m_akCvAimbot, XStr( /*aim_ak_active*/ 0x04, 0x0D, 0x00, 0x61686F5C, 0x656E5966, 0x6B7D637D, 0x69000000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Aimbot active?
	RegisterVar(m_akCvFov, XStr( /*aim_ak_fov*/ 0x03, 0x0A, 0x00, 0x61686F5C, 0x656E5961, 0x677F0000 ).c(), XStr( /*30*/ 0x01, 0x02, 0x00, 0x33310000 ).c(), AIM_SECTION, TYPE_INTEGER); // FOV
	RegisterVar(m_akCvDistBasedFov, XStr( /*aim_ak_distbasedfov*/ 0x05, 0x13, 0x00, 0x61686F5C, 0x656E5963, 0x617A7E69, 0x6D7E6B6B, 0x767E6400 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Distance based FOV
	RegisterVar(m_akCvMaxDist, XStr( /*aim_ak_maxdist*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x656E596A, 0x69716E62, 0x7F790000 ).c(), XStr( /*100*/ 0x01, 0x03, 0x00, 0x31313200 ).c(), AIM_SECTION, TYPE_INTEGER); // Maximum real 3D distance
	RegisterVar(m_akCvMinDist, XStr( /*aim_ak_mindist*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x656E596A, 0x61676E62, 0x7F790000 ).c(), XStr( /*5*/ 0x01, 0x01, 0x00, 0x35000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Minimum real 3D distance
	RegisterVar(m_akCvHitbox, XStr( /*aim_ak_hitbox*/ 0x04, 0x0D, 0x00, 0x61686F5C, 0x656E596F, 0x617D6864, 0x74000000 ).c(), XStr( /*9*/ 0x01, 0x01, 0x00, 0x39000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox (9 = head)
	RegisterVar(m_akCvHeight, XStr( /*aim_ak_height*/ 0x04, 0x0D, 0x00, 0x61686F5C, 0x656E596F, 0x6D606D63, 0x78000000 ).c(), XStr( /*10*/ 0x01, 0x02, 0x00, 0x31310000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox height adjustment
	RegisterVar(m_akCvSmooth, XStr( /*aim_ak_smooth*/ 0x04, 0x0D, 0x00, 0x61686F5C, 0x656E5974, 0x6566657F, 0x64000000 ).c(), XStr( /*5.0*/ 0x01, 0x03, 0x00, 0x352F3200 ).c(), AIM_SECTION, TYPE_FLOAT); // Smooth aim angles
	RegisterVar(m_akCvNoRecoil, XStr( /*aim_ak_norecoil*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x656E5969, 0x677B6F68, 0x63646200 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // No recoil

	// AWP
	RegisterVar(m_awpCvTrigger, XStr( /*aim_awp_trigger*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x65727658, 0x7C7B636C, 0x6B687C00 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // Activate triggerbot
	RegisterVar(m_awpCvAimbot, XStr( /*aim_awp_active*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x65727658, 0x696A7E62, 0x7A680000 ).c(), XStr( /*0*/ 0x01, 0x01, 0x00, 0x30000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Aimbot active?
	RegisterVar(m_awpCvFov, XStr( /*aim_awp_fov*/ 0x03, 0x0B, 0x00, 0x61686F5C, 0x65727658, 0x6E667C00 ).c(), XStr( /*30*/ 0x01, 0x02, 0x00, 0x33310000 ).c(), AIM_SECTION, TYPE_INTEGER); // FOV
	RegisterVar(m_awpCvDistBasedFov, XStr( /*aim_awp_distbasedfov*/ 0x05, 0x14, 0x00, 0x61686F5C, 0x65727658, 0x6C60797F, 0x6E6C7D6A, 0x74777D65 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Distance based FOV
	RegisterVar(m_awpCvMaxDist, XStr( /*aim_awp_maxdist*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x65727658, 0x6568726F, 0x657E7A00 ).c(), XStr( /*100*/ 0x01, 0x03, 0x00, 0x31313200 ).c(), AIM_SECTION, TYPE_INTEGER); // Maximum real 3D distance
	RegisterVar(m_awpCvMinDist, XStr( /*aim_awp_mindist*/ 0x04, 0x0F, 0x00, 0x61686F5C, 0x65727658, 0x6560646F, 0x657E7A00 ).c(), XStr( /*5*/ 0x01, 0x01, 0x00, 0x35000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Minimum real 3D distance
	RegisterVar(m_awpCvHitbox, XStr( /*aim_awp_hitbox*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x65727658, 0x60607E69, 0x63750000 ).c(), XStr( /*9*/ 0x01, 0x01, 0x00, 0x39000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox (9 = head)
	RegisterVar(m_awpCvHeight, XStr( /*aim_awp_height*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x65727658, 0x606C636C, 0x64790000 ).c(), XStr( /*10*/ 0x01, 0x02, 0x00, 0x31310000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox height adjustment
	RegisterVar(m_awpCvSmooth, XStr( /*aim_awp_smooth*/ 0x04, 0x0E, 0x00, 0x61686F5C, 0x65727658, 0x7B646564, 0x78650000 ).c(), XStr( /*5.0*/ 0x01, 0x03, 0x00, 0x352F3200 ).c(), AIM_SECTION, TYPE_FLOAT); // Smooth aim angles
	RegisterVar(m_awpCvNoRecoil, XStr( /*aim_awp_norecoil*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x65727658, 0x6666786E, 0x6F626763 ).c(), XStr( /*0*/ 0x01, 0x01, 0x00, 0x30000000 ).c(), AIM_SECTION, TYPE_INTGER); // No recoil

	// All Other weapons
	RegisterVar(m_otherCvTrigger, XStr( /*aim_other_trigger*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x6B716E62, 0x7A567E79, 0x656A696A, 0x62000000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // Activate triggerbot
	RegisterVar(m_otherCvAimbot, XStr( /*aim_other_active*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x6B716E62, 0x7A566B68, 0x7864786A ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Aimbot active?
	RegisterVar(m_otherCvFov, XStr( /*aim_other_fov*/ 0x04, 0x0D, 0x00, 0x61686F5C, 0x6B716E62, 0x7A566C64, 0x7A000000 ).c(), XStr( /*30*/ 0x01, 0x02, 0x00, 0x33310000 ).c(), AIM_SECTION, TYPE_INTEGER); // FOV
	RegisterVar(m_otherCvDistBasedFov, XStr( /*aim_other_distbasedfov*/ 0x06, 0x16, 0x00, 0x61686F5C, 0x6B716E62, 0x7A566E62, 0x7F796C6E, 0x63747675, 0x7B630000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Distance based FOV
	RegisterVar(m_otherCvMaxDist, XStr( /*aim_other_maxdist*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x6B716E62, 0x7A56676A, 0x7469677C, 0x64000000 ).c(), XStr( /*100*/ 0x01, 0x03, 0x00, 0x31313200 ).c(), AIM_SECTION, TYPE_INTEGER); // Maximum real 3D distance
	RegisterVar(m_otherCvMinDist, XStr( /*aim_other_mindist*/ 0x05, 0x11, 0x00, 0x61686F5C, 0x6B716E62, 0x7A566762, 0x6269677C, 0x64000000 ).c(), XStr( /*5*/ 0x01, 0x01, 0x00, 0x35000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Minimum real 3D distance
	RegisterVar(m_otherCvHitbox, XStr( /*aim_other_hitbox*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x6B716E62, 0x7A566262, 0x786F6177 ).c(), XStr( /*9*/ 0x01, 0x01, 0x00, 0x39000000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox (9 = head)
	RegisterVar(m_otherCvHeight, XStr( /*aim_other_height*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x6B716E62, 0x7A56626E, 0x656A667B ).c(), XStr( /*10*/ 0x01, 0x02, 0x00, 0x31310000 ).c(), AIM_SECTION, TYPE_INTEGER); // Hitbox height adjustment
	RegisterVar(m_otherCvSmooth, XStr( /*aim_other_smooth*/ 0x04, 0x10, 0x00, 0x61686F5C, 0x6B716E62, 0x7A567966, 0x63627A67 ).c(), XStr( /*5.0*/ 0x01, 0x03, 0x00, 0x352F3200 ).c(), AIM_SECTION, TYPE_FLOAT); // Smooth aim angles
	RegisterVar(m_otherCvNoRecoil, XStr( /*aim_other_norecoil*/ 0x05, 0x12, 0x00, 0x61686F5C, 0x6B716E62, 0x7A566464, 0x7E686D60, 0x797D0000 ).c(), XStr( /*1*/ 0x01, 0x01, 0x00, 0x31000000 ).c(), AIM_SECTION, TYPE_INTGER); // No recoil

	// Set intial values to "other"
	m_cvTrigger = m_otherCvTrigger;
	m_cvAimbot = m_otherCvAimbot;
	m_cvFov = m_otherCvFov;
	m_cvDistBasedFov = m_otherCvDistBasedFov;
	m_cvMaxDist = m_otherCvMaxDist;
	m_cvMinDist = m_otherCvMinDist;
	m_cvHitbox = m_otherCvHitbox;
	m_cvHeight = m_otherCvHeight;
	m_cvSmooth = m_otherCvSmooth;
	m_cvNoRecoil = m_otherCvNoRecoil;
}

float CAimbot::CalcFOV( float fDistance )
{
	float fFov = m_cvFov.fValue;
	if( m_cvDistBasedFov.bValue )
	{
		fFov *= 25.0f;
		fFov /= fDistance;
	}
	else
		fFov *= 15.0f;

	return fFov;
}

void CAimbot::FindTarget( CPlayerObject *const pPlayer )
{
	if( !pPlayer || !g_cPlayers.GetLocalPlayer().bAlive )
		return;

	// Got a target and lock timer isn't expired
	if( m_bGotTarget && !m_cLockTimer.Expired() ) // don't do nothing
			return;

	float fScreen[2];
	float fScreenDistance = 0;
	float fRealDistance = 0;

	// Valid player that has hitbox, is alive, visible and not in our team
	if( IsValidPlayer(pPlayer) )
	{
		// Calculate 2D position on screen of hitbox
		if( CalcScreen(pPlayer->vHitbox, fScreen) )
		{
			// Get real distance in 3D space
			fRealDistance = GetDistance(g_cPlayers.GetLocalPlayer().vEyeOrigin, pPlayer->pEnt->origin);
			// Get distance from screen center (cursor)
			fScreenDistance = GetDistanceFromCursor(fScreen);

			if( fRealDistance >= m_cvMinDist.fValue && fRealDistance <= m_cvMaxDist.fValue )
			{
				// Is it in the desired FOV?
				if( IsInFOV(fScreen, CalcFOV(fRealDistance)) )
				{
					// If we don't have a target, set this one. If we have one and the distance of this one is lower, replace
					if( !m_bGotTarget || fScreenDistance < m_fCurrentTargetDistance )
					{
						m_pCurrentTarget = pPlayer;
						m_fCurrentTargetDistance = fScreenDistance;
						m_fCurrentTargetRealDistance = fRealDistance;

						VectorCopy(fScreen, m_fCurrentTargetScreenPosition);

						if( m_cvLock.iValue > 0 )
							m_cLockTimer.Countdown(m_cvLock.iValue);
						m_bGotTarget = true;
					}
				}

			}
		}

	}
}

bool CAimbot::IsInFOV( float *fPosition, float fFov )
{
	if( fPosition[0] >= (g_iScreenCenterX-fFov) &&
		fPosition[0] <= (g_iScreenCenterX+fFov) &&
		fPosition[1] >= (g_iScreenCenterY-fFov) &&
		fPosition[1] <= (g_iScreenCenterY+fFov) )
		return true;
	return false;

}

float CAimbot::GetDistanceFromCursor( float *fScreen )
{
	float fHorizontalDistance, fVerticalDistance, fAbsoluteDistance;

	fHorizontalDistance = g_iScreenCenterX - fScreen[0];
	fVerticalDistance   = g_iScreenCenterY - fScreen[1];

	// Pythagoras: a² + b² = √c²
	// Remove sqrt to gain performance
	fAbsoluteDistance = /*sqrt(*/square(fHorizontalDistance) + square(fVerticalDistance)/*)*/;

	return fAbsoluteDistance;
}

void CAimbot::Think( void )
{
	if( m_bGotTarget )
	{
		if( g_cPlayers.GetLocalPlayer().bAlive && m_bGotTarget )
		{
			Vector vAimAngles, vDelta;
			VectorSubtract(m_pCurrentTarget->vHitbox, m_pCurrentTarget->pEnt->origin, vDelta);
			PredictTarget( );
			VectorAdd(m_pCurrentTarget->vHitbox, vDelta, m_pCurrentTarget->vHitbox);
			CalculateAngles(m_pCurrentTarget->vHitbox, vAimAngles);
			SmoothAngles(vAimAngles);
			VectorCopy(vAimAngles, m_vAimAngles);

			if( g_iSecurityLevel >= 3 )
			{
				if( CanAim() && !g_cPlayers.GetLocalPlayer().IsReloading() && g_cPlayers.GetLocalPlayer().bAlive && m_cvAimbotGlobal.bValue )
				{
					if( m_cvAuto.bValue || g_cSecureEngine.Get()->xGetAsyncKeyState(VK_LBUTTON) )
						g_cSecureEngine.Get()->SetViewAngles(m_vAimAngles);
				}
			}

			m_bAnglesCalculated = true;
		}
	}
}

void CAimbot::CL_CreateMove( float frameTime, usercmd_t *pCmd )
{
	if( !CanAim() || g_cPlayers.GetLocalPlayer().IsReloading() || !g_cPlayers.GetLocalPlayer().bAlive || !m_cvAimbotGlobal.bValue )
		return; // don't do shit

	if( m_bAnglesCalculated )
	{
		if( (m_cvAuto.bValue || (pCmd->buttons & IN_ATTACK)) && m_cvAimbot.bValue )
		{
			if( m_cvSilent.iValue )
			{
				vec3_t viewforward, viewright, viewup, aimforward, aimright, aimup;
				float newforward, newright, newup;
				float forward = pCmd->forwardmove;
				float right = pCmd->sidemove;
				float up = pCmd->upmove;

				g_cSecureEngine.Get()->pfnAngleVectors(pCmd->viewangles, viewforward, viewright, viewup);

				// calculate aimangles into cmd->viewangles
				VectorCopy(m_vAimAngles, pCmd->viewangles);

				g_cSecureEngine.Get()->pfnAngleVectors(pCmd->viewangles, aimforward, aimright, aimup);

				newforward = DotProduct(forward * viewforward.Normalize(), aimforward) + DotProduct(right * viewright.Normalize(), aimforward) + DotProduct(up * viewup.Normalize(), aimforward);
				newright = DotProduct(forward * viewforward.Normalize(), aimright) + DotProduct(right * viewright.Normalize(), aimright) + DotProduct(up * viewup.Normalize(), aimright);
				newup = DotProduct(forward * viewforward.Normalize(), aimup) + DotProduct(right * viewright.Normalize(), aimup) + DotProduct(up * viewup.Normalize(), aimup);

				pCmd->forwardmove = newforward;
				pCmd->sidemove = newright;
				pCmd->upmove = newup;
			}
			else
			{
				VectorCopy(m_vAimAngles, pCmd->viewangles);
				g_cSecureEngine.Get()->SetViewAngles(m_vAimAngles);
			}
		}
	}

	if( m_cvTrigger.bValue )
	{
		if( CalculateTriggerBot(frameTime, pCmd) )
		{
			static bool bShoot = true;
			if( bShoot )
				pCmd->buttons |= IN_ATTACK;
			else
				pCmd->buttons &= ~IN_ATTACK;
			bShoot = !bShoot;
		}
	}

	if( (m_cvNoRecoil.iValue > 0) && m_cvAimbot.bValue )
		NoRecoil(frameTime, pCmd->viewangles);
}

void CAimbot::CalculateAngles( Vector &vOrigin, Vector &vAngles )
{
	Vector vDelta;
	VectorSubtract(g_cPlayers.GetLocalPlayer().vEyeOrigin, vOrigin, vDelta);

	float fAd = sqrt(square(vDelta.x) + square(vDelta.y));
	vAngles.x = (float)RadToDeg(atan(vDelta.z / fAd));

	if( vDelta.x >= 0 )
	{
		if( vDelta.y >= 0 )
			vAngles.y = (float)RadToDeg(atan(fabs(vDelta.y / vDelta.x))) + 180.0f;
		else
			vAngles.y = (float)RadToDeg(atan(fabs(vDelta.x / vDelta.y))) + 90.0f;
	}
	else
	{
		if( vDelta.y >= 0 )
			vAngles.y = (float)RadToDeg(atan(fabs(vDelta.x / vDelta.y))) + 270.0f;
		else
			vAngles.y = (float)RadToDeg(atan(fabs(vDelta.y / vDelta.x)));
	}

	vAngles.z = 0.0f;
}

void CAimbot::Cleanup( void )
{
	// Only reset target if lock timer is expired, but also reset if current target isn't valid, alive, not in fov or visible anymore
	if( m_cLockTimer.Expired() || !m_pCurrentTarget->bValid || !m_pCurrentTarget->bAlive || !PathFree(m_pCurrentTarget->pEnt->origin) || !IsInFOV(m_fCurrentTargetScreenPosition, CalcFOV(m_fCurrentTargetRealDistance)) )
	{
		m_cLockTimer.SetExpired();
		m_bGotTarget = false;
		VectorClear(m_fCurrentTargetScreenPosition);
		m_fCurrentTargetDistance = 0;
		m_fCurrentTargetRealDistance = 0;
	}

	// Reset angles
	VectorClear(m_vAimAngles);
	m_bAnglesCalculated = false;
}

bool CAimbot::CalculateHitbox( PSTUDIO_MODEL_RENDERER_FRAME pRendererFrame )
{
	__try
	{
		if( !pRendererFrame || !pRendererFrame->m_pCurrentEntity )
			return false;
		CPlayerObject *pPlayer = g_cPlayers.GetPlayer(pRendererFrame->m_pCurrentEntity->index);
		if( !pPlayer || !pRendererFrame || pPlayer->bGotHitbox )
			return false;

		mstudiobbox_t *pStudioBox = (mstudiobbox_t*)((PBYTE)pRendererFrame->m_pStudioHeader+pRendererFrame->m_pStudioHeader->hitboxindex);
		PTRANSFORM_MATRIX pTransform = (PTRANSFORM_MATRIX)g_cSecureEngine.Get()->StudioGetBoneTransform( );
		if( !pTransform )
			return false;

		Vector vMin, vMax;
		VectorTransform(pStudioBox[m_cvHitbox.iValue].bbmin, (*pTransform)[pStudioBox[m_cvHitbox.iValue].bone], vMin);
		VectorTransform(pStudioBox[m_cvHitbox.iValue].bbmax, (*pTransform)[pStudioBox[m_cvHitbox.iValue].bone], vMax);

		pPlayer->vHitbox = (vMin + vMax) * 0.5f;
		pPlayer->vHitbox.z += m_cvHeight.iValue;
		pPlayer->bGotHitbox = true;
		return true;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		Log(("Exception at CalculateHitbox %x", GetExceptionCode()));
		return false;
	}
}

// Latency based prediction
void CAimbot::PredictTarget( void )
{
	net_status_s netStatus;
	g_cSecureEngine.Get()->Status(&netStatus);
	cl_entity_s *pEnt = m_pCurrentTarget->pEnt;
	if( !pEnt )
		return;

	int iHistory = (pEnt->current_position + HISTORY_MAX - m_cvPredict.iValue) % HISTORY_MAX;

	Vector vOldOrigin, vCurOrigin, vDeltaOrigin;
	vOldOrigin = pEnt->ph[iHistory].origin;
	vCurOrigin = pEnt->ph[pEnt->current_position].origin;
	VectorSubtract( vCurOrigin, vOldOrigin, vDeltaOrigin );

	float fLatency = netStatus.latency;
	if( fLatency < 0.0f )
		fLatency = -fLatency;
	VectorScale(vDeltaOrigin, fLatency, vDeltaOrigin);

	VectorAdd(pEnt->origin, vDeltaOrigin, m_pCurrentTarget->vHitbox);
}

// Credits: seren1ty
void CAimbot::SmoothAngles( Vector vAimAngles )
{
	if( m_cvSmooth.fValue == 0.0f )
		return;

	Vector vDelta, vViewAngles;
	g_cSecureEngine.Get()->GetViewAngles( vViewAngles );
	float fSmooth = m_cvSmooth.fValue;

	BoundValue(fSmooth, MAX_SMOOTH, MIN_SMOOTH);
	VectorSubtract(vAimAngles, vViewAngles, vDelta);

	if( vDelta.y > 180.0f )
		vDelta.y -= 360.0f;

	if( vDelta.y < -180.0f )
		vDelta.y += 360.0f;

	if( vDelta.x < (MAX_SMOOTH-fSmooth+2.5f) &&
		vDelta.x > -(MAX_SMOOTH-fSmooth+2.5f) &&
		vDelta.y < (MAX_SMOOTH-fSmooth+2.5f) &&
		vDelta.y > -(MAX_SMOOTH-fSmooth+2.5f) )
	{
		return;
	}

	vDelta.x = (vDelta.x / fSmooth) + vViewAngles.x;
	vDelta.y = (vDelta.y / fSmooth) + vViewAngles.y;

	if( vDelta.y > 360.0f )
		vDelta.y -= 360.0f;

	if( vDelta.y < 0.0f )
		vDelta.y += 360.0f;

	VectorCopy(vDelta, vAimAngles);
}

// TODO: Shoots before enemy is at "crosshair"
// Maybe add check for trace fraction or PathFree check?!
bool CAimbot::CalculateTriggerBot( float frameTime, usercmd_t *pCmd )
{
	// TODO: Add NoSpread
	Vector vTrace, vViewAngles, vFoward;

	VectorCopy(pCmd->viewangles, vViewAngles);
	// Take recoil into account
	NoRecoil(frameTime, vViewAngles);
	g_cSecureEngine.Get()->pfnAngleVectors(vViewAngles, vFoward, NULL, NULL);

	float maxShootDistance = 0;
	if( IsHandGun() )
		maxShootDistance = MAX_SHOOT_DISTANCE_HANDGUN;
	else
		maxShootDistance = MAX_SHOOT_DISTANCE;

	vTrace = vFoward * MAX_SHOOT_DISTANCE + g_cPlayers.GetLocalPlayer().vEyeOrigin;

	pmtrace_t gVis;
	g_cSecureEngine.Get()->EV_SetTraceHull( 2 );
	g_cSecureEngine.Get()->EV_PlayerTrace( g_cPlayers.GetLocalPlayer().vEyeOrigin, vTrace, PM_STUDIO_BOX, -1, &gVis );

	int iEntity = g_cSecureEngine.Get()->EV_IndexFromTrace ( &gVis );
	CPlayerObject *pPlayer = g_cPlayers.GetPlayer(iEntity);
	if( pPlayer && pPlayer->bValid && pPlayer->bAlive && pPlayer->eTeam != g_cPlayers.GetLocalPlayer().eTeam )
		return true;
	return false;
}

bool CAimbot::CalculateTriggerBotLevel2( void )
{
	// TODO: Add NoSpread
	Vector vTrace, vViewAngles, vFoward;

	g_cSecureEngine.Get()->GetViewAngles(vViewAngles);
	g_cSecureEngine.Get()->pfnAngleVectors(vViewAngles, vFoward, NULL, NULL);

	float maxShootDistance = 0;
	if( IsHandGun() )
		maxShootDistance = MAX_SHOOT_DISTANCE_HANDGUN;
	else
		maxShootDistance = MAX_SHOOT_DISTANCE;

	vTrace = vFoward * MAX_SHOOT_DISTANCE + g_cPlayers.GetLocalPlayer().vEyeOrigin;

	pmtrace_t gVis;
	g_cSecureEngine.Get()->EV_SetTraceHull( 2 );
	g_cSecureEngine.Get()->EV_PlayerTrace( g_cPlayers.GetLocalPlayer().vEyeOrigin, vTrace, PM_STUDIO_BOX, -1, &gVis );

	int iEntity = g_cSecureEngine.Get()->EV_IndexFromTrace ( &gVis );
	CPlayerObject *pPlayer = g_cPlayers.GetPlayer(iEntity);
	if( pPlayer && pPlayer->bValid && pPlayer->eTeam != g_cPlayers.GetLocalPlayer().eTeam )
		return true;
	return false;
}

bool CAimbot::IsValidPlayer( CPlayerObject *const pPlayer )
{
	if( pPlayer && pPlayer->bValid && pPlayer->bGotHitbox && pPlayer->bAlive && pPlayer->bVisible
		&& pPlayer->eTeam != g_cPlayers.GetLocalPlayer().eTeam )
		return true;
	return false;
}

bool CAimbot::CanAim( void )
{
	// Check if we have a valid weapon in our slot
	// and return true if we have.

	cl_entity_s *pEnt = g_cSecureEngine.Get()->GetViewModel();
	if( pEnt && pEnt->model )
	{
		char *szName = pEnt->model->name;
		if( strstr(szName, XStr( /*knife*/ 0x02, 0x05, 0x00, 0x6B6F6B65, 0x61000000 ).c()) || strstr(szName, XStr( /*c4*/ 0x01, 0x02, 0x00, 0x63350000 ).c()) || strstr(szName, XStr( /*grenade*/ 0x02, 0x07, 0x00, 0x6773676D, 0x65616300 ).c()) ||
			strstr(szName, XStr( /*flashbang*/ 0x03, 0x09, 0x00, 0x666D6370, 0x6C676769, 0x6F000000 ).c()) || strstr(szName, XStr( /*v_knife*/ 0x02, 0x07, 0x00, 0x765E696D, 0x6D636300 ).c()) )
			return false;
	}
	return true;
}

void CAimbot::NoRecoil( float frameTime, float *flNewAngles )
{
	float Punch[3], Length;
	VectorCopy(g_cPlayers.GetLocalPlayer().vPunchAngle, Punch);
	Length = VectorNormalize(Punch);
	Length -= (10.0 + Length * 0.5) * frameTime;
	Length = max(Length, 0.0);
	VectorScale(Punch, Length, Punch);

	flNewAngles[0] -= Punch[0];
	flNewAngles[1] -= Punch[1];
}

bool CAimbot::IsHandGun( void )
{
	cl_entity_s *pEnt = g_cSecureEngine.Get()->GetViewModel();
	if( pEnt && pEnt->model )
	{
		char *model = pEnt->model->name;
		if( strstr(model, XStr( /*deagle*/ 0x02, 0x06, 0x00, 0x64646364, 0x68600000 ).c()) || strstr(model, XStr( /*glock*/ 0x02, 0x05, 0x00, 0x676D6D60, 0x6F000000 ).c()) || strstr(model, XStr( /*usp*/ 0x01, 0x03, 0x00, 0x75727200 ).c())
			|| strstr(model, XStr( /*p228*/ 0x01, 0x04, 0x00, 0x7033303B ).c()) )
			return true;
	}
	return false;
}

void CAimbot::SetSettingsBasedOnWeapon( void )
{
	static char szLastWeapon[64] = { '\0' };
	cl_entity_s *pEnt = g_cSecureEngine.Get()->GetViewModel();
	if( pEnt && pEnt->model && g_cPlayers.GetLocalPlayer().bAlive )
	{
		char *pszModelName = pEnt->model->name;
		// Only do if model changed and it is a weapon that is eligible for aiming
		if( strcmp(szLastWeapon, pszModelName) && CanAim() )
		{
			if( strstr(pszModelName, XStr( /*usp*/ 0x01, 0x03, 0x00, 0x75727200 ).c()) )
			{
				m_cvTrigger = m_uspCvTrigger;
				m_cvAimbot = m_uspCvAimbot;
				m_cvFov = m_uspCvFov;
				m_cvDistBasedFov = m_uspCvDistBasedFov;
				m_cvMaxDist = m_uspCvMaxDist;
				m_cvMinDist = m_uspCvMinDist;
				m_cvHitbox = m_uspCvHitbox;
				m_cvHeight = m_uspCvHeight;
				m_cvSmooth = m_uspCvSmooth;
				m_cvNoRecoil = m_uspCvNoRecoil;
				Log(("Weapon changed to usp -> Adjusted aimbot"));
			}
			else if( strstr(pszModelName, XStr( /*glock*/ 0x02, 0x05, 0x00, 0x676D6D60, 0x6F000000 ).c()) )
			{
				m_cvTrigger = m_glockCvTrigger;
				m_cvAimbot = m_glockCvAimbot;
				m_cvFov = m_glockCvFov;
				m_cvDistBasedFov = m_glockCvDistBasedFov;
				m_cvMaxDist = m_glockCvMaxDist;
				m_cvMinDist = m_glockCvMinDist;
				m_cvHitbox = m_glockCvHitbox;
				m_cvHeight = m_glockCvHeight;
				m_cvSmooth = m_glockCvSmooth;
				m_cvNoRecoil = m_glockCvNoRecoil;
				Log(("Weapon changed to glock -> Adjusted aimbot"));
			}
			else if( strstr(pszModelName, XStr( /*deagle*/ 0x02, 0x06, 0x00, 0x64646364, 0x68600000 ).c() ) )
			{
				m_cvTrigger = m_deagleCvTrigger;
				m_cvAimbot = m_deagleCvAimbot;
				m_cvFov = m_deagleCvFov;
				m_cvDistBasedFov = m_deagleCvDistBasedFov;
				m_cvMaxDist = m_deagleCvMaxDist;
				m_cvMinDist = m_deagleCvMinDist;
				m_cvHitbox = m_deagleCvHitbox;
				m_cvHeight = m_deagleCvHeight;
				m_cvSmooth = m_deagleCvSmooth;
				m_cvNoRecoil = m_deagleCvNoRecoil;
				Log(("Weapon changed to deagle -> Adjusted aimbot"));
			}
			else if( strstr(pszModelName, XStr( /*mp5*/ 0x01, 0x03, 0x00, 0x6D713700 ).c()) )
			{
				m_cvTrigger = m_mp5CvTrigger;
				m_cvAimbot = m_mp5CvAimbot;
				m_cvFov = m_mp5CvFov;
				m_cvDistBasedFov = m_mp5CvDistBasedFov;
				m_cvMaxDist = m_mp5CvMaxDist;
				m_cvMinDist = m_mp5CvMinDist;
				m_cvHitbox = m_mp5CvHitbox;
				m_cvHeight = m_mp5CvHeight;
				m_cvSmooth = m_mp5CvSmooth;
				m_cvNoRecoil = m_mp5CvNoRecoil;
				Log(("Weapon changed to mp5 -> Adjusted aimbot"));
			}
			else if( strstr(pszModelName, XStr( /*m4a1*/ 0x01, 0x04, 0x00, 0x6D356332 ).c()) )
			{
				m_cvTrigger = m_m4a1CvTrigger;
				m_cvAimbot = m_m4a1CvAimbot;
				m_cvFov = m_m4a1CvFov;
				m_cvDistBasedFov = m_m4a1CvDistBasedFov;
				m_cvMaxDist = m_m4a1CvMaxDist;
				m_cvMinDist = m_m4a1CvMinDist;
				m_cvHitbox = m_m4a1CvHitbox;
				m_cvHeight = m_m4a1CvHeight;
				m_cvSmooth = m_m4a1CvSmooth;
				m_cvNoRecoil = m_m4a1CvNoRecoil;
				Log(("Weapon changed to m4a1 -> Adjusted aimbot"));
			}
			else if( strstr(pszModelName, XStr( /*ak47*/ 0x01, 0x04, 0x00, 0x616A3634 ).c()) )
			{
				m_cvTrigger = m_akCvTrigger;
				m_cvAimbot = m_akCvAimbot;
				m_cvFov = m_akCvFov;
				m_cvDistBasedFov = m_akCvDistBasedFov;
				m_cvMaxDist = m_akCvMaxDist;
				m_cvMinDist = m_akCvMinDist;
				m_cvHitbox = m_akCvHitbox;
				m_cvHeight = m_akCvHeight;
				m_cvSmooth = m_akCvSmooth;
				m_cvNoRecoil = m_akCvNoRecoil;
				Log(("Weapon changed to ak -> Adjusted aimbot"));
			}
			else if( strstr(pszModelName, XStr( /*awp*/ 0x01, 0x03, 0x00, 0x61767200 ).c()) )
			{
				m_cvTrigger = m_awpCvTrigger;
				m_cvAimbot = m_awpCvAimbot;
				m_cvFov = m_awpCvFov;
				m_cvDistBasedFov = m_awpCvDistBasedFov;
				m_cvMaxDist = m_awpCvMaxDist;
				m_cvMinDist = m_awpCvMinDist;
				m_cvHitbox = m_awpCvHitbox;
				m_cvHeight = m_awpCvHeight;
				m_cvSmooth = m_awpCvSmooth;
				m_cvNoRecoil = m_awpCvNoRecoil;
				Log(("Weapon changed to awp -> Adjusted aimbot"));
			}
			else
			{
				m_cvTrigger = m_otherCvTrigger;
				m_cvAimbot = m_otherCvAimbot;
				m_cvFov = m_otherCvFov;
				m_cvDistBasedFov = m_otherCvDistBasedFov;
				m_cvMaxDist = m_otherCvMaxDist;
				m_cvMinDist = m_otherCvMinDist;
				m_cvHitbox = m_otherCvHitbox;
				m_cvHeight = m_otherCvHeight;
				m_cvSmooth = m_otherCvSmooth;
				m_cvNoRecoil = m_otherCvNoRecoil;
				Log(("Weapon changed to %s (other) -> Adjusted aimbot", pszModelName));
			}
		}

		strcpy(szLastWeapon, pszModelName);
	}

}
