// Timer Class
// Copyright (C) 2007 by Avalon
// Contact: admin@avalon-net.com
// Last change: 16.02.2007 12:28 GMT+1
#pragma once

class CTimer
{
public:
	CTimer(); // Constructor
	virtual ~CTimer() {}; // Destructor
	void Start(); // Start a normal timer
	void Countdown( unsigned long lMs ); // Start a countdown timer
	bool Expired(); // Is the Countdown over?
	void SetExpired(); // Reset the Countdown
	unsigned long GetElapsedMilliseconds(); // Get elapsed Milliseconds since start
	unsigned long GetElapsedSeconds();  // Get elapsed Seconds since start
	unsigned long GetElapsedMinutes(); // Get elapsed Minutes since start
	unsigned long GetMillisecondsToCount(); // Get Milliseconds remaining
	unsigned long GetSecondsToCount(); // Get Seconds remaining
	unsigned long GetMinutesToCount(); // Get Minutes remaining
	bool IsRunning(); // Returns true if running and false if not

private:
	unsigned long m_lTime; // Time when timer starts to count
	unsigned long m_lExpirationTime; // Countdown time
	bool m_bRunning; // Is the timer running?
	bool m_bCountdown; // Is it a countdown timer?
};
