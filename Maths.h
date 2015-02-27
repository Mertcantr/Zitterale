#pragma once

#define _DotProduct( x, y ) ((x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2])
#define M_PI 3.14159265358979323846
#define RadToDeg(x) (x * (180 / M_PI))

template <typename tType>
__forceinline tType square( const tType &tValue )
{
	return tValue * tValue;
}
template <typename tType>
__forceinline void BoundValue( tType &tValue, const tType tMax, const tType tMin )
{
	if( tValue > tMax )
		tValue = tMax;
	if( tValue < tMin )
		tValue = tMin;
}

typedef float TRANSFORM_MATRIX[MAXSTUDIOBONES][3][4];
typedef TRANSFORM_MATRIX *PTRANSFORM_MATRIX;

bool _fastcall CalcScreen( float *fInput, float *fOutput );
void VectorAngles( const float * fForward, float * fAngles );
inline float VectorAngle( const float * fAngleA, const float * fAngleB );
void MakeVector( const float * fAngle, float * fVector );
void VectorRotateX( const float * fAngleIn, float fDegree, float * fAngleOut );
void VectorRotateY( const float * fAngleIn, float fDegree, float * fAngleOut );
void VectorRotateZ( const float * fAngleIn, float fDegree, float * fAngleOut );
void VectorTransform( float *in1, float in2[3][4], float *out );
bool PathFree( vec3_t vTo );
float _fastcall GetDistance( float *fFrom, float *fTo );
bool GetScreenCenter( void );
float VectorNormalize (vec3_t v);

extern int g_iScreenCenterY;
extern int g_iScreenCenterX;
