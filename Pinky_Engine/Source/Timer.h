#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "External Libraries\SDL\include\SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Stop();
	void Step();

	Uint32 Read();
	float ReadSec();
	float ReadSecScale(float scale);//read time with scale factor


private:

	bool running;
	Uint32	started_at;
	Uint32	stopped_at;
	Uint32	timeElapsed;
	Uint32	timeStep;
	float currentTime;
	float lastTime;
};

#endif //__TIMER_H__