// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

// ---------------------------------------------
void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
	currentTime = 0;
	lastTime = 0;
	timeElapsed = 0;
	timeStep = 0;
}

// ---------------------------------------------
void Timer::Stop()
{
	timeElapsed = float((SDL_GetTicks() - started_at - timeStep) / 1000.0f);
	running = false;
	stopped_at = SDL_GetTicks();
}

void Timer::Step()
{
	Uint32 time = SDL_GetTicks();
	timeStep = time - stopped_at + timeStep;
	running = true;
}

// ---------------------------------------------
Uint32 Timer::Read()
{
	if(running == true)
	{
		return SDL_GetTicks() - started_at;
	}
	else
	{
		return stopped_at - started_at;
	}
}

float Timer::ReadSec()
{
	if (running)
	{
		return float(SDL_GetTicks() - started_at - timeStep) / 1000.0f;
	}
	else
	{
		return timeElapsed;
	}
}

float Timer::ReadeSecScale(float scale)
{
	float realTime = float((SDL_GetTicks() - started_at - timeStep) / 1000.0f);
	float time = realTime - currentTime;

	lastTime = lastTime + time * scale;
	currentTime = realTime;
	return lastTime;
}




