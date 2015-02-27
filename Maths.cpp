#include "stdafx.h"
#include "Sdk.h"
#include "Maths.h"
#include "Client.h"
#include "CPlayer.h"
#include "SecureEngine.h"

int g_iScreenCenterY;
int g_iScreenCenterX;

bool GetScreenCenter( void )
{
	SCREENINFO sScreen;
	sScreen.iSize = sizeof(SCREENINFO);
	g_cSecureEngine.Get()->pfnGetScreenInfo( &sScreen );

	g_iScreenCenterX = sScreen.iWidth  / 2;
	g_iScreenCenterY = sScreen.iHeight / 2;
	return true;
}

bool _fastcall CalcScreen( float *fInput, float *fOutput )
{
	int iRasterizer	= g_cSecureEngine.Get()->WorldToScreen( fInput, fOutput );
	if( fOutput[0] < 1 && fOutput[1] < 1 && fOutput[0] > -1 && fOutput[1] > -1 && !iRasterizer )
	{
		fOutput[0] =  fOutput[0] * g_iScreenCenterX + g_iScreenCenterX;
		fOutput[1] = -fOutput[1] * g_iScreenCenterY + g_iScreenCenterY;

		return true;
	}

	return false;
}

// Credits: GD (Game Deception Cheat Lite or OGC)

void VectorAngles( const float * fForward, float * fAngles )
{
	float fYaw = 0.0f;
	float fPitch = 0.0f;
	float fDot = 0.0f;

	if( fForward[1] == 0 && fForward[0] == 0 )
	{
		fYaw = 0;
		if( fForward[2] > 0 )	{ fPitch = 90.0f; }
		else					{ fPitch = 270.0f; }
	}
	else
	{
		fYaw = (float)(atan2( fForward[1], fForward[0] ) * 180 / M_PI );
		if( fYaw < 0 ) { fYaw += 360.0f; }
		fDot = sqrt( fForward[0] * fForward[0] + fForward[1] * fForward[1] );
		fPitch = (float)(atan2( fForward[2], fDot ) * 180 / M_PI);
	}

	fAngles[0] = fPitch;
	fAngles[1] = fYaw;
	fAngles[2] = 0;
}

inline float VectorAngle( const float * fAngleA, const float * fAngleB )
{
	float fLengthA = VectorLength( fAngleA );
	float fLengthB = VectorLength( fAngleB );
	float fLength = (fLengthA * fLengthB);
	if( fLength != 0.0 )
	{
		return( (float)(acos( _DotProduct( fAngleA, fAngleB ) / fLength ) * (180.0f / M_PI)) );
	}
	return( 0.0f );
}

void MakeVector( const float * fAngle, float * fVector )
{
	float fPitch = (float)(fAngle[0] * (M_PI / 180));
	float fYaw = (float)(fAngle[1] * (M_PI / 180));
	float fCosPitch = (float)cos( fPitch );

	fVector[0] = (float)(-fCosPitch * -cos( fYaw ));
	fVector[1] = (float)(sin( fYaw ) * fCosPitch);
	fVector[2] = (float)-sin( fPitch );
}

void VectorRotateX( const float * fAngleIn, float fDegree, float * fAngleOut )
{
	float fPitch = (float)(fDegree * (M_PI / 180));
	float fCos = (float)cos( fPitch );
	float fSin = (float)sin( fPitch );

	fAngleOut[0] = fAngleIn[0];
	fAngleOut[1] = fCos * fAngleIn[1] - fSin * fAngleIn[2];
	fAngleOut[2] = fSin * fAngleIn[1] + fCos * fAngleIn[2];
}

void VectorRotateY( const float * fAngleIn, float fDegree, float * fAngleOut )
{
	float fPitch = (float)(fDegree * (M_PI / 180));
	float fCos = (float)cos( fPitch );
	float fSin = (float)sin( fPitch );

	fAngleOut[0] = fCos * fAngleIn[0] + fSin * fAngleIn[2];
	fAngleOut[1] = fAngleIn[1];
	fAngleOut[2] = -fSin * fAngleIn[0] + fCos * fAngleIn[2];
}

void VectorRotateZ( const float * fAngleIn, float fDegree, float * fAngleOut )
{
	float fPitch = (float)(fDegree * (M_PI / 180));
	float fCos = (float)cos( fPitch );
	float fSin = (float)sin( fPitch );

	fAngleOut[0] = fCos * fAngleIn[0] - fSin * fAngleIn[1];
	fAngleOut[1] = fSin * fAngleIn[0] + fCos * fAngleIn[1];
	fAngleOut[2] = fAngleIn[2];
}

 void VectorTransform( float *in1, float in2[3][4], float *out )
{
	out[0] = DotProduct(in1, in2[0]) + in2[0][3];
	out[1] = DotProduct(in1, in2[1]) + in2[1][3];
	out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}

float _fastcall GetDistance( float *fFrom, float *fTo )
{
	if( !fFrom || !fTo )
		return -1.0f;

	float fAngles[3];
	VectorSubtract(fTo, fFrom, fAngles);

	return sqrt(square(fAngles[0])+square(fAngles[1])+square(fAngles[2])) / 22.0f;
}

bool PathFree( vec3_t vTo )
{
	pmtrace_t pmTrace;
	g_cSecureEngine.Get()->EV_SetTraceHull( 2 );
	g_cSecureEngine.Get()->EV_PlayerTrace( vTo, g_cPlayers.GetLocalPlayer().vEyeOrigin, PM_GLASS_IGNORE,
		g_cPlayers.GetLocalPlayer().iIndex, &pmTrace );
	return( pmTrace.fraction >= 0.85f );
}

float VectorNormalize (vec3_t v)
{
	float	length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	length = sqrt (length);		// FIXME

	if (length)
	{
		ilength = 1/length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;

}
