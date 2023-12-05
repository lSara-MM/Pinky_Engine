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
	time_passed = 0;
	offset = 0;
}

// ---------------------------------------------
void Timer::Stop()
{
	time_passed = float((SDL_GetTicks() - started_at - offset) / 1000.0f);
	running = false;
	stopped_at = SDL_GetTicks();
}

void Timer::Resume()
{
	Uint32 time = SDL_GetTicks();
	offset = time - stopped_at + offset;
	running = false;
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
	if (running == true)
	{
		return float(SDL_GetTicks() - started_at) / 1000.0f;
	}
	else
	{
		return float(stopped_at - started_at) / 1000.0f;
	}
}

float Timer::ReadeSecScale(float scale)
{
	float realTime = float((SDL_GetTicks() - started_at) / 1000.0f);
	float time = realTime - currentTime;

	lastTime = lastTime + time * scale;
	currentTime = realTime;
	return lastTime;
}




