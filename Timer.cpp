// Timer Class
// Last change: 16.02.2007 12:28 GMT+1
#include "stdafx.h"
#include "Timer.h"

// NOTE: You don't need to stop this timer, just restart it.
// TODO: Pause the timer

// For compatibility. If you don't use Win32 just define another function
// similiar to GetTickCount or make your own.
#define CURRENT_TIME GetTickCount

CTimer::CTimer()
{
	m_lTime = NULL;
	m_lExpirationTime = NULL;
	m_bRunning = false;
}

void CTimer::Start()
{
	m_lTime = CURRENT_TIME();
	m_bRunning = true;
}

void CTimer::Countdown( unsigned long lMs )
{
	Start();
	m_lExpirationTime = lMs;
	m_bCountdown = true;
}

bool CTimer::Expired()
{
	if( !m_bRunning )
		return true;
	if( CURRENT_TIME() - m_lTime >= m_lExpirationTime )
	{
		m_bRunning = false;
		m_lTime = NULL;
		m_lExpirationTime = NULL;
		return true;
	}
	else
		return false;
}

void CTimer::SetExpired()
{
	if( m_bRunning )
	{
		m_lTime = NULL;
		m_lExpirationTime = NULL;
		m_bRunning = false;
	}
}

unsigned long CTimer::GetElapsedMilliseconds()
{
	if( !m_bRunning )
		return NULL;
	return(CURRENT_TIME()-m_lTime);
}

unsigned long CTimer::GetElapsedSeconds()
{
	if( !m_bRunning )
		return NULL;
	return((CURRENT_TIME()-m_lTime) / 1000);
}

unsigned long CTimer::GetElapsedMinutes()
{
	if( !m_bRunning )
		return NULL;
	if( (CURRENT_TIME()-m_lTime) / 1000 >= 60 && m_bRunning )
		return (((CURRENT_TIME()-m_lTime) / 1000) / 60);

	return NULL;
}

unsigned long CTimer::GetMillisecondsToCount()
{
	if( !m_bCountdown || !m_bRunning )
		return NULL;
	return (m_lExpirationTime-GetElapsedMilliseconds());
}

unsigned long CTimer::GetSecondsToCount()
{
	if( !m_bCountdown || !m_bRunning )
		return NULL;
	return ((m_lExpirationTime/1000)-GetElapsedSeconds());
}

unsigned long CTimer::GetMinutesToCount()
{
	if( !m_bCountdown || !m_bRunning )
		return NULL;
	if ( (m_lExpirationTime/1000) >= 60 && m_bRunning && m_bCountdown )
		return (((m_lExpirationTime/1000) / 60)-GetElapsedMinutes());
	return NULL;
}

bool CTimer::IsRunning()
{
	return m_bRunning;
}
